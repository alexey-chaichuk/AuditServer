package auditServer;

import java.sql.*;
import java.util.concurrent.SynchronousQueue;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/26/11
 * Time: 5:39 PM
 */
public class UserDBWriter implements Runnable {
    private SynchronousQueue<User> usersQueue;
    static final String driverClassName = "com.mysql.jdbc.Driver";

    private String url;
    private String user;
    private String password;

    public UserDBWriter(SynchronousQueue<User> usersQueue, String url, String user, String password) {
        this.usersQueue = usersQueue;
        this.url = url;
        this.user = user;
        this.password = password;
    }

    private Connection getConnection() throws SQLException {
        Connection conn = null;
        try {
            Class.forName(driverClassName);
            conn = DriverManager.getConnection(url, user, password);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return conn;
    }

    private void updateUser(User user) {
        String sql1 = "INSERT INTO users(computername,domain,ipaddress,loggedin,login) VALUES(?,?,?,?,?)";
        Connection conn = null;
        PreparedStatement ps1 = null;
        try {
            conn = getConnection();
            ps1 = conn.prepareStatement(sql1, Statement.RETURN_GENERATED_KEYS);
            ps1.setString(1, user.getComputerName());
            ps1.setString(2, user.getDomain());
            ps1.setString(3, user.getIpAddress());
            ps1.setTimestamp(4, new Timestamp(user.getLoggedIn().getTime()));
            ps1.setString(5, user.getLogin());
            ps1.executeUpdate();
            ResultSet rs1 = ps1.getGeneratedKeys();
            if(rs1.next()) {
                user.setId(rs1.getInt(1));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            try {
                ps1.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
            try {
                conn.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void run() {
        while(true) {
            try {
                User user = usersQueue.take();
                System.out.println("!!! got new user: " + user);
                updateUser(user);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
