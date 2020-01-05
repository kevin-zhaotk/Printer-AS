package com.industry.printer.Utils;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.Thread.UncaughtExceptionHandler;
import java.util.ArrayList;
import java.util.Calendar;

import android.content.Context;
import android.content.SharedPreferences;

import com.industry.printer.DataTransferThread;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.SettingsTabActivity;

public class CrashCatcher implements UncaughtExceptionHandler {

	private static final String TAG = CrashCatcher.class.getSimpleName();
	
	private static CrashCatcher mInstance;
	
	private Context mContext;
	//系统默认的UncaughtException处理类       
    private Thread.UncaughtExceptionHandler mDefaultHandler;
	
	public static CrashCatcher getInstance() {
		if (mInstance == null) {
			mInstance = new CrashCatcher();
		}
		return mInstance;
	}
	
	public void init(Context ctx) {
		mContext = ctx;
		mDefaultHandler = Thread.getDefaultUncaughtExceptionHandler();
		Thread.setDefaultUncaughtExceptionHandler(this);
	}
	
	@Override
	public void uncaughtException(Thread thread, Throwable arg1) {
		Debug.e(TAG, "*****************Exception begin*********************");
		Debug.e(TAG, "***");
		Debug.e(TAG, "--->uncaughtException: " + arg1.getMessage());
		arg1.printStackTrace();
		Debug.e(TAG, "***");
		Debug.e(TAG, "*****************Exception end*********************");
		DataTransferThread dtt = DataTransferThread.getInstance(mContext);
		if (dtt.isRunning()) {
			SharedPreferences p = mContext.getSharedPreferences(SettingsTabActivity.PREFERENCE_NAME, Context.MODE_PRIVATE);
			p.edit().putBoolean(PreferenceConstants.PRINTING_BEFORE_CRASH, true).commit();
		}
		ArrayList<String> usbs = ConfigPath.getMountedUsb();
		if (usbs == null && usbs.size() == 0) {
			return;
		}
		
		File log = new File(usbs.get(0) + "/log-" + getTimeString() + ".txt");
		
		try {
			PrintWriter pWriter = new PrintWriter(log);
			arg1.printStackTrace(pWriter);
			Throwable cause = arg1.getCause();
			while (cause != null) {
				cause.printStackTrace(pWriter);
				cause = cause.getCause();
			}
			pWriter.flush();
			pWriter.close();
			
		} catch (IOException e) {
			Debug.e(TAG, e.getMessage());
		} finally {
			
		}
		
		mDefaultHandler.uncaughtException(thread, arg1);
	}

	private String getTimeString() {
		Calendar calendar = Calendar.getInstance();
		
		StringBuilder builder = new StringBuilder();
		builder.append(calendar.get(Calendar.YEAR))
				.append(StringUtil.formatTo(calendar.get(Calendar.MONTH), 2))
				.append(StringUtil.formatTo(calendar.get(Calendar.DATE), 2))
				.append(StringUtil.formatTo(calendar.get(Calendar.HOUR_OF_DAY), 2))
				.append(StringUtil.formatTo(calendar.get(Calendar.MINUTE), 2))
				.append(StringUtil.formatTo(calendar.get(Calendar.SECOND), 2))
				.append(StringUtil.formatTo(calendar.get(Calendar.MILLISECOND), 4));

		return builder.toString();
	}

}
