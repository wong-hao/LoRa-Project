//https://blog.csdn.net/u014209205/article/details/80461122

package socketserver;

import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.Runnable;

public class Server2 {

	public static void main(String[] args) {

		try {
			// ���������socket
			ServerSocket serverSocket = new ServerSocket(1680);
			
			// �����ͻ���socket
			Socket socket = new Socket();	
			
			//ѭ�������ȴ��ͻ��˵�����
            while(true){
            	// �����ͻ���
            	socket = serverSocket.accept();
            	
            	ServerThread thread = new ServerThread(socket);
            	thread.start();
            	
            	InetAddress address=socket.getInetAddress();
                System.out.println("The IP address of Client��"+address.getHostAddress());
            }
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}

	}

}
