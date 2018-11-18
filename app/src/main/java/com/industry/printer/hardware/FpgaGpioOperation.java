package com.industry.printer.hardware;

import java.util.Calendar;
import java.util.logging.Logger;

import android.R.integer;
import android.content.Context;
import android.os.SystemClock;

import com.industry.printer.BinInfo;
import com.industry.printer.DataTransferThread;
import com.industry.printer.MessageTask;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.DataTask;
import com.printer.corelib.Paramter;

/**
 * @author kevin
 * 用于操作Fpga Gpio的类
 */
public class FpgaGpioOperation {
	
	
	/**
	 * IOCMD
	 */
	public static final int FPGA_CMD_SETTING	= 0x01;
	public static final int FPGA_CMD_SENDDATA	= 0x02;
	public static final int FPGA_CMD_SYNCDATA	= 0x03;
	public static final int FPGA_CMD_STARTPRINT	= 0x04;
	public static final int FPGA_CMD_STOPPRINT	= 0x05;
	public static final int FPGA_CMD_CLEAN		= 0x06;
	
	/**
	 * 0x00 输出数据状态
	 * 0x01 设置状态
	 * 0x02 保留
	 * 0x03 清空状态
	 */
	public static final int FPGA_STATE_OUTPUT	= 0x00;
	public static final int FPGA_STATE_SETTING	= 0x01;
	public static final int FPGA_STATE_RESERVED	= 0x02;
	public static final int FPGA_STATE_CLEAN	= 0x03;
	public static final int FPGA_STATE_PURGE	= 0x05;

	
	public static final String FPGA_DRIVER_FILE = "/dev/fpga-gpio";
	public static int mFd=0;
	/**
	 * GPIO JNI APIs 
	 **/
	/**
	 * 打开GPIO设备文件
	 * @param dev  GPIO驱动设备文件
	 * @return
	 */
	static public native int open(String dev);
	
	/**
	 * 向GPIO写入数据
	 * @param fd	设备句柄
	 * @param buffer	要写到GPIO的数据buffer
	 * @param count	写入数据长度，单位 sizeof（char）
	 * @return
	 */
	static public native int write(int fd, char[] buffer, int count);
	
	/**
	 * 讀取FPGA數據
	 * @param fd
	 * @return
	 */
	static public native int read(int fd);
	/**
	 * 向GPIO写入数据
	 * @param fd	设备句柄
	 * @param buffer	要写到GPIO的数据buffer
	 * @param count	写入数据长度，单位 sizeof（char）
	 * @return
	 */
	static public native int ioctl(int fd, int cmd, long arg);
	
	/**
	 * 查询GPIO是否可写
	 * @param fd	设备句柄
	 * @return
	 */
	static public native int poll(int fd);
	
	/**
	 * 关闭GPIO驱动设备文件
	 * @param fd	设备句柄
	 * @return
	 */
	static public native int close(int fd);
	
	
	//TAG
	public final static String TAG = FpgaGpioOperation.class.getSimpleName();
	
	
	public static volatile FpgaGpioOperation mInstance;
	
	public static FpgaGpioOperation getInstance() {
		if (mInstance == null) {
			synchronized (FpgaGpioOperation.class) {
				if (mInstance == null) {
					mInstance = new FpgaGpioOperation();
				}
			}
		}
		return mInstance;
	}
	
	public FpgaGpioOperation() {
		
	}
	
	public static int open() {
		if(mFd <= 0) {
			mFd = open(FPGA_DRIVER_FILE);
		}
		return mFd;
	}
	
	public static void close() {
		if(mFd > 0) {
			close(mFd);
		}
	}
	
	public int read() {
		open();
		return read(mFd);
	}
	/**
	 * writeData 下发打印数据接口
	 * 每次在启动打印的时候设置为输出，在打印过程中不允许修改PG0 PG1状态
	 * @param type 数据类型，设置or打印数据
	 * @param data
	 * @param len
	 * @return
	 */
	public static synchronized int writeData(int type, char data[], int len) {
		
		int fd = open();
		if(fd <= 0) {
			return -1;
		}
		if (type < FPGA_STATE_OUTPUT || type > FPGA_STATE_PURGE) {
			Debug.d(TAG, "===>wrong data type");
			return -1;
		}
		ioctl(fd, FPGA_CMD_SETTING, type);
		Debug.d(TAG, "--->writeData len=" + len);
		int wlen = write(fd, data, len);
		if(wlen != len) {
			//close(fd);
			return -1;
		}
		// close(fd);
		return wlen;
	}
	
	/**
	 * pollState 轮训内核buffer状态
	 * 由于该函数会调用native的poll函数，native的poll函数会一直阻塞直到内核kernel Buffer状态为空，
	 * 所以不能在UI线程内调用该函数，请在单独的Thread中调用，防止ANR
	 * @return
	 */
	public static int pollState() {
		int ret=-1;
		int fd = open();
		if(fd <= 0) {
			return -1;
		}
		
		ret = poll(fd);
		return ret;
	}
	
