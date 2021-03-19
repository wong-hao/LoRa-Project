//https://blog.csdn.net/qq7342272/article/details/9698413

package socketclient;

import java.io.*;
import java.net.*;

public class Client3 {
    public static void main(String[] args) {
        try {
            //1.�����ͻ���socket���ӣ�ָ��������λ�ü��˿�
            Socket socket =new Socket("localhost",8800);
            //2.�õ�socket��д��
            OutputStream os=socket.getOutputStream();
            PrintWriter pw=new PrintWriter(os);
            //������
            InputStream is=socket.getInputStream();
            BufferedReader br=new BufferedReader(new InputStreamReader(is));
            //3.����������һ���Ĳ�������socket���ж�д����
            String info="�û�����Tom,�û����룺123456";
            pw.write(info);
            pw.flush();
            socket.shutdownOutput();
            //���շ���������Ӧ
            //String reply=null;
            //while(!((reply=br.readLine())==null)){
            //    System.out.println("���շ���������Ϣ��"+reply);
            //}
            //4.�ر���Դ
            br.close();
            is.close();
            pw.close();
            os.close();
            socket.close();
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

