package com.industry.printer;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;

import com.industry.printer.Constants.Constants;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Rfid.RfidScheduler;
import com.industry.printer.Rfid.RfidTask;
import com.industry.printer.Serial.EC_DOD_Protocol;
import com.industry.printer.Serial.SerialHandler;
import com.industry.printer.Serial.SerialProtocol3;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.DataTask;
import com.industry.printer.data.NativeGraphicJni;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.Serial.SerialPort;
import com.industry.printer.interceptor.ExtendInterceptor;
import com.industry.printer.interceptor.ExtendInterceptor.ExtendStat;
import com.industry.printer.interceptor.LogIntercepter;
import com.industry.printer.object.BarcodeObject;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;

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
	RfidScheduler	mScheduler;
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
		final boolean dotHd = (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT);

		if (isRunning()) {
			FpgaGpioOperation.uninit();
			finish();
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
					launch(mContext);
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
		if (head != PrinterNozzle.MESSAGE_TYPE_16_DOT && head != PrinterNozzle.MESSAGE_TYPE_32_DOT && head != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
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
				if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
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
	public void setRemoteTextSeperated(String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");
		String[] recvStrs = data.split(EC_DOD_Protocol.TEXT_SEPERATOR);

		int strIndex = 0;
		boolean needUpdate = false;

		ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
		for(BaseObject baseObject: objList) {
			if(baseObject instanceof CounterObject) {
// H.M.Wang 2019-12-15 支持串口文本通过间隔符分割，对于计数器的文本如果超过位数，多余部分切割功能移至计数器Object类，不在这里处理
				if(strIndex < recvStrs.length) {
					Debug.d(TAG, "Counter[" + strIndex + "]: " + recvStrs[strIndex]);
					((CounterObject)baseObject).setRemoteContent(recvStrs[strIndex]);
					strIndex++;
				}
				needUpdate = true;
			} else if(baseObject instanceof BarcodeObject) {
				if(((BarcodeObject)baseObject).isDynamicQRCode() && recvStrs.length >= 11) {
					Debug.d(TAG, "Dynamic QRCode: " + recvStrs[10]);
					((BarcodeObject)baseObject).setContent(recvStrs[10]);
				}
// End. -----
			}
		}
		mNeedUpdate = needUpdate;
	}
// End. -----

// H.M.Wang 2019-12-19 追加函数，处理未由分隔符分开的字符串，根据计数器的位数向前逐个填充计数器，数据不足时计数器内容为空，主要满足串口协议1
	public void setRemoteTextFitCounter(String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");

		int strIndex = 0;
		int counterIndex = 0;
		boolean needUpdate = false;

		ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
		for(BaseObject baseObject: objList) {
			if(baseObject instanceof CounterObject) {
				if(strIndex < data.length()) {
					int readLen = Math.min(((CounterObject)baseObject).getBits(), data.length() - strIndex);
					Debug.d(TAG, "Counter[" + counterIndex + "]: " + data.substring(strIndex, strIndex + readLen));
					((CounterObject)baseObject).setRemoteContent(data.substring(strIndex, strIndex + readLen));
					strIndex += readLen;
					needUpdate = true;
				} else {
					Debug.d(TAG, "Counter[" + counterIndex + "]: ");
				}
				counterIndex++;
			}
		}
		mNeedUpdate = needUpdate;
	}
// End. -----

// H.M.Wang 2019-12-19 追加函数，将字符串直接付给第一个计数器，主要满足串口协议3
	public void setRemoteTextDirect(String data) {
		Debug.d(TAG, "String from Remote = [" + data + "]");

		boolean needUpdate = false;

		ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
		for(BaseObject baseObject: objList) {
			if(baseObject instanceof CounterObject) {
				Debug.d(TAG, "Counter[0]: " + data);
				((CounterObject)baseObject).setRemoteContent(data);
				needUpdate = true;
				break;
			}
		}
		mNeedUpdate = needUpdate;
	}
// End. -----

	public boolean launch(Context ctx) {
		// H.M.Wang 2019-12-31 设置mContext，以避免因为mContext=null而导致程序崩溃
		mContext = ctx;
		// End of H.M.Wang 2019-12-31 设置mContext，以避免因为mContext=null而导致程序崩溃

		// H.M.Wang 2019-12-19 支持多种串口协议的修改
		// H.M.Wang 2019-10-23 串口发送数据支持
		final SerialHandler serialHandler =  SerialHandler.getInstance();
		serialHandler.setPrintCommandListener(new SerialHandler.OnSerialPortCommandListenner() {
			@Override
			public void onCommandReceived(int cmd, byte[] data) {
				if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {

					if (cmd == EC_DOD_Protocol.CMD_TEXT) {                         // 发送一条文本	0x0013
						ArrayList<BaseObject> objList = mDataTask.get(index()).getObjList();
						for (BaseObject baseObject : objList) {
							if (baseObject instanceof CounterObject) {
								((CounterObject) baseObject).setRemoteContent("");
							}
						}

						String datastring = new String(data, 7, data.length - 7);
						if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1) {
							setRemoteTextFitCounter(datastring);
						} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2) {
							setRemoteTextSeperated(datastring);
						}
						serialHandler.sendCommandProcessResult(EC_DOD_Protocol.CMD_TEXT, 1, 0, 0, "");
						// H.M.Wang 2019-12-7 反转命令立即生效
					} else if (cmd == EC_DOD_Protocol.CMD_SET_REVERSE) {
						mNeedUpdate = true;
					}
				} else if (SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3) {
					String datastring = new String(data, 0, data.length);
					setRemoteTextDirect(datastring);
					serialHandler.sendCommandProcessResult(SerialProtocol3.CMD_DUMMY, 1, 0, 0, datastring + " set.");
				}
			}
		});

		// End of H.M.Wang 2019-12-19 支持多种串口协议的修改

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

		// H.M.Wang 2019-10-23 串口发送数据支持
		SerialHandler serialHandler =  SerialHandler.getInstance();
		serialHandler.setPrintCommandListener(null);
		// End --------------------

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
			mScheduler = new RfidScheduler(mContext);
		}

		SystemConfigFile configFile = SystemConfigFile.getInstance(context);
		mScheduler.init();
		int headIndex = configFile.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
		int heads = PrinterNozzle.getInstance(headIndex).mHeads;
		/**如果是4合2的打印头，需要修改为4头*/