	/**
	 * clean 下发清空数据命令到FPGA	
	 */
	public static void clean() {
		int fd = open();
		if(fd <= 0) {
			Debug.d(TAG, "===>open fpga file error");
			return;
		}
		ioctl(fd, FPGA_CMD_CLEAN, 0);
		// close(fd);
	}
	
	public static final int SETTING_TYPE_NORMAL = 1;
	public static final int SETTING_TYPE_PURGE1 = 2;
	public static final int SETTING_TYPE_PURGE2 = 3;
	
	/**
	 * updateSettings 下发系统设置
	 * 如果要下发设置数据，必须先停止打印
	 * FPGA驅動接收32個參數，其中前24個參數是下發給FPGA設備的，後8個給驅動備用
	 * 參數24： 表示列高（經過補償後的字節數），用於加重處理
	 * @param context
	 */
	
	public static void updateSettings(Context context, DataTask task, int type ) {
		
		if (DataTransferThread.getInstance().isRunning()) {
			Debug.d(TAG, "===>print Thread is running now, please stop it then update settings");
			return;
		}
		int fd = open();
		if(fd <= 0) {
			return;
		}
		char data[] = new char[Configs.gParams];
		SystemConfigFile config = SystemConfigFile.getInstance(context);
//		config.paramTrans();
		RFIDManager manager = RFIDManager.getInstance(context);
		RFIDDevice device = manager.getDevice(0);
		Paramter paramter = Paramter.getInstance();
		int feature4 = 0;
		int feature5 = 0;
		if (device != null && device.mFeature!= null && device.mFeature.length >= 5) {
			feature4 = device.mFeature[4];
			feature5 = device.mFeature[5];
		}
		paramter.paramTrans(config.mParam, feature4, feature5, config.getHeads());
		for (int i = 0; i < 24; i++) {
			data[i] = (char) paramter.getFPGAParam(i);
 		}
		
		if (type != SETTING_TYPE_NORMAL) {
			data[1] = 4;
			data[3] = 100;
			data[4] = 200;
			data[5] = 100;
			data[15] = 1;
		}
		if (type == SETTING_TYPE_PURGE1) {
			data[17] = (char) (data[17] | 0x010);
		} else if (type == SETTING_TYPE_PURGE2) {
			data[17] = (char) (data[17] & 0xffef);
		}
		/* else {
			data[1] = (char) SystemConfigFile.mParam2;
			data[3] = (char) SystemConfigFile.mParam4;
			data[4] = (char) SystemConfigFile.mParam5;
			data[5] = (char) SystemConfigFile.mParam6;
			data[15] = (char) SystemConfigFile.mResv16;
		}
		data[2] = (char) SystemConfigFile.mParam3;
		data[6] = (char) SystemConfigFile.mParam7;
		data[7] = (char) SystemConfigFile.mParam8;
		Debug.d(TAG, "===>data7:" + Integer.toHexString(data[7]));
		data[8] = (char) SystemConfigFile.mParam9;
		data[9] = (char) SystemConfigFile.mParam10;
		data[10] = (char) SystemConfigFile.mResv11;
		data[11] = (char) SystemConfigFile.mResv12;
		data[12] = (char) SystemConfigFile.mResv13;
		data[13] = (char) SystemConfigFile.mResv14;
		data[14] = (char) SystemConfigFile.mResv15;
		
		data[16] = (char) SystemConfigFile.mResv17;
		data[17] = (char) SystemConfigFile.mResv18;
		data[18] = (char) SystemConfigFile.mResv19;
		data[19] = (char) SystemConfigFile.mResv20;
		data[20] = (char) SystemConfigFile.mResv21;
		data[21] = (char) SystemConfigFile.mResv22;
		data[22] = (char) SystemConfigFile.mResv23;
		data[23] = (char) SystemConfigFile.mResv24;
		*/
		BinInfo info = task.getInfo();
		data[24] = (char) info.getBytesFeed();
		
		//是否雙列打印
		data[25] = (char)config.getParam(31-1);
		//雙列偏移量
		data[26] = (char) config.getParam(32-1);
		//时间参数放在最后3个
		/*
		Calendar c = Calendar.getInstance();
		int hour = c.get(Calendar.HOUR_OF_DAY);  
        int minute = c.get(Calendar.MINUTE);
        int second = c.get(Calendar.SECOND);
		data[Configs.gParams - 3] = (char)hour;
		data[Configs.gParams - 2] = (char)minute;
		data[Configs.gParams - 1] = (char)second;
		*/
		writeData(FPGA_STATE_SETTING, data, data.length*2);	
	}
	
	/**
	 * 启动打印时调用，用于初始化内核轮训线程
	 */
	public static void init() {
		int fd = open();
		if(fd <= 0) {
			return ;
		}
		/*设置状态为输出*/
		// ioctl(fd, FPGA_CMD_SETTING, FPGA_STATE_OUTPUT);
		/*启动内核轮训线程*/
		ioctl(fd, FPGA_CMD_STARTPRINT, 0);
		
	}
	
	/**
	 * 停止打印时调用，用于停止内核轮训线程
	 */
	public static void uninit() {
		int fd = open();
		if(fd <= 0) {
			return ;
		}
		ioctl(fd, FPGA_CMD_STOPPRINT, 0);
	}
}
