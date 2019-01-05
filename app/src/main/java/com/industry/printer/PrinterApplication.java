package com.industry.printer;

import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.CrashCatcher;
import com.industry.printer.Utils.KZFileObserver;
import com.industry.printer.Utils.ToastUtil;

import android.app.Application;
import android.text.TextUtils;

import java.util.HashMap;
import java.util.Map;

public class PrinterApplication extends Application {

	private KZFileObserver sysObserver;
	private KZFileObserver qrObserver;

	private static PrinterApplication sInstance = null;

	public static PrinterApplication getInstance() {
		return sInstance;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		CrashCatcher catcher = CrashCatcher.getInstance();
		catcher.init(getApplicationContext());
		registerFileListener();
		sInstance = this;
	}

	private void registerFileListener() {
		// system_config.xml
		sysObserver = new KZFileObserver(this, Configs.CONFIG_PATH_FLASH + Configs.SYSTEM_CONFIG_DIR);
		qrObserver = new KZFileObserver(this, Configs.QR_DIR);
		sysObserver.startWatching();
		qrObserver.startWatching();
	}

	/**
	 * pause listening
	 */
	public void pauseFileListener() {
		sysObserver.stopWatching();
	}

	/**
	 * resume listening
	 */
	public void resumeFileListener() {
		sysObserver.startWatching();
	}

	public void registerCallback(String path, KZFileObserver.KZFileObserverInterface callback) {
		sysObserver.registerCallback(path, callback);
//		sysObserver.registerCallback(Configs.QR_DATA,callback);
	}

	public void registeQRCallback(String path, KZFileObserver.KZFileObserverInterface callback) {
		qrObserver.registerCallback(path, callback);
//		sysObserver.registerCallback(Configs.QR_DATA,callback);
	}


}
