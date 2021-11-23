package com.industry.printer.pccommand;

import android.content.Context;

import com.industry.printer.Utils.StreamTransport;
import com.industry.printer.Utils.Debug;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by hmwan on 2021/10/30.
 */

public class ClientSocket {
    private static final String TAG = ClientSocket.class.getSimpleName();

    private Context mContext = null;
    private Socket mSocket = null;
    //    private BufferedReader mBufferedReader = null;
//    private PrintWriter mPrintWriter = null;
    private StreamTransport mStreamTransport = null;

    public ClientSocket(Socket socket, Context ctx) {
        mSocket = socket;
        mContext = ctx;
        try {
            mStreamTransport = new StreamTransport(socket.getInputStream(), socket.getOutputStream());
            mStreamTransport.writeLine("connected success!!!");
        } catch (IOException e) {
            Debug.e(TAG, e.getMessage());
        }
/*
        Timer timer = new Timer();
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                Debug.d(TAG, "isConnected: " + mSocket.isConnected());
                Debug.d(TAG, "isClosed: " + mSocket.isClosed());
                Debug.d(TAG, "isInputShutdown: " + mSocket.isInputShutdown());
            }
        }, 0, 1000L);
*/
    }

    public void close() {
        try {
            mStreamTransport.close();
            if(null != mSocket){
                mSocket.close();
            }
        } catch (IOException e) {
            Debug.e(TAG, e.getMessage());
        }
    }

    public StreamTransport getStreamTransport() {
        return mStreamTransport;
    }
}
