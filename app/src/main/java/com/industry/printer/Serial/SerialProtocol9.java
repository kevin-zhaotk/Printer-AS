package com.industry.printer.Serial;

import android.content.Context;

import com.industry.printer.DataTransferThread;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StreamTransport;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2021/9/24.
 */

public class SerialProtocol9 extends SerialProtocol {
    public static String TAG = SerialProtocol9.class.getSimpleName();

/* 数据源使用串口协议9。具体内容是：
    // 接收上位机查询指令 "AA 0D 0A"，如处于待机状态（即打印状态）则返回"DD 0D 0A"，如果不是该状态则不返回或返回其他内容，选择"EE 0D 0A"返回
    // 接收到28位数字字符串 如：2110042290110005265129211009
    // 其中：Bit1-2 (21) -> DT0
    // 		Bit3-4 (10) -> DT1
    // 		Bit5-6 (04) -> DT2
    // 		Bit7-9 (229) -> DT3
    // 		Bit10 (0) -> DT4
    // 		Bit11 (1) -> 如果为0，1，2，3，则分别向DT5赋值N，L，M，H
    // 		Bit12-17 (100052) -> DT6
    // 		Bit18-28 (65129211009) -> DT7
*/

    private final static int REQURYING_CMD             = 0x81000000;   // 查询状态命令
    private final static int ERROR_INVALID_CMD         = 0x83000000;   // 不可识别的命令

    public SerialProtocol9(/*StreamTransport st*/SerialPort serialPort, Context ctx) {
        super(serialPort, ctx);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        try {
            byte[] msg = recvMsg.toByteArray();

            if(msg.length == 3 && msg[0] == (byte)0xAA && msg[1] == (byte)0x0D && msg[2] == (byte)0x0A) {
                DataTransferThread dt = DataTransferThread.mInstance;
                if(null != dt && dt.isRunning()) {
                    msg[0] = (byte)0xDD;        // 代表待机状态
                } else {
                    msg[0] = (byte)0xEE;        // 代表错误状态
                }
                super.sendCommandProcessResult(msg);
                return REQURYING_CMD;
            }
            if(msg.length >= 28) {
                return ERROR_SUCESS;
            } else {
                return ERROR_INVALID_CMD;
            }
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
        return ERROR_FAILED;
    }

    @Override
    protected byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
        ByteArrayBuffer sendBuffer = new ByteArrayBuffer(0);

        return sendBuffer.toByteArray();
    }

    @Override
    public void handleCommand(SerialHandler.OnSerialPortCommandListenner normalCmdListenner, SerialHandler.OnSerialPortCommandListenner printCmdListenner, ByteArrayBuffer bab) {
        setListeners(normalCmdListenner, printCmdListenner);

        int result = parseFrame(bab);
        byte[] recvData = bab.toByteArray();

        switch(result & ERROR_MASK) {
            case REQURYING_CMD:                // 查询命令
                Debug.e(TAG, "REQURYING_CMD");
                break;
            case ERROR_INVALID_CMD:                  // 命令格式错误
                Debug.e(TAG, "ERROR_INVALID_CMD");
                super.sendCommandProcessResult("ERROR_INVALID_CMD".getBytes());
                procError(R.string.invalid_protocol);
                break;
            case ERROR_FAILED:                      // 解析帧失败
                Debug.e(TAG, "ERROR_FAILED");
                super.sendCommandProcessResult("ERROR_FAILED".getBytes());
                procError("ERROR_FAILED");
                break;
            case ERROR_SUCESS:
                procCommands(ERROR_SUCESS, recvData);
                break;
            default:
                break;
        }
    }

    @Override
    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        byte[] retMsg = createFrame(cmd, ack, devStatus, cmdStatus, message.getBytes(Charset.forName("UTF-8")));
        super.sendCommandProcessResult(retMsg);
    }

}

