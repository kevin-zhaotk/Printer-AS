package com.industry.printer.Socket_Server;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketTimeoutException;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class ClientThread implements Runnable {
    private Socket s;
    private Socket [] s_array=new Socket[20];
    private Handler handler;
    public Handler revHandler;
    BufferedReader br = null;
    OutputStream os = null;
   
    String ip;
    String port;
    public ClientThread(Handler handler,String ip,String port) {
        this.handler = handler;
        this.ip=ip;
        this.port=port;
    }

    public void run() {
        try {
            s = new Socket( this.ip, Integer.parseInt(this.port));//参数1为主机ip
            s_array[0]=s;
            br = new BufferedReader(new InputStreamReader(s.getInputStream()));
            os = s_array[0].getOutputStream();
            new Thread() {
                public void run() {
                    String content = null;
                    try {
                        while ((content = br.readLine()) != null) {
                            Message msg = new Message();
                            if("connected success".equals(content))
                            {
                            msg.what = 1;
                            }
                            else
                            {
                             msg.what = 2;	
                             s.close();
                            }
                            msg.obj = content;
                            handler.sendMessage(msg);
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                };
            }.start();
            Looper.prepare();
            revHandler = new Handler() {
                public void handleMessage(Message msg) {
                    if (msg.what == 0x345) {
                        try {
                            os.write((msg.obj.toString() + "\r\n")
                                    .getBytes("utf-8"));
                          
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                };
            };
            Looper.loop();
        } catch (SocketTimeoutException e) {
            System.out.println("网络连接超时!");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}