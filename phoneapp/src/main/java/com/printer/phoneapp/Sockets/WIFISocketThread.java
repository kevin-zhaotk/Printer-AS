package com.printer.phoneapp.Sockets;

import com.printer.phoneapp.Devices.ConnectDevice;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.charset.Charset;

/**
 * Created by hmwan on 2021/9/7.
 */

public class WIFISocketThread extends SocketThread {
    public WIFISocketThread(ConnectDevice dev) {
        super(dev);
    }

    @Override
    public void sendString(final String msg, final OnSocketStringListener l) {
        mSocketThread.execute(new Runnable() {
            @Override
            public void run() {
                Socket socket = null;
                OutputStream os = null;
//                DataInputStream dis = null;
//                byte[] recv = new byte[8192];
                BufferedReader br = null;
                try {
//                        Socket socket = new Socket(dev.getAddress(), 3550);
//                        socket.setSoTimeout(10000);
                    socket = new Socket();
                    SocketAddress socAddress = new InetSocketAddress(mDevice.getAddress(), 3550);
                    socket.connect(socAddress, 5000);

                    br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    br.readLine();  // connected success!!!

                    os = socket.getOutputStream();
                    os.write(msg.getBytes(Charset.forName("UTF-8")));
                    os.write(new byte[]{0x0A});
                    os.flush();
                    if (null != l) l.onSent(mDevice, msg);
//                        mSocket.setSoTimeout(5000);

//                    dis = new DataInputStream(socket.getInputStream());
//                    int length = dis.read(recv);
//                    if (null != l) l.onReceived(mDevice, recv, length);
                    String is = br.readLine();
                    if (null != l) l.onReceived(mDevice, is);
                } catch (Exception e) {
                    if(null != l) l.onFailed(mDevice, e.getMessage());
                } finally {
                    try {
                        if(null != os) {
                            os.close();
                        }
                        if(null != br) {
                            br.close();
                        }
                        if(null != socket) {
                            socket.close();
                        }
                    } catch (Exception e) {
                    }
                }
            }
        });
    }
}
