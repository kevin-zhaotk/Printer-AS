package com.industry.printer;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;

import org.apache.http.impl.conn.IdleConnectionHandler;

import android.R.bool;
import android.R.color;
import android.R.integer;
import android.app.LauncherActivity;
import android.app.PendingIntent.OnFinished;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;

import com.industry.printer.MessageTask.MessageType;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Rfid.RfidScheduler;
import com.industry.printer.Rfid.RfidTask;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.DataTask;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.interceptor.ExtendInterceptor;
import com.industry.printer.interceptor.ExtendInterceptor.ExtendStat;
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
	
	private InkLevelListener mInkListener = null;
	
	public static DataTransferThread getInstance() {
		if(mInstance == null) {
			synchronized (DataTransferThread.class) {
				if (mInstance == null) {
					mInstance = new DataTransferThread();
				}
			}
			Debug.d(TAG, "===>new thread");
		}
		return mInstance;
	}
	
	public DataTransferThread() {
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
		if (mIndex >= mDataTask.size()) {
			mIndex = 0;
		}
	}
	
	public synchronized int index() {
		return mIndex;
	}
	
	boolean needRestore = false;
	
	public void purge(final Context context) {
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
		final int head = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
		final boolean dotHd = (head == MessageType.MESSAGE_TYPE_16_DOT || head == MessageType.MESSAGE_TYPE_32_DOT);
		
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
				purge(mContext, task, buffer, FpgaGpioOperation.SETTING_TYPE_PURGE2);
			
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
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		FpgaGpioOperation.clean();
	}
	
	public void clean(final Context context) {
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
		final int head = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
		ThreadPoolManager.mThreads.execute(new Runnable() {
			
			@Override
			public void run() {
				DataTask task = new DataTask(context, null);
				Debug.e(TAG, "--->task: " + task);
				String purgeFile = "purge/single.bin";
				if (head == MessageType.MESSAGE_TYPE_16_DOT || head == MessageType.MESSAGE_TYPE_32_DOT) {
					purgeFile = "purge/bigdot.bin";
				}
				char[] buffer = task.preparePurgeBuffer(purgeFile);
				
				for (int i = 0; i < 20; i++) {
					Debug.e(TAG, "--->buffer len: " + buffer.length);
					
					FpgaGpioOperation.updateSettings(context, task, FpgaGpioOperation.SETTING_TYPE_PURGE1);
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_PURGE, buffer, buffer.length*2);
					try {
						Thread.sleep(3000 * 5);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					FpgaGpioOperation.clean();
					FpgaGpioOperation.updateSettings(context, task, FpgaGpioOperation.SETTING_TYPE_PURGE2);
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_PURGE, buffer, buffer.length*2);
					try {
						Thread.sleep(3000 * 5);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					FpgaGpioOperation.clean();
				}
			}
			
		});
	}
	
	public boolean isRunning() {
		return mRunning;
	}
	
	public boolean launch(Context ctx) {
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
		int heads = configFile.getHeads();
		/**如果是4合2的打印头，需要修改为4头*/
		heads = configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0 ? configFile.getParam(42) : heads;
		for (int i = 0; i < heads; i++) {
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
		} else {
			for (int i = 0; i < mcountdown.length; i++) {
				mcountdown[i] = 0;
			}
		}
		for (int i = 0; i < mcountdown.length; i++) {
			mcountdown[i] = getInkThreshold(i);
			//Debug.d(TAG, "--->initCount countdown[" + i + "] = " + mcountdown[i]);
		}
	}
	/**
	 * 倒计数，当计数倒零时表示墨水量需要减1，同时倒计数回归
	 * @return true 墨水量需要减1； false 墨水量不变
	 */
	private void countDown() {
		for (int i = 0; i < mScheduler.count(); i++) {
			mcountdown[i]--;
			if (mcountdown[i] <= 0) {
				// 赋初值
				mcountdown[i] = getInkThreshold(i);
				mInkListener.onInkLevelDown(i);
			}
		}
	}
	
	public int getCount() {
		
		return mcountdown[0];
	}
	
	/**
	 * 通过dot count计算RFID减1的阀值
	 * @param head 喷头索引
	 * @return
	 */
	public int getInkThreshold(int head) {
		int bold = 1;
		int index = index();
		int dotCount = getDotCount(mDataTask.get(index), head);
		// Debug.d(TAG, "--->getInkThreshold  head: " + head + "   index = " + index + " dataTask: " + mDataTask.size());
		Debug.d(TAG, "--->dotCount: " + dotCount + "  bold=" + bold);
		if (dotCount <= 0) {
			return 1;
		}
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
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
			return mDataTask.get(0).getHeads();
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
		public void onComplete();
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
			int index = index();
			buffer = mDataTask.get(index).getPrintBuffer();
			Debug.d(TAG, "--->print buffer ready");
			int type = mDataTask.get(index).getHeadType();

			FileUtil.deleteFolder("/mnt/sdcard/print.bin");
			
			ExtendInterceptor interceptor = new ExtendInterceptor(mContext);
			ExtendStat extend = interceptor.getExtend();
			// save print.bin to /mnt/sdcard/ folder
			int cH = mDataTask.get(mIndex).getInfo().mBytesPerHFeed*8*mDataTask.get(mIndex).getHeads();
			Debug.d(TAG, "--->cH: " + cH);
			BinCreater.saveBin("/mnt/sdcard/print.bin", buffer, cH);
			int n=0;

			Debug.e(TAG, "--->write data");
			FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length*2);
			last = SystemClock.currentThreadTimeMillis();

			FpgaGpioOperation.init();
			
			boolean isFirst = true;
			while(mRunning == true) {
				
//				Debug.e(TAG, "--->start print");
				//FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length*2);
				int writable = FpgaGpioOperation.pollState();
				// writable = 1;
//				Debug.e(TAG, "--->writable: " + writable);
				if (writable == 0) { //timeout
//					Debug.d(TAG, "--->timeout");
				} else if (writable == -1) {
//					Debug.d(TAG, "--->pollstate " + writable);
				} else {
//					
//					Debug.d(TAG, "--->FPGA buffer is empty");
					mInterval = SystemClock.currentThreadTimeMillis() - last;
					mHandler.removeMessages(MESSAGE_DATA_UPDATE);
					mNeedUpdate = false;

					synchronized (DataTransferThread.class) {
						if (!mDataTask.get(index()).isReady) {
							mRunning = false;
							if (mCallback != null) {
								mCallback.OnFinished(CODE_BARFILE_END);
							}
							break;
						}
//						if (isFirst) {
//							next();
//							Debug.d(TAG, "===>buffer getPrintbuffer");
//							buffer = mDataTask.get(index()).getPrintBuffer();
//							Debug.d(TAG, "===>buffer getPrintbuffer finish");
//							isFirst = false;
//						}
						next();
						buffer = mDataTask.get(index()).getPrintBuffer();
						FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT, buffer, buffer.length * 2);
						last = SystemClock.currentThreadTimeMillis();
						countDown();
						mInkListener.onCountChanged();
						mScheduler.schedule();
						if (mCallback != null) {
							mCallback.onComplete();
						}
					}
				}

				if(mNeedUpdate == true) {
					mHandler.removeMessages(MESSAGE_DATA_UPDATE);
					//在此处发生打印数据，同时
					buffer = mDataTask.get(index()).getPrintBuffer();
					Debug.d(TAG, "===>buffer size="+buffer.length);
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
