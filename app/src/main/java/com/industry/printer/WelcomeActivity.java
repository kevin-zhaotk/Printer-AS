package com.industry.printer;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.Utils.PackageInstaller;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.Utils.ToastUtil;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Window;
import android.widget.Toast;

public class WelcomeActivity extends Activity {
	
	private static final String TAG = WelcomeActivity.class.getSimpleName(); 
	private Context mContext;
	
	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.welcome_layout);
		mContext = getApplicationContext();
		/*初始化系统配置*/
		Configs.initConfigs(mContext);

// H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
		if (!upgrade()) {
// End of H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
//			asyncInit();
			Intent intent = new Intent();
			intent.setClass(this, MainActivity.class);
			startActivity(intent);
			finish();
// H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
		}
// End of H.M.Wang 2022-5-12 修改升级的方法。取消启动后自动升级，改为在设置页面按钮启动升级
	}

	@Override
	public void onBackPressed() {
		return ;
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		Debug.d("", "--->onConfigurationChanged");
	}
	
	private boolean upgrade() {
		if (PlatformInfo.PRODUCT_SMFY_SUPER3.equals(PlatformInfo.getProduct())) {
			//FileUtil.deleteFolder(Configs.FONT_DIR);
			PackageInstaller installer = PackageInstaller.getInstance(this);
			return installer.silentUpgrade();
		}
		return false;
	}

}
