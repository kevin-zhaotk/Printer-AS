package com.industry.printer;

import com.industry.printer.Serial.SerialPort;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.CrashCatcher;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.Utils.KZFileObserver;
import com.industry.printer.Utils.PreferenceConstants;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.data.NativeGraphicJni;
import com.industry.printer.hardware.SmartCard;
import com.industry.printer.hardware.SmartCardManager;

import android.app.Application;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.text.TextUtils;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

public class PrinterApplication extends Application {
	public static final String TAG="PrinterApplication";

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
		asyncInit();

		// H.M.Wang 增加一个线程，用来将so文件拷贝到/system/lib目录下
		new Thread() {
			@Override
			public void run() {
				try {
					Debug.d(TAG, "su -> start");
					Process process = Runtime.getRuntime().exec("su");
					DataOutputStream os = new DataOutputStream(process.getOutputStream());
					sleep(100);

					Debug.d(TAG, "chmod 777 /system/lib");
					os.writeBytes("chmod 777 /system/lib\n");
					sleep(100);

                    AssetManager assetManager = sInstance.getAssets();

// H.M.Wang 2020-12-26 追加硬件库赋值功能
                    // 复制硬件处理库
                    Debug.d(TAG, "chmod 777 /system/lib/" + Configs.UPGRADE_HARDWARE_SO);
                    os.writeBytes("chmod 777 /system/lib/" + Configs.UPGRADE_HARDWARE_SO);
                    sleep(100);

                    InputStream is = assetManager.open(Configs.UPGRADE_HARDWARE_SO);

                    FileUtil.writeFile("/system/lib/" + Configs.UPGRADE_HARDWARE_SO, is);
// End of H.M.Wang 2020-12-26 追加硬件库赋值功能

                    // 复制图片处理库
					Debug.d(TAG, "chmod 777 /system/lib/" + Configs.UPGRADE_NATIVEGRAPHIC_SO);
					os.writeBytes("chmod 777 /system/lib/" + Configs.UPGRADE_NATIVEGRAPHIC_SO);
					sleep(100);

					is = assetManager.open(Configs.UPGRADE_NATIVEGRAPHIC_SO);

					FileUtil.writeFile("/system/lib/" + Configs.UPGRADE_NATIVEGRAPHIC_SO, is);

					is.close();

                    // 复制SmartCard库
					Debug.d(TAG, "chmod 777 /system/lib/" + Configs.UPGRADE_SMARTCARD_SO);
					os.writeBytes("chmod 777 /system/lib/" + Configs.UPGRADE_SMARTCARD_SO);
					sleep(100);

					is = assetManager.open(Configs.UPGRADE_SMARTCARD_SO);

					FileUtil.writeFile("/system/lib/" + Configs.UPGRADE_SMARTCARD_SO, is);

					is.close();

                    // 复制SerialPort库
                    Debug.d(TAG, "chmod 777 /system/lib/" + Configs.UPGRADE_SERIAL_SO);
                    os.writeBytes("chmod 777 /system/lib/" + Configs.UPGRADE_SERIAL_SO);
                    sleep(100);

                    is = assetManager.open(Configs.UPGRADE_SERIAL_SO);

                    FileUtil.writeFile("/system/lib/" + Configs.UPGRADE_SERIAL_SO, is);

                    is.close();

                    os.writeBytes("exit\n");

					NativeGraphicJni.loadLibrary();
					if(SmartCardManager.SMARTCARD_ACCESS) SmartCard.loadLibrary();
					SerialPort.loadLibrary();

                } catch (Exception e) {
					Debug.e(TAG, "--->e: " + e.getMessage());
				}
			}
		}.start();

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

	private void asyncInit() {
		long version = PreferenceConstants.getLong(this, PreferenceConstants.LAST_VERSION_CODE);

		Debug.d(TAG, "BuildConfig.VERSION_CODE: " + BuildConfig.VERSION_CODE);
		Debug.d(TAG, "version: " + version);

		if (BuildConfig.VERSION_CODE == version) {
			return;
		}
		new Thread() {
			@Override
			public void run() {
				File font = new File(Configs.FONT_DIR);

				if (font.exists()) {
					FileUtil.deleteFolder(Configs.FONT_DIR);
				}
				font.mkdirs();
				FileUtil.releaseAssets(sInstance, "fonts", Configs.CONFIG_PATH_FLASH);
				PreferenceConstants.putLong(sInstance, PreferenceConstants.LAST_VERSION_CODE, BuildConfig.VERSION_CODE);
			}
		}.start();
	}
}
