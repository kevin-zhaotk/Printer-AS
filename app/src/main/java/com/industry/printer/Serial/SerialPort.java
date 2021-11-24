package com.industry.printer.Serial;

import com.industry.printer.Utils.ByteArrayUtils;
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

    private native static FileDescriptor openStream(String path, int baudrate);
    private native static void closeStream(FileDescriptor fd);

    private native int openSerial(String path, int baudrate);
    private native int closeSerial(int fd);
    private native int read(int fd);        // 一定不能是static的，否则，JNI无法访问到回调函数nativeDataReceived
    private native int write(int fd, byte[] data, int len);
    private native void stop();

    // H.M.Wang 2021-11-24 恢复原来的串口功能。原来的串口支持二进制传送，无通讯的结束符标志，使用Stream不可行。因此，PC命令的使用Stream，原有的串口通讯还是按着原来的二进制传送，靠超时判断传送结束
    private static int mFdId = -1;                      // 原有的文件描述符
    private static FileDescriptor mFdStream = null;     // Stream用的文件描述符
    private StreamTransport mStreamTransport = null;

    static {
        System.loadLibrary("SerialPort");
    }

    public static void loadLibrary() {
        Debug.d(TAG, "Loading SerialPort library...");
    }

    public interface SerialPortDataReceiveListenner {
        public void onDataReceived(byte[] data);
    }
    private SerialPortDataReceiveListenner mListener;

    public void setListener(SerialPortDataReceiveListenner l) {
        mListener = l;
    }

    public int openSerial(String port) {
        mFdId = openSerial(port, 9600);
        return mFdId;
    }

    public StreamTransport openStream(String port) {
        mFdStream = openStream(port, 9600);
        mStreamTransport = new StreamTransport(new FileInputStream(mFdStream), new FileOutputStream(mFdStream));
        return mStreamTransport;
    }

    public void stopReading() {
        stop();
        mListener = null;
    }

    public void closeStream() {
        mStreamTransport.close();
        closeStream(mFdStream);
    }

    public StreamTransport getStreamTransport() {
        return mStreamTransport;
    }

    public void closeSerial() {
        if(closeSerial(mFdId) > 0) {
            mFdId = -1;
        }
    }

    public void nativeDataReceived(byte[] data) {
        if(null != mListener) {
            Debug.d(TAG, "Recv Data :[" + ByteArrayUtils.toHexString(data) + "]");
            mListener.onDataReceived(data);
        }
    }

    public void readSerial(SerialPortDataReceiveListenner l) {
        if(mFdId <= 0) {
            Debug.e(TAG, "Serial port not opened.");
            return;
        }
        mListener = l;
        new Thread() {
            @Override
            public void run() {
                read(mFdId);
            }
        }.start();
    }

    public int writeSerial(byte[] value) {
        if(mFdId <= 0) {
            Debug.e(TAG, "Serial port not opened.");
            return -1;
        }
        Debug.d(TAG, "Send Data :[" + ByteArrayUtils.toHexString(value) + "]");
        int ret = write(mFdId, value, value.length);

        return ret;
    }

    public int writeStringSerial(String str) {
        return writeSerial(str.getBytes());
    }
}
