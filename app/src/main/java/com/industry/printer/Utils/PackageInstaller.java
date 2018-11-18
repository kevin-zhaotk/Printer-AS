package com.industry.printer.Utils;

import java.io.File;

import com.industry.printer.R;
import com.industry.printer.ui.CustomerDialog.LoadingDialog;

import android.R.integer;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
//import android.content.pm.IPackageInstallObserver;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.PowerManager;
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
		LoadingDialog dialog = LoadingDialog.show(mContext, R.string.str_upgrade_progress);
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
