package com.industry.printer.Serial;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StreamTransport;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;

/**
 * Created by hmwan on 2019/10/23.
 */

public class SerialPort {
    private static final String TAG = SerialPort.class.getSimpleName();

    //    private native int open(String path, int baudrate);
//    private native int close(int fd);
    private native static FileDescriptor open(String path, int baudrate);
    private native static void close();
//    private native int read(int fd);        // 一定不能是static的，否则，JNI无法访问到回调函数nativeDataReceived
//    private native int read1(int fd);
//    private native int write(int fd, byte[] data, int len);
//    private native void stop();

    //    private static int mFd = -1;
    private static FileDescriptor mFd = null;
    private StreamTransport mStreamTransport = null;

    static {
        System.loadLibrary("SerialPort");
    }

    public static void loadLibrary() {
        Debug.d(TAG, "Loading SerialPort library...");
    }

/*    public void setListener(SerialPortDataReceiveListenner l) {
        mListener = l;
    }
*/

//    public int openSerial(String port) {
//        mFd = open(port, 9600);
//        return mFd;
//    }

    public void openSerial(String port) {
        mFd = open(port, 9600);
        mStreamTransport = new StreamTransport(new FileInputStream(mFd), new FileOutputStream(mFd));
    }

//    public void stopReading() {
//        stop();
//        mListener = null;
//    }

    public void closeSerial() {
        mStreamTransport.close();
        close();
    }

    public StreamTransport getStreamTransport() {
        return mStreamTransport;
    }

//    public void closeSerial() {
//        if(close(mFd) > 0) {
//            mFd = -1;
//        }
//    }

/*
    public void nativeDataReceived(byte[] data) {
        if(null != mListener) {
            Debug.d(TAG, "Recv Data :[" + ByteArrayUtils.toHexString(data) + "]");
            mListener.onDataReceived(data);
        }
    }

    public int writeSerial(byte[] value) {
        if(mFd <= 0) {
            Debug.e(TAG, "Serial port not opened.");
            return -1;
        }
        Debug.d(TAG, "Send Data :[" + ByteArrayUtils.toHexString(value) + "]");
        int ret = write(mFd, value, value.length);

        return ret;
    }
*/
//    public int writeStringSerial(String str) {
//        return writeSerial(str.getBytes());
//    }

}
