package auditServer;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.SynchronousQueue;

/**
 * Created by IntelliJ IDEA.
 * User: chaichukau
 * Date: 4/18/11
 * Time: 4:13 PM
 */
public class Server {
    public static void main(String[] args) {
        int port = 8889;
        SynchronousQueue<Computer> computersQueue = new SynchronousQueue<Computer>(true);
        SynchronousQueue<User> usersQueue = new SynchronousQueue<User>(true);

        Thread tComputerDBWriter = new Thread(new ComputerDBWriter(computersQueue));
        tComputerDBWriter.start();
        Thread tUserDBWriter = new Thread(new UserDBWriter(usersQueue));
        tUserDBWriter.start();

        try {
            ServerSocket s = new ServerSocket(port);
            while(true) {
                Socket incoming = s.accept();
                Runnable r = new ServerHandler(incoming, computersQueue, usersQueue);
                Thread t = new Thread(r);
                t.start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
