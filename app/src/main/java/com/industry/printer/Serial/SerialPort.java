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
    private native byte[] read(int fd);
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
    }

    public void closeStream() {
        mStreamTransport.close();
        closeStream(mFdStream);
    }

    public StreamTransport getStreamTransport() {
        return mStreamTransport;
    }

    public void closeSerial() {
        closeSerial(mFdId);
    }

    public byte[] readSerial() {
        if(mFdId <= 0) {
            Debug.e(TAG, "Serial port not opened.");
            return null;
        }

        // JNI的read函数，如果没有数据接收则一直等待，如果有数据接收，在连续接收之后，出现100ms的区间没有数据接收的话，则视为接收结束，返回结果给data。单次最大接收1024字节
        byte[] data = read(mFdId);
        Debug.d(TAG, "Recv Data :[" + ByteArrayUtils.toHexString(data) + "]");
        return data;
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
