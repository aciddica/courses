package project3;
import java.net.*;
import java.io.*;
import java.util.Scanner;
import java.lang.Thread;


public class ChattingClient
{
    static final int SIZE = 1024;
    static final int PORT = 3000;
    Socket sk;
    OutputStream sk_out;
    InetAddress addr;
    boolean isrunning = false;
    public ChattingClient()
    {
  
        try
        {
            isrunning = true;
            addr = InetAddress.getByName(null);
            sk = new Socket(addr, PORT);
            new OutputThread();
        }
        catch(IOException e)   { }
        Scanner terminal_in = new Scanner(System.in);   //input string from the terminal
        while(isrunning)
        {
            String s = terminal_in.nextLine();  //read in lines            
            try
            {
                sk_out = sk.getOutputStream();
                sk_out.write(s.getBytes());                
            }
            catch(IOException e)
            {
                System.out.println("[Server failed]");  
                try { sk.close(); } catch(IOException e1) {}              
                break;
            }
            if(s.equals("quit")) isrunning = false;
        }
        terminal_in.close();
        try { sk.close(); }
        catch(IOException e) { }
    }
    public static void main(String[] args) 
    {
        new ChattingClient();
    }

    class OutputThread extends Thread
    {
        InputStream in;
        public OutputThread()
        {
            this.start();
        }
        @Override
        public void run()
        {
            try
            {
                while(isrunning)
                {
                    in = sk.getInputStream();
                    byte [] b = new byte [SIZE];
                    in.read(b);
                    System.out.println(new String(b));
                }                
            }
            catch(IOException e)
            {
                try {sk.close();} catch(IOException e1) {}
                System.out.println("[Quitted the chatting room]");
            }            
        }
    }
}
