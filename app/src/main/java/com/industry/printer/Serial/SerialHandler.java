package com.industry.printer.Serial;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Debug;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2019/10/26.
 */

public class SerialHandler {
    public static String TAG = SerialHandler.class.getSimpleName();

    private final String SERIAL_PORT = "/dev/ttyS4";

    private final int ERROR_MASK = 0xffff0000;
    private final int CMD_MASK = 0x0000ffff;

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
//        if(!isInitialized()) return;

        ByteArrayBuffer bab = new ByteArrayBuffer(0);
        bab.append(data, 0, data.length);

        if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {

            EC_DOD_Protocol p = new EC_DOD_Protocol();
            int result = p.parseFrame(bab);
            byte[] recvData = bab.toByteArray();

            switch(result & ERROR_MASK) {
                case EC_DOD_Protocol.ERROR_INVALID_STX:                // 起始标识错误
                    sendCommandProcessResult(0, 1, 0, 1, "");
//                mSerialPort.writeStringSerial( "<!-- Invalid STX -->");
                    break;
                case EC_DOD_Protocol.ERROR_INVALID_ETX:                // 终止标识错误
                    sendCommandProcessResult(0, 1, 0, 1, "");
//                mSerialPort.writeStringSerial( "<!-- Invalid ETX -->");
                    break;
                case EC_DOD_Protocol.ERROR_CRC_FAILED:                 // CRC校验失败
                    sendCommandProcessResult(result & CMD_MASK, 1, 0, 1, "");
//                mSerialPort.writeStringSerial("<!-- CRC Failed -->");
                    break;
                case EC_DOD_Protocol.ERROR_UNKNOWN_CMD:                     // 解析帧失败
                    sendCommandProcessResult(0, 1, 0, 1, "");
//                mSerialPort.writeStringSerial("<!-- Unknown Command -->");
                    break;
                case EC_DOD_Protocol.ERROR_FAILED:                     // 解析帧失败
//                mSerialPort.writeStringSerial("<!-- Failed -->");
//                break;
                case EC_DOD_Protocol.ERROR_SUCESS:
                    dispatchCommands(result & CMD_MASK, recvData);
                    break;
                default:
                    sendCommandProcessResult(0, 1, 0, 1, "");
//                mSerialPort.writeStringSerial("<!-- Failed -->");
                    break;
            }
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3) {
            SerialProtocol3 p = new SerialProtocol3();
            int result = p.parseFrame(bab);
            if(result == SerialProtocol3.CMD_DUMMY) {
                byte[] recvData = bab.toByteArray();

                procCommands(result, recvData);
            }
        }
    }

    private void dispatchCommands(int cmd, byte[] data) {
//        if(!isInitialized()) return;

        switch (cmd) {
            case EC_DOD_Protocol.CMD_SET_NOZZLE_HEIGHT:            // 设定喷头喷印高度	0x0002
            case EC_DOD_Protocol.CMD_GET_NOZZLE_HEIGHT:            // 读取喷头喷印高度	0x0003
            case EC_DOD_Protocol.CMD_SET_DOT_INTERVAL:             // 设定喷头喷印点距	0x0004
            case EC_DOD_Protocol.CMD_GET_DOT_INTERVAL:             // 读取喷头喷印点距	0x0005
            case EC_DOD_Protocol.CMD_SET_DROP_SIZE:                // 设定喷头喷印墨滴大小	0x0006
            case EC_DOD_Protocol.CMD_GET_DROP_SIZE:                // 读取喷头墨滴大小	0x0007
            case EC_DOD_Protocol.CMD_GET_PRINT_DELAY:              // 读取喷头喷印延时	0x0009
            case EC_DOD_Protocol.CMD_GET_MOVE_SPEED:               // 读取物体移动速度	0x000b
            case EC_DOD_Protocol.CMD_SET_EYE_STATUS:               // 设定喷头电眼状态	0x000c
            case EC_DOD_Protocol.CMD_GET_EYE_STATUS:               // 读取喷头电眼状态	0x000d
            case EC_DOD_Protocol.CMD_SET_SYNC_STATUS:              // 设定喷头同步器状态	0x000e
            case EC_DOD_Protocol.CMD_GET_SYNC_STATUS:              // 读取喷头同步器状态	0x000f
            case EC_DOD_Protocol.CMD_GET_REVERSE:                  // 读取喷头翻转喷印	0x0011
            case EC_DOD_Protocol.CMD_GET_USABLE_IDS:               // 获取当前文件中可用的ID	0x0012
            case EC_DOD_Protocol.CMD_SAVE_CURRENT_INFO:            // 保存当前信息	0x0014
            case EC_DOD_Protocol.CMD_START_PRINT_A:                // A喷头喷印	0x0017
            case EC_DOD_Protocol.CMD_START_PRINT_B:                // B喷头喷印	0x0018
            case EC_DOD_Protocol.CMD_STOP_PRINT_A:                 // A喷印完成	0x0019
            case EC_DOD_Protocol.CMD_STOP_PRINT_B:                 // B喷印完成	0x0020
//                mSerialPort.writeStringSerial( "<!-- Not Support Command -->");
                sendCommandProcessResult(cmd, 1, 0, 1, "");
                break;
            case EC_DOD_Protocol.CMD_CHECK_CHANNEL:                // 检查信道	0x0001
            case EC_DOD_Protocol.CMD_SET_PRINT_DELAY:              // 设定喷头喷印延时	0x0008
            case EC_DOD_Protocol.CMD_SET_MOVE_SPEED:               // 设定物体移动速度	0x000a
            case EC_DOD_Protocol.CMD_SET_REVERSE:                  // 设定喷头翻转喷印	0x0010
            case EC_DOD_Protocol.CMD_TEXT:                         // 发送一条文本	0x0013
            case EC_DOD_Protocol.CMD_START_PRINT:                  // 启动喷码机开始喷印	0x0015
            case EC_DOD_Protocol.CMD_STOP_PRINT:                   // 停机命令	0x0016
                procCommands(cmd, data);
                break;
            default:
                sendCommandProcessResult(cmd, 1, 0, 1, "");
                break;
        }
    }

    private void procCommands(int cmd, byte[] data) {
        if(null != mNormalCmdListeners) {
            mNormalCmdListeners.onCommandReceived(cmd, data);
        }
        if(null != mPrintCmdListeners) {
            mPrintCmdListeners.onCommandReceived(cmd, data);
        }
    }

    public void setNormalCommandListener(OnSerialPortCommandListenner l) {
        mNormalCmdListeners = l;
    }

    public void setPrintCommandListener(OnSerialPortCommandListenner l) {
        mPrintCmdListeners = l;
    }

    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        if(isInitialized()) {
            byte[] response;
            if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
                SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {

                EC_DOD_Protocol p = new EC_DOD_Protocol();
                response = p.createFrame(cmd, ack, devStatus, cmdStatus, message.getBytes(Charset.forName("UTF-8")));
                mSerialPort.writeSerial(response);

            } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3) {
                SerialProtocol3 p = new SerialProtocol3();

                response = p.createFrame(cmd, ack, devStatus, cmdStatus, message.getBytes(Charset.forName("UTF-8")));
                mSerialPort.writeSerial(response);
            }
        }
    }
}
