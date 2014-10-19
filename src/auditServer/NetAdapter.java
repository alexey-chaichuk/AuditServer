package auditServer;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 10/13/11
 * Time: 12:07 PM
 * To change this template use File | Settings | File Templates.
 */
public class NetAdapter {
    private int id;
    private int type;
    private int isdhcp;
    private String mac;
    private String ipaddr;
    private String gateway;

    public NetAdapter() {
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getIsdhcp() {
        return isdhcp;
    }

    public void setIsdhcp(int isdhcp) {
        this.isdhcp = isdhcp;
    }

    public String getMac() {
        return mac;
    }

    public void setMac(String mac) {
        this.mac = mac;
    }

    public String getIpaddr() {
        return ipaddr;
    }

    public void setIpaddr(String ipaddr) {
        this.ipaddr = ipaddr;
    }

    public String getGateway() {
        return gateway;
    }

    public void setGateway(String gateway) {
        this.gateway = gateway;
    }

    @Override
    public String toString() {
        return "NetAdapter{" +
                "id=" + id +
                ", type=" + type +
                ", mac='" + mac + '\'' +
                ", isdhcp='" + isdhcp + '\'' +
                ", ipaddr='" + ipaddr + '\'' +
                ", gateway='" + gateway + '\'' +
                '}';
    }
}
