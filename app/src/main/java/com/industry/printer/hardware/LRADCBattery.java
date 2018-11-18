package com.industry.printer.hardware;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;

import com.industry.printer.R.string;

public class LRADCBattery {

	private static final String TAG = LRADCBattery.class.getSimpleName();
	
	/*LRADC Battery 驱动的系统文件目录*/
	private static final String SYSFS_BATTERY_LRADC_PATH = "/sys/class/power_supply/battery-lradc/";
	
	/*power now属性对应的文件*/
	private static final String SYSFS_BATTERY_LRADC_POWERNOW =  SYSFS_BATTERY_LRADC_PATH + "power_now";
	
	
	public static int getPower() {
		String power = readSysfs(SYSFS_BATTERY_LRADC_POWERNOW);
		if (power == null) {
			return 0;
		}
		int state = 0;
		try {
			state = Integer.parseInt(power);
		} catch (Exception e) {

		}
		return state;
	}
	
	
	public static boolean writeSysfs(String file, String content) {
		FileOutputStream mStream = null;
		try {
			mStream = new FileOutputStream(file);
			mStream.write(content.getBytes());
			mStream.flush();
			mStream.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return false;
	}
	
	public static String readSysfs(String file) {
		if (file == null) {
			return null;
		}
		
		try {
			BufferedReader mReader = new BufferedReader(new FileReader(file));
			String content = mReader.readLine();
			mReader.close();
			return content;
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
}
