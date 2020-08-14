package com.industry.printer;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.view.KeyEvent;
import android.widget.Toast;

import com.industry.printer.FileFormat.PackageListReader;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Rfid.IInkScheduler;
import com.industry.printer.Rfid.InkSchedulerFactory;
import com.industry.printer.Serial.EC_DOD_Protocol;
import com.industry.printer.Serial.SerialHandler;
import com.industry.printer.Serial.SerialProtocol;
import com.industry.printer.Serial.SerialProtocol7;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.DataTask;
import com.industry.printer.hardware.BarcodeScanParser;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.hardware.IInkDevice;
import com.industry.printer.hardware.InkManagerFactory;
import com.industry.printer.hardware.SmartCardManager;
import com.industry.printer.interceptor.LogIntercepter;
import com.industry.printer.object.BarcodeObject;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.DynamicText;
import com.industry.printer.object.HyperTextObject;

/**
 * class DataTransferThread
 * 用一个独立的线程读取fpga的buffer状态，
 * 如果kernel已经把打印数据发送给FPGA，那么kernel的Buffer状态为空，可写
 * 此时，需要把下一条打印数据下发给kernel Buffer；
 * 如果kernel的buffer状态不为空，不可写
 * 此时，线程轮训buffer，直到kernel buffer状态为空；
 * @author kevin
 *
 */
public class DataTransferThread {
	
	public static final String TAG = DataTransferThread.class.getSimpleName();
	private static final int MESSAGE_EXCEED_TIMEOUT = 60 * 1000;
	
	public static boolean mRunning;
	public boolean pcReset;
	public static volatile DataTransferThread mInstance;
	
	private Context mContext;
	
	public boolean mNeedUpdate=false;
	private boolean isBufferReady = false;
	
	private int mcountdown[];
	/**打印数据buffer**/
	public List<DataTask> mDataTask;
	/* task index currently printing */
	private int mIndex;
	IInkScheduler mScheduler;
	private static long mInterval = 0;
	private int mThreshold;

	private PrintTask mPrinter;
	
	private int testCount = 0;

	//
	private Lock mPurgeLock;
	public  boolean isCleaning;

	private static Map<String,char[]> mLanBuffer;


	private InkLevelListener mInkListener = null;

	public static DataTransferThread getInstance(Context ctx) {
		if(mInstance == null) {
			synchronized (DataTransferThread.class) {
				if (mInstance == null) {
					mInstance = new DataTransferThread(ctx);
				}
			}
			Debug.d(TAG, "===>new thread");
		}
		return mInstance;
	}
	
	public DataTransferThread(Context ctx) {
		mContext = ctx;
		mPurgeLock = new ReentrantLock();
	}
	
	/**
	 * 数据更新机制：
	 * 每次发送数据时同时触发一个delay 10s的handler
	 * 如果pollState返回不为0，即数据打印完毕，则remove handlermessage
	 * 否则，处理这个message并置数据更新状态为true
	 * run函数中一旦检测到数据更新状态变为true，就重新生成buffer并下发
	 */
	


	private synchronized void next() {
		mIndex++;
		if (isLanPrint()) {
			if (!mLanBuffer.containsKey(String.valueOf(mIndex))){
				mIndex = 0;
			}
		} else {
			if (mIndex >= mDataTask.size()) {
				mIndex = 0;
			}
		}
		Debug.i(TAG, "--->next: " + mIndex);
	}

	private boolean isLanPrint() {
		return (SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_BIN);
	}

