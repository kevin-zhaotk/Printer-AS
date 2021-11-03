package com.industry.printer.pccommand;

import android.content.Context;

import com.industry.printer.ControlTabActivity;
import com.industry.printer.Serial.SerialPort;
import com.industry.printer.Utils.StreamTransport;
import com.industry.printer.Utils.Debug;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created by hmwan on 2021/10/28.
 */

public class PCCommandManager {
    private static final String TAG = PCCommandManager.class.getSimpleName();

    private Context mContext = null;
    private ControlTabActivity mControlTabActivity = null;

    private ServerThread mServerThread = null;
    private ClientSocket mClientSocket = null;
    private PCCommandHandler mSocketHandler = null;
    private SerialPort mSerialPort = null;
    private PCCommandHandler mSerialHandler = null;

    private static PCCommandManager mInstance = null;

    public static PCCommandManager getInstance() {
        return mInstance;
    }

    public PCCommandManager(Context ctx, ControlTabActivity act) {
        mInstance = this;
        mControlTabActivity = act;
        mContext = ctx;
        mServerThread = null;
        mServerThread = new ServerThread();
        mServerThread.start();
    }

    public void close() {
        if(null != mServerThread) {
            mServerThread.stopServer();
        }
        if(null != mSocketHandler) {
            mSocketHandler.close();
        }
        if(null != mClientSocket) {
            mClientSocket.close();
        }
        if(null != mSerialHandler) {
            mSerialHandler.close();
        }
    }

    class ServerThread extends Thread {
        private static final int PORT = 3550; // port number;
        private ServerSocket mServerSocket = null; //socket service object

        public ServerThread() {
            mServerSocket = null;
        }

        public void stopServer(){
            try {
                if(null != mServerSocket){
                    interrupt();
                    mServerSocket.close();
                }
            } catch (IOException e) {
                Debug.e(TAG, e.getMessage());
            }
        }

        @Override
        public void run() {
            try {
                mServerSocket = new ServerSocket(PORT);
            } catch (IOException e) {
                Debug.e(TAG, e.getMessage());
                return;
            }

            Socket client;

            while(!isInterrupted()) {
                try {
                    client = mServerSocket.accept();
                    client.setSoTimeout(2 * 1000);
                    //client.setSoTimeout(5000);

                    if(null != mSocketHandler) {
                        mSocketHandler.close();
                    }
                    if(null != mClientSocket) {
                        mClientSocket.close();
                    }

                    mClientSocket = new ClientSocket(client, mContext);
                    mSocketHandler = new PCCommandHandler(mContext, mClientSocket.getStreamTransport(), mControlTabActivity);
                    mSocketHandler.work();
                }catch ( IOException e) {
                    Debug.e(TAG, e.getMessage());
                }
            }
        }
    }

    public void addSeriHandler(SerialPort serialPort) {
        if(null != mSerialHandler) {
            mSerialHandler.close();
        }
        if(null != mSerialPort) {
            mSerialPort.closeSerial();
        }
        mSerialPort = serialPort;
        mSerialHandler = new PCCommandHandler(mContext, mSerialPort.getStreamTransport(), mControlTabActivity);
        mSerialHandler.work();
    }

    public void sendMessage(String msg) {
        if(null != mSocketHandler) {
            mSocketHandler.sendmsg(msg);
        }
        if(null != mSerialHandler) {
            mSerialHandler.sendmsg(msg);
        }
    }
}
