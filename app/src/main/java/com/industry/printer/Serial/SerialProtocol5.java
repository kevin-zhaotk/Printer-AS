package com.industry.printer.Serial;

import com.industry.printer.Utils.Debug;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2020/5/18.
 */

public class SerialProtocol5 extends SerialProtocol {
    public static String TAG = SerialProtocol5.class.getSimpleName();

//    public final static int ERROR_FAILED = 0x85000000;   // 解析帧失败

//    private SerialPort mSerialPort = null;
//    private SerialHandler.OnSerialPortCommandListenner mNormalCmdListeners = null;
//    private SerialHandler.OnSerialPortCommandListenner mPrintCmdListeners = null;

    public SerialProtocol5(SerialPort serialPort){
        super(serialPort);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        if(recvMsg.length() == 32) {
            return ERROR_SUCESS;
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
        }
    }

    @Override
    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        byte[] retMsg = createFrame(cmd, ack, devStatus, cmdStatus, message.getBytes(Charset.forName("UTF-8")));
        super.sendCommandProcessResult(retMsg);
    }
}
