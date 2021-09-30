package com.industry.printer.Serial;

import android.content.Context;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2021/9/28.
 */

/**
 * Created by hmwan on 2020/6/9.
 */

public class SerialProtocol10 extends SerialProtocol {
    public static String TAG = SerialProtocol5.class.getSimpleName();

    public SerialProtocol10(SerialPort serialPort, Context ctx){
        super(serialPort, ctx);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        if(recvMsg.length() < 12) {             // 本来定义的数据长度是36，但是只有18字以内有效，且仅使用9-12的内容，因此可以判断是否大于12就可以了
            return ERROR_FAILED;
        }

        return ERROR_SUCESS;
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
}
