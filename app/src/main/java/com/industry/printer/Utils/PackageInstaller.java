package com.industry.printer.Utils;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

import com.industry.printer.FileFormat.XmlInputStream;
import com.industry.printer.R;
import com.industry.printer.ui.CustomerDialog.LoadingDialog;

import android.R.integer;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
//import android.content.pm.IPackageInstallObserver;
import android.content.pm.PackageManager.NameNotFoundException;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.net.Uri;
import android.os.PowerManager;
import android.util.Log;
import android.widget.Toast;

public class PackageInstaller {
	public static final String TAG = PackageInstaller.class.getSimpleName();
	
	public Context mContext;
	
	public static PackageInstaller mInstance;
	
	public static PackageInstaller getInstance(Context context) {
		if (mInstance == null) {
			mInstance = new PackageInstaller(context);
		}
		return mInstance;
	}
	public PackageInstaller(Context context) {
		mContext = context;
	}
	/*
	public void silentInstall(String packageName, String path) {
		int flags = 0;
		Uri uri = Uri.fromFile(new File(path));
		PackageManager pm = mContext.getPackageManager();
		try {
			PackageInfo pInfo = pm.getPackageInfo(packageName, PackageManager.GET_UNINSTALLED_PACKAGES);
			if (pInfo != null) {
				flags |= PackageManager.INSTALL_REPLACE_EXISTING;
			}
		} catch (Exception e) {
		}
		PackageInstallObserver observer = new PackageInstallObserver();
		pm.installPackage(uri, observer, flags, packageName);
	}
	*/
	private void install() {
		//PowerManager manager = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
		// manager.upgrade();
		ReflectCaller.PowerManagerUpgrade(mContext);
	}
	
	public boolean silentUpgrade() {
		int curVersion = 0;
		final String pkName = mContext.getPackageName();
		final String path = ConfigPath.getUpgradePath();
		Debug.d(TAG, "path:" + path);

		/*判断升级包是否存在*/
		if (path == null || !new File(path).exists()) {
			return false;
		}
		/*判断版本号*/
		PackageManager pm = mContext.getPackageManager();
		try {
			curVersion = pm.getPackageInfo(pkName, 0).versionCode;
		} catch (NameNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		PackageInfo pInfo = pm.getPackageArchiveInfo(path, PackageManager.GET_ACTIVITIES);
		if (pInfo == null) {
			Debug.e(TAG, "===>package info ");
			return false;
		}
		int newVersion = pInfo.versionCode;
		Debug.d(TAG, "===>curVer:" + curVersion + ",  newVer:" + newVersion);
		if (curVersion == newVersion) {
			ToastUtil.show(mContext, R.string.str_no_upgrade);
			return false;
		}
/*
// H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
// 		版本号修改为9位，前5位作为开发版本号，由开发者添加管理，后四位作为用户的特征码，由发布者添加管理
//		如，308461138。30846代表开发版本号，顺序增加；1138作为用户的特征码，由发布者根据特定算法生成添加
//		新的升级策略位，
//			（1） 如果当前apk为旧版本（5位版本号），待升级apk为新版本apk（9位版本号），则无条件升级 （O）
//			（2） 如果当前apk为新版本apk（9位版本号），待升级apk为旧版本（5位版本号），则无不能升级
//			（3） 如果当前apk与待升级apk均为版本apk（9位版本号），根据后4位特征码决定升级方法
//				（3-1）如果两者的特征码相同，则如果开发版本号不同，则执行升级；如果相同，则无需升级 （O）
//				（3-2）如果两者的特征码不同，但两者之中有一个为9999，则执行升级	（O）
//				（3-2）如果两者的特征码不同，但两者之中无一为9999，则执行不能升级

		int curFeature = 0;
		int newFeature= 0;

		if(curVersion / 100000 > 0) {
			curFeature = curVersion % 10000;
		}
		if(newVersion / 100000 > 0) {
			newFeature = newVersion % 10000;
		}

		if(curFeature == 0) {	// 当前apk为旧版apk，可以升级
		} else if(newFeature == curFeature) {	// 两个apk的特征码一致，则可以升级
		} else if(curFeature == 9999 || newFeature == 9999) {	// 两个apk的特征码中有一个为9999
		} else {
			ToastUtil.show(mContext, R.string.str_no_permission);
			return false;
		}

//		LoadingDialog dialog = LoadingDialog.show(mContext, R.string.str_upgrade_progress);
// End of H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
*/
		new Thread() {
			public void run() {
				try{
					Thread.sleep(5000);
				}catch (Exception e) {
					
				}
				// silentInstall(pkName, path);
				install();
			}
		}.start();
		return true;
	}

	public boolean silentUpgrade2() {
		int curVersion = 0;
		final String pkName = mContext.getPackageName();
		final String path = ConfigPath.getUpgradePath();
		Debug.d(TAG, "path:" + path);

		/*判断升级包是否存在*/
		if (path == null || !new File(path).exists()) {
			return false;
		}
		/*判断版本号*/
		PackageManager pm = mContext.getPackageManager();
		try {
			curVersion = pm.getPackageInfo(pkName, 0).versionCode;
		} catch (NameNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		PackageInfo pInfo = pm.getPackageArchiveInfo(path, PackageManager.GET_ACTIVITIES);
		if (pInfo == null) {
			Debug.e(TAG, "===>package info ");
			return false;
		}
		int newVersion = pInfo.versionCode;
		Debug.d(TAG, "===>curVer:" + curVersion + ",  newVer:" + newVersion);
		if (curVersion == newVersion) {
			ToastUtil.show(mContext, R.string.str_no_upgrade);
			return false;
		}

// H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
// 		版本号修改为9位，前5位作为开发版本号，由开发者添加管理，后四位作为用户的特征码，由发布者添加管理
//		如，308461138。30846代表开发版本号，顺序增加；1138作为用户的特征码，由发布者根据特定算法生成添加
//		新的升级策略位，
//			（1） 如果当前apk为旧版本（5位版本号），待升级apk为新版本apk（9位版本号），则无条件升级 （O）
//			（2） 如果当前apk为新版本apk（9位版本号），待升级apk为旧版本（5位版本号），则无不能升级
//			（3） 如果当前apk与待升级apk均为版本apk（9位版本号），根据后4位特征码决定升级方法
//				（3-1）如果两者的特征码相同，则如果开发版本号不同，则执行升级；如果相同，则无需升级 （O）
//				（3-2）如果两者的特征码不同，但两者之中有一个为9999，则执行升级	（O）
//				（3-2）如果两者的特征码不同，但两者之中无一为9999，则执行不能升级

		int curFeature = 0;
		int newFeature= 0;

		if(curVersion / 100000 > 0) {		// 5位以上
			curFeature = curVersion % 10000;		// 取后4位
		}
		if(newVersion / 100000 > 0) {		// 5位以上
			newFeature = newVersion % 10000;		// 取后4位
		}

		if(curFeature == 0) {	// 当前apk为旧版apk，可以升级
		} else if(newFeature == curFeature) {	// 两个apk的特征码一致，则可以升级
//		} else if(curFeature == 9999 || newFeature == 9999) {	// 两个apk的特征码中有一个为9999
		} else {
			ToastUtil.show(mContext, R.string.str_no_permission);
			return false;
		}

//		LoadingDialog dialog = LoadingDialog.show(mContext, R.string.str_upgrade_progress);
// End of H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级

		new Thread() {
			public void run() {
				try{
					Thread.sleep(5000);
				}catch (Exception e) {

				}
				// silentInstall(pkName, path);
				File file = new File(Configs.CONFIG_PATH_FLASH + Configs.LAST_FEATURE_XML);
				if(file.exists()) file.delete();
				install();
			}
		}.start();
		return true;
	}

	
/*	
	class PackageInstallObserver extends IPackageInstallObserver.Stub {  
        public void packageInstalled(String packageName, int returnCode) {  
            // 该方法里面可以根据returnCode的值判断apk安装成功或者失败  
            // 1表示成功 0表示失败  
            if (returnCode == 1) {  
                Debug.e(TAG, packageName + " install Success");  
            } else {  
            	Debug.e(TAG, packageName + "install fail ");  
            }  
        }
    }
    */  
}
