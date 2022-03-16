
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.io.*;
import java.lang.Thread;

public class ChattingServer
{
    public static final int PORT = 3000;
    public List<ReceiveThread> ClientList = new ArrayList<ReceiveThread>();
    ServerSocket server = null;
    int num;
    public static void main(String[] args) 
    {
        new ChattingServer();
    }
    public ChattingServer()
    {
        num = 0;
        try
        {
            server = new ServerSocket(PORT);
            System.out.println("[Server start]");
            while(true)
            {
                Socket new_client = server.accept();
                num++;
                ReceiveThread new_client_receive = new ReceiveThread(new_client, num);
                //System.out.println("haha");
                new_client_receive.start();
                ClientList.add(new_client_receive);
                String message = "[Client " + num + " joined the chatting room]";
                System.out.println(message);
                for(ReceiveThread client : ClientList)
                {
                    new SendThread(client.sk, message);
                }                
            }
        }
        catch(IOException e)
        {
            try { server.close(); } catch(IOException e1) {}
            e.printStackTrace();
        }
        
    }
    class ReceiveThread extends Thread
    {
        static final int MESSAGE_SIZE = 1024;
        Socket sk;
        InputStream in;
        OutputStream out;
        int id;
        public ReceiveThread(Socket s,int i)
        {
            try
            {
                id = i;
                sk = s;
                in = sk.getInputStream();
                out = sk.getOutputStream();   
            }
            catch(IOException e1)
            {
                try { sk.close(); } catch(IOException e2) { }
                e1.printStackTrace();
            }
        }
        @Override
        public void run()
        {
            while(true)
                {
                    byte [] b = new byte [MESSAGE_SIZE];                    
                    try
                    {
                        in.read(b);
                        String s = new String(ModifyByteArray(b));
                        if(s.equals("quit")) 
                        { 
                            ClientList.remove(this);
                            String message = "[Client " + id + " left chatting room]";
                            System.out.println(message);
                            for(ReceiveThread client : ClientList)
                            {
                                //System.out.println("sendfinal");
                                new SendThread(client.sk, message);
                            }
                            sk.close();
                            //server.close();
                            break;
                            
                        } //cut the connection
                        else
                        {
                            String message = "[Client " + id + "] : " + s;
                            System.out.println(message);
                            for(ReceiveThread client : ClientList)
                            {
                                //System.out.println("send1");
                                new SendThread(client.sk, message);
                            }
                        }
                    }
                    catch(IOException e1)
                    {
                        try {sk.close();} catch(IOException e2) { }
                        e1.printStackTrace();
                        break;                        
                    }
                }
        }
    }

    class SendThread extends Thread
    {
        Socket sk;
        String message;
        SendThread(Socket s, String m)
        {
            sk = s;
            message = m;
            this.start();
        }
        @Override
        public void run()
        {
            try
            {
                //System.out.println("hhh");
                sk.getOutputStream().write(message.getBytes(), 0, message.getBytes().length);
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
    }
    //the byte array contains too many '0's when we create a large space for it, to convert to a string, we need to delete them
    private byte [] ModifyByteArray(byte [] b1) 
    {
        int i = 0;
        while(b1[i] != 0) i++;
        byte [] new_b = new byte[i];
        for(int j = 0; j < i; j++) new_b[j] = b1[j];
        return new_b;
    }
}