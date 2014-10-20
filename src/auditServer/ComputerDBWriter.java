package auditServer;


import java.sql.*;
import java.util.concurrent.SynchronousQueue;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/19/11
 * Time: 2:38 PM
 */
public class ComputerDBWriter implements Runnable {
    private SynchronousQueue<Computer> computersQueue;
    static final String driverClassName = "com.mysql.jdbc.Driver";

    private String url;
    private String user;
    private String password;

    public ComputerDBWriter(SynchronousQueue<Computer> computersQueue, String url, String user, String password) {
        this.computersQueue = computersQueue;
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

    private void updateComputer(Computer computer) {
        String sql1 = "INSERT INTO computers(ipaddress,name,domain,osversion,processor,ram_kb,updated) VALUES(?,?,?,?,?,?,?)";
        String sql2 = "INSERT INTO software(computer_id,name,version) VALUES(?,?,?)";
        String sql3 = "INSERT INTO netadapters(computer_id,type,mac,ipaddr,gateway,isdhcp) VALUES(?,?,?,?,?,?)";
        Connection conn = null;
        PreparedStatement ps1 = null;
        PreparedStatement ps2 = null;
        PreparedStatement ps3 = null;
        try {
            conn = getConnection();
            ps1 = conn.prepareStatement(sql1, Statement.RETURN_GENERATED_KEYS);
            ps1.setString(1, computer.getIpAddress());
            ps1.setString(2, computer.getName());
            ps1.setString(3, computer.getDomain());
            ps1.setString(4, computer.getOSVersion());
            ps1.setString(5, computer.getProcessor());
            ps1.setInt(6, computer.getRam_kb());
            ps1.setTimestamp(7, new Timestamp(computer.getUpdated().getTime()));
            ps1.executeUpdate();
            ResultSet rs1 = ps1.getGeneratedKeys();
            if(rs1.next()) {
                computer.setId(rs1.getInt(1));

                for(Product p : computer.getProducts()) {
                    ps2 = conn.prepareStatement(sql2);
                    ps2.setInt(1, computer.getId());
                    ps2.setString(2, p.getName());
                    ps2.setString(3, p.getVersion());
                    ps2.executeUpdate();
                }

                for(NetAdapter na : computer.getNetAdapters()) {
                    ps3 = conn.prepareStatement(sql3);
                    ps3.setInt(1, computer.getId());
                    ps3.setInt(2, na.getType());
                    ps3.setString(3, na.getMac());
                    ps3.setString(4, na.getIpaddr());
                    ps3.setString(5, na.getGateway());
                    ps3.setInt(6, na.getIsdhcp());
                    ps3.executeUpdate();
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            try {
                ps1.close();
                ps2.close();
                ps3.close();
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
                Computer computer = computersQueue.take();
                System.out.println("!!! got new computer: " + computer);
                //for(Product p : computer.getProducts()) {
                //    System.out.println(p);
                //}
                updateComputer(computer);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
