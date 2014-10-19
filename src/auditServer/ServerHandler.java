package auditServer;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import java.io.IOException;
import java.net.Socket;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.concurrent.SynchronousQueue;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/18/11
 * Time: 4:14 PM
 */
public class ServerHandler implements Runnable {
    private Socket incoming;
    private SynchronousQueue<Computer> computersQueue;
    private SynchronousQueue<User> usersQueue;

    public ServerHandler(Socket incoming, SynchronousQueue<Computer> computersQueue, SynchronousQueue<User> usersQueue) {
        this.incoming = incoming;
        this.computersQueue = computersQueue;
        this.usersQueue = usersQueue;
    }

    @Override
    public void run() {
        try {

            LinkedList<Product> products = new LinkedList<Product>();
            LinkedList<NetAdapter> netAdapters = new LinkedList<NetAdapter>();
            Computer c = null;
            User u = null;
            boolean eok = false;

//            Scanner in = new Scanner(incoming.getInputStream());
//            while(in.hasNextLine()) {
//                System.out.println(in.nextLine());
//            }

            System.out.println("Got connection from " + incoming.getInetAddress().getHostAddress());

            XMLInputFactory factory = XMLInputFactory.newInstance();
            try {
                XMLStreamReader parser = factory.createXMLStreamReader(incoming.getInputStream());
                String eName;

                while(parser.hasNext()) {
                    int event = parser.next();
                    if(event == XMLStreamConstants.START_ELEMENT) {
                        eName = parser.getLocalName();
                        if(eName.equals("computer")) {
                            c = new Computer(incoming.getInetAddress().getHostAddress(), new java.util.Date());
                            int attrCount = parser.getAttributeCount();
                            for(int i=0; i<attrCount; i++) {
                                String attrName = parser.getAttributeLocalName(i);
                                if(attrName.equals("NBName")) c.setName(parser.getAttributeValue(i));
                                else if(attrName.equals("Domain")) c.setDomain(parser.getAttributeValue(i));
                                else if(attrName.equals("OSVersion")) c.setOSVersion(parser.getAttributeValue(i));
                                else if(attrName.equals("Processor")) c.setProcessor(parser.getAttributeValue(i));
                                else if(attrName.equals("RAM")) c.setRam_kb(Integer.parseInt(parser.getAttributeValue(i)));
                            }
                            System.out.println(c);

                        } else if(eName.equals("product")) {
                            Product p = new Product(parser.getAttributeValue(0),
                                    parser.getAttributeValue(1));
                            products.add(p);
                            //System.out.println(p);
                        } else if(eName.equals("user")) {
                            u = new User();
                            u.setIpAddress(incoming.getInetAddress().getHostAddress());
                            u.setLoggedIn(new java.util.Date());
                            int attrCount = parser.getAttributeCount();
                            for(int i=0; i<attrCount; i++) {
                                String attrName = parser.getAttributeLocalName(i);
                                if(attrName.equals("NBName")) u.setComputerName(parser.getAttributeValue(i));
                                else if(attrName.equals("Domain")) u.setDomain(parser.getAttributeValue(i));
                                else if(attrName.equals("Login")) u.setLogin(parser.getAttributeValue(i));
                            }
                        } else if(eName.equals("netAdapter")) {
                            NetAdapter na = new NetAdapter();
                            int attrCount = parser.getAttributeCount();
                            for(int i=0; i<attrCount; i++) {
                                String attrName = parser.getAttributeLocalName(i);
                                if(attrName.equals("type")) na.setType(Integer.parseInt(parser.getAttributeValue(i)));
                                else if(attrName.equals("isdhcp")) na.setIsdhcp(Integer.parseInt(parser.getAttributeValue(i)));
                                else if(attrName.equals("mac")) na.setMac(parser.getAttributeValue(i));
                                else if(attrName.equals("ip")) na.setIpaddr(parser.getAttributeValue(i));
                                else if(attrName.equals("gateway")) na.setGateway(parser.getAttributeValue(i));
                            }
                            netAdapters.add(na);
                        }


                    } else if(event == XMLStreamConstants.END_DOCUMENT) {
                        eok = true;
                        //System.out.println("END_DOCUMENT");
                    }
                }
            } catch (XMLStreamException e) {
                e.printStackTrace();
            }
            incoming.close();

            if((eok)&&(c != null)&&(products.size() > 0)) {
                c.setProducts(products);
                c.setNetAdapters(netAdapters);
                System.out.println(netAdapters);
                try {
                    computersQueue.put(c);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if((eok)&&(u != null)) {
                try {
                    usersQueue.put(u);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}