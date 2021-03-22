//https://blog.csdn.net/u014209205/article/details/80461122

package socketserver;

import java.io.*;
import java.net.*;
import java.util.*;

public class Server1 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			byte[] bytes = new byte[1024];
			DatagramPacket packet = new DatagramPacket(bytes, bytes.length);
			
			DatagramSocket socket = new DatagramSocket(8088);
			
			socket.receive(packet);
			String receiveMsg = new String(packet.getData(),0,packet.getLength());
			System.out.println(packet.getLength());
			System.out.println(receiveMsg);
			
			socket.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
