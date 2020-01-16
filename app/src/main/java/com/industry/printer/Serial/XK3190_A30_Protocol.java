package com.industry.printer.Serial;

import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;

import org.apache.http.util.ByteArrayBuffer;

/**
 * Created by hmwan on 2020/1/13.
 */

public class XK3190_A30_Protocol {
    public static String TAG = XK3190_A30_Protocol.class.getSimpleName();

    /*
        发送方帧格式：
                -------------------------------------------------------------
                [STX] 				帧起始标志0x02	1字节	    固定位置
                [FLAG] 				符号			    1字节 	    固定位置
                [VALUE] 			数据    		    6字节	    固定位置
                [DOT]      	        小数点位置  		1字节	    固定位置
                [CHKSUM]   		    帧校验   		2字节	    固定位置
                [ETX]           	帧终止标志0x03	1字节	    固定位置
                  -------------------------------------------------------------
    */

    // 帧头标志
    private final byte TAG_STX = 0x02;
    // 帧尾标志
    private final byte TAG_ETX = 0x03;

    private final int TAG_START_POS             = 0;
    private final int TAG_FLAG_POS              = 1;
    private final int TAG_DATA_POS              = 2;
    private final int TAG_DOT_POS               = 8;
    private final int TAG_CHECKSUM_POS          = 9;
    private final int TAG_END_POS               = 11;

    private final int FIXED_FRAME_SIZE          = TAG_END_POS - TAG_START_POS + 1;

    public final static int ERROR_SUCESS              = 0x00000000;   // 无错误
    public final static int ERROR_INVALID_STX         = 0x81000000;   // 起始标识错误
    public final static int ERROR_INVALID_ETX         = 0x82000000;   // 终止标识错误
    public final static int ERROR_INVALID_CMD         = 0x83000000;   // 不可识别的命令
    public final static int ERROR_CHECK_FAILED        = 0x84000000;   // 校验失败
    public final static int ERROR_FAILED              = 0x85000000;   // 解析帧失败

    public int parseFrame(ByteArrayBuffer recvMsg) {
        int recvCmd = 0;

        try {
            byte[] msg = recvMsg.toByteArray();

            if(msg.length != FIXED_FRAME_SIZE) {
                return ERROR_INVALID_CMD;
            }

            if(msg[TAG_START_POS] != TAG_STX) {
                return ERROR_INVALID_STX;
            }

            if(msg[TAG_END_POS] != TAG_ETX) {
                return ERROR_INVALID_ETX;
            }

// 0x00 ^ 0x31 ^ 0x32 ^ 0x33 ^ 0x34 ^ 0x35 ^ 0x36 ^ 0x33 = 0x34
// + : 0x2B; - : 0x2D

// 暂时取消CRC验证
            int recvCheck = (msg[TAG_CHECKSUM_POS] > '9' ? msg[TAG_CHECKSUM_POS] - 'A' + 10 : msg[TAG_CHECKSUM_POS] - '0');
            recvCheck <<= 4;
            recvCheck |= (msg[TAG_CHECKSUM_POS+1] > '9' ? msg[TAG_CHECKSUM_POS+1] - 'A' + 10 : msg[TAG_CHECKSUM_POS+1] - '0');

            int calCheck = 0;
            calCheck ^= (0x00ff & msg[1]);
            calCheck ^= (0x00ff & msg[2]);
            calCheck ^= (0x00ff & msg[3]);
            calCheck ^= (0x00ff & msg[4]);
            calCheck ^= (0x00ff & msg[5]);
            calCheck ^= (0x00ff & msg[6]);
            calCheck ^= (0x00ff & msg[7]);
            calCheck ^= (0x00ff & msg[8]);

            Debug.d(TAG, "calCheck = " + Integer.toHexString((0x0000ffff & calCheck)));
            Debug.d(TAG, "recvCheck = " + Integer.toHexString((0x0000ffff & recvCheck)));

            if(calCheck != recvCheck) {
                return ERROR_CHECK_FAILED;
            }

            recvCmd = (msg[TAG_DATA_POS] - 0x30);
            recvCmd *= 10;
            recvCmd += (msg[TAG_DATA_POS+1] - 0x30);
            recvCmd *= 10;
            recvCmd += (msg[TAG_DATA_POS+2] - 0x30);
            recvCmd *= 10;
            recvCmd += (msg[TAG_DATA_POS+3] - 0x30);
            recvCmd *= 10;
            recvCmd += (msg[TAG_DATA_POS+4] - 0x30);
            recvCmd *= 10;
            recvCmd += (msg[TAG_DATA_POS+5] - 0x30);

            for(int i=0; i<(msg[TAG_DOT_POS] - 0x30); i++) {
                recvCmd /= 10;
            }

// 暂时不考虑符号
//            recvCmd = (msg[TAG_FLAG_POS] > 0 ? recvCmd : -recvCmd);

            return recvCmd;
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
        return ERROR_FAILED;
    }

    public byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
        return msg;
    }

    public String createErrorMessage(int error) {
        String errStr = "";

        switch(error) {
            case ERROR_INVALID_STX:
                errStr = "ERROR_INVALID_STX";
                break;
            case ERROR_INVALID_ETX:
                errStr = "ERROR_INVALID_ETX";
                break;
            case ERROR_INVALID_CMD:
                errStr = "ERROR_INVALID_CMD";
                break;
            case ERROR_CHECK_FAILED:
                errStr = "ERROR_CHECK_FAILED";
                break;
            case ERROR_FAILED:
                errStr = "ERROR_FAILED";
                break;
        }
        return errStr;
    }
}
