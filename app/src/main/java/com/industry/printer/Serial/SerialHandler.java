package com.industry.printer.Serial;

import android.content.Context;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StreamTransport;
import com.industry.printer.pccommand.PCCommandManager;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2019/10/26.
 */

public class SerialHandler {
    public static String TAG = SerialHandler.class.getSimpleName();

    private final String SERIAL_PORT = "/dev/ttyS4";
// H.M.Wang 2022-4-4 追加341串口（ttyUSB0）
    private final String SERIAL_CH341_PORT = "/dev/ttyUSB0";
// End of H.M.Wang 2022-4-4 追加341串口（ttyUSB0）

    private SerialPort mSerialPort = null;
    private Context mContext;

    public interface OnSerialPortCommandListenner {
        public void onCommandReceived(int cmd, byte[] data);
        public void onError(String errCode);
    }

    private OnSerialPortCommandListenner mNormalCmdListeners = null;
    private OnSerialPortCommandListenner mPrintCmdListeners = null;

    private static SerialHandler mSerialHandler = null;
    private boolean mRunning = false;

    public static SerialHandler getInstance(Context ctx) {
        if(null == mSerialHandler) {
            mSerialHandler = new SerialHandler(ctx);
            mSerialHandler.init();
        }
        return mSerialHandler;
    }

    public static SerialHandler getInstance() {
        if(null == mSerialHandler) {
            mSerialHandler = new SerialHandler(null);
            mSerialHandler.init();
        }
        return mSerialHandler;
    }

    public SerialHandler(Context ctx) {
        mContext = ctx;
        mRunning = false;
    }

    public boolean isInitialized() {
        return (mSerialPort != null);
    }

