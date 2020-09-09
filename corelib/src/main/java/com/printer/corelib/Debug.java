package com.printer.corelib;

import android.util.Log;

@Deprecated
public class Debug {
	
	public static final String TAG="Printer";
	private static final boolean DEBUG = false;
	
	public Debug()
	{	
	}
	
	public static int d(String tag, String log)
	{
		if (!DEBUG) {
			return 0;
		}
		Log.d(TAG, getLineNumber()+ "  " + tag + ":" + log);
		return 0;
	}
	
	public static int d(String tag, String log, Throwable tr)
	{
		if (!DEBUG) {
			return 0;
		}
		Log.d(TAG, getLineNumber()+ "  " + tag+":"+log, tr);
		return 0;
	}
	public static int i(String tag, String log)
	{
		if (!DEBUG) {
			return 0;
		}
		Log.i(TAG, getLineNumber()+ "  " + tag+":"+log);
		return 0;
	}
	
	public static int i(String tag, String log, Throwable tr)
	{
		if (!DEBUG) {
			return 0;
		}
		Log.i(TAG, getLineNumber()+ "  " + tag+":"+log, tr);
		return 0;
	}
	
	public static int v(String tag, String log)
	{
		if (!DEBUG) {
			return 0;
		}
		Log.v(TAG, getLineNumber()+ "  " + tag+":"+log);
		return 0;
	}
	public static int v(String tag, String log, Throwable tr)
	{
		if (!DEBUG) {
			return 0;
		}
		Log.v(TAG, getLineNumber()+ "  " + tag+":"+log, tr);
		return 0;
	}

	public static int e(String tag, String log)
	{
		Log.e(TAG, getLineNumber()+ "  " + tag+":"+log);
		return 0;
	}
	
	public static int e(String tag, String log, Throwable tr)
	{
		Log.e(TAG, getLineNumber()+ "  " + tag+":"+log, tr);
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
