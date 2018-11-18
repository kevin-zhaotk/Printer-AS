package com.industry.printer.Utils;

import android.util.Log;

public class Debug {
	
	public static final String TAG="Printer";
	
	public Debug()
	{	
	}
	
	public static int d(String tag, String log)
	{
		if (!Configs.DEBUG) {
			return 0;
		}
		Log.d(tag, getLineNumber()+ ":" + log);
		return 0;
	}
	
	public static int d(String tag, String log, Throwable tr)
	{
		if (!Configs.DEBUG) {
			return 0;
		}
		Log.d(TAG, getLineNumber()+ ":"+log, tr);
		return 0;
	}
	public static int i(String tag, String log)
	{
		if (!Configs.DEBUG) {
			return 0;
		}
		Log.i(tag, getLineNumber()+ ":"+log);
		return 0;
	}
	
	public static int i(String tag, String log, Throwable tr)
	{
		if (!Configs.DEBUG) {
			return 0;
		}
		Log.i(tag, getLineNumber() + ":"+log, tr);
		return 0;
	}
	
	public static int v(String tag, String log)
	{
		if (!Configs.DEBUG) {
			return 0;
		}
		Log.v(tag, getLineNumber() + ":"+log);
		return 0;
	}
	public static int v(String tag, String log, Throwable tr)
	{
		if (!Configs.DEBUG) {
			return 0;
		}
		Log.v(tag, getLineNumber() + ":"+log, tr);
		return 0;
	}

	public static int e(String tag, String log)
	{
		Log.e(tag, getLineNumber() + ":"+log);
		return 0;
	}
	
	public static int e(String tag, String log, Throwable tr)
	{
		Log.e(tag, getLineNumber() + ":"+log, tr);
		return 0;
	}
	
	public static void print(String tag, byte[] value) {
		if (value == null) {
			return;
		}
		String log = "";
		for (int i = 0; i < value.length; i++) {
			log += "0x"+Integer.toHexString(value[i]&0x0ff)+" " ;
		}
		Debug.e(TAG, getLineNumber()+ "  " + tag + " [ " + log + " ]");
	}
	
	private static String getLineNumber() {
		Exception e = new Exception();
		StackTraceElement[] trace =e.getStackTrace();
		if(trace==null||trace.length==0) 
			return "";
		int line = trace[2].getLineNumber();
		String file = trace[2].getFileName();
		return file + ":" + line;
	}
	
}