	public static synchronized void setLanBuffer(Context context, int index, char[] buffer) {
		if (mLanBuffer == null) {
			mLanBuffer = new HashMap<String, char[]>();
		}
//		Debug.i(TAG, "--->setlanBuffer: [" + index + "," + buffer.length + "]");
		mLanBuffer.put(String.valueOf(index), Arrays.copyOf(buffer, buffer.length));
		if (index == DataTransferThread.getInstance(context).index()) {
			FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);
		}
	}

	public static synchronized char[] getLanBuffer(int index) {
		if (mLanBuffer != null) {
			return mLanBuffer.get(String.valueOf(index));
		}
		return new char[2];
	}

	public static synchronized void deleteLanBuffer(int index) {
		if (mLanBuffer != null) {
			mLanBuffer.remove(String.valueOf(index));
		}
	}

	public static synchronized void cleanLanBuffer() {
		if (mLanBuffer != null) {
			mLanBuffer.clear();
		}
	}

	public synchronized int index() {
		return mIndex;
	}

	public synchronized void resetIndex() {
		mIndex = 0;
		//pcReset = true;
		char[] buffer = getLanBuffer(index());
		FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);
	}
	
	boolean needRestore = false;
	
	public void purge(final Context context) {
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
		final int headIndex = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
		PrinterNozzle head = PrinterNozzle.getInstance(headIndex);

		// H.M.Wang 修改下列两行
//		final boolean dotHd = (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT);
// H.M.Wang 2020-7-23 追加32DN打印头
//		final boolean dotHd = (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT);
		final boolean dotHd =
				(head == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
				head == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
				head == PrinterNozzle.MESSAGE_TYPE_32DN ||
				head == PrinterNozzle.MESSAGE_TYPE_64_DOT);
// End of H.M.Wang 2020-7-23 追加32DN打印头

		if (isRunning()) {
			FpgaGpioOperation.uninit();
// 2020-7-21 取消清洗时停止打印操作，改为下发适当设置参数
//			finish();
// End of 2020-7-21 取消清洗时停止打印操作，改为下发适当设置参数
			FpgaGpioOperation.clean();
			needRestore = true;
		}
		
		ThreadPoolManager.mThreads.execute(new Runnable() {
			
			@Override
			public void run() {
				DataTask task = new DataTask(context, null);
				Debug.e(TAG, "--->task: " + task + "   dotHD: " + dotHd);
				
				String purgeFile = "purge/single.bin";
				if (dotHd) {
					purgeFile = "purge/bigdot.bin";
				}
				
				char[] buffer = task.preparePurgeBuffer(purgeFile);

				if (dotHd) {
					purge(mContext, task, buffer, FpgaGpioOperation.SETTING_TYPE_PURGE1);
					return;
				}

				purge(mContext, task, buffer, FpgaGpioOperation.SETTING_TYPE_PURGE1);
//				purge(mContext, task, buffer, FpgaGpioOperation.SETTING_TYPE_PURGE2);
//
//
//				purge(mContext, task, buffer, FpgaGpioOperation.SETTING_TYPE_PURGE1);
//				purge(mContext, task, buffer, FpgaGpioOperation.SETTING_TYPE_PURGE2);
				if (needRestore) {
// 2020-7-21 取消清洗时停止打印操作，改为下发适当设置参数
//					launch(mContext);
					FpgaGpioOperation.updateSettings(mContext, mDataTask.get(mIndex), FpgaGpioOperation.SETTING_TYPE_NORMAL);
					FpgaGpioOperation.init();
					resendBufferToFPGA();
// End of 2020-7-21 取消清洗时停止打印操作，改为下发适当设置参数
					needRestore = false;
				}
				
			}
		
		});
	}
	
	private void purge(Context context, DataTask task, char[] buffer, int purgeType) {
		
		Debug.e(TAG, "--->buffer len: " + buffer.length);
		FpgaGpioOperation.updateSettings(context, task, purgeType);
		FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_PURGE, buffer, buffer.length*2);
		try {
			Thread.sleep(2500);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		FpgaGpioOperation.clean();
	}

	/**
	 *
	 * @param context
	 */
	public void clean(final Context context) {
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
		final int headIndex = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
		final PrinterNozzle head = PrinterNozzle.getInstance(headIndex);

		// H.M.Wang 修改下列两行
//		if (head != PrinterNozzle.MESSAGE_TYPE_16_DOT && head != PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//		if (head != PrinterNozzle.MESSAGE_TYPE_16_DOT && head != PrinterNozzle.MESSAGE_TYPE_32_DOT && head != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		if (head != PrinterNozzle.MESSAGE_TYPE_16_DOT &&
			head != PrinterNozzle.MESSAGE_TYPE_32_DOT &&
			head != PrinterNozzle.MESSAGE_TYPE_32DN &&
			head != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
			return;
		}

		// access lock before cleaning begin
		mPurgeLock.lock();
		ThreadPoolManager.mThreads.execute(new Runnable() {
			
			@Override
			public void run() {
				isCleaning = true;
				DataTask task = new DataTask(context, null);
				Debug.e(TAG, "--->task: " + task);
				String purgeFile = "purge/single.bin";

				// H.M.Wang 修改下列两行
//				if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//				if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
				if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_32DN || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
					purgeFile = "purge/bigdot.bin";
				}
				char[] buffer = task.preparePurgeBuffer(purgeFile);
				
				for (int i = 0; i < 20; i++) {
					Debug.e(TAG, "--->buffer len: " + buffer.length);
					
					FpgaGpioOperation.updateSettings(context, task, FpgaGpioOperation.SETTING_TYPE_PURGE1);
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_PURGE, buffer, buffer.length*2);
					try {
//						Thread.sleep(3000 * 5);
						mPurgeLock.tryLock(15, TimeUnit.SECONDS);
						break;
					} catch (InterruptedException e) {
						// e.printStackTrace();
						// mPurgeLock.unlock();
					}
					FpgaGpioOperation.clean();
					FpgaGpioOperation.updateSettings(context, task, FpgaGpioOperation.SETTING_TYPE_PURGE2);
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_PURGE, buffer, buffer.length*2);
					try {
						mPurgeLock.tryLock(15, TimeUnit.SECONDS);
						break;
					} catch (InterruptedException e) {
						// e.printStackTrace();
					}
					FpgaGpioOperation.clean();
				}
				mPurgeLock.unlock();
				isCleaning = false;
			}
			
		});
	}

	/**
	 * interrupt the cleaning task
	 */
	public void interruptClean() {
		mPurgeLock.unlock();
	}
	
	public boolean isRunning() {
		return mRunning;
	}

// H.M.Wang 2019-12-19 函数名变更，处理由分隔符分开的字符串，主要满足数据源为以太网和串口协议2的情况
// H.M.Wang 2019-12-16 将计数器和动态二维码替代部分函数化，以对应串口和网络两方面的需求
	public void setRemoteTextSeparated(final String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");
		mHandler.post(new Runnable() {
			@Override
			public void run() {
				if(null != mRemoteRecvedPromptDlg) {
					mRemoteRecvedPromptDlg.setMessage(data);
					mRemoteRecvedPromptDlg.show();		// 不知道为啥，hide之后，必须要show两次才能够及时显示出来
					mRemoteRecvedPromptDlg.show();
				}
			}
		});
		String[] recvStrs = data.split(EC_DOD_Protocol.TEXT_SEPERATOR);

		int strIndex = 0;
		boolean needUpdate = false;

		ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
		for(BaseObject baseObject: objList) {
			if(baseObject instanceof DynamicText) {
// H.M.Wang 2019-12-15 支持串口文本通过间隔符分割，对于计数器的文本如果超过位数，多余部分切割功能移至计数器Object类，不在这里处理
				if(strIndex < recvStrs.length) {
					Debug.d(TAG, "DynamicText[" + strIndex + "]: " + recvStrs[strIndex]);
					baseObject.setContent(recvStrs[strIndex]);
					strIndex++;
					needUpdate = true;
				}
			} else if(baseObject instanceof BarcodeObject) {
				if(((BarcodeObject)baseObject).isDynamicCode() && recvStrs.length >= 11) {
					Debug.d(TAG, "Dynamic QRCode: " + recvStrs[10]);
					((BarcodeObject)baseObject).setContent(recvStrs[10]);
					needUpdate = true;
				}
// End. -----
			}
		}
		mNeedUpdate = needUpdate;

// 2020-7-3 标识网络快速打印状态下数据更新
		if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FAST_LAN) {
            mDataUpdatedForFastLan = true;
		}
// End of 2020-7-3 标识网络快速打印状态下数据更新
	}
// End. -----

// H.M.Wang 2019-12-19 追加函数，处理未由分隔符分开的字符串，根据计数器的位数向前逐个填充计数器，数据不足时计数器内容为空，主要满足串口协议1
	public void setRemoteTextFitCounter(final String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");
		mHandler.post(new Runnable() {
			@Override
			public void run() {
				if(null != mRemoteRecvedPromptDlg) {
					mRemoteRecvedPromptDlg.setMessage(data);
					mRemoteRecvedPromptDlg.show();
					mRemoteRecvedPromptDlg.show();
				}
			}
		});

		int strIndex = 0;
		int counterIndex = 0;
		boolean needUpdate = false;

		ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
		for(BaseObject baseObject: objList) {
			if(baseObject instanceof DynamicText) {
				if(strIndex < data.length()) {
					int readLen = Math.min(((DynamicText)baseObject).getBits(), data.length() - strIndex);
					Debug.d(TAG, "DynamicText[" + counterIndex + "]: " + data.substring(strIndex, strIndex + readLen));
					baseObject.setContent(data.substring(strIndex, strIndex + readLen));
					strIndex += readLen;
					needUpdate = true;
				} else {
					Debug.d(TAG, "DynamicText[" + counterIndex + "]: ");
				}
				counterIndex++;
			}
		}
		mNeedUpdate = needUpdate;
	}
// End. -----

// H.M.Wang 2019-12-19 追加函数，将字符串直接付给第一个计数器，主要满足串口协议3和协议4
	public void setRemoteTextDirect(final String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");
		mHandler.post(new Runnable() {
			@Override
			public void run() {
				if(null != mRemoteRecvedPromptDlg) {
					mRemoteRecvedPromptDlg.setMessage(data);
					mRemoteRecvedPromptDlg.show();
					mRemoteRecvedPromptDlg.show();
				}
			}
		});

		boolean needUpdate = false;

		ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
		for(BaseObject baseObject: objList) {
			if(baseObject instanceof DynamicText) {
				Debug.d(TAG, "DynamicText[0]: " + data);
				baseObject.setContent(data);
				needUpdate = true;
				break;
			}
		}
		mNeedUpdate = needUpdate;
	}
// End. -----

	public void setScanDataToDt(final String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");
		mHandler.post(new Runnable() {
			@Override
			public void run() {
				if(null != mRemoteRecvedPromptDlg) {
					mRemoteRecvedPromptDlg.setMessage(data);
					mRemoteRecvedPromptDlg.show();
					mRemoteRecvedPromptDlg.show();
				}
			}
		});

// H.M.Wang 2020-6-7 修改支持包号->批号检索功能
		String[] dts = new String[8];
// End of H.M.Wang 2020-6-7 修改支持包号->批号检索功能

		dts[0] = data.substring(7, 9);
		dts[1] = data.substring(9, 11);
		dts[2] = data.substring(11, 13);

		int dt34 = 0;
		try {
			dt34 = Integer.valueOf(data.substring(14, 18));
		} catch(NumberFormatException e) {
			Debug.e(TAG, e.getMessage());
		}
		dt34 += SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_PARAM_63);
		dts[3] = String.valueOf(dt34/10);
		dts[4] = String.valueOf(dt34%10);

		dts[5] = data.substring(22, 23);
		dts[6] = data.substring(26, 32);

// H.M.Wang 2020-6-7 修改支持包号->批号检索功能
		PackageListReader plr = PackageListReader.getInstance(mContext);
		dts[7] = plr.getBatchCode(dts[6]);
		if(null == dts[7]) dts[7] = "";
// End of H.M.Wang 2020-6-7 修改支持包号->批号检索功能

		for(DataTask dataTask : mDataTask) {
			ArrayList<BaseObject> objList = dataTask.getObjList();
			for(BaseObject baseObject: objList) {
				if(baseObject instanceof DynamicText) {
					int dtIndex = ((DynamicText)baseObject).getDtIndex();
// H.M.Wang 2020-6-7 修改支持包号->批号检索功能
					if(dtIndex >= 0 && dtIndex < 8) {
// End of H.M.Wang 2020-6-7 修改支持包号->批号检索功能
						baseObject.setContent(dts[dtIndex]);
						mNeedUpdate = true;
					}
				}
			}
		}
	}
	// End. -----
// H.M.Wang 2020-6-9 追加串口6协议
	public void setSP6DataToDt(final String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");
		mHandler.post(new Runnable() {
			@Override
			public void run() {
				if (null != mRemoteRecvedPromptDlg) {
					mRemoteRecvedPromptDlg.setMessage(data);
					mRemoteRecvedPromptDlg.show();
					mRemoteRecvedPromptDlg.show();
				}
			}
		});
// H.M.Wang 2020-6-7 修改支持包号->批号检索功能
		String[] dts = new String[6];
// End of H.M.Wang 2020-6-7 修改支持包号->批号检索功能

		dts[0] = data.substring(7, 8);
		dts[1] = data.substring(8, 9);
		dts[2] = data.substring(9, 10);
		dts[3] = data.substring(10, 11);
		dts[4] = data.substring(12, 13);
		dts[5] = data.substring(13, 14);

		for(DataTask dataTask : mDataTask) {
			ArrayList<BaseObject> objList = dataTask.getObjList();
			for(BaseObject baseObject: objList) {
				if(baseObject instanceof DynamicText) {
					int dtIndex = ((DynamicText)baseObject).getDtIndex();
					if(dtIndex >= 0 && dtIndex < 6) {
						baseObject.setContent(dts[dtIndex]);
						mNeedUpdate = true;
					}
				}
			}
		}
	}
// End of H.M.Wang 2020-6-9 追加串口6协议

	private AlertDialog mRemoteRecvedPromptDlg = null;

	public boolean launch(Context ctx) {
		// H.M.Wang 2019-12-31 设置mContext，以避免因为mContext=null而导致程序崩溃
		mContext = ctx;
		// End of H.M.Wang 2019-12-31 设置mContext，以避免因为mContext=null而导致程序崩溃

		// H.M.Wang 2019-12-19 支持多种串口协议的修改
		// H.M.Wang 2019-10-23 串口发送数据支持
		final SerialHandler serialHandler = SerialHandler.getInstance();
		serialHandler.setPrintCommandListener(new SerialHandler.OnSerialPortCommandListenner() {
			@Override
			public void onCommandReceived(int cmd, byte[] data) {
				if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {

					if (cmd == EC_DOD_Protocol.CMD_TEXT) {                         // 发送一条文本	0x0013
						ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
						for (BaseObject baseObject : objList) {
							if (baseObject instanceof DynamicText) {
								baseObject.setContent("");
							}
						}

						String datastring = new String(data, 7, data.length - 7);
						if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1) {
							setRemoteTextFitCounter(datastring);
						} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {
							setRemoteTextSeparated(datastring);
						}
						serialHandler.sendCommandProcessResult(EC_DOD_Protocol.CMD_TEXT, 1, 0, 0, "");
						// H.M.Wang 2019-12-7 反转命令立即生效
					} else if (cmd == EC_DOD_Protocol.CMD_SET_REVERSE) {
						mNeedUpdate = true;
					}
				} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3) {
					String datastring = new String(data, 0, data.length);
					setRemoteTextDirect(datastring);
					serialHandler.sendCommandProcessResult(SerialProtocol.ERROR_SUCESS, 1, 0, 0, datastring + " set.");
				} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_4) {
					String datastring = new String(data, 0, data.length);
					setRemoteTextDirect(datastring);
					serialHandler.sendCommandProcessResult(SerialProtocol.ERROR_SUCESS, 1, 0, 0, datastring + " set.");
				} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_5) {
					String datastring = new String(data, 0, data.length);
					setScanDataToDt(datastring);
					serialHandler.sendCommandProcessResult(SerialProtocol.ERROR_SUCESS, 1, 0, 0, datastring + " set.");
// H.M.Wang 2020-6-9 追加串口6协议
				} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_6) {
					String datastring = new String(data, 0, data.length);
					setSP6DataToDt(datastring);
					serialHandler.sendCommandProcessResult(SerialProtocol.ERROR_SUCESS, 1, 0, 0, datastring + " set.");
// End of H.M.Wang 2020-6-9 追加串口6协议
// H.M.Wang 2020-8-13 追加串口7协议
				} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_7) {
					if (cmd == SerialProtocol7.CMD_TEXT) {                         // 发送一条文本	0x0013
						ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
						for (BaseObject baseObject : objList) {
							if (baseObject instanceof DynamicText) {
								baseObject.setContent("");
							}
						}

						Debug.d(TAG, "CMD = " + cmd + "; Data = [" + data + "]");
						String datastring = new String(data, 7, data.length - 7);
						Debug.d(TAG, "CMD = " + cmd + "; Data = [" + data + "]");
						setRemoteTextFitCounter(datastring);
						Debug.d(TAG, "CMD = " + cmd + "; Data = [" + data + "]");
						serialHandler.sendCommandProcessResult(EC_DOD_Protocol.CMD_TEXT, 1, 0, 0, "");
					}
// End of H.M.Wang 2020-8-13 追加串口7协议
				}
			}
		});

		// End of H.M.Wang 2019-12-19 支持多种串口协议的修改

		BarcodeScanParser.setListener(new BarcodeScanParser.OnScanCodeListener() {
			@Override
			public void onCodeReceived(String code) {
				setScanDataToDt(code);
			}
		});

		AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
		mRemoteRecvedPromptDlg = builder.setTitle(R.string.strRecvedRemote).setMessage("").setPositiveButton(R.string.str_ok, new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				mRemoteRecvedPromptDlg.hide();
			}
		}).create();
// H.M.Wang 2020-6-3 解决提示对话窗在显示时，扫码枪的信息被其劫持，而无法识别的问题
        mRemoteRecvedPromptDlg.setOnKeyListener(new DialogInterface.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
				if(event.getAction() == KeyEvent.ACTION_DOWN) {
					if(keyCode == KeyEvent.KEYCODE_ENTER) {
						return true;
					} else {
						BarcodeScanParser.append(event.getDisplayLabel());
					}
				}
                return false;
            }
        });
// End of H.M.Wang 2020-6-3 解决提示对话窗在显示时，扫码枪的信息被其劫持，而无法识别的问题

		mRunning = true;

		mPrinter = new PrintTask();
		if (!isBufferReady || mDataTask == null) {
			return false;
		}

		mPrinter.start();
//		thread.start();
		return true;
	}
	
	public void finish() {
		mRunning = false;

		if(null != mRemoteRecvedPromptDlg) {
			mRemoteRecvedPromptDlg.dismiss();
			mRemoteRecvedPromptDlg = null;
		}
		// H.M.Wang 2019-10-23 串口发送数据支持
		SerialHandler serialHandler =  SerialHandler.getInstance();
		serialHandler.setPrintCommandListener(null);
		// End --------------------

		BarcodeScanParser.setListener(null);

		PrintTask t = mPrinter;
		mPrinter = null;
		mHandler.removeMessages(MESSAGE_DATA_UPDATE);
		if (t != null) {
			t.interrupt();
		}
		if (mScheduler == null) {
			return;
		}
		mScheduler.doAfterPrint();
	}
	
	public void setOnInkChangeListener(InkLevelListener listener) {
		mInkListener = listener;
	}
	
	
	public static final int MESSAGE_DATA_UPDATE = 1;
	
	public Handler mHandler = new Handler(){
		
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
				case MESSAGE_DATA_UPDATE:
					mNeedUpdate = true;
					break;
			}
		}
	};

	public void resetTask(List<MessageTask> task) {
		synchronized (DataTransferThread.class) {
			mIndex = 0;
			mDataTask.clear();
			for (MessageTask t : task) {
				DataTask data = new DataTask(mContext, t);
				mDataTask.add(data);
				data.prepareBackgroudBuffer();
			}
			setDotCount(task);
			mNeedUpdate = true;
		}
	}

	public void initDataBuffer(Context context, List<MessageTask> task) {
		if (mDataTask == null) {
			mDataTask = new ArrayList<DataTask>();
		}
		mIndex = 0;
		mDataTask.clear();
		for (MessageTask t : task) {
			DataTask data = new DataTask(mContext, t);
			mDataTask.add(data);
		}
		Debug.d(TAG, "--->prepare buffer: " + mDataTask.size());

		for (DataTask tk : mDataTask) {
			isBufferReady |= tk.prepareBackgroudBuffer();
		}

		if (mScheduler == null) {
			mScheduler = InkSchedulerFactory.getScheduler(mContext);
		}

		SystemConfigFile configFile = SystemConfigFile.getInstance(context);

		int headIndex = configFile.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
		int heads = PrinterNozzle.getInstance(headIndex).mHeads;
		/**如果是4合2的打印头，需要修改为4头*/
		mScheduler.init(heads * configFile.getHeadFactor());
//		heads = configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0 ? configFile.getParam(42) : heads;

	}


	public List<DataTask> getData() {
		synchronized (DataTransferThread.class) {
			return mDataTask;
		}
	}

	public DataTask getCurData() {
		return mDataTask.get(index());
	}

	public void setDotCount(List<MessageTask> messages) {
		if (isLanPrint()) return;
		for (int i = 0; i < mDataTask.size(); i++) {
			DataTask t = mDataTask.get(i);
			if (messages.size() <= i) {
				break;
			}
			int[] dots = messages.get(i).getDots();
			int totalDot = 0;
			for (int j = 0; j < dots.length; j++) {
				totalDot += dots[j];
			}
			t.setDots(totalDot);
			t.setDotsEach(dots);
		}
		initCount();
	}
	
	public int getDotCount(DataTask task, int head) {
		if (task == null) {
			Debug.e(TAG, "---> task is null");
			return 1;
		}
			
		return task.getDots(head);
	}
	
	
	public void initCount() {
		if (mcountdown == null) {
			mcountdown = new int[8];
// H.M.Wang 2019-10-10 取消内部初始化0的操作，这样不全面
//		} else {
//			for (int i = 0; i < mcountdown.length; i++) {
//				mcountdown[i] = 0;
//			}
		}

		// H.M.Wang 初始化0的部分移到外部，这样更全面
		for (int i = 0; i < mcountdown.length; i++) {
			mcountdown[i] = 0;
		}

		// H.M.Wang 2019-10-10 注释掉添加初值的部分，如果初值为0，则表示该处置还没有初始化，待后续计算后添加
//		for (int i = 0; i < mcountdown.length; i++) {
//			mcountdown[i] = getInkThreshold(i);
			//Debug.d(TAG, "--->initCount countdown[" + i + "] = " + mcountdown[i]);
//		}
	}
	/**
	 * 倒计数，当计数倒零时表示墨水量需要减1，同时倒计数回归
	 * @return true 墨水量需要减1； false 墨水量不变
	 */
	private void countDown() {
		for (int i = 0; i < mScheduler.count(); i++) {
			// H.M.Wang 2019-10-10 添加初值是否为0的判断，如果为0，则判定为还没有初始化，首先进行初始化
			if(mcountdown[i] == 0) mcountdown[i] = getInkThreshold(i);

			Debug.d(TAG, "mCountDown[" + i + "] = " + mcountdown[i]);
			mcountdown[i]--;
			if (mcountdown[i] <= 0) {
				// 赋初值
				mcountdown[i] = getInkThreshold(i);
				mInkListener.onInkLevelDown(i);
			}
		}
	}
	
	public int getCount(int head) {
		if (mcountdown == null) {
			initCount();
		}
		return mcountdown[head];
	}
	
	/**
	 * 通过dot count计算RFID减1的阀值
	 * @param head 喷头索引
	 * @return
	 */
	public int getInkThreshold(int head) {
		//if (isLanPrint()) return 1;
		int bold = 1;
		int index = isLanPrint() ? 0 : index();

//		int dotCount = getDotCount(mDataTask.get(index), head);
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
// H.M.Wang2019-9-28 考虑1带多的情况
//		int one2multiple = SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_ONE_MULTIPLE);
//		if( one2multiple == 2 || 		// 1带2
//			one2multiple == 3 || 		// 1带3
//			one2multiple == 4  			// 1带4
//		) {
//			dotCount = getDotCount(mDataTask.get(index), 0);		// 使用第一个头的数据
//		}

// Kevin.Zhao 2019-11-12 1带多用12，13，14表示1带2，1带3，1带4....
		int dotCount = getDotCount(mDataTask.get(index), config.getMainHeads(head));

		Debug.d(TAG, "--->dotCount[" + head + "]: " + dotCount + "  bold=" + bold);

//		dotCount = dotCount/config.getHeadFactor();		// ??????为什么要除以头数
		// Debug.d(TAG, "--->getInkThreshold  head: " + head + "   index = " + index + " dataTask: " + mDataTask.size());
		if (dotCount <= 0) {
// H.M.Wang 2019-9-28 当该打印头没有数据可打印的时候，原来返回1，会产生错误效果，返回一个尽量大的数以避免之
//			return 1;
			return 65536 * 8;						// 无打印内容时可能错误减记
		}

		if (config.getParam(SystemConfigFile.INDEX_PRINT_DENSITY) <= 0) {
			bold = 1;
		} else {
// H.M.Wang 2020-6-12 16,32,64点头减锁修改为不受分辨率影响
			final int headIndex = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
			final PrinterNozzle hType = PrinterNozzle.getInstance(headIndex);
// H.M.Wang 2020-7-23 追加32DN打印头
//			if (hType != PrinterNozzle.MESSAGE_TYPE_16_DOT && hType != PrinterNozzle.MESSAGE_TYPE_32_DOT && hType != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
			if (hType != PrinterNozzle.MESSAGE_TYPE_16_DOT &&
				hType != PrinterNozzle.MESSAGE_TYPE_32_DOT &&
				hType != PrinterNozzle.MESSAGE_TYPE_32DN &&
				hType != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
				bold = config.getParam(SystemConfigFile.INDEX_PRINT_DENSITY)/150;
			} else {
				bold = 1;
			}
// End of H.M.Wang 2020-6-12 16,32,64点头减锁修改为不受分辨率影响
		}

// H.M.Wang 2020-4-19 追加12.7R5头。dotcount放大相应倍数
// H.M.Wang 2020-5-9 12.7R5d打印头类型不参与信息编辑，因此不通过信息的打印头类型判断其是否为12.7R5的信息，而是通过参数来规定现有信息的打印行为
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//		if(config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_12_7_R5) {
		if(config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X48 ||
			config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X50) {
//		if(mDataTask.get(index).getPNozzle() == PrinterNozzle.MESSAGE_TYPE_12_7_R5) {
// End of H.M.Wang 2020-5-9 12.7R5d打印头类型不参与信息编辑，因此不通过信息的打印头类型判断其是否为12.7R5的信息，而是通过参数来规定现有信息的打印行为
            dotCount *= (PrinterNozzle.R6_PRINT_COPY_NUM - 1);
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
        }
// End of H.M.Wang 2020-4-19 追加12.7R5头。dotcount放大相应倍数

		Debug.d(TAG, "--->dotCount[" + head + "]: " + dotCount + "  bold=" + bold);
		return Configs.DOTS_PER_PRINT/(dotCount * bold);
	}
	
	public int getHeads() {
		if (mDataTask != null && mDataTask.size() > 0) {
			return mDataTask.get(0).getPNozzle().mHeads;
		}
		return 1;
	}
	/**
	 * 打印間隔0~100ms（每秒鐘打印 > 20次），爲高速打印，每個打印間隔只執行1步操作
	 * 打印間隔100~200ms（每秒鐘打印 > 20次），爲高速打印，每個打印間隔只執行2步操作
	 * 打印間隔200~500ms（每秒鐘打印 > 20次），爲高速打印，每個打印間隔只執行4步操作
	 * 打印間隔500~1000ms（每秒鐘打印 > 20次），爲高速打印，每個打印間隔只執行8步操作
	 * @return
	 */
	public static long getInterval() {
		if (mInterval >= 1000) {
			return 8;
		} else if (mInterval >= 500) {
			return 4;
		} else if (mInterval >= 200) {
			return 2;
		} else {
			return 1;
		}
	}

	
	private Callback mCallback;
	
	public void setCallback(Callback callback) {
		mCallback = callback;
	}
	
	
	public interface Callback {
		/**
		 * 整個任務打印完成
		 */
		public void OnFinished(int code);
		/**
		 * 一個任務打印完成
		 */
		public void onComplete(int index);

		void onPrinted(int index);

        void onPrint(int index);
	}
	
	public static final int CODE_BARFILE_END = 1;
	public static final int CODE_NO_BARFILE = 2;
	public static final int CODE_PRINT_FAILED = 3;

//	private char[] getPrintBuffer() {
//		char[] buffer;
//		int htype = getHeads();
//		// specific process for 9mm header
//		if (htype == MessageType.MESSAGE_TYPE_9MM) {
//			int columns = mDataTask.getBufferColumns();
//			int h = mDataTask.getBufferHeightFeed();
//			char[] b1 = mDataTask.getPrintBuffer();
//			char[] b2 = mDataTask.getPrintBuffer();
//			char[] b3 = mDataTask.getPrintBuffer();
//			char[] b4 = mDataTask.getPrintBuffer();
//			char[] b5 = mDataTask.getPrintBuffer();
//			char[] b6 = mDataTask.getPrintBuffer();
//			buffer = new char[columns * h * 6];
//			for (int i = 0; i < columns; i++) {
//				System.arraycopy(b1, i * h, buffer, i * h *6, h);
//				System.arraycopy(b2, i * h, buffer, i * h * (6 + 1), h);
//				System.arraycopy(b3, i * h, buffer, i * h * (6 + 2), h);
//				System.arraycopy(b4, i * h, buffer, i * h * (6 + 3), h);
//				System.arraycopy(b5, i * h, buffer, i * h * (6 + 4), h);
//				System.arraycopy(b6, i * h, buffer, i * h * (6 + 5), h);
//			}
//		} else {
//			buffer = mDataTask.getPrintBuffer();
//		}
//		return buffer;
//	}

// H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整
	private void setCounterNext() {
		Debug.d(TAG, "--->setCounterNext");
		if (mDataTask == null) {
			return;
		}
		for (DataTask task : mDataTask) {
			for (BaseObject object : task.getObjList()) {
				if (object instanceof CounterObject) {
					((CounterObject) object).goNext();
// H.M.Wang 2020-7-31 追加超文本及条码当中超文本的计数器打印后调整
				} else if (object instanceof HyperTextObject) {
					((HyperTextObject) object).goNext();
				} else if (object instanceof BarcodeObject) {
					((BarcodeObject) object).goNext();
// End of H.M.Wang 2020-7-31 追加超文本及条码当中超文本的计数器打印后调整
				}
			}
		}
	}
// End of H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整

// H.M.Wang 2020-5-6 参数设置页面，在未修改计数器的情况下，点击OK保存后，计数器会跳数，分析是因为设置了	mNeedUpdate=true，重新生成打印缓冲区，重新生成时计数器会自动增量，所以增加了一个重新下发的函数，而不重新生成缓冲区
	public void resendBufferToFPGA() {
		if(null != mPrintBuffer) {
			FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, mPrintBuffer, mPrintBuffer.length * 2);
		}
	}

	char[] mPrintBuffer = null;
// H.M.Wang 2020-5-6 参数设置页面，...

	private static int mPrintCount = 0;

// 2020-6-29 处于打印状态时，如果用户确认设置，需要向FPGA下发设置内容，按一定原则延迟下发
	public long Time1 = 0;
	public long Time2 = 0;
// 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
	public int  DataRatio = 0;
// End of 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
// End of 2020-6-29 处于打印状态时，如果用户确认设置，需要向FPGA下发设置内容，按一定原则延迟下发

// 2020-6-30 追加是否为网络快速打印的第一次数据生成标识
	private boolean mFirstForLanFast = false;
// End of 2020-6-30 追加是否为网络快速打印的第一次数据生成标识

// 2020-7-3 追加网络快速打印状态下数据是否更新的标识
	private boolean mDataUpdatedForFastLan = false;
// End of 2020-7-3 追加网络快速打印状态下数据是否更新的标识

// H.M.Wang 2020-7-9 追加计数器重置标识
	public boolean mCounterReset=false;
// End of H.M.Wang 2020-7-9 追加计数器重置标识

	public class PrintTask extends Thread {

		@Override
		public void run() {
// 2020-6-29 处于打印状态时，如果用户确认设置，需要向FPGA下发设置内容，按一定原则延迟下发
			Time1 = System.currentTimeMillis();
			Time2 = System.currentTimeMillis();
// End of 2020-6-29 处于打印状态时，如果用户确认设置，需要向FPGA下发设置内容，按一定原则延迟下发

// 2020-6-30 网络快速打印的第一次数据生成标识设真
			if(SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FAST_LAN) {
				mFirstForLanFast = true;
// H.M.Wang 2020-8-4 这个不设的话，可能上次停止打印时接收到数据后，mDataUpdatedForFastLan被设成true了，导致后面生成打印缓冲区误操作
				mDataUpdatedForFastLan = false;
// End of H.M.Wang 2020-8-4 这个不设的话，可能上次停止打印时接收到数据后，mDataUpdatedForFastLan被设成true了，导致后面生成打印缓冲区误操作
			}
// End of 2020-6-30 网络快速打印的第一次数据生成标识设真

			long last = 0;
			/*逻辑要求，必须先发数据*/
			Debug.d(TAG, "--->print run");

			mPrintBuffer = mDataTask.get(index()).getPrintBuffer(true);
			if (isLanPrint()) {
				setLanBuffer(mContext, index(), mPrintBuffer);
			} else {
				Debug.d(TAG, "--->print buffer ready");
				// H.M.Wang 2019-10-10 追加计算打印区对应于各个头的打印点数
				DataTask t = mDataTask.get(index());
//				Debug.d(TAG, "GetPrintDots Start Time: " + System.currentTimeMillis());
/*				int[] dots = NativeGraphicJni.GetPrintDots(mPrintBuffer, mPrintBuffer.length, t.getInfo().mCharsPerHFeed, t.getPNozzle().mHeads);

				int totalDot = 0;
				for (int j = 0; j < dots.length; j++) {
					// H.M.Wang 2019-10-11 获得的点数乘2
					SystemConfigFile config = SystemConfigFile.getInstance(mContext);
					final int headIndex = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
					final PrinterNozzle head = PrinterNozzle.getInstance(headIndex);

					if (head != PrinterNozzle.MESSAGE_TYPE_16_DOT && head != PrinterNozzle.MESSAGE_TYPE_32_DOT && head != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
						dots[j] *= 2;
					} else {
						dots[j] *= 200;
					}
					totalDot += dots[j];
				}
				t.setDots(totalDot);
				t.setDotsEach(dots);
*/
//				Debug.d(TAG, "GetPrintDots Done Time: " + System.currentTimeMillis());

				final StringBuilder sb = new StringBuilder();
				sb.append("Dots per Head: [");
				for (int i=0; i<8; i++) {
					if(i != 0) {
						sb.append(", ");
					}
// 2020-5-11
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//					if(config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_12_7_R5) {
					if(SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X48 ||
						SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X50) {
//						sb.append(t.getDots(i<6?0:i)*(PrinterNozzle.R5x6_PRINT_COPY_NUM - 1));
						sb.append(t.getDots(i<6?0:i)*(PrinterNozzle.R6_PRINT_COPY_NUM - 1));
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
					} else {
						sb.append(t.getDots(i));
					}
// 2020-5-11
				}
				sb.append("]");
				Debug.d(TAG, sb.toString());
				mHandler.post(new Runnable() {
					@Override
					public void run() {
						Toast.makeText(mContext, sb.toString(), Toast.LENGTH_LONG).show();
					}
				});

// 2020-6-30 网络快速打印的第一次数据生成后不下发
				if(!mFirstForLanFast) {
					Debug.e(TAG, "--->write data");
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, mPrintBuffer, mPrintBuffer.length * 2);
// 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
					DataRatio = (mPrintBuffer.length * 2 - 1) / (16 * 1024);
// End of 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
				}
// End of 2020-6-30 网络快速打印的第一次数据生成后不下发

// H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号
// H.M.Wang 2020-6-28 追加向网络发送打印数据缓冲区准备完成消息
				if(null != mCallback && mDataTask.size() > 1) {
					mCallback.onPrint(index());
//                    mCallback.onComplete(index());
				}
// H.M.Wang 2020-7-6 原来放在前面这个判断里面是给群组用的，不是群组不会起作用，移到这里
                if(SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FAST_LAN) {
                    if(null != mCallback) {
                        mCallback.onComplete(index());
                    }
                }
// End of H.M.Wang 2020-7-6 原来放在前面这个判断里面是给群组用的，不是群组不会起作用，移到这里
// End of H.M.Wang 2020-6-28 追加向网络发送打印数据缓冲区准备完成消息
// End of H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号

				/**  save log */
				LogIntercepter.getInstance(mContext).execute(getCurData());

			}
			last = SystemClock.currentThreadTimeMillis();
			FpgaGpioOperation.init();

			boolean isFirst = true;
			while(mRunning == true) {

				int writable = FpgaGpioOperation.pollState();

				if (writable == 0) { //timeout
//					Debug.e(TAG, "--->FPGA timeout");
//					if (isLanPrint() && pcReset == true) {
//						buffer = getLanBuffer(index());
//						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);
//						pcReset = false;
//					}
				} else if (writable == -1) {
//					Debug.e(TAG, "--->FPGA error");
				} else {
// 2020-7-3 在网络快速打印状态下，如果没有接收到新的数据，即使触发也不生成新的打印缓冲区下发
					if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FAST_LAN) {
						if(!mDataUpdatedForFastLan) {
							continue;
						}
                        mDataUpdatedForFastLan = false;
					}
// End of 2020-7-3 在网络快速打印状态下，如果没有接收到新的数据，即使触发也不生成新的打印缓冲区下发
					Debug.d(TAG, "--->FPGA buffer is empty");
					Time1 = Time2;
					Time2 = System.currentTimeMillis();
					if (mCallback != null) {
						mCallback.onPrinted(index());
					}
					mInterval = SystemClock.currentThreadTimeMillis() - last;
					mHandler.removeMessages(MESSAGE_DATA_UPDATE);
					mNeedUpdate = false;

// H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整，取消从前在生成打印缓冲区时调整
					setCounterNext();
// End of H.M.Wang 2020-7-2 调整计数器增量策略

					synchronized (DataTransferThread.class) {
////////////////////////////////////////////////////////
						IInkDevice id = InkManagerFactory.inkManager(mContext);
						if(id instanceof SmartCardManager) {
							if(mPrintCount == 0) {
								mPrintCount = 10;
								((SmartCardManager) id).updateLevel();
							}
							mPrintCount--;
						}
////////////////////////////////////////////////////////
						next();
						if (isLanPrint()) {
							mPrintBuffer = getLanBuffer(index());
							Debug.i(TAG, "--->mPrintBuffer.length: " + mPrintBuffer.length);
						} else {
// H.M.Wang 2020-5-19 QR文件打印最后一行后无反应问题。应该先生成打印缓冲区，而不是先判断是否到了终点。顺序不对
							Debug.i(TAG, "mIndex: " + index());
							mPrintBuffer = mDataTask.get(index()).getPrintBuffer(false);
							if (!mDataTask.get(index()).isReady) {
								if (mCallback != null) {
									mCallback.OnFinished(CODE_BARFILE_END);
								}
								break;
							}
//							Debug.i(TAG, "mIndex: " + index());
//							mPrintBuffer = mDataTask.get(index()).getPrintBuffer();
// End of H.M.Wang 2020-5-19 QR文件打印最后一行后无反应问题。应该先生成打印缓冲区，而不是先判断是否到了终点。顺序不对
						}
						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, mPrintBuffer, mPrintBuffer.length * 2);
// 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						DataRatio = (mPrintBuffer.length * 2 - 1) / (16 * 1024);
// End of 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
                        Debug.d(TAG, "--->FPGA data sent!");

// H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号
						if(mCallback != null && mDataTask.size() > 1) {
							mCallback.onPrint(index());
						}
// End of H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号

						last = SystemClock.currentThreadTimeMillis();
						countDown();
						mInkListener.onCountChanged();
						mScheduler.schedule();
						if (mCallback != null) {
							mCallback.onComplete(index());
						}

						/**  save log */
						LogIntercepter.getInstance(mContext).execute(getCurData());
					}
				}

				if(mNeedUpdate == true) {
// H.M.Wang 2020-6-28 修改打印数据缓冲区更新策略，当网络快速打印的时候不再根据数据更新重新生成打印缓冲区
					if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) != SystemConfigFile.DATA_SOURCE_FAST_LAN) {
// End of H.M.Wang 2020-6-28 修改打印数据缓冲区更新策略，当网络快速打印的时候不再根据数据更新重新生成打印缓冲区
						mHandler.removeMessages(MESSAGE_DATA_UPDATE);
					//在此处发生打印数据，同时
// H.M.Wang 2019-12-29 在重新生成打印缓冲区的时候，考虑网络打印的因素
						if (isLanPrint()) {
							mPrintBuffer = getLanBuffer(index());
						} else {
// H.M.Wang 2020-6-24 修改重新生成打印缓冲区的时候计数器自动增值和读条码文件下一条的问题
//							mPrintBuffer = mDataTask.get(index()).getPrintBuffer(false);
							mPrintBuffer = mDataTask.get(index()).getPrintBuffer(true, false);
// End of H.M.Wang 2020-6-24 修改重新生成打印缓冲区的时候计数器自动增值和读条码文件下一条的问题
						}
// End of H.M.Wang 2019-12-29 在重新生成打印缓冲区的时候，考虑网络打印的因素
						Debug.d(TAG, "===>mPrintBuffer size=" + mPrintBuffer.length);
						// H.M.Wang 2019-12-20 关闭print.bin保存
//						// H.M.Wang 2019-12-17 每次重新生成print内容后，都保存print.bin
//						BinCreater.saveBin("/mnt/sdcard/print.bin", buffer, mDataTask.get(mIndex).getInfo().mBytesPerHFeed * 8 * mDataTask.get(mIndex).getPNozzle().mHeads);
//						// End.
						try {sleep(30);}catch(Exception e){};
						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, mPrintBuffer, mPrintBuffer.length * 2);
// 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						DataRatio = (mPrintBuffer.length * 2 - 1) / (16 * 1024);
// End of 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						mHandler.sendEmptyMessageDelayed(MESSAGE_DATA_UPDATE, MESSAGE_EXCEED_TIMEOUT);
						mNeedUpdate = false;
// 2020-6-30 网络快速打印时第一次收到网络数据后下发
// 2020-7-3 追加判断是否有网络快速打印数据更新
//					} else if(mFirstForLanFast) {
					} else if(mFirstForLanFast && mDataUpdatedForFastLan) {
// End of 2020-7-3 追加判断是否有网络快速打印数据更新
						mPrintBuffer = mDataTask.get(index()).getPrintBuffer(true, false);
						Debug.d(TAG, "First print buffer deliver to FPGA. size = " + mPrintBuffer.length);
						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, mPrintBuffer, mPrintBuffer.length * 2);
// 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						DataRatio = (mPrintBuffer.length * 2 - 1) / (16 * 1024);
// End of 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						mNeedUpdate = false;
						mFirstForLanFast = false;
// H.M.Wang 2020-7-9 解决开始打印后，首次内容被打印两次的问题
                        mDataUpdatedForFastLan = false;
// End of H.M.Wang 2020-7-9 解决开始打印后，首次内容被打印两次的问题
// H.M.Wang 2020-7-9 解决开始打印后，仅打印首次内容一次的问题
						if (mCallback != null) {
							mCallback.onComplete(index());
						}
// End of H.M.Wang 2020-7-9 解决开始打印后，仅打印首次内容一次的问题
// H.M.Wang 2020-7-9 追加计数器重置标识
					} else if(mCounterReset) {
						mPrintBuffer = mDataTask.get(index()).getPrintBuffer(true, false);
						Debug.d(TAG, "Counter reset. rebuild print buffer and deliver to FPGA. size = " + mPrintBuffer.length);
						try {sleep(30);}catch(Exception e){};
						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, mPrintBuffer, mPrintBuffer.length * 2);
// 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						DataRatio = (mPrintBuffer.length * 2 - 1) / (16 * 1024);
// End of 2020-7-21 为修改计算等待时间添加倍率变量（新公式为：N=(打印缓冲区字节数-1）/16K；时长=3/(2N+4)
						mNeedUpdate = false;
						mCounterReset = false;
// End of H.M.Wang 2020-7-9 追加计数器重置标识
// H.M.Wang 2020-7-14 设置计数器后再次向PC发送0001，要数据
                        if (mCallback != null) {
                            mCallback.onComplete(index());
                        }
// End of H.M.Wang 2020-7-14 设置计数器后再次向PC发送0001，要数据
					}
// End of 2020-6-30 网络快速打印时第一次收到网络数据后下发
				}
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				//Debug.d(TAG, "===>kernel buffer empty, fill it");
				//TO-DO list 下面需要把打印数据下发
			}
// H.M.Wang 2020-7-2 由于调整计数器增量策略，在打印完成时调整，因此无需rollback
//			rollback();
// End of H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整，因此无需rollback
		}
	}
}
