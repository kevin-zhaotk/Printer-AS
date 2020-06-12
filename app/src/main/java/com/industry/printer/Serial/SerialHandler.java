package com.industry.printer.Serial;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2019/10/26.
 */

public class SerialHandler {
    public static String TAG = SerialHandler.class.getSimpleName();

    private final String SERIAL_PORT = "/dev/ttyS4";

    private SerialPort mSerialPort = null;

    public interface OnSerialPortCommandListenner {
        public void onCommandReceived(int cmd, byte[] data);
    }

    private OnSerialPortCommandListenner mNormalCmdListeners = null;
    private OnSerialPortCommandListenner mPrintCmdListeners = null;

    private static SerialHandler mSerialHandler = null;

    public static SerialHandler getInstance() {
        if(null == mSerialHandler) {
            mSerialHandler = new SerialHandler();
            if(!mSerialHandler.isInitialized()) {
                mSerialHandler.init();
            }
        }
        return mSerialHandler;
    }

    public boolean isInitialized() {
        return (mSerialPort != null);
    }

    private void init() {
        mSerialPort = new SerialPort();
        mSerialPort.openSerial(SERIAL_PORT);
//        mSerialPort.writeStringSerial("\n\n <!-- Hello from Printer -->\n\n");
        mSerialPort.readSerial(new SerialPort.SerialPortDataReceiveListenner() {
            @Override
            public void onDataReceived(byte[] data) {
                dispatchProtocol(data);
            }
        });
    }

    public void stop() {
        if(isInitialized()) {
            mSerialPort.stopReading();
            mSerialPort.closeSerial();
            mSerialPort = null;
            mSerialHandler = null;
        }
    }

    private void dispatchProtocol(byte[] data) {
        if(!isInitialized()) return;

        ByteArrayBuffer bab = new ByteArrayBuffer(0);
        bab.append(data, 0, data.length);

        if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {
            EC_DOD_Protocol p = new EC_DOD_Protocol(mSerialPort);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3) {
            SerialProtocol3 p = new SerialProtocol3(mSerialPort);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_4) {
            XK3190_A30_Protocol p = new XK3190_A30_Protocol(mSerialPort);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_5) {
            SerialProtocol5 p = new SerialProtocol5(mSerialPort);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// H.M.Wang 2020-6-9 追加串口6协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_6) {
            SerialProtocol6 p = new SerialProtocol6(mSerialPort);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2020-6-9 追加串口6协议
        }
    }

    public void setNormalCommandListener(OnSerialPortCommandListenner l) {
        mNormalCmdListeners = l;
    }

    public void setPrintCommandListener(OnSerialPortCommandListenner l) {
        mPrintCmdListeners = l;
    }

    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        if(!isInitialized()) return;

        if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {
            EC_DOD_Protocol p = new EC_DOD_Protocol(mSerialPort);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3) {
            SerialProtocol3 p = new SerialProtocol3(mSerialPort);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_4) {
            XK3190_A30_Protocol p = new XK3190_A30_Protocol(mSerialPort);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_5) {
            SerialProtocol5 p = new SerialProtocol5(mSerialPort);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// H.M.Wang 2020-6-9 追加串口6协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_6) {
            SerialProtocol6 p = new SerialProtocol6(mSerialPort);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// End of H.M.Wang 2020-6-9 追加串口6协议
        }
    }
}