    private void init() {
        mSerialPort = new SerialPort();
        if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_PC_COMMAND) {
            Debug.i(TAG, "Open " + SERIAL_PORT);
            mSerialPort.openStream(SERIAL_PORT);
            Debug.i(TAG, "Start PCCommand Receiver");
            PCCommandManager pcCmdManager = PCCommandManager.getInstance();
            if(null != pcCmdManager)pcCmdManager.addSeriHandler(mSerialPort);
        } else {
// H.M.Wang 2022-4-4 根据数据源的类型选择串口
            if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_11 ||
// H.M.Wang 2022-5-16 追加串口协议2无线
                SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_2_WIFI) {
// End of H.M.Wang 2022-5-16 追加串口协议2无线
                Debug.i(TAG, "Open " + SERIAL_CH341_PORT);
                mSerialPort.openSerial(SERIAL_CH341_PORT);
            } else {
                Debug.i(TAG, "Open " + SERIAL_PORT);
                mSerialPort.openSerial(SERIAL_PORT);
            }
// End of H.M.Wang 2022-4-4 根据数据源的类型选择串口
            Debug.i(TAG, "Start normal Receiver");
            new Thread() {
                @Override
                public void run() {
                    mRunning = true;
                    while(mRunning) {
                        byte[] data = mSerialPort.readSerial();
                        if(null != data) {
                            dispatchProtocol(data);
                        }
                    }
                }
            }.start();
        }
    }

    public void stop() {
        if(isInitialized()) {
            mRunning = false;
            mSerialPort.closeSerial();
            mSerialPort = null;
            mSerialHandler = null;
        }
    }

    private void dispatchProtocol(byte[] data) {
        if(!isInitialized()) return;

        ByteArrayBuffer bab = new ByteArrayBuffer(0);
        bab.append(data, 0, data.length);

//        Debug.d(TAG, "DataSource: " + SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE));

        if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_1 ||
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_2 ||
// H.M.Wang 2022-5-16 追加串口协议2无线
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_2_WIFI) {
// End of H.M.Wang 2022-5-16 追加串口协议2无线
            EC_DOD_Protocol p = new EC_DOD_Protocol(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_3) {
            SerialProtocol3 p = new SerialProtocol3(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_4) {
            XK3190_A30_Protocol p = new XK3190_A30_Protocol(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_SCANER1) {
            SerialProtocol5 p = new SerialProtocol5(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// H.M.Wang 2020-6-9 追加串口6协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_6) {
            SerialProtocol6 p = new SerialProtocol6(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2020-6-9 追加串口6协议
// H.M.Wang 2020-8-13 追加串口7协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_7) {
            SerialProtocol7 p = new SerialProtocol7(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2020-8-13 追加串口7协议
// H.M.Wang 2020-10-30 追加扫描2串口协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_SCANER2) {
            Scaner2Protocol p = new Scaner2Protocol(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2020-10-30 追加扫描2串口协议
// H.M.Wang 2021-1-15 追加扫描协议3，协议内容与扫描2协议完全一致，仅在打印的时候，仅可以打印一次
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_SCANER3) {
            Scaner2Protocol p = new Scaner2Protocol(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2021-1-15 追加扫描协议3
// H.M.Wang 2021-3-6 追加串口协议8
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_8) {
            SerialProtocol8 p = new SerialProtocol8(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2021-3-6 追加串口协议8
// H.M.Wang 2021-9-24 追加串口协议9
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_9) {
            SerialProtocol9 p = new SerialProtocol9(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2021-9-24 追加串口协议9
// H.M.Wang 2021-9-28 追加串口协议10
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_10) {
            SerialProtocol10 p = new SerialProtocol10(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2021-9-28 追加串口协议10
// H.M.Wang 2022-4-5 追加串口协议11(341串口)
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_11) {
            SerialProtocol11 p = new SerialProtocol11(mSerialPort, mContext);
            p.handleCommand(mNormalCmdListeners, mPrintCmdListeners, bab);
// End of H.M.Wang 2022-4-5 追加串口协议11(341串口)
        }
    }

    public void setNormalCommandListener(OnSerialPortCommandListenner l) {
        mNormalCmdListeners = l;
    }

    public void setPrintCommandListener(OnSerialPortCommandListenner l) {
        mPrintCmdListeners = l;
    }

    // cmdStatus = 0: Success; cmdStatus = 1: Failed
    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        if(!isInitialized()) return;

        if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_1 ||
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_2 ||
// H.M.Wang 2022-5-16 追加串口协议2无线
            SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_2_WIFI) {
// End of H.M.Wang 2022-5-16 追加串口协议2无线
            EC_DOD_Protocol p = new EC_DOD_Protocol(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_3) {
            SerialProtocol3 p = new SerialProtocol3(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_4) {
            XK3190_A30_Protocol p = new XK3190_A30_Protocol(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_SCANER1) {
            SerialProtocol5 p = new SerialProtocol5(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// H.M.Wang 2020-6-9 追加串口6协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_6) {
            SerialProtocol6 p = new SerialProtocol6(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// End of H.M.Wang 2020-6-9 追加串口6协议
// H.M.Wang 2020-8-13 追加串口7协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_7) {
            SerialProtocol7 p = new SerialProtocol7(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// End of H.M.Wang 2020-8-13 追加串口7协议
// H.M.Wang 2020-10-30 追加扫描2串口协议
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_SCANER2) {
            Scaner2Protocol p = new Scaner2Protocol(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// End of H.M.Wang 2020-10-30 追加扫描2串口协议
// H.M.Wang 2021-3-6 追加串口协议8
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_8) {
            SerialProtocol8 p = new SerialProtocol8(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// End of H.M.Wang 2021-3-6 追加串口协议8
// H.M.Wang 2021-9-24 追加串口协议9
        } else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_9) {
            SerialProtocol9 p = new SerialProtocol9(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
// End of H.M.Wang 2021-9-24 追加串口协议9
        }
    }

    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, byte[] message) {
        if (!isInitialized()) return;

// H.M.Wang 2022-4-5 追加串口协议11(341串口)
        if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS232_11) {
            SerialProtocol11 p = new SerialProtocol11(mSerialPort, mContext);
            p.sendCommandProcessResult(cmd, ack, devStatus, cmdStatus, message);
        }
// End of H.M.Wang 2022-4-5 追加串口协议11(341串口)
    }


    public SerialPort getSerialPort() {
        return mSerialPort;
    }
}
