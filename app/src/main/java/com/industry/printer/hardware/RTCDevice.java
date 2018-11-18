package com.industry.printer.hardware;

import java.lang.reflect.Method;

import android.app.AlarmManager;
import android.content.Context;
import android.graphics.Bitmap.Config;

import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.ReflectCaller;
import com.industry.printer.Utils.SystemFs;

public class RTCDevice {

	public static native int open(String dev);
	public static native int close(int fd);
	public static native void syncSystemTimeToRTC(int fd);	//write
	public static native void syncSystemTimeFromRTC(int fd); //read
	
	private static final String TAG = RTCDevice.class.getSimpleName();
	public static final String RTC_DEV = "/dev/rtc1"; 

	//I2C debug
	private final String I2C_READ = "/sys/class/device_of_i2c/read";
	private final String I2C_WRITE = "/sys/class/device_of_i2c/write";
	private final String I2C_DEVICE = "/sys/class/device_of_i2c/device";
	
	public static RTCDevice mInstance = null;
	
	public static RTCDevice getInstance(Context context) {
		if (mInstance == null) {
			mInstance = new RTCDevice(context);
		}
		return mInstance;
	}
	
	public RTCDevice(Context context) {
		
		SystemFs.writeSysfs(I2C_DEVICE, "1,0x68");
		String cmd = "5,0x08";
		SystemFs.writeSysfs(I2C_READ, cmd);
	}
	
	public void initSystemTime(Context context) {
		// AlarmManager aManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
		// aManager.HwToSystemClock();
		ReflectCaller.AlarmManagerHwToSystemClock(context);
	}
	
	public void syncSystemTimeToRTC(Context context) {
		// AlarmManager aManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
		// aManager.SystemClockToHw();
		ReflectCaller.AlarmManagerSystemClockToHw(context);
	}
	
	/**
	 * 
	 */
	public boolean isFirst() {
		
		return false;
	}
	/**
	 * 总计数为8位10进制数字，保存在DS1338的NVRAM中的08H～11H 4个字节中
	 */
	public void writeCounter(Context context, int count) {
		byte byte0 = (byte) (count & 0x0ff);
		byte byte1 = (byte) ((count >> 8) & 0x0ff);
		byte byte2 = (byte) ((count >> 16) & 0x0ff);
		byte byte3 = (byte) ((count >> 24) & 0x0ff);
		byte checkSum = (byte) ((byte0 + byte1 + byte2 + byte3) & 0x0ff);
		String cmd = "0x08," + Integer.toHexString(byte0) + ","
					+ Integer.toHexString(byte1) + "," + Integer.toHexString(byte2) + "," 
					+ Integer.toHexString(byte3) + "," + Integer.toHexString(checkSum);
		SystemFs.writeSysfs(I2C_WRITE, cmd);
	}
	
	/**
	 * 总计数为8位10进制数字，保存在DS1338的NVRAM中的08H～11H 4个字节中
	 */
	public int readCounter(Context context) {
		SystemFs.writeSysfs(I2C_DEVICE, "1,0x68");
		int index=0;
		String out = SystemFs.readSysfs(I2C_READ);
		if (out==null) {
			return 0;
		}
		Debug.d(TAG, "--->NVRAM out = " + out);
		String[] bytes = out.split("/r/n");
		if (bytes == null || bytes.length < 8) {
			return 0;
		}
		Debug.d(TAG, "---> NVRAM Counter <---");
		Debug.d(TAG, bytes[3]);
		Debug.d(TAG, bytes[4]);
		Debug.d(TAG, bytes[5]);
		Debug.d(TAG, bytes[6]);
		Debug.d(TAG, bytes[7]);
		Debug.d(TAG, "---> NVRAM Counter end<---");
		index = bytes[3].lastIndexOf("0x");
		//int byte1 = Integer.parseInt("123", 16);
		byte byte1 = (byte) Integer.parseInt(bytes[3].substring(index+2), 16);
		byte byte2 = (byte) Integer.parseInt(bytes[4].substring(index+2), 16);
		byte byte3 = (byte) Integer.parseInt(bytes[5].substring(index+2), 16);
		byte byte4 = (byte) Integer.parseInt(bytes[6].substring(index+2), 16);
		byte byte5 = (byte) Integer.parseInt(bytes[7].substring(index+2), 16);
		int count = (byte1 & 0x0ff) + (byte2 & 0x0ff) * 256 + (byte3 & 0x0ff) * 256 * 256 + (byte4 & 0x0ff) * 256 * 256 * 256;
		
		//检查校验和
		byte checksum = (byte) ((byte1 + byte2 + byte3 + byte4) & 0x0ff);
		if (checksum == byte5) {
			return count;
		}
		return 0;
	}
	
}