//		heads = configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0 ? configFile.getParam(42) : heads;
		for (int i = 0; i < heads * configFile.getHeadFactor(); i++) {
			mScheduler.add(new RfidTask(i, mContext));
		}
		mScheduler.load();
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

			mcountdown[i]--;
			if (mcountdown[i] <= 0) {
				// 赋初值
				mcountdown[i] = getInkThreshold(i);
				mInkListener.onInkLevelDown(i);
			}
		}
	}
	
	public int getCount() {
		if (mcountdown == null) {
			initCount();
		}
		return mcountdown[0];
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

		dotCount = dotCount/config.getHeadFactor();
		// Debug.d(TAG, "--->getInkThreshold  head: " + head + "   index = " + index + " dataTask: " + mDataTask.size());
		Debug.d(TAG, "--->dotCount: " + dotCount + "  bold=" + bold);
		if (dotCount <= 0) {
// H.M.Wang 2019-9-28 当该打印头没有数据可打印的时候，原来返回1，会产生错误效果，返回一个尽量大的数以避免之
//			return 1;
			return 65536 * 8;
		}

		if (config.getParam(2) <= 0) {
			bold = 1;
		} else {
			bold = config.getParam(2)/150;
		}
		Debug.d(TAG, "--->dotCount: " + dotCount + "  bold=" + bold);
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
	
	private void rollback() {
		Debug.d(TAG, "--->rollback");
		if (mDataTask == null) {
			return;
		}
		for (DataTask task : mDataTask) {
			for (BaseObject object : task.getObjList()) {
				if (object instanceof CounterObject) {
					((CounterObject) object).rollback();
				}
			}
		}
	}


	public class PrintTask extends Thread {

		@Override
		public void run() {

			char[] buffer = null;
			long last = 0;
			/*逻辑要求，必须先发数据*/
			Debug.d(TAG, "--->print run");

			buffer = mDataTask.get(index()).getPrintBuffer();
			if (isLanPrint()) {
				setLanBuffer(mContext, index(), buffer);
			} else {
				Debug.d(TAG, "--->print buffer ready");
//			int type = mDataTask.get(index).getHeadType();

				FileUtil.deleteFolder("/mnt/sdcard/print.bin");
				ExtendInterceptor interceptor = new ExtendInterceptor(mContext);
				ExtendStat extend = interceptor.getExtend();// save print.bin to /mnt/sdcard/ folder
				int cH = mDataTask.get(mIndex).getInfo().mBytesPerHFeed * 8 * mDataTask.get(mIndex).getPNozzle().mHeads;
				Debug.d(TAG, "--->cH: " + cH);
				BinCreater.saveBin("/mnt/sdcard/print.bin", buffer, cH);

				// H.M.Wang 2019-10-10 追加计算打印区对应于各个头的打印点数
				DataTask t = mDataTask.get(mIndex);
//				Debug.d(TAG, "GetPrintDots Start Time: " + System.currentTimeMillis());
				int[] dots = NativeGraphicJni.GetPrintDots(buffer, buffer.length, t.getInfo().mCharsPerHFeed, t.getPNozzle().mHeads);

				int totalDot = 0;
				for (int j = 0; j < dots.length; j++) {
					// H.M.Wang 2019-10-11 获得的点数乘2
                    dots[j] *= 2;
					totalDot += dots[j];
				}
				t.setDots(totalDot);
				t.setDotsEach(dots);

//				Debug.d(TAG, "GetPrintDots Done Time: " + System.currentTimeMillis());

				StringBuilder sb = new StringBuilder();
				sb.append("Dots per Head: [");
				for (int i=0; i<dots.length; i++) {
					if(i != 0) {
						sb.append(", ");
					}
					sb.append(dots[i]);
				}
				sb.append("]");
				Debug.d(TAG, sb.toString());

				Debug.e(TAG, "--->write data");
				FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);

// H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号
                if(mDataTask.size() > 1) {
					mCallback.onPrint(mIndex);
                }
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

//					if (isLanPrint() && pcReset == true) {
//						buffer = getLanBuffer(index());
//						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);
//						pcReset = false;
//					}
				} else if (writable == -1) {
				} else {
					Debug.d(TAG, "--->FPGA buffer is empty");
					if (mCallback != null) {
						mCallback.onPrinted(mIndex);
					}
					mInterval = SystemClock.currentThreadTimeMillis() - last;
					mHandler.removeMessages(MESSAGE_DATA_UPDATE);
					mNeedUpdate = false;

					synchronized (DataTransferThread.class) {
						next();
						if (isLanPrint()) {
							buffer = getLanBuffer(index());
							Debug.i(TAG, "--->buffer.length: " + buffer.length);
						} else {
							if (!mDataTask.get(index()).isReady) {
								mRunning = false;
								if (mCallback != null) {
									mCallback.OnFinished(CODE_BARFILE_END);
								}
								break;
							}
							Debug.i(TAG, "mIndex: " + index());
							buffer = mDataTask.get(index()).getPrintBuffer();
						}
						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);

// H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号
                        if(mCallback != null && mDataTask.size() > 1) {
							mCallback.onPrint(mIndex);
                        }
// End of H.M.Wang 2020-1-7 追加群组打印时，显示正在打印的MSG的序号

						last = SystemClock.currentThreadTimeMillis();
						countDown();
						mInkListener.onCountChanged();
						mScheduler.schedule();
						if (mCallback != null) {
							mCallback.onComplete(mIndex);
						}

						/**  save log */
						LogIntercepter.getInstance(mContext).execute(getCurData());
					}
				}

				if(mNeedUpdate == true) {
					mHandler.removeMessages(MESSAGE_DATA_UPDATE);
					//在此处发生打印数据，同时
// H.M.Wang 2019-12-29 在重新生成打印缓冲区的时候，考虑网络打印的因素
					if (isLanPrint()) {
						buffer = getLanBuffer(index());
					} else {
						buffer = mDataTask.get(index()).getPrintBuffer();
					}
// End of H.M.Wang 2019-12-29 在重新生成打印缓冲区的时候，考虑网络打印的因素
					Debug.d(TAG, "===>buffer size="+buffer.length);
					// H.M.Wang 2019-12-20 关闭print.bin保存
//					// H.M.Wang 2019-12-17 每次重新生成print内容后，都保存print.bin
//					BinCreater.saveBin("/mnt/sdcard/print.bin", buffer, mDataTask.get(mIndex).getInfo().mBytesPerHFeed * 8 * mDataTask.get(mIndex).getPNozzle().mHeads);
//					// End.
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length*2);
					mHandler.sendEmptyMessageDelayed(MESSAGE_DATA_UPDATE, MESSAGE_EXCEED_TIMEOUT);
					mNeedUpdate = false;
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
			rollback();

		}
	}
}
