package com.industry.printer.Serial;

import android.content.Context;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StreamTransport;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2022/4/5.
 */

public class SerialProtocol11 extends SerialProtocol {
    public static String TAG = SerialProtocol5.class.getSimpleName();

    public SerialProtocol11(/*StreamTransport st*/SerialPort serialPort, Context ctx){
        super(serialPort, ctx);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        try {
            byte[] msg = recvMsg.toByteArray();

            if(msg.length < 7) {    // 帧长度异常
                return ERROR_FAILED;
            }

            if(msg[0] != (byte)0x1B || msg[1] != (byte)0x53) {              // 帧头错误
                return ERROR_FAILED;
            }

            if(msg[2] != (byte)0x31 || msg[3] != (byte)0x31) {              // 功能码错误
                return ERROR_FAILED;
            }

            if(msg[msg.length-2] != (byte)0x0D || msg[msg.length-1] != (byte)0x0A) {    // 帧尾错误
                return ERROR_FAILED;
            }

            return ERROR_SUCESS;                        // 协议格式正确
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
        return ERROR_FAILED;
    }

    @Override
    protected byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
        return msg;
    }

    @Override
    public void handleCommand(SerialHandler.OnSerialPortCommandListenner normalCmdListenner, SerialHandler.OnSerialPortCommandListenner printCmdListenner, ByteArrayBuffer bab) {
        setListeners(normalCmdListenner, printCmdListenner);

        int result = parseFrame(bab);
        if (result == ERROR_SUCESS) {
            byte[] recvData = bab.toByteArray();
            procCommands(result, recvData);
        } else {
            procError("ERROR_FAILED");
        }
    }

    @Override
    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        byte[] retMsg = createFrame(cmd, ack, devStatus, cmdStatus, message.getBytes(Charset.forName("UTF-8")));
        super.sendCommandProcessResult(retMsg);
    }

    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
        byte[] retMsg = createFrame(cmd, ack, devStatus, cmdStatus, msg);
        super.sendCommandProcessResult(retMsg);
    }

}
