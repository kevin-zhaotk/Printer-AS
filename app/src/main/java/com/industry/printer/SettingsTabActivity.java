package com.industry.printer;

import java.io.File;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Locale;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.KZFileObserver;
import com.industry.printer.Utils.PackageInstaller;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.Utils.ReflectCaller;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.hardware.ExtGpio;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.ui.ExtendMessageTitleFragment;
import com.industry.printer.ui.CustomerAdapter.SettingsListAdapter;
import com.industry.printer.ui.CustomerDialog.CalendarDialog;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnPositiveListener;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnNagitiveListener;;
import com.industry.printer.ui.CustomerDialog.PasswordDialog;

import android.R.integer;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
//import android.os.SystemProperties;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class SettingsTabActivity extends Fragment implements OnClickListener, OnTouchListener {
public static final String TAG="SettingsTabActivity";
	
	SharedPreferences mPreference=null;
	public final static String PREFERENCE_NAME="Settings";
/*	
	1.  Byte 2-3,  setting 00,  Reserved. 
	2.  Byte 4-5,  setting 01,  printing frequency.  Unit: Hz, (0-43K Hz)
	3.  Byte 6-7,  setting 02,  Delay.               Unit: 0.1mmm  
	4.  Byte 8-9,  setting 03,  Reserved. 
	5.  Byte 10-11,  setting 04, Photocell.           00 00 : ON.  00 01 OFF. 
	6.  Byte 12-13,  setting 05, Encoder.             00 00 : ON.  00 01 OFF.  
	7.  Byte 14-15,  setting 06,  Bold.  
	8.  Byte 16-17,  setting 07,  Fix length trigger.  Unit: 0.1mmm 
	9.  Byte 18-19,  setting 08,  Fix time  trigger.  Unit: ms
	10. Byte 20-21,  setting 09,  Temperature of print head.  Unit: C 00-130.
	11. Byte 20-21,   setting 09,  Temperature of reservoir.  Unit: C   00-130.
	12. Others ,     Setting 10-63,  Reserved. 
*/
	public static final String PREF_PARAM_0="param0";			//00
	public static final String PREF_PRINTSPEED="printspeed";	//01
	public static final String PREF_DELAY="delay";				//02
	public static final String PREF_TRIGER="triger";			//04
	public static final String PREF_ENCODER="encoder";			//05
	public static final String PREF_BOLD="bold";				//06
	public static final String PREF_FIX_LEN="fix_length_triger";	//07
	public static final String PREF_FIX_TIME="fix_time_triger";	//08
	public static final String PREF_HEAD_TEMP="head_temp";		//09
	public static final String PREF_RESV_TEMP="reservoir_temp";	//10
	public static final String PREF_FONT_WIDTH="fontwidth";		//11
	public static final String PREF_DOT_NUMBER="dot_number";	//12
	public static final String PREF_RESERVED_12="reserved12";	//13
	public static final String PREF_RESERVED_13="reserved13";	//14
	public static final String PREF_RESERVED_14="reserved14";	//15
	public static final String PREF_RESERVED_15="reserved15";	//16
	public static final String PREF_RESERVED_16="reserved16";	//17
	public static final String PREF_RESERVED_17="reserved17";	//18
	public static final String PREF_RESERVED_18="reserved18";	//19
	public static final String PREF_RESERVED_19="reserved19";	//20
	public static final String PREF_RESERVED_20="reserved20";	//21
	public static final String PREF_RESERVED_21="reserved21";	//22
	public static final String PREF_RESERVED_22="reserved22";	//23
	public static final String PREF_RESERVED_23="reserved23";	//24
		
	public static final String PREF_DIRECTION="direction";
	public static final String PREF_HORIRES="horires";
	public static final String PREF_VERTRES="vertres";
	
	
	public TextView mTime;
	public TextView mVersion;

	public RelativeLayout		mSave;
	
	public RelativeLayout		mUpgrade;
	public RelativeLayout		mSetDate;
	public RelativeLayout		mSettings;
	public RelativeLayout 		mPagePrev;
	public RelativeLayout 		mPageNext;
	public RelativeLayout		mTimeset;
	public RelativeLayout		mClean;

	public Context 			mContext;
	
	public ScrollView			mScrollView;
	public PHSettingFragment 	mPHSettings;
	public SettingsFragment		mSettingsFragment;
	public ListView 			mListView;
	public SettingsListAdapter 	mAdapter;
	
	public ExtendMessageTitleFragment mMsgTitle;
	
	public SystemConfigFile mSysconfig;
	
	public SettingsTabActivity() {
//		mMsgTitle = (ExtendMessageTitleFragment)fragment;
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		Debug.d(TAG, "-->onCreateView");
		return inflater.inflate(R.layout.setting_layout, container, false);
	}
	
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		
		super.onActivityCreated(savedInstanceState);
		//requestWindowFeature(Window.FEATURE_NO_TITLE);
		//this.addPreferencesFromResource(R.xml.settings_layout);
		mContext = getActivity();
		Debug.d(TAG, "--->onActivityCreated");
		
