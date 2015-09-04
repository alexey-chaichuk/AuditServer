// audit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <IPHlpApi.h>
#include <windows.h>
#include <iostream>

#define SECURITY_WIN32
#include <Security.h>


using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define BUF_SIZE 32768

int	enum_installed_applications(const char* server_addr, int server_port, BOOL IsUpdates, BOOL IsUserKey);
int enum_user_info(const char* server_addr, int server_port);
int wide_to_utf8(TCHAR *src, char *dst, int dst_size);
int escape_xml_string(TCHAR *src, int src_size, TCHAR *dst, int dst_size);
int remove_extra_spaces(char* buf);

int _tmain(int argc, _TCHAR* argv[])
{
	bool scan_user = false;
	//wchar_t* wserver_addr = NULL;
	char* server_addr = NULL;
	int server_port = 0;
	size_t str_size = 0;

	for(int i=1; i<argc; ++i) {
		if(argv[i][0] == '-' && wcslen(argv[i]) >= 2) {			
			switch(argv[i][1]) {
			case 'u':
				scan_user = true;
				break;
			case 'a':
				str_size = wcslen(argv[i]+2);
				if(str_size > 0) {
					server_addr = new char[str_size+1];
					wcstombs(server_addr, argv[i]+2, str_size+1);
				}
				break;
			case 'p':
				str_size = wcslen(argv[i]+2);
				if(str_size > 0) {
					server_port = _wtoi(argv[i]+2);
				}
				break;
			}
		}
	}

	if((server_addr == NULL)||(server_port <= 0)) {
		wcout << "Usage: " << argv[0] << " [-u] -aXXX.XXX.XXX.XXX -pXXXX" << endl;
		return -1;
	}

	cout << "server " << server_addr << ":" << server_port << endl;
	if(scan_user == true) {
		cout << "will scan user" << endl;
		enum_user_info(server_addr, server_port);
	} else {
		enum_installed_applications(server_addr, server_port, false, false);
	}

	return 0;
}

int enum_user_info(const char* server_addr, int server_port)
{
	SOCKET s;
	sockaddr_in addr;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	char buf[BUF_SIZE];
	const char xml_header[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	const char xml_user_start[] = "<user";
	const char xml_user_login[] = " Login=\"";
	const char xml_user_computer_name[] = " NBName=\"";
	const char xml_user_computer_domain[] = " Domain=\"";
	const char xml_user_end[] = "</user>";
	

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return -1;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(server_port);
	addr.sin_addr.S_un.S_addr = inet_addr(server_addr);

	if(connect(s, (SOCKADDR*) &addr, sizeof(addr)) == SOCKET_ERROR) {
		int error = GetLastError();
		printf("ERROR CONNECT: %s (%x)\n", strerror(error), error);
		//system("pause");
		return -1;
	}

	TCHAR computer_name[BUF_SIZE];
	DWORD computer_name_count = BUF_SIZE;
	int computer_name_utf8_count;
	GetComputerNameEx(ComputerNamePhysicalNetBIOS, computer_name, &computer_name_count);

	TCHAR user_computer_domain[BUF_SIZE];
	DWORD user_computer_domain_count = BUF_SIZE;
	int user_computer_domain_utf8_count;
	GetComputerNameEx(ComputerNamePhysicalDnsDomain, user_computer_domain, &user_computer_domain_count);

	TCHAR login[1024];	
	DWORD login_size = 1024;
	int login_utf8_count;
	if(GetUserNameEx(NameUserPrincipal, (TCHAR*)login, &login_size) == 0) { 	
		int error = GetLastError();
		printf("ERROR GetUserNameEx: %s (0x%x)\n", strerror(error), error);
		if(GetUserName((TCHAR*)login, &login_size) == 0) {
			printf("ERROR GetUserName: %s (0x%x)\n", strerror(error), error);
			return -1;
		}
	}

	send(s, xml_header, strlen(xml_header), 0);
	send(s, xml_user_start, strlen(xml_user_start), 0);	
	send(s, xml_user_computer_name, strlen(xml_user_computer_name), 0);
	computer_name_utf8_count = wide_to_utf8(computer_name, buf, sizeof(buf));
	send(s, buf, computer_name_utf8_count, 0);
	send(s, "\"", 1, 0);
	send(s, xml_user_login, strlen(xml_user_login), 0);
	login_utf8_count = wide_to_utf8(login, buf, sizeof(buf));
	send(s, buf, login_utf8_count, 0);

	cout << buf << endl;

	send(s, "\"", 1, 0);
	send(s, xml_user_computer_domain, strlen(xml_user_computer_domain), 0);
	user_computer_domain_utf8_count = wide_to_utf8(user_computer_domain, buf, sizeof(buf));
	send(s, buf, user_computer_domain_utf8_count, 0);
	send(s, "\">", 2, 0);
	send(s, xml_user_end, strlen(xml_user_end), 0);
	
	closesocket(s);
	WSACleanup();

	return 0;
}

int enum_installed_applications(const char* server_addr, int server_port, BOOL IsUpdates, BOOL IsUserKey)
{
	DWORD dwSize = MAX_PATH, dwType, dwValue;
	bool bIsSystemComponent, bIsUpdate;
	TCHAR szProcessorName[BUF_SIZE];
	TCHAR szProcessorNameEsc[BUF_SIZE];
	TCHAR szParentKeyName[MAX_PATH];
	TCHAR szDisplayName[BUF_SIZE];
	TCHAR szDisplayNameEsc[BUF_SIZE];
	TCHAR szKeyName[MAX_PATH];	
	char buf[BUF_SIZE];
	HKEY hKey, hRootKey, hAppKey;
	LONG ItemIndex = 0;
	const char xml_header[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	const char xml_computer_start[] = "<computer";
	const char xml_computer_name[] = " NBName=\"";
	const char xml_computer_osversion[] = " OSVersion=\"";
	const char xml_computer_domain[] = " Domain=\"";
	const char xml_computer_end[] = "</computer>";
    const char xml_computer_processor[] = " Processor=\"";
	const char xml_computer_ram[] = " RAM=";

	const char xml_product_start[] = "<product";
	const char xml_product_name[] = " name=\"";

	const char xml_netadapter_start[] = "<netAdapter";
	const char xml_netadapter_end[] = " />";
	const char xml_netadapter_type[] = " type=";
	const char xml_netadapter_isdhcp[] = " isdhcp=";
	const char xml_netadapter_mac[] = " mac=\"";
	const char xml_netadapter_ip[] = " ip=\"";
	const char xml_netadapter_gateway[] = " gateway=\"";


	hRootKey = IsUserKey ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;

	SOCKET s;
	sockaddr_in addr;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return -1;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(server_port);
	addr.sin_addr.S_un.S_addr = inet_addr(server_addr);

	if(connect(s, (SOCKADDR*) &addr, sizeof(addr)) == SOCKET_ERROR) {
		int error = GetLastError();
		printf("ERROR CONNECT: %s (%x)\n", strerror(error), error);
		//system("pause");
		return -1;
	}

	TCHAR computer_name[BUF_SIZE];
	DWORD computer_name_count = BUF_SIZE;
	int computer_name_utf8_count;

	TCHAR computer_domain[BUF_SIZE];
	DWORD computer_domain_count = BUF_SIZE;
	int computer_domain_utf8_count;

	GetComputerNameEx(ComputerNamePhysicalNetBIOS, computer_name, &computer_name_count);
	GetComputerNameEx(ComputerNamePhysicalDnsDomain, computer_domain, &computer_domain_count);

	// <computer>
    send(s, xml_header, strlen(xml_header), 0);
	send(s, xml_computer_start, strlen(xml_computer_start), 0);
	send(s, xml_computer_name, strlen(xml_computer_name), 0);
	computer_name_utf8_count = wide_to_utf8(computer_name, buf, sizeof(buf));
	send(s, buf, computer_name_utf8_count, 0);
	send(s, "\"", 1, 0);
	send(s, xml_computer_domain, strlen(xml_computer_domain), 0);
	computer_domain_utf8_count = wide_to_utf8(computer_domain, buf, sizeof(buf));
	send(s, buf, computer_domain_utf8_count, 0);
	send(s, "\"", 1, 0);

	SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    wchar_t* reg_hwdesc_path = _T("Hardware\\Description\\System\\CentralProcessor\\0");
	if (RegOpenKey(HKEY_LOCAL_MACHINE, reg_hwdesc_path, &hKey) == ERROR_SUCCESS) {
	    dwSize = MAX_PATH;
	    if (RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, &dwType, (LPBYTE)szProcessorName, &dwSize) == ERROR_SUCCESS) {
            send(s, xml_computer_processor, strlen(xml_computer_processor), 0);
            escape_xml_string(szProcessorName, wcslen(szProcessorName), szProcessorNameEsc, BUF_SIZE);
            int name_len = wide_to_utf8(szProcessorNameEsc, buf, sizeof(buf));
			name_len = remove_extra_spaces(buf);
            send(s, buf, name_len, 0);
            name_len = sprintf(buf, " (%u cores)", sysinfo.dwNumberOfProcessors);
            send(s, buf, name_len, 0);
            send(s, "\"", 1, 0);
	    }
	    RegCloseKey(hKey);
	}

	MEMORYSTATUSEX memstat;
	memstat.dwLength = sizeof(memstat);
	if (GlobalMemoryStatusEx(&memstat) != 0) {
		send(s, xml_computer_ram, strlen(xml_computer_ram), 0);
		sprintf(buf, "\"%d\"", memstat.ullTotalPhys/1024);
		send(s, buf, strlen(buf), 0);
	}

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&osvi) != 0) {
		send(s, xml_computer_osversion, strlen(xml_computer_osversion), 0);
		char versionSP[BUF_SIZE];
		CharToOem(osvi.szCSDVersion, versionSP);
		sprintf(buf, "Windows %d.%d.%d, %s\"", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, versionSP);
		send(s, buf, strlen(buf), 0);
	}

	send(s, ">", 1, 0);
	// </computer>

    ULONG buflen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)malloc(buflen);

	if(GetAdaptersInfo(pAdapterInfo, &buflen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(buflen);		
	}
	if(GetAdaptersInfo(pAdapterInfo, &buflen) == NO_ERROR) {
		for(PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next) {
			cout << pAdapter->Description << "[" << pAdapter->IpAddressList.IpAddress.String << "]" << endl;
			send(s, xml_netadapter_start, strlen(xml_netadapter_start), 0);

			send(s, xml_netadapter_type, strlen(xml_netadapter_type), 0);
            sprintf(buf, "\"%d\"", pAdapter->Type);
            send(s, buf, strlen(buf), 0);

            send(s, xml_netadapter_isdhcp, strlen(xml_netadapter_isdhcp), 0);
            sprintf(buf, "\"%d\"", pAdapter->DhcpEnabled);
            send(s, buf, strlen(buf), 0);

            send(s, xml_netadapter_ip, strlen(xml_netadapter_ip), 0);
            sprintf(buf, "%s\"", pAdapter->IpAddressList.IpAddress.String);
            send(s, buf, strlen(buf), 0);

            send(s, xml_netadapter_gateway, strlen(xml_netadapter_gateway), 0);
            sprintf(buf, "%s\"", pAdapter->GatewayList.IpAddress.String);
            send(s, buf, strlen(buf), 0);

            send(s, xml_netadapter_mac, strlen(xml_netadapter_mac), 0);
            sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X\"",
                pAdapter->Address[0], pAdapter->Address[1],
                pAdapter->Address[2], pAdapter->Address[3],
                pAdapter->Address[4], pAdapter->Address[5]);
            send(s, buf, strlen(buf), 0);

            send(s, xml_netadapter_end, strlen(xml_netadapter_end), 0);
		}
	}


	wchar_t* reg_uninstall_path[] = {_T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"),
								     _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall")};
	for(int i=0; i<2; ++i) {
		if (RegOpenKey(hRootKey, reg_uninstall_path[i], &hKey) != ERROR_SUCCESS) {
			continue;
		}

		while (RegEnumKeyEx(hKey, ItemIndex, szKeyName, &dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {		
			if (RegOpenKey(hKey, szKeyName, &hAppKey) == ERROR_SUCCESS) {
				dwType = REG_DWORD;
				dwSize = sizeof(DWORD);

				if (RegQueryValueEx(hAppKey, _T("SystemComponent"), NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS) {
					bIsSystemComponent = (dwValue == 0x1);
				}
				else {
					bIsSystemComponent = FALSE;
				}

				dwType = REG_SZ;
				dwSize = MAX_PATH;
				bIsUpdate = (RegQueryValueEx(hAppKey, _T("ParentKeyName"), NULL, &dwType, (LPBYTE)szParentKeyName, &dwSize) == ERROR_SUCCESS);

				dwSize = MAX_PATH;
				if (RegQueryValueEx(hAppKey, _T("DisplayName"), NULL, &dwType, (LPBYTE)szDisplayName, &dwSize) == ERROR_SUCCESS) {
					if (!bIsSystemComponent) {
						if ((IsUpdates == FALSE && !bIsUpdate) || /* Applications only */
							(IsUpdates == TRUE && bIsUpdate)) /* Updates only */
						{
							send(s, xml_product_start, strlen(xml_product_start), 0);
							send(s, xml_product_name, strlen(xml_product_name), 0);
							escape_xml_string(szDisplayName, wcslen(szDisplayName), szDisplayNameEsc, BUF_SIZE);
							int name_len = wide_to_utf8(szDisplayNameEsc, buf, sizeof(buf));
							send(s, buf, name_len, 0);						
							send(s, "\"/>", 3, 0);
						}
					}
				}	
				RegCloseKey(hAppKey);
			}

			dwSize = MAX_PATH;
			++ItemIndex;
		}
		RegCloseKey(hKey);
	}

	send(s, xml_computer_end, strlen(xml_computer_end), 0);

	closesocket(s);
	WSACleanup();

	return 0;
}

int wide_to_utf8(TCHAR *src, char *dst, int dst_size) {
	int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)src, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)src, -1, dst, dst_size, NULL, NULL);
	return len-1;
}

int escape_xml_string(TCHAR *src, int src_size, TCHAR *dst, int dst_size) {

	if((src_size == 0)||(dst_size==0)) return 0;
	if((src == NULL)||(dst == NULL)) return 0;
	int i,j;	
	for(i=0, j=0; (i<src_size)&&(j<(dst_size-1)); i++, j++){
		if(src[i] == _T('\"')) {
			dst[j] = _T('&');
			dst[++j] = _T('q');
			dst[++j] = _T('u');
			dst[++j] = _T('o');
			dst[++j] = _T('t');
			dst[++j] = _T(';');
		} else if(src[i] == _T('<')) {
			dst[j] = _T('&');
			dst[++j] = _T('l');
			dst[++j] = _T('t');								
			dst[++j] = _T(';');
		} else if(src[i] == _T('>')) {
			dst[j] = _T('&');
			dst[++j] = _T('g');
			dst[++j] = _T('t');
			dst[++j] = _T(';');
		} else if(src[i] == _T('&')) {
			dst[j] = _T('&');
			dst[++j] = _T('a');
			dst[++j] = _T('m');
			dst[++j] = _T('p');
			dst[++j] = _T(';');
		} else {
			dst[j] = src[i];
		}
	}
	dst[j] = _T('\0');
	return j;
}

int remove_extra_spaces(char* buf) 
{
	int i = 0;
	int j = 0;
	int blanks = 0;
	bool started = false;
	printf("%s\n", buf);	
	while(buf[i] != '\0') {
		if((buf[i] == ' ') || (buf[i] == '\t')) {
			if(started) {
				blanks = 1;
			} 			
		} else {
			started = true;
			if(blanks != 0) {
				buf[j] = ' ';
				j++;
				blanks = 0;
			}
			buf[j] = buf[i];
			j++;
		}
		i++;
	}
	buf[j] = '\0';
	printf("%s\n", buf);		
	return j;
}
