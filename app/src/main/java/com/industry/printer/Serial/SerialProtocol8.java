package com.industry.printer.Serial;

import android.content.Context;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StreamTransport;

import org.apache.http.util.ByteArrayBuffer;

import java.nio.charset.Charset;

/**
 * Created by hmwan on 2021/3/6.
 */

public class SerialProtocol8 extends SerialProtocol {
    public static String TAG = SerialProtocol8.class.getSimpleName();

/*  协议内容
    ！！！ 文档中使用的CRC，貌似为CRC16-MODBUS
    1. 读取
       接收报文： 01 03 00 00 00 02 C4 0B
                == == ===== ===== =====
                |  |    |     |     | CRC-16校验码
                |  |    |     | 数据双字节无用
                |  |    | 内存地址无用
                |  | 读命令
                | 本机ID
           * 本机通过参数58设置本机ID值，报文的本机ID必须与本机的参数58设定参数一致才做反应，否则忽略
           * 本机ID值为10进制数值（即，虽然内部保存的是16进制的10，但是需要按10进制10处理）
             （另外一种解释：参数58输入参数值为10时，要与报文第一字节的0x10做匹配）
// H.M.Wang 2021-4-15 回复报文增加品种代码
       回复报文： 01 03 04 03 E8 00 00 03 E8 00 00 7A 43.
                == == == =========== =========== =====
                |  |  |       |           |        | CRC-16校验码
                |  |  |       |           | 品种代码
                |  |  |       | 返回数据
                |  |  | 返回数据字节数
                |  | 读命令
                | 本机ID

    2. 写入
// H.M.Wang 2021-4-11 追加4字节品种代码
// H.M.Wang 2021-4-13 修改写入数据和品种代码的数据格式及处理方法
       接收报文： 01 10 00 00 00 02 04 00 00 00 00 03 E9 00 00 22 1F
                == == ===== ===== == =========== =========== =====
                |  |    |     |   |       |           |        | CRC-16校验码
                |  |    |     |   |       |           | 品种代码
                |  |    |     |   |       | 写入数据
                |  |    |     |   | 收到数据字节数
                |  |    |     | 数据双字节无用
                |  |    | 内存地址无用
                |  | 写命令
                | 本机ID
           * 写入数据为后两个字节为高位，前两个字节为低位，但内部则高位在前，如(03E90000)，解析为(000003E9)，然后变换为10进制后为n位数字(03E9为1001)，最后一位(1)赋给DT1，前面其他的数值(100)赋给DT0，不足DT0宽度的话，前面置空格
           * 品种代码的处理方法如写入数据
       回复报文： 01 10 00 00 00 02 41 C8.
                == == ===== ===== =====
                |  |    |     |     | CRC-16校验码
                |  |    |     | 固定字节长度无用
                |  |    | 内存地址无用
                |  | 写命令
                | 本机ID

*/
    private final int CMD_READ                  = 0x03;
    private final int CMD_WRITE                 = 0x10;

    private final static int TAG_DEVICE_ID_POS         = 0;
    private final static int TAG_CMDID_POS             = 1;
    private final static int TAG_READ_CRC_POS          = 6;
// H.M.Wang 2021-4-11 追加4字节品种代码，相应调整CRC位置设置
    private final static int TAG_WRITE_CRC_POS         = 15;
    public  final static int TAG_PRODUCT_TYPE_POS      = 11;
    public  final static int TAG_WRITE_DATA_POS        = 7;
// End of H.M.Wang 2021-4-11 追加4字节品种代码，相应调整CRC位置设置

    private final static int ERROR_ID_NOT_MATCH        = 0x81000000;   // 与本地ID不匹配
    private final static int ERROR_UNKNOWN_CMD         = 0x83000000;   // 不可识别的命令(CMD_READ,CMD_WRITE以外的值)
    private final static int ERROR_CRC_FAILED          = 0x84000000;   // CRC校验失败
    private final static int ERROR_FAILED              = 0x85000000;   // 解析帧失败

    private int mLocalID = 0;
    private int mCmd = 0;
    private static int mLastValue = 0;
    private static int mTypeCode = 0;