//		mTime = (TextView) findViewById(R.id.tv_systemTime);
//		mTimeRefreshHandler.sendEmptyMessageDelayed(0, 2000);
		
		mPagePrev = (RelativeLayout) getView().findViewById(R.id.btn_prev);
		mPagePrev.setOnClickListener(this);
		mPagePrev.setOnTouchListener(this);
		
		mPageNext = (RelativeLayout) getView().findViewById(R.id.btn_next);
		mPageNext.setOnClickListener(this);
		mPageNext.setOnTouchListener(this);
		
		mSave = (RelativeLayout) getView().findViewById(R.id.btn_setting_ok);
		mSave.setOnClickListener(this);
		mSave.setOnTouchListener(this);
		
		mUpgrade = (RelativeLayout) getView().findViewById(R.id.btn_setting_upgrade);
		mUpgrade.setOnClickListener(this);
		mUpgrade.setOnTouchListener(this);
		
		mTimeset = (RelativeLayout) getView().findViewById(R.id.btn_setting_timeset);
		mTimeset.setOnClickListener(this);
		mTimeset.setOnTouchListener(this);
		
		mSettings = (RelativeLayout) getView().findViewById(R.id.btn_system_setting);
		mSettings.setOnClickListener(this);
		mSettings.setOnTouchListener(this);
		
		mClean = (RelativeLayout) getView().findViewById(R.id.btn_setting_clean);
		mClean.setOnClickListener(this);
		
		//mScrollView = (ScrollView) getView().findViewById(R.id.setting_frame);
		
		FragmentTransaction transaction = getFragmentManager().beginTransaction();
		/*
		mPHSettings = new PHSettingFragment(mContext);
		transaction.replace(R.id.phsetting_fragment, mPHSettings);
		*/
		/*从U盘中读取系统设置，解析*/
		mSysconfig = SystemConfigFile.getInstance(mContext);
		
		//mSettingsFragment = new SettingsFragment(mContext);
		//transaction.replace(R.id.phsetting_fragment, mSettingsFragment);
		mListView = (ListView) getView().findViewById(R.id.settings_list_view);
		mAdapter = new SettingsListAdapter(mContext);
		mListView.setAdapter(mAdapter);
		//transaction.commit();
		// setupViews();

		PrinterApplication application = (PrinterApplication) mContext.getApplicationContext();
		application.registerCallback("system_config.xml", new KZFileObserver.KZFileObserverInterface() {
			@Override
			public void onChanged() {
				reloadSettings();
			}
		});
	}
	
	private void setupViews() {
		if (PlatformInfo.PRODUCT_SMFY_SUPER3.equals( PlatformInfo.getProduct())) {
			mUpgrade.setVisibility(View.GONE);
			mPagePrev.setVisibility(View.VISIBLE);
			mPageNext.setVisibility(View.VISIBLE);
			mTimeset.setVisibility(View.VISIBLE);
		} else if (PlatformInfo.PRODUCT_FRIENDLY_4412.equals( PlatformInfo.getProduct())) {
			mUpgrade.setVisibility(View.VISIBLE);
			mPagePrev.setVisibility(View.GONE);
			mPageNext.setVisibility(View.GONE);
			mTimeset.setVisibility(View.GONE);
		}
		
	}
	
	@Override
	public void onStart() {
		super.onStart();
		Debug.d(TAG, "--->onstart");
	}
	

	@Override
	public void onResume() {
		super.onResume();
		Debug.d(TAG, "--->onResume");
	}
	
	@Override
	public void onDestroyView() {
		super.onDestroyView();
		Debug.d(TAG, "--->onDestroyView");
	}
	
	@Override
	public void onDestroy() {
		super.onDestroy();
		Debug.d(TAG, "--->onDestroy");
	}
	
	private TextView tv_btnOk; 
	private TextView tv_btnCancel;
	private TextView tv_btnTime;
	private TextView tv_btnSetting; 
	
	public void onConfigureChanged() {
		mAdapter.notifyDataSetChanged();
		tv_btnOk = (TextView) getView().findViewById(R.id.btn_ok_tv);
		tv_btnOk.setText(R.string.str_btn_settings_sync);
		
		tv_btnTime = (TextView) getView().findViewById(R.id.btn_time_tv);
		tv_btnTime.setText(R.string.str_setting_time);
		
		tv_btnSetting = (TextView) getView().findViewById(R.id.btn_setting_tv);
		tv_btnSetting.setText(R.string.str_system_setting);
    }
//	@Override
//	public boolean  onKeyDown(int keyCode, KeyEvent event)  
//	{
//		Debug.d(TAG, "keycode="+keyCode);
//		
//		if(keyCode==KeyEvent.KEYCODE_BACK || keyCode == KeyEvent.KEYCODE_HOME)
//		{
//			Debug.d(TAG, "back key pressed, ignore it");
//			return true;	
//		}
//		return false;
//	}
//	
//
//	@Override
//	public boolean onTouchEvent(MotionEvent event)
//	{
//		Debug.d(TAG, "event:"+event.toString());
//		InputMethodManager manager = (InputMethodManager)getSystemService(Service.INPUT_METHOD_SERVICE);
//		Debug.d(TAG, "ime is active? "+manager.isActive());
//		manager.hideSoftInputFromWindow(SettingsTabActivity.this.getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
////			manager.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_NOT_ALWAYS);
//		return true;
//	}
	
	private void loadSettings() {
		
		mAdapter.loadSettings();
		mAdapter.notifyDataSetChanged();
	}
	
	public void setLocale()
	{
		Configuration config = getResources().getConfiguration(); 
		DisplayMetrics dm = getResources().getDisplayMetrics(); 
		config.locale = Locale.SIMPLIFIED_CHINESE; 
		getResources().updateConfiguration(config, dm); 
		
	}
	
	public void savePreference()
	{
		
	}
	
	public void reloadSettings() {
		Configs.initConfigs(mContext);
		loadSettings();
		// mPHSettings.reloadSettings();
		
	}
	
	public void setParam(int param, int value) {
		mAdapter.setParam(param, value);
	}
	
	public Handler mTimeRefreshHandler = new Handler(){
		public void handleMessage(Message msg) { 
			switch(msg.what)
			{
				case 0:		//
					 long sysTime = System.currentTimeMillis();
					 SimpleDateFormat dateFormat = new SimpleDateFormat("MM/dd/yyyy HH:mm:ss");
//					 mTime.setText(dateFormat.format(new Date()));
					 
					break;
			}
			mTimeRefreshHandler.sendEmptyMessageDelayed(0, 500);
		}
	};
	@Override
	public void onClick(View arg0) {
		if (arg0 == null) {
			return;
		}
		ExtGpio.playClick();
		switch (arg0.getId()) {
			case R.id.btn_prev:
				// mScrollView.arrowScroll(View.FOCUS_UP);
				// mScrollView.scrollBy(0, -300);
				 mListView.smoothScrollBy(-300, 2);
				break;
			case R.id.btn_next:
				// mScrollView.arrowScroll(View.FOCUS_DOWN);
				// mScrollView.scrollBy(0, 300);
				mListView.smoothScrollBy(300, 2);
				break;
			case R.id.btn_setting_ok:
				PasswordDialog pdialog = new PasswordDialog(mContext);
				pdialog.setOnPositiveClickedListener(new OnPositiveListener() {

					@Override
					public void onClick() {
						saveParam();
						ToastUtil.show(mContext, R.string.toast_save_success);
					}

					@Override
					public void onClick(String content) {
						saveParam();
					}
				});
				pdialog.setOnNagitiveClickedListener(new OnNagitiveListener() {
					
					@Override
					public void onClick() {
						reloadSettings();
					}
				});
				pdialog.show();
				break;
			case R.id.btn_setting_cancel:
				// mPHSettings.reloadSettings();
				break;
			case R.id.btn_system_setting:	//进入系统设置
				Intent intent = new Intent();
				intent.setClassName("com.android.settings","com.android.settings.Settings");
				startActivity(intent);
				break;
			case R.id.btn_setting_upgrade:
				
				// ArrayList<String> paths = ConfigPath.getMountedUsb();
				if (PlatformInfo.PRODUCT_SMFY_SUPER3.equals(PlatformInfo.getProduct())) {
					PackageInstaller installer = PackageInstaller.getInstance(getActivity());
					installer.silentUpgrade();
					
				} else {
					String str = ConfigPath.getUpgradePath();
					File file = new File(str);
					Debug.d(TAG, "===>file:"+file.getPath());
					if (!file.exists()) {
						ToastUtil.show(mContext, R.string.strUpgradeNoApk);
						break;
					}
					Debug.d(TAG, "===>start upgrade service");
					// System.setProperty("ctl.start", "Upgrade");
					// SystemProperties.set("ctl.start","Upgrade");
					ToastUtil.show(mContext, R.string.str_upgrade_progress);
					ReflectCaller.SysPropUpgrade();
				}
				break;
			case R.id.btn_setting_timeset:
				CalendarDialog dialog = new CalendarDialog(this.getActivity(), R.layout.calendar_setting);
				dialog.show();
				break;

			case R.id.btn_setting_clean:
				DataTransferThread dThread = DataTransferThread.getInstance();
				dThread.clean(mContext);
				break;
			default :
				Debug.d(TAG, "===>unknown view clicked");
				break;
		}
	}

	@Override
	public boolean onTouch(View view, MotionEvent event) {
		
		switch(view.getId()) {
			case R.id.btn_prev:
			case R.id.btn_next:
			case R.id.btn_setting_ok:
			case R.id.btn_setting_upgrade:
			case R.id.btn_setting_timeset:
			case R.id.btn_system_setting:
//			case R.id.btn_setting_clean:
				if (event.getAction() == MotionEvent.ACTION_DOWN) {
//					PWMAudio.Play();
				}
			default:
				break;
		}
		return false;
	}
	
	private void saveParam() {
		Debug.d(TAG, "===>onclick");
		mAdapter.checkParams();
		mAdapter.notifyDataSetChanged();

		PrinterApplication application = (PrinterApplication) mContext.getApplicationContext();
		application.pauseFileListener();
		mSysconfig.saveConfig();
		((MainActivity) getActivity()).onConfigChange();
		application.resumeFileListener();
		// FpgaGpioOperation.updateSettings(mContext);
		//FpgaGpioOperation device = FpgaGpioOperation.getInstance();
		// device.read();
		if(mSysconfig.getParam(30)==7)
		{				Debug.i(TAG, "111===>onclick");
		   PlatformInfo.SetDotMatrixType(1);
		   
		}else if(mSysconfig.getHeads()==8)
		{				Debug.i(TAG, "222===>onclick");
		   PlatformInfo.SetDotMatrixType(2);
		} 
		else
		{				Debug.i(TAG, "=333==>onclick");
			  PlatformInfo.SetDotMatrixType(0);			
		}
		Debug.i(TAG, "===>onclick: " + mSysconfig.getParam(30));
	}
}