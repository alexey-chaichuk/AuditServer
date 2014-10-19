package auditServer;

import java.util.Date;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/26/11
 * Time: 5:33 PM
 */
public class User {
    private int id;
    private String login;
    private String domain;
    private Date loggedIn;
    private String computerName;
    private String ipAddress;

    public User() {

    }

    public User(String login, String domain, Date loggedIn, String computerName, String ipAddress) {
        this.login = login;
        this.domain = domain;
        this.loggedIn = loggedIn;
        this.computerName = computerName;
        this.ipAddress = ipAddress;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getLogin() {
        return login;
    }

    public void setLogin(String login) {
        this.login = login;
    }

    public String getDomain() {
        return domain;
    }

    public void setDomain(String domain) {
        this.domain = domain;
    }

    public Date getLoggedIn() {
        return loggedIn;
    }

    public void setLoggedIn(Date loggedIn) {
        this.loggedIn = loggedIn;
    }

    public String getComputerName() {
        return computerName;
    }

    public void setComputerName(String computerName) {
        this.computerName = computerName;
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public void setIpAddress(String ipAddress) {
        this.ipAddress = ipAddress;
    }

    @Override
    public String toString() {
        return "User{" +
                "login='" + login + '\'' +
                ", domain='" + domain + '\'' +
                ", computerName='" + computerName + '\'' +
                '}';
    }
}
