package com.industry.printer.Serial;

import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;

import org.apache.http.util.ByteArrayBuffer;

/**
 * Created by hmwan on 2019/12/20.
 */

public class SerialProtocol3 {
    public static String TAG = SerialProtocol3.class.getSimpleName();

    public final static int ERROR_FAILED = 0x85000000;   // 解析帧失败

    public final static int CMD_DUMMY = 0x0000;       // 无命令

    public int parseFrame(ByteArrayBuffer recvMsg) {
        int recvCmd = ERROR_FAILED;
        try {
            for (int i=recvMsg.length()-1; i>=0; i--) {
                if (recvMsg.byteAt(i) == 0x0A) {
                    recvMsg.setLength(i);
                    recvCmd = CMD_DUMMY;
                    break;
                }
            }
            return recvCmd;
        } catch (Exception e) {
            Debug.e(TAG, e.getMessage());
        }
        return ERROR_FAILED;
    }

    public byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
        return msg;
    }
}