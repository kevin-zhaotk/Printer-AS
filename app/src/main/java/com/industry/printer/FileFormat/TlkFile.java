package com.industry.printer.FileFormat;

import android.content.Context;

import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;

public class TlkFile {
	
	private static final String TAG = TlkFile.class.getSimpleName();
	
	public Context mContext;
	public String mPath;
	public String mFile;
	private String mDirectory;
	
	public TlkFile(Context context, String file) {
		mContext = context;
		mFile = file;
		mDirectory = ConfigPath.getTlkDir(file);
		setTlk(ConfigPath.getTlkAbsolute(file));
	}
	/**
	 * 设置需要解析的tlk文件名，可以是绝对路径或相对路径
	 */
	public void setTlk(String file) {
		// String path = ConfigPath.getTlkPath();
		if (file == null || file.isEmpty())
			return;
		if (file.startsWith("/")) {
			mPath = file;
		} else {
			mPath = ConfigPath.getTlkPath() + file;
		}
		Debug.d(TAG, "--->setTlk: " + mPath);
	}
	
	public String getDirectory() {
		return mDirectory;
	}
}
