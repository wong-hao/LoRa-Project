package socketclient;

//https://blog.csdn.net/u014209205/article/details/80461122

import java.io.*;
import java.net.*;

public class Client2 {

	public static void main(String[] args) {

		try {
			// �ͷ�������������
			Socket socket = new Socket("localhost",8088);
			
			// Ҫ���͸�����������Ϣ
			OutputStream os = socket.getOutputStream();
			PrintWriter pw = new PrintWriter(os);
			pw.write("�ͻ��˷�����Ϣ");
			pw.flush();
			
			socket.shutdownOutput();
			
			// �ӷ��������յ���Ϣ
			InputStream is = socket.getInputStream();
			BufferedReader br = new BufferedReader(new InputStreamReader(is));
			String info = null;
			while((info = br.readLine())!=null){
				System.out.println("���ǿͻ��ˣ�������������Ϣ��"+info);
			}
			
			br.close();
			is.close();
			os.close();
			pw.close();
			socket.close();
		} catch (Exception e) {
			e.printStackTrace();

	}
	}
}
