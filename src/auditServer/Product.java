package auditServer;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/19/11
 * Time: 11:26 AM
 * To change this template use File | Settings | File Templates.
 */
public class Product {
    private String name;
    private String version;

    public Product(String name, String version) {
            this.name = name;
            this.version = version;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    @Override
    public String toString() {
        if((version == null)||(version.isEmpty())) return name;
        else return (name + " (" + version + ")");
    }
}
