//https://blog.csdn.net/qq7342272/article/details/9698413

package socketserver;

import java.io.*;
import java.net.*;
import java.util.*;

public class Server3 {
    public static void main(String[] args) {
        try {
            //1.����һ��������Socket(ServerSocket)��ָ���˿�
            ServerSocket serverSocket=new ServerSocket(8800);
            //2.ʹ��accept()������ֹ�ȴ����������������
            Socket socket=serverSocket.accept();
            //3.���������
            InputStream is=socket.getInputStream();
            BufferedReader br=new BufferedReader(new InputStreamReader(is));
            //��������
            OutputStream os=socket.getOutputStream();
            PrintWriter pw=new PrintWriter(os);
            //4.��ȡ�û�������Ϣ
            String info=null;
            while(!((info=br.readLine())==null)){
                System.out.println("���Ƿ��������û���ϢΪ��"+info);
            }
            //���ͻ�һ����Ӧ
            //String reply="welcome";
            //pw.write(reply);
            //pw.flush();
            //5.�ر���Դ
            pw.close();
            os.close();
            br.close();
            is.close();
            socket.close();
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }    
    }
}
