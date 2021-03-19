//https://blog.csdn.net/u014209205/article/details/80461122

package socketclient;

import java.io.*;
import java.net.*;

public class Client1 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
            String sendMsg = "CNMD";
            
            InetAddress addr = InetAddress.getByName("localhost");
            
            DatagramPacket packet = new DatagramPacket(sendMsg.getBytes(),
            		sendMsg.getBytes().length, addr, 8088); 
            
            DatagramSocket socket = new DatagramSocket();
            
            socket.send(packet);
 
            socket.close();
            
        } catch (Exception e) {
            e.printStackTrace();
        }
	}

}
