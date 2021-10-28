package com.industry.printer.pccommand;

import android.content.Context;
import android.os.Bundle;
import android.os.Message;
import android.os.SystemClock;

import com.industry.printer.Constants.Constants;
import com.industry.printer.DataTransferThread;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.R;
import com.industry.printer.Socket_Server.PCCommand;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.DataTask;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.hardware.RTCDevice;

import java.io.File;
import java.util.Calendar;

/**
 * Created by hmwan on 2021/10/28.
 */

public class PCCommandHandler {
    private static final String TAG = PCCommandHandler.class.getSimpleName();

    private Context mContext = null;
    // H.M.Wang 2020-1-8 增加网络命令ID，在向PC报告打印状态的时候用来识别命令
    private String mPCCmdId = "";
    // End of H.M.Wang 2020-1-8 增加网络命令ID，在向PC报告打印状态的时候用来识别命令

    public interface HandleMessageReceiver {
        public void handledMessage(String msg);
    }
    private HandleMessageReceiver mHandledMessageReceiver = null;

    public PCCommandHandler(Context ctx) {
        mContext = ctx;
    }

    private void sendmsg(String msg) {
        if(null != mHandledMessageReceiver) {
            mHandledMessageReceiver.handledMessage(msg);
        }
    }

    public void handle(String msg, HandleMessageReceiver l) {
        mHandledMessageReceiver = l;

        PCCommand cmd = PCCommand.fromString(msg);

        // End of H.M.Wang 2020-1-8 提取命令ID
        mPCCmdId = cmd.check;
        // End of H.M.Wang 2020-1-8 提取命令ID

// H.M.Wang 当解析命令失败时，抛弃这个命令
// H.M.Wang 2019-12-30 收到空命令的时候，返回错误
        if(null == cmd) {
            sendmsg(Constants.pcErr("<Null Command>"));
// End of H.M.Wang 2019-12-30 收到空命令的时候，返回错误
        } else if (PCCommand.CMD_SEND_BIN.equalsIgnoreCase(cmd.command) ||  // LAN Printing
                   PCCommand.CMD_SEND_BIN_S.equalsIgnoreCase(cmd.command)) {  // LAN Printing
            cacheBin(Gsocket, msg);
        } else if (PCCommand.CMD_DEL_LAN_BIN.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_DEL_LAN_BIN_S.equalsIgnoreCase(cmd.command)) {
            DataTransferThread.deleteLanBuffer(Integer.valueOf(cmd.content));
        } else if (PCCommand.CMD_RESET_INDEX.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_RESET_INDEX_S.equalsIgnoreCase(cmd.command)) {
            if(null != mDTransThread) {
                mDTransThread.resetIndex();
                sendmsg(Constants.pcOk(msg));
            } else {
                sendmsg(Constants.pcErr(msg));
            }
// H.M.Wang 2019-12-16 支持网络下发计数器和动态二维码的值
        } else if (PCCommand.CMD_SET_REMOTE.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_SET_REMOTE_S.equalsIgnoreCase(cmd.command)) {
            // H.M.Wang 2019-12-18 判断参数41，是否采用外部数据源，为true时才起作用
            if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_LAN ||
// H.M.Wang 2020-6-28 追加专门为网络快速打印设置
                SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FAST_LAN) {
// End of H.M.Wang 2020-6-28 追加专门为网络快速打印设置
                if(null != mDTransThread && mDTransThread.isRunning()) {
                    mDTransThread.setRemoteTextSeparated(cmd.content);
                    this.sendmsg(Constants.pcOk(msg));
                } else {
                    this.sendmsg(Constants.pcErr(msg));
                }
            } else {
                this.sendmsg(Constants.pcErr(msg));
            }
           // End.
// End. -----
        } else if (PCCommand.CMD_PRINT.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_PRINT_S.equalsIgnoreCase(cmd.command)) {
            File msgfile = new File(cmd.content);
            if (!isTlkReady(msgfile)) {
                sendmsg(Constants.pcErr(msg));
                return;
            }

            if(PrinterFlag==0) {
                //打印赵工写好了，再测试
                PrnComd="100";
                PrinterFlag=1;
//         		StopFlag=1;
                CleanFlag=0;

// H.M.Wang 2020-6-23 设置mObjPath，以便打开成功后显示信息名称
                mObjPath = msgfile.getName();
// End of H.M.Wang 2020-6-23 设置mObjPath，以便打开成功后显示信息名称

                Message message = mHandler.obtainMessage(MESSAGE_OPEN_TLKFILE);
                Bundle bundle = new Bundle();
                bundle.putString("file", mObjPath);  // f表示信息名称
                bundle.putString(Constants.PC_CMD, msg);
                message.setData(bundle);
                mHandler.sendMessage(message);
            }
        } else if (PCCommand.CMD_CLEAN.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_CLEAN_S.equalsIgnoreCase(cmd.command)) {
            //200是清洗
            CleanFlag=1;
            if(null != mDTransThread) {
                mDTransThread.purge(mContext);
                this.sendmsg(Constants.pcOk(msg));
            } else {
// 2020-7-21 在未开始打印前，网络清洗命令不响应问题解决
                thread = DataTransferThread.getInstance(mContext);
                thread.purge(mContext);
                this.sendmsg(Constants.pcOk(msg));
// End of 2020-7-21 在未开始打印前，网络清洗命令不响应问题解决
//				sendmsg(Constants.pcErr(msg));
            }
        } else if (PCCommand.CMD_SEND_FILE.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_SEND_FILE_S.equalsIgnoreCase(cmd.command)) {
            //300发文件
            AddPaths="";
            if(SendFileFlag==0) { //发文件等赵工写好了，再测试
                SendFileFlag=1;
                this.sendmsg(WriteFiles(Gsocket,msg));
            }
        } else if (PCCommand.CMD_READ_COUNTER.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_READ_COUNTER_S.equalsIgnoreCase(cmd.command)) {
            //400取计数器
// H.M.Wang 2020-4-22 修改读取Counter命令返回格式
            StringBuilder sb = new StringBuilder();
            sb.append("" + mCounter);
            for(int i=0; i<8; i++) {
                sb.append("|" + (int)(mInkManager.getLocalInk(i)));
            }
// H.M.Wang 2020-6-29 打印任务还没有启动时，DataTransferThread.getInstance(mContext)会自动生成instance，导致错误，应避免使用
            sb.append("|" + (null != mDTransThread && mDTransThread.isRunning() ? "T" : "F") + "|");
// H.M.Wang 2020-6-29 打印任务还没有启动时，DataTransferThread.getInstance(mContext)会自动生成instance，导致错误，应避免使用
            sb.append(msg);
            this.sendmsg(Constants.pcOk(sb.toString()));
// End of H.M.Wang 2020-4-22 修改读取Counter命令返回格式

/*
// H.M.Wang 2020-7-1 临时版本，回复原来的回复格式
                                        for(int i=0;i<7;i++)
                                        {
                                            sendmsg("counter:" + mCounter+" |ink:" + mInkManager.getLocalInkPercentage(i) + "|state:" + (null != mDTransThread && mDTransThread.isRunning()));
                                            //获取INK无显示问题，赵工这地方改好，前面注示去掉就OK了
                                            this.sendmsg(Constants.pcOk(msg));
                                        }
// End of H.M.Wang 2020-7-1 临时版本，回复原来的回复格式
*/
        } else if (PCCommand.CMD_STOP_PRINT.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_STOP_PRINT_S.equalsIgnoreCase(cmd.command)) {
            //500停止打印
            if(null != mDTransThread && mDTransThread.isRunning()) {
//	       	if(StopFlag==1) {
//	       		StopFlag=0;
                PrinterFlag=0;
                Message message = mHandler.obtainMessage(MESSAGE_PRINT_STOP);
                Bundle bundle = new Bundle();
                bundle.putString(Constants.PC_CMD, msg);
                message.setData(bundle);
                message.sendToTarget();
                //this.sendmsg(msg+"recv success!");
             }
// H.M.Wang 2020-6-16 这个条件重复，应该注释掉
/*
		                            else if(PCCommand.CMD_SET_REMOTE.equalsIgnoreCase(cmd.command)) {
		                           //600字符串长成所需文件

		                    			StrInfo_Stack.push(cmd.content);//用堆栈存储收的信息，先进称出;
		                    			try {
											int count = Integer.parseInt(cmd.content);
											for (MessageTask task : mMsgTask) {
												for (BaseObject object : task.getObjects()) {
													if (object instanceof CounterObject) {
														object.setContent(cmd.content);
													}
												}
											}
										} catch (Exception e) {
											// TODO: handle exception
											this.sendmsg(Constants.pcErr(msg));
										}
										this.sendmsg(Constants.pcOk(msg));
		                            }*/
// End of H.M.Wang 2020-6-16 这个条件重复，应该注释掉
        } else if (PCCommand.CMD_MAKE_TLK.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_MAKE_TLK_S.equalsIgnoreCase(cmd.command)) {
            //700
            this.sendmsg(getString(R.string.str_build_tlk_start));
            String[] parts = msg.split("\\|");
            for (int j = 0; j < parts.length; j++) {
                Debug.d(TAG, "--->parts[" + j + "] = " + parts[j]);
            }

            if (parts != null || parts.length > 4) {
                MakeTlk(parts[3]);
            }
        } else if (PCCommand.CMD_DEL_FILE.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_DEL_FILE_S.equalsIgnoreCase(cmd.command)) {
            //600字符串长成所需文件
            if (deleteFile(msg)) {
                this.sendmsg(Constants.pcOk(msg));
            } else {
                this.sendmsg(Constants.pcErr(msg));
            }
        } else if (PCCommand.CMD_DEL_DIR.equalsIgnoreCase(cmd.command) ||
                   PCCommand.CMD_DEL_DIR_S.equalsIgnoreCase(cmd.command)) {
            //600字符串长成所需文件
            if (deleteDirectory(msg)) {
                this.sendmsg(Constants.pcOk(msg));
            } else {
                this.sendmsg(Constants.pcErr(msg));
            }
// H.M.Wang 2019-12-25 追加速度和清洗命令
        } else if (PCCommand.CMD_SET_DOTSIZE.equalsIgnoreCase(cmd.command)) {
            try {
// H.M.Wang 2019-12-27 暂时取消3.7倍的系数。修改设置参数为23。取值范围0-6000。 2019-12-28 内部保存在参数33
                SystemConfigFile.getInstance().setParamBroadcast(SystemConfigFile.INDEX_DOT_SIZE, Math.max(0, Math.min(6000, Integer.parseInt(cmd.content))));
//                SystemConfigFile.getInstance().setParamBroadcast(0, Math.round(3.7f * Integer.parseInt(cmd.content)));
// End of H.M.Wang 2019-12-27 暂时取消3.7倍的系数。修改设置参数为23。取值范围0-6000
                SystemConfigFile.getInstance().saveConfig();
                if(null != mDTransThread && mDTransThread.isRunning()) {
// H.M.Wang 2019-12-29 修改在打印状态下设置FPGA参数的逻辑
                    DataTask task = mDTransThread.getCurData();
// 2020-5-8												FpgaGpioOperation.clean();
                    FpgaGpioOperation.updateSettings(mContext, task, FpgaGpioOperation.SETTING_TYPE_NORMAL);
//					mDTransThread.mNeedUpdate = true;
//					while(mDTransThread.mNeedUpdate) {
//						Thread.sleep(10);
//					}
                    FpgaGpioOperation.init(mContext);
// H.M.Wang 2020-7-9 取消下发参数设置后重新下发打印缓冲区操作
//					mDTransThread.resendBufferToFPGA();
// End of H.M.Wang 2020-7-9 取消下发参数设置后重新下发打印缓冲区操作
// End of H.M.Wang 2019-12-29 修改在打印状态下设置FPGA参数的逻辑
                }
                this.sendmsg(Constants.pcOk(msg));
            } catch (NumberFormatException e) {
                Debug.e(TAG, e.getMessage());
            }
        } else if (PCCommand.CMD_SET_CLEAN.equalsIgnoreCase(cmd.command)) {
            if(null != mDTransThread && mDTransThread.isRunning()) {
                DataTask task = mDTransThread.getCurData();
                int param0 = SystemConfigFile.getInstance().getParam(0);
// H.M.Wang 2019-12-27 修改取值，以达到下发FPGA时参数4的值为9
                SystemConfigFile.getInstance().setParam(0, 18888);
// End of H.M.Wang 2019-12-27 修改取值，以达到下发FPGA时参数4的值为9
                FpgaGpioOperation.updateSettings(mContext, task, FpgaGpioOperation.SETTING_TYPE_NORMAL);
// H.M.Wang 2019-12-27 间隔时间修改为10ms
                Thread.sleep(10);
// End of H.M.Wang 2019-12-27 间隔时间修改为10ms
                SystemConfigFile.getInstance().setParam(0, param0);
// 2020-5-8											FpgaGpioOperation.clean();
                FpgaGpioOperation.updateSettings(mContext, task, FpgaGpioOperation.SETTING_TYPE_NORMAL);
// H.M.Wang 2019-12-27 重新启动打印
//				mDTransThread.mNeedUpdate = true;
//				while(mDTransThread.mNeedUpdate) {
//					Thread.sleep(10);
//				}
                FpgaGpioOperation.init(mContext);
// H.M.Wang 2020-7-9 取消下发参数设置后重新下发打印缓冲区操作
//												mDTransThread.resendBufferToFPGA();
// End of H.M.Wang 2020-7-9 取消下发参数设置后重新下发打印缓冲区操作
// End of H.M.Wang 2019-12-27 重新启动打印
            }
// End of H.M.Wang 2019-12-25 追加速度和清洗命令
            this.sendmsg(Constants.pcOk(msg));
// H.M.Wang 2020-7-1 追加一个计数器设置数值命令
        } else if (PCCommand.CMD_SET_COUNTER.equalsIgnoreCase(cmd.command)) {
            try {
                int cIndex = Integer.valueOf(cmd.content);
                if(cIndex < 0 || cIndex > 9) {
                    Debug.e(TAG, "CMD_SET_COUNTER command, Index overflow.");
                    this.sendmsg(Constants.pcErr(msg));
                } else {
                    try {
                        int cValue = Integer.valueOf(cmd.note2);
                        SystemConfigFile.getInstance().setParamBroadcast(cIndex + SystemConfigFile.INDEX_COUNT_1, cValue);
                        RTCDevice.getInstance(mContext).write(cValue, cIndex);
                        DataTransferThread dt = DataTransferThread.mInstance;
                        if(null != dt && dt.isRunning()) {
                            resetCounterIfNeed();
                            dt.mNeedUpdate = true;
// H.M.Wang 2020-7-9 追加计数器重置标识
                            dt.mCounterReset = true;
// End of H.M.Wang 2020-7-9 追加计数器重置标识
                        }

                        this.sendmsg(Constants.pcOk(msg));
                    } catch (NumberFormatException e) {
                        Debug.e(TAG, "CMD_SET_COUNTER command, invalid value.");
                        this.sendmsg(Constants.pcErr(msg));
                    }
                }
            } catch (NumberFormatException e) {
                Debug.e(TAG, "CMD_SET_COUNTER command, invalid index.");
                this.sendmsg(Constants.pcErr(msg));
            }
// End of H.M.Wang 2020-7-1 追加一个计数器设置数值命令
        } else if (PCCommand.CMD_SET_TIME.equalsIgnoreCase(cmd.command)) {
            try {
                int cYear = Integer.valueOf(cmd.content.substring(0,2));
                int cMonth = Integer.valueOf(cmd.content.substring(2,4));
                int cDate = Integer.valueOf(cmd.content.substring(4,6));
                int cHour = Integer.valueOf(cmd.content.substring(6,8));
                int cMinute = Integer.valueOf(cmd.content.substring(8,10));
                int cSecond = Integer.valueOf(cmd.content.substring(10,12));

                if(cYear < 0 || cYear > 99) {
                    Debug.e(TAG, "CMD_SET_TIME command, invalid year.");
                    this.sendmsg(Constants.pcErr(msg));
                } else if(cMonth < 1 || cMonth > 12) {
                    Debug.e(TAG, "CMD_SET_TIME command, invalid month.");
                    this.sendmsg(Constants.pcErr(msg));
                } else if(cDate < 1 || cDate > 31) {
                    Debug.e(TAG, "CMD_SET_TIME command, invalid date.");
                    this.sendmsg(Constants.pcErr(msg));
                } else if(cHour < 0 || cHour > 23) {
                    Debug.e(TAG, "CMD_SET_TIME command, invalid hour.");
                    this.sendmsg(Constants.pcErr(msg));
                } else if(cMinute < 0 || cMinute > 59) {
                    Debug.e(TAG, "CMD_SET_TIME command, invalid minute.");
                    this.sendmsg(Constants.pcErr(msg));
                } else if(cSecond < 0 || cSecond > 59) {
                    Debug.e(TAG, "CMD_SET_TIME command, invalid second.");
                    this.sendmsg(Constants.pcErr(msg));
                } else {
                    Calendar c = Calendar.getInstance();

                    c.set(cYear + 2000, cMonth-1, cDate, cHour, cMinute, cSecond);
                    SystemClock.setCurrentTimeMillis(c.getTimeInMillis());
                    RTCDevice rtcDevice = RTCDevice.getInstance(mContext);
                    rtcDevice.syncSystemTimeToRTC(mContext);
                    Debug.d(TAG, "Set time to: " + (cYear + 2000) + "-" + cMonth + "-" + cDate + " " + cHour + ":" + cMinute + ":" + cSecond);
                    this.sendmsg(Constants.pcOk(msg));
                }
            } catch (Exception e) {
                Debug.e(TAG, "CMD_SET_TIME command, " + e.getMessage() + ".");
                this.sendmsg(Constants.pcErr(msg));
            }
// H.M.Wang 2020-7-28 追加一个设置参数命令
        } else if (PCCommand.CMD_SET_PARAMS.equalsIgnoreCase(cmd.command)) {
            try {
                int cIndex = Integer.valueOf(cmd.content);
                cIndex--;
                if(cIndex < 0 || cIndex > 63) {
                    Debug.e(TAG, "Invalid PARAM index.");
                    this.sendmsg(Constants.pcErr(msg));
                } else {
                    try {
                        int cValue = Integer.valueOf(cmd.note2);
//	    				if(cIndex == 3 || cIndex == 0 || cIndex == 1 || cIndex == 32 || (cIndex >= SystemConfigFile.INDEX_COUNT_1 && cIndex <= SystemConfigFile.INDEX_COUNT_10)) {
                        mSysconfig.setParamBroadcast(cIndex, cValue);
//						} else {
//							mSysconfig.setParam(cIndex, cValue);
//						}
                        this.sendmsg(Constants.pcOk(msg));
                    } catch (NumberFormatException e) {
                        Debug.e(TAG, "Invalid PARAM value.");
                        this.sendmsg(Constants.pcErr(msg));
                    }
                }
            } catch (NumberFormatException e) {
                Debug.e(TAG, "Invalid PARAM index.");
                this.sendmsg(Constants.pcErr(msg));
            }
// End of H.M.Wang 2020-7-28 追加一个设置参数命令
// H.M.Wang 2020-9-28 追加一个心跳协议
        } else if (PCCommand.CMD_HEARTBEAT.equalsIgnoreCase(cmd.command)) {
            mLastHeartBeat = System.currentTimeMillis();
            this.sendmsg(Constants.pcOk(msg));
// End of H.M.Wang 2020-9-28 追加一个心跳协议
// H.M.Wang 2021-2-4 追加软启动打印命令
        } else if (PCCommand.CMD_SOFT_PHO.equalsIgnoreCase(cmd.command)) {
            FpgaGpioOperation.softPho();
            this.sendmsg(Constants.pcOk(msg));
// End of H.M.Wang 2021-2-4 追加软启动打印命令
        } else {
            this.sendmsg(Constants.pcErr(msg));
        }
    }
}
