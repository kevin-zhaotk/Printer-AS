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

import android.content.Context;
import android.text.TextUtils;

/**
 * 從QR.data文件中讀取二維碼數據
 * 在QRlast.xml裏記錄上次讀取的位置
 * @author kevin
 *
 */
public class QRReader {
	
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
		File last = new File(Configs.QR_LAST);
		
		try {
			if (!last.exists()) {
				mRow = 0;
				last.createNewFile();
			}
			FileReader reader = new FileReader(last);
			BufferedReader bReader = new BufferedReader(reader);
			String row = bReader.readLine();
			try {
				mRow = Integer.parseInt(row);
			} catch (Exception e) {
				mRow = 0;
			}
			Debug.d("XXX", "--->row: " + mRow);
			if (mRow < 0) {
				mRow = 0;
			}
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
			Debug.d("XXX", "--->exception: " + e.getMessage());
		}
	}
	
	public String read() {
		if (mReader == null) {
			return  null;
		}
		
		try {
			String line = mReader.readLine();
			if (TextUtils.isEmpty(line)) {
				Debug.e("XXX", "--->read out null");
				return null;
			}
			int index = line.indexOf(",");
			Debug.d("XXX", "--->line: " + line);
			String content = line.substring(index + 1);
			// mWriter = new FileOutputStream(mRoot + Configs.QR_LAST);
			FileWriter w = new FileWriter(Configs.QR_LAST);
			w.write(String.valueOf(mRow++));
			w.flush();
			w.close();
//			mWriter.write(String.valueOf(mRow++).getBytes());
//			mWriter.flush();
//			mWriter.close();
			Debug.d("XXX", "--->content: " + content.trim());
			return content.trim();
		} catch (Exception e) {
			Debug.d("XXX", "--->e: " + e.getMessage());
			return null;
		}
		
	}
	
	public boolean isReady() {
		return mReader != null;
	}
}
