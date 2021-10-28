package com.industry.printer.pccommand;

import android.content.Context;

import com.industry.printer.ControlTabActivity;
import com.industry.printer.Utils.Debug;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by hmwan on 2021/10/28.
 */

public class PCCommandManager {
    private static final String TAG = PCCommandManager.class.getSimpleName();

    private Context mContext = null;
    private ServerThread mServerThread = null;

    public PCCommandManager(Context ctx) {
        mContext = ctx;
        mServerThread = null;
        mServerThread = new ServerThread();
        mServerThread.start();
    }

    public void stop() {
        if(null != mServerThread) {
            mServerThread.stopServer();
        }
    }

    class ServerThread extends Thread {
        private static final int PORT = 3550; // port number;
        private ServerSocket mServerSocket = null; //socket service object
        private ExecutorService mExecutor = null; //hnadle ExecutorService
        private ClientSocketThread mClientSocketThread = null;

        public ServerThread() {
            mServerSocket = null;
            mExecutor = Executors.newCachedThreadPool();
            mClientSocketThread = null;
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

                    if(null != mClientSocketThread) {
                        mClientSocketThread.stop();
                    }

                    mClientSocketThread = new ClientSocketThread(client, mContext);
                    mExecutor.execute(mClientSocketThread);
                }catch ( IOException e) {
                    Debug.e(TAG, e.getMessage());
                }
            }
        }
    }
}
