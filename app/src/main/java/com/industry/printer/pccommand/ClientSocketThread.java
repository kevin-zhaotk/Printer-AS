package com.industry.printer.pccommand;

import android.content.Context;
import android.os.Bundle;
import android.os.Message;
import android.os.SystemClock;

import com.industry.printer.Constants.Constants;
import com.industry.printer.DataTransferThread;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.R;
import com.industry.printer.Socket_Server.PCCommand;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.DataTask;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.hardware.RTCDevice;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Calendar;

/**
 * Created by hmwan on 2021/10/28.
 */

public class ClientSocketThread implements Runnable {
    private static final String TAG = ClientSocketThread.class.getSimpleName();

    private Context mContext = null;
    private Socket mClientSocket = null;
    private BufferedReader mBufferedReader = null;
    private PrintWriter mPrintWriter = null;
    private PCCommandHandler mPCCommandHandler = null;

    private boolean mRunning = false;

    public ClientSocketThread(Socket socket, Context ctx) {
        mClientSocket = socket;
        mContext = ctx;
        try {
            mBufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            mPrintWriter = new PrintWriter(new BufferedWriter(new OutputStreamWriter(mClientSocket.getOutputStream())), true);
            mPCCommandHandler = new PCCommandHandler(ctx);
            sendmsg("connected success!!!");
        } catch (IOException e) {
            Debug.e(TAG, e.getMessage());
        }
    }

    public void stop() {
        mRunning = false;
        try {
            if(null != mClientSocket){
                mClientSocket.close();
            }
        } catch (IOException e) {
            Debug.e(TAG, e.getMessage());
        }
    }

    public void sendmsg(String msg) {
        Debug.d(TAG, "--->send: " + msg);
        if(null != mPrintWriter) mPrintWriter.println(msg);
    }

    @Override
    public void run() {
        String msg = "";

        while(mRunning) {
            try {
                if((msg = mBufferedReader.readLine()) != null) {
                    Debug.d(TAG, "--->fromPc: " + msg);
                    mPCCommandHandler.handle(msg, new PCCommandHandler.HandleMessageReceiver() {
                        @Override
                        public void handledMessage(String msg) {
                            sendmsg(msg);
                        }
                    });
                }
            } catch (Exception e) {
                Debug.e(TAG, e.getMessage());
                mRunning = false;
                sendmsg(Constants.pcErr(msg + e.getMessage()));
            }
        }

        Debug.d(TAG, "--->thread " + Thread.currentThread().getName() + " stop");
    }
}