    public SerialProtocol8(/*StreamTransport st*/SerialPort serialPort, Context ctx){
        super(serialPort, ctx);

        SystemConfigFile sysConfig = SystemConfigFile.getInstance();
        mLocalID = sysConfig.getParam(SystemConfigFile.INDEX_LOCAL_ID);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        int crcPos = 0;

        try {
            byte[] msg = recvMsg.toByteArray();
// H.M.Wang 2021-4-15 增加命令合法性检验
            if(msg.length < TAG_CMDID_POS + 1) {
                return ERROR_FAILED;
            }
// End of H.M.Wang 2021-4-15 增加命令合法性检验

// 2021-4-9 取消将16进制数值按BCD换算的操作
//            int localID = (msg[TAG_DEVICE_ID_POS] / 16 * 10 + msg[TAG_DEVICE_ID_POS] % 16) & 0x00ff;
//            if(mLocalID != localID) {
            if(mLocalID != msg[TAG_DEVICE_ID_POS]) {
// End of 2021-4-9 取消将16进制数值按BCD换算的操作
                return ERROR_ID_NOT_MATCH;
            }

            mCmd = msg[TAG_CMDID_POS];
            if(mCmd == CMD_READ) {
// H.M.Wang 2021-4-15 增加命令合法性检验
                if(msg.length < TAG_READ_CRC_POS + 2) {
                    return ERROR_UNKNOWN_CMD;
                }
// End of H.M.Wang 2021-4-15 增加命令合法性检验
                crcPos = TAG_READ_CRC_POS;
            } else if(mCmd == CMD_WRITE) {
// H.M.Wang 2021-4-15 增加命令合法性检验
                if(msg.length < TAG_WRITE_CRC_POS + 2) {
                    return ERROR_UNKNOWN_CMD;
                }
// End of H.M.Wang 2021-4-15 增加命令合法性检验
                crcPos = TAG_WRITE_CRC_POS;
                mLastValue =
                        (((msg[TAG_WRITE_DATA_POS+2] & 0x000000ff) << 24) +
                         ((msg[TAG_WRITE_DATA_POS+3] & 0x000000ff) << 16) +
                         ((msg[TAG_WRITE_DATA_POS] & 0x000000ff) << 8) +
                         (msg[TAG_WRITE_DATA_POS+1] & 0x000000ff));
                mTypeCode =
                        (((msg[TAG_PRODUCT_TYPE_POS+2] & 0x000000ff) << 24) +
                         ((msg[TAG_PRODUCT_TYPE_POS+3] & 0x000000ff) << 16) +
                         ((msg[TAG_PRODUCT_TYPE_POS] & 0x000000ff) << 8) +
                         (msg[TAG_PRODUCT_TYPE_POS+1] & 0x000000ff));
            } else {
                return ERROR_UNKNOWN_CMD;
            }

            short recvCRC = (short)((0x00ff & msg[crcPos+1]) * 0x0100 + ((0x00ff) & msg[crcPos]));
//            Debug.d(TAG, "recvCRC = " + Integer.toHexString((0x0000ffff & recvCRC)));
            byte[] procData = ByteArrayUtils.pickPartial(msg, 0, crcPos);
//            Debug.d(TAG, "[" + ByteArrayUtils.toHexString(procData) + "]");
//            Debug.d(TAG, "CRC(calculated) [" + Integer.toHexString((0x0000ffff & CRC16_X25.getCRCCode(procData))) + "]");

// 暂时取消CRC验证
//            if(CRC16_MODBUS.getCRCCode(procData) != recvCRC) {
//                recvCmd |= ERROR_CRC_FAILED;
//                // return ERROR_CRC_FAILED;
//            }

//            Debug.d(TAG, "CMD = " + Integer.toHexString((0x0000ffff & recvCmd)) + "; DATA = [" + ByteArrayUtils.toHexString(recvMsg.toByteArray()) + "]");
            return ERROR_SUCESS;
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
        return ERROR_FAILED;
    }

    @Override
    protected byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
//        Debug.d(TAG, "[" + ByteArrayUtils.toHexString(msg) + "]");

        ByteArrayBuffer sendBuffer = new ByteArrayBuffer(0);

// H.M.Wang 2021-4-15 增加错误报文发送
        if(cmdStatus == 1) {
            return msg;
        }
// End of H.M.Wang 2021-4-15 增加错误报文发送

        // 添加本机ID
// 2021-4-14 取消将16进制数值按BCD换算的操作,4-9日修改漏改
//        sendBuffer.append((byte)(mLocalID/10*16+mLocalID%10));
        sendBuffer.append((byte)(mLocalID));
// End of 2021-4-14 取消将16进制数值按BCD换算的操作,4-9日修改漏改

        // 添加命令
        sendBuffer.append(mCmd);

        if(CMD_READ == mCmd) {
            // 添加数据字节数
            sendBuffer.append(0x04);
            // 添加数据高位
            sendBuffer.append((byte)((mLastValue >> 8) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)(mLastValue & 0x000000ff));
            // 添加数据高位
            sendBuffer.append((byte)((mLastValue >> 24) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)((mLastValue >> 16) & 0x000000ff));
            // 添加数据高位
            sendBuffer.append((byte)((mTypeCode >> 8) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)(mTypeCode & 0x000000ff));
            // 添加数据高位
            sendBuffer.append((byte)((mTypeCode >> 24) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)((mTypeCode >> 16) & 0x000000ff));
        } else if(CMD_WRITE == mCmd) {
            // 添加0000
            sendBuffer.append(0x00);
            sendBuffer.append(0x00);
            // 添加0002
            sendBuffer.append(0x00);
            sendBuffer.append(0x02);
            // 添加数据高位
            sendBuffer.append((byte)((mLastValue >> 8) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)(mLastValue & 0x000000ff));
            // 添加数据高位
            sendBuffer.append((byte)((mLastValue >> 24) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)((mLastValue >> 16) & 0x000000ff));
            // 添加数据高位
            sendBuffer.append((byte)((mTypeCode >> 8) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)(mTypeCode & 0x000000ff));
            // 添加数据高位
            sendBuffer.append((byte)((mTypeCode >> 24) & 0x000000ff));
            // 添加数据低位
            sendBuffer.append((byte)((mTypeCode >> 16) & 0x000000ff));
        }

        short crc = CRC16_MODBUS.getCRCCode(sendBuffer.toByteArray());

        sendBuffer.append(crc);
        sendBuffer.append(crc >> 8);

        Debug.d(TAG, "Created Response: [" + ByteArrayUtils.toHexString(sendBuffer.toByteArray()) + "]");
        return sendBuffer.toByteArray();
    }

