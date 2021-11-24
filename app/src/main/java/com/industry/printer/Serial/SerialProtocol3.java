package com.industry.printer.Serial;

import android.content.Context;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StreamTransport;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2019/12/20.
 */

public class SerialProtocol3 extends SerialProtocol {
    public static String TAG = SerialProtocol3.class.getSimpleName();

    public SerialProtocol3(/*StreamTransport st*/SerialPort serialPort, Context ctx){
        super(serialPort, ctx);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        int recvCmd = ERROR_FAILED;
        try {
            for (int i=recvMsg.length()-1; i>=0; i--) {
                if (recvMsg.byteAt(i) == 0x0A) {
                    recvMsg.setLength(i);
                    recvCmd = ERROR_SUCESS;
                    break;
                }
            }
            return recvCmd;
        } catch (Exception e) {
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
}