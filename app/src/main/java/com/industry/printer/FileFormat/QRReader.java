package com.industry.printer.FileFormat;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.util.List;

import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.SystemFs;

import android.content.Context;
import android.text.TextUtils;

/**
 * 從QR.data文件中讀取二維碼數據
 * 在QRlast.xml裏記錄上次讀取的位置
 * @author kevin
 *
 */
public class QRReader {
	private static final String TAG = QRReader.class.getSimpleName();

	public Context mContext;
	
	/* 當前行數 */
	public int mRow;
	String mRoot;
	
	BufferedReader mReader;
	
	FileOutputStream mWriter;
	
	private static volatile QRReader mInstance;
	
	public static QRReader getInstance(Context ctx) {
		if (mInstance == null) {
			synchronized (QRReader.class) {
				if (mInstance == null) {
					mInstance = new QRReader(ctx);
				}
			}
		}
		return mInstance;
	}
	
	public static QRReader reInstance(Context ctx) {
		mInstance = null;
		synchronized (QRReader.class) {
			if (mInstance == null) {
				mInstance = new QRReader(ctx);
			}
		}
		return mInstance;
	}
	
	private QRReader(Context ctx) {
		mContext = ctx;
		init();
	}

	private void init() {
//		List<String> paths = ConfigPath.getMountedUsb();
//		if (paths == null || paths.size() <= 0) {
//			return;
//		}
//		mRoot = paths.get(0);
// H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存
//		File last = new File(Configs.QR_LAST);
		checkDeviceAddress();
		mRow = readQRLast();
// End of H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存

		try {
// H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存
//			if (!last.exists()) {
//				mRow = 0;
//				last.createNewFile();
//			}
//			FileReader reader = new FileReader(last);
//			BufferedReader bReader = new BufferedReader(reader);
//			String row = bReader.readLine();
//			try {
//				mRow = Integer.parseInt(row);
//			} catch (Exception e) {
//				mRow = 0;
//			}
//			Debug.d(TAG, "--->row: " + mRow);
//			if (mRow < 0) {
//				mRow = 0;
//			}
// End of H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存

			String path = null;
			if (new File(Configs.QR_CSV).exists()) {
				path =  Configs.QR_CSV;
			} else if (new File(Configs.QR_DATA).exists()) {
				path = Configs.QR_DATA;
			}
			if (path == null) {
				return;
			}
			FileReader r = new FileReader(path);
			mReader = new BufferedReader(r);
			for (int i = 0; i < mRow; i++) {
				mReader.readLine();
			}
		} catch(Exception e) {
			mReader = null;
			Debug.d(TAG, "--->exception: " + e.getMessage());
		}
	}
	
	public String read() {
		if (mReader == null) {
			return  null;
		}
		
		try {
			String line = mReader.readLine();
			if (TextUtils.isEmpty(line)) {
				Debug.e(TAG, "--->read out null");
				return null;
			}
			int index = line.indexOf(",");
			Debug.d(TAG, "--->line: " + line);
			String content = line.substring(index + 1);
			// mWriter = new FileOutputStream(mRoot + Configs.QR_LAST);
// H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存
			mRow++;
			SystemConfigFile.getInstance().setParamBroadcast(SystemConfigFile.INDEX_QRCODE_LAST, mRow);
			writeQRLast(mRow);
//			FileWriter w = new FileWriter(Configs.QR_LAST);
//			w.write(String.valueOf(mRow++));
//			w.flush();
//			w.close();
// End of H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存
//			mWriter.write(String.valueOf(mRow++).getBytes());
//			mWriter.flush();
//			mWriter.close();
			Debug.d(TAG, "--->content: " + content.trim());
			return content.trim();
		} catch (Exception e) {
			Debug.d(TAG, "--->e: " + e.getMessage());
			return null;
		}
	}
	
	public boolean isReady() {
		return mReader != null;
	}

// H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存
	private final String I2C_READ = "/sys/class/device_of_i2c/read";
	private final String I2C_WRITE = "/sys/class/device_of_i2c/write";
	private final String I2C_DEVICE = "/sys/class/device_of_i2c/device";
	private String DeviceAddress = "1,0x68";
	private final String REGISTER = "0x38";

	private void checkDeviceAddress() {
		SystemFs.writeSysfs(I2C_DEVICE, "1,0x68");
		// 自动探测RTC的I2C地址
		String cmdWR = "0x0f,0x55";
		SystemFs.writeSysfs(I2C_WRITE, cmdWR);
		String cmdRD = "1,0x0f";
		SystemFs.writeSysfs(I2C_READ, cmdRD);
		String out = SystemFs.readSysfs(I2C_READ);
		if (out==null) {
			return ;
		}

		String[] bytes = out.split("/r/n");
		if (bytes == null || bytes.length < 4) {
			return ;
		}
		int pos = bytes[3].lastIndexOf("0x");

		byte check = (byte) Integer.parseInt(bytes[3].substring(pos+2), 16);

		Debug.d(TAG, "--->NVRAM init out = " + String.valueOf(check));

		if (check == 0x55) {
			DeviceAddress = "1,0x68";
		} else {
			DeviceAddress = "2,0x68";
		}
		Debug.d(TAG, "Device Address = " + DeviceAddress);
	}

	public int readQRLast() {
		SystemFs.writeSysfs(I2C_DEVICE, DeviceAddress);

		String cmd = "4," + REGISTER;
		Debug.d(TAG, "--->Reading QRLast from register " + REGISTER);

		SystemFs.writeSysfs(I2C_READ, cmd);
		String out = SystemFs.readSysfs(I2C_READ);
		if (out == null) {
			return 0;
		}

		int pos =0;
		String[] bytes = out.split("/r/n");
		if (bytes == null || bytes.length < 7) {
			return 0;
		}
		pos = bytes[3].lastIndexOf("0x");
		byte byte1 = (byte) Integer.parseInt(bytes[3].substring(pos+2), 16);
		byte byte2 = (byte) Integer.parseInt(bytes[4].substring(pos+2), 16);
		byte byte3 = (byte) Integer.parseInt(bytes[5].substring(pos+2), 16);
		byte byte4 = (byte) Integer.parseInt(bytes[6].substring(pos+2), 16);

		int count = (byte1 & 0x0ff) + (byte2 & 0x0ff) * 256 + (byte3 & 0x0ff) * 256 * 256 + (byte4 & 0x0ff) * 256 * 256 * 256;
		Debug.d(TAG, "--->QRLast = " + count);

		return count;
	}

	public void writeQRLast(int count) {
		SystemFs.writeSysfs(I2C_DEVICE, DeviceAddress);

		StringBuilder cmd = new StringBuilder(REGISTER);
		Debug.d(TAG, "--->Writing QRLast(" + count + ") to register " + REGISTER);

		byte byte0 = (byte) (count & 0x0ff);
		byte byte1 = (byte) ((count >> 8) & 0x0ff);
		byte byte2 = (byte) ((count >> 16) & 0x0ff);
		byte byte3 = (byte) ((count >> 24) & 0x0ff);
		cmd.append(",");
		cmd.append("0x" + Integer.toHexString(byte0));
		cmd.append(",");
		cmd.append("0x" + Integer.toHexString(byte1));
		cmd.append(",");
		cmd.append("0x" + Integer.toHexString(byte2));
		cmd.append(",");
		cmd.append("0x" + Integer.toHexString(byte3));

		SystemFs.writeSysfs(I2C_WRITE, cmd.toString());
	}
// End of H.M.Wang 2020-5-15 QRLast移植RTC的0x38地址保存
}
