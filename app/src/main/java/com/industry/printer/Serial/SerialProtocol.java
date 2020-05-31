package com.industry.printer.Serial;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2020/5/18.
 */

abstract public class SerialProtocol {
    protected final static int ERROR_MASK               = 0xffff0000;
    protected final static int CMD_MASK                 = 0x0000ffff;

    public final static int ERROR_SUCESS                = 0x00000000;   // 无错误
    public final static int ERROR_FAILED                = 0x85000000;   // 解析帧失败

    protected SerialPort mSerialPort = null;
    protected SerialHandler.OnSerialPortCommandListenner mNormalCmdListeners = null;
    protected SerialHandler.OnSerialPortCommandListenner mPrintCmdListeners = null;

    protected SerialProtocol(SerialPort serialPort){
        mSerialPort = serialPort;
    }

    abstract protected int parseFrame(ByteArrayBuffer recvMsg);
    abstract protected byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg);
    abstract protected void handleCommand(SerialHandler.OnSerialPortCommandListenner normalCmdListenner, SerialHandler.OnSerialPortCommandListenner printCmdListenner, ByteArrayBuffer bab);
    abstract protected void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message);

    protected void setListeners(SerialHandler.OnSerialPortCommandListenner normalCmdListenner, SerialHandler.OnSerialPortCommandListenner printCmdListenner) {
        mNormalCmdListeners = normalCmdListenner;
        mPrintCmdListeners = printCmdListenner;
    }

    protected void procCommands(int cmd, byte[] data) {
        if(null != mNormalCmdListeners) {
            mNormalCmdListeners.onCommandReceived(cmd, data);
        }
        if(null != mPrintCmdListeners) {
            mPrintCmdListeners.onCommandReceived(cmd, data);
        }
    }

    protected void sendCommandProcessResult(byte[] message) {
        if(null != mSerialPort && null != message) {
            mSerialPort.writeSerial(message);
        }
    }
}
