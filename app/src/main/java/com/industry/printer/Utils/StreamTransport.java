package com.industry.printer.Utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.SocketTimeoutException;
import java.nio.charset.Charset;
import java.util.concurrent.TimeoutException;

/**
 * Created by hmwan on 2021/10/30.
 */

public class StreamTransport {
    private static final String TAG = StreamTransport.class.getSimpleName();

    private InputStream mFileInputStream = null;
    private OutputStream mFileOutputStream = null;

    public StreamTransport(InputStream is, OutputStream os) {
        mFileInputStream = is;
        mFileOutputStream = os;
    }

    public void close() {
        try {
            if(null != mFileInputStream) {
                mFileInputStream.close();
                mFileInputStream = null;
                Debug.d(TAG, "InputStream closed");
            }
            if(null != mFileOutputStream) {
                mFileOutputStream.close();
                mFileOutputStream = null;
                Debug.d(TAG, "OutputStream closed");
            }
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
    }

    public void write(byte[] buffer, int offset, int count) {
        try {
            mFileOutputStream.write(buffer, offset, count);
            mFileOutputStream.flush();
            Debug.d(TAG, "Send Data :[" + ByteArrayUtils.toHexString(buffer, offset, count) + "](" + count + " bytes)");
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
    }

    public void write(byte[] buffer) {
        write(buffer, 0, buffer.length);
    }

// H.M.Wang 2021-11-23 发送字符串的时候，自动加上一个换行符，以便对方接收，对方可能是在readLine。函数名从write(String)改为writeLine(String)
    public void writeLine(String msg) {
        try {
            Debug.d(TAG, "Send Data :[" + msg + "]");
// 使用BufferedWriter
//            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(mFileOutputStream, Charset.forName("UTF-8")));
//            bw.write(msg + "\n");
// 使用PrintWriter
//            PrintWriter pw = new PrintWriter(new BufferedWriter(new OutputStreamWriter(mFileOutputStream, Charset.forName("UTF-8"))));
//            pw.println(msg);
// 直接使用OutputStream
            write((msg + "\n").getBytes(Charset.forName("UTF-8")));
        } catch(Exception e) {
            Debug.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }
// End of H.M.Wang 2021-11-23 发送字符串的时候，自动加上一个换行符，以便对方接收，对方可能是在readLine

    public String readLine() {
        String line = null;
        // br.readLine()的返回值：
        // 当直接收到0x0A或、和0x0D时，返回空字符串；
        // 当超时时，按接收到空字符串处理
        // 当发生异常的时候，返回null，将根据这个信息上层决定关闭该链接
        // 当连接关闭时，返回null，将根据这个信息上层决定关闭该链接
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(mFileInputStream, Charset.forName("UTF-8")));
            line = br.readLine();
            Debug.d(TAG, "Recv Data :[" + line + "]");
        } catch(SocketTimeoutException e) {
            line = "";
        } catch(Exception e) {
            line = null;
            Debug.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
        return line;
    }

    public int read(byte[] buffer, int offset, int count) {
        int recv = 0;
        try {
            while(recv < count) {
                int ret = mFileInputStream.read(buffer, offset + recv, count - recv);
                if(ret == -1) break;
                recv += ret;
            }
            Debug.d(TAG, "Recv Data :[" + ByteArrayUtils.toHexString(buffer, offset, recv) + "](" + recv + " bytes)");
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
        return recv;
    }

    public int read(byte[] buffer) {
        return read(buffer, 0, buffer.length);
    }

    public InputStream getInputStream() {
        return mFileInputStream;
    }

    public OutputStream getOutputStream() {
        return mFileOutputStream;
    }
}
