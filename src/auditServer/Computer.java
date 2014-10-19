package auditServer;

import java.util.Date;
import java.util.LinkedList;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/19/11
 * Time: 11:28 AM
 */
public class Computer {
    private int id;
    private String name;
    private String domain;
    private String OSVersion;
    private String processor;
    private String ipAddress;
    private int ram_kb;
    private Date updated;
    private LinkedList<Product> products;
    private LinkedList<NetAdapter> netAdapters;

    public Computer(String ipAddress, Date updated) {
        this.updated = updated;
        this.ipAddress = ipAddress;
    }

    public Computer(String ipAddress, String name, String domain, String OSVersion, String processor, Date updated) {
        this.ipAddress = ipAddress;
        this.name = name;
        this.domain = domain;
        this.OSVersion = OSVersion;
        this.processor = processor;
        this.updated = updated;
    }

    public int getRam_kb() {
        return ram_kb;
    }

    public void setRam_kb(int ram_kb) {
        this.ram_kb = ram_kb;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public Date getUpdated() {
        return updated;
    }

    public void setUpdated(Date updated) {
        this.updated = updated;
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public void setIpAddress(String ipAddress) {
        this.ipAddress = ipAddress;
    }

    public LinkedList<Product> getProducts() {
        return products;
    }

    public void setProducts(LinkedList<Product> products) {
        this.products = products;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDomain() {
        return domain;
    }

    public void setDomain(String domain) {
        this.domain = domain;
    }

    public String getOSVersion() {
        return OSVersion;
    }

    public void setOSVersion(String OSVersion) {
        this.OSVersion = OSVersion;
    }

    public String getProcessor() {
        return processor;
    }

    public void setProcessor(String processor) {
        this.processor = processor;
    }

    public LinkedList<NetAdapter> getNetAdapters() {
        return netAdapters;
    }

    public void setNetAdapters(LinkedList<NetAdapter> netAdapters) {
        this.netAdapters = netAdapters;
    }

    @Override
    public String toString() {
        return "Computer{" +
                "name='" + name + '\'' +
                ", domain='" + domain + '\'' +
                ", ipAddress='" + ipAddress + '\'' +
                '}';
    }
}
