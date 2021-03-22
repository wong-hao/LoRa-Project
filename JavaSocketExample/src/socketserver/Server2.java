//https://blog.csdn.net/u014209205/article/details/80461122

package socketserver;

import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.Runnable;

public class Server2 {

	public static void main(String[] args) {

		try {
			// 创建服务端socket
			ServerSocket serverSocket = new ServerSocket(1680);
			
			// 创建客户端socket
			Socket socket = new Socket();	
			
			//循环监听等待客户端的连接
            while(true){
            	// 监听客户端
            	socket = serverSocket.accept();
            	
            	ServerThread thread = new ServerThread(socket);
            	thread.start();
            	
            	InetAddress address=socket.getInetAddress();
                System.out.println("The IP address of Client："+address.getHostAddress());
            }
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}

	}

}