    @Override
    public void handleCommand(SerialHandler.OnSerialPortCommandListenner normalCmdListenner, SerialHandler.OnSerialPortCommandListenner printCmdListenner, ByteArrayBuffer bab) {
        setListeners(normalCmdListenner, printCmdListenner);

        int result = parseFrame(bab);
        byte[] recvData = bab.toByteArray();

        switch(result & ERROR_MASK) {
            case ERROR_ID_NOT_MATCH:                // 本地ID不匹配错误
                Debug.e(TAG, "ERROR_ID_NOT_MATCH");
                procError("ERROR_ID_NOT_MATCH");
                break;
            case ERROR_CRC_FAILED:                  // CRC校验失败
                Debug.e(TAG, "ERROR_CRC_FAILED");
// H.M.Wang 2021-4-15 增加错误报文发送
                sendCommandProcessResult(0,0,0,1,"CRC failed");
// End of H.M.Wang 2021-4-15 增加错误报文发送
                break;
            case ERROR_UNKNOWN_CMD:                 // 解析帧失败
                Debug.e(TAG, "ERROR_UNKNOWN_CMD");
// H.M.Wang 2021-4-15 增加错误报文发送
                sendCommandProcessResult(0,0,0,1,"Unknown command");
// End of H.M.Wang 2021-4-15 增加错误报文发送
                break;
            case ERROR_FAILED:                      // 解析帧失败
                Debug.e(TAG, "ERROR_FAILED");
                procError("ERROR_FAILED");
                break;
            case ERROR_SUCESS:
                if(CMD_WRITE == mCmd) procCommands(mCmd, recvData);
                sendCommandProcessResult(0,0,0,0,"");
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
