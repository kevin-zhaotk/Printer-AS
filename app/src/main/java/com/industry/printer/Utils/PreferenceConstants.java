package com.industry.printer.Utils;

import android.content.Context;
import android.content.SharedPreferences;

public class PreferenceConstants {

	/**
	 * 记录崩溃时是否正在执行打印任务
	 */
	public static final String PRINTING_BEFORE_CRASH = "printing_before_crash";
	/**
	 * 记录崩溃时是否正在加载打印信息
	 */
	public static final String LOADING_BEFORE_CRASH = "loading_before_crash";

	public static final String LAST_VERSION_CODE = "last_version_code";

	/** 打印相关的 SP数据存放在这个文件 */
	public static final String SP_PRINT = "printSp";

	public static final String LOG_COUNT = "log_count";


	public static String getString(Context ctx, String name) {
		SharedPreferences sp = ctx.getSharedPreferences(LAST_VERSION_CODE, Context.MODE_PRIVATE);
		return sp.getString(name, "");
	}


	public static long getLong(Context ctx, String name) {
		SharedPreferences sp = ctx.getSharedPreferences(LAST_VERSION_CODE, Context.MODE_PRIVATE);
		return sp.getLong(name, 0);
	}

	public static int getInt(Context ctx, String name) {
		SharedPreferences sp = ctx.getSharedPreferences(LAST_VERSION_CODE, Context.MODE_PRIVATE);
		return sp.getInt(name, 0);
	}

	public static void putLong(Context ctx, String name, long value) {
		SharedPreferences sp = ctx.getSharedPreferences(LAST_VERSION_CODE, Context.MODE_PRIVATE);
		sp.edit().putLong(name, value).commit();
	}


	public static void putString(Context ctx, String name, String value) {
		SharedPreferences sp = ctx.getSharedPreferences(LAST_VERSION_CODE, Context.MODE_PRIVATE);
		sp.edit().putString(name, value).commit();
	}
}
