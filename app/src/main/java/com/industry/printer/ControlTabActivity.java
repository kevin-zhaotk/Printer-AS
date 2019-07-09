package com.industry.printer;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeoutException;

import com.industry.printer.Constants.Constants;
import com.industry.printer.FileFormat.DotMatrixFont;
import com.industry.printer.FileFormat.QRReader;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Socket_Server.Network;
import com.industry.printer.Socket_Server.PCCommand;
import com.industry.printer.Socket_Server.Paths_Create;
import com.industry.printer.Socket_Server.Printer_Database;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

import com.industry.printer.Utils.KZFileObserver;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.Utils.PreferenceConstants;
import com.industry.printer.Utils.PrinterDBHelper;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.data.DataTask;
import com.industry.printer.hardware.ExtGpio;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.hardware.LRADCBattery;
import com.industry.printer.hardware.RFIDDevice;
import com.industry.printer.hardware.RFIDManager;
import com.industry.printer.hardware.RTCDevice;
import com.industry.printer.hardware.UsbSerial;
import com.industry.printer.interceptor.ExtendInterceptor;
import com.industry.printer.interceptor.ExtendInterceptor.ExtendStat;
import com.industry.printer.object.BarcodeObject;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.TlkObject;
import com.industry.printer.ui.CustomerDialog.ConfirmDialog;
import com.industry.printer.ui.CustomerDialog.DialogListener;
import com.industry.printer.ui.CustomerDialog.MessageGroupsortDialog;
import com.industry.printer.ui.ExtendMessageTitleFragment;
import com.industry.printer.ui.CustomerAdapter.PreviewAdapter;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnPositiveListener;
import com.industry.printer.ui.CustomerDialog.LoadingDialog;
import com.industry.printer.ui.CustomerDialog.MessageBrowserDialog;
import com.industry.printer.ui.CustomerDialog.MessageBrowserDialog.OpenFrom;

import android.app.ActionBar.LayoutParams;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.HorizontalScrollView;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class ControlTabActivity extends Fragment implements OnClickListener, InkLevelListener, OnTouchListener, DataTransferThread.Callback {
	public static final String TAG="ControlTabActivity";
	
	public static final String ACTION_REOPEN_SERIAL="com.industry.printer.ACTION_REOPEN_SERIAL";
	public static final String ACTION_CLOSE_SERIAL="com.industry.printer.ACTION_CLOSE_SERIAL";
	public static final String ACTION_BOOT_COMPLETE="com.industry.printer.ACTION_BOOT_COMPLETED";

	private int repeatTimes = 10;

	public Context mContext;
	public ExtendMessageTitleFragment mMsgTitle;
	public int mCounter;
	public RelativeLayout mBtnStart;
	public TextView		  mTvStart; 
	public RelativeLayout mBtnStop;
	public TextView		  mTvStop;
	public RelativeLayout mBtnClean;
	public TextView		  mTvClean;
	public Button mBtnOpen;
	public TextView		  mTvOpen;
	//public Button mGoto;
	//public EditText mDstline;
	
	public RelativeLayout	mBtnOpenfile;
	public LinearLayout mllPreview;
	public HorizontalScrollView mScrollView;
	public TextView mMsgFile;
	private TextView tvMsg;
	// public EditText mMsgPreview;
	public TextView mMsgPreview;
	public ImageView mMsgPreImg;
	public Button 	mBtnview;
	public RelativeLayout	mForward;
	public RelativeLayout 	mBackward;
	
	public TextView mRecords;
	
	public LinkedList<Map<String, String>>	mMessageMap;
	public PreviewAdapter mMessageAdapter;
	public ListView mMessageList;
	
	public PreviewScrollView mPreview;
	public ArrayList<BaseObject> mObjList;
	
	public static int mFd;
	
	public BinInfo mBg;
	BroadcastReceiver mReceiver;
	public Handler mCallback;

	private boolean mFlagAlarming = false;
	
	public static FileInputStream mFileInputStream;
	Vector<Vector<TlkObject>> mTlkList;
	Map<Vector<TlkObject>, byte[]> mBinBuffer;
	/*
	 * whether the print-head is doing print work
	 * if no, poll state Thread will read print-header state
	 * 
	 */
	public boolean isRunning;
	// public PrintingThread mPrintThread;
	public DataTransferThread mDTransThread;
	
	private ExtendStat extendStat = null;
	
	public int mIndex;
	public TextView mPrintStatus;
	public TextView mtvInk;
	public TextView mInkLevel;
	public TextView mInkLevel2;
	public TextView mTVPrinting;
	public TextView mTVStopped;
	public TextView mPhotocellState;
	public TextView mEncoderState;
//	public TextView mPower;
	public TextView mPowerV;
	private ImageView mPowerStat;
	public TextView mTime;

	private ImageButton mMsgNext;
	private  ImageButton mMsgPrev;


	public SystemConfigFile mSysconfig;
	/**
	 * UsbSerial device name
	 */
	public String mSerialdev;
	
	private RFIDDevice mRfidDevice;
	private RFIDManager mRfidManager;
	/**
	 * current tlk path opened
	 */
	public String mObjPath=null;

	private int mRfid = 100;
	/**
	 * MESSAGE_OPEN_TLKFILE
	 *   message tobe sent when open tlk file
	 */
	public static final int MESSAGE_OPEN_TLKFILE=0;
	/**
	 * MESSAGE_UPDATE_PRINTSTATE
	 *   message tobe sent when update print state
	 */
	public static final int MESSAGE_UPDATE_PRINTSTATE=1;
	/**
	 * MESSAGE_UPDATE_INKLEVEL
	 *   message tobe sent when update ink level
	 */
	public static final int MESSAGE_UPDATE_INKLEVEL=2;
	/**
	 * MESSAGE_DISMISS_DIALOG
	 *   message tobe sent when dismiss loading dialog 
	 */
	public static final int MESSAGE_DISMISS_DIALOG=3;
	
	/**
	 * MESSAGE_PAOMADENG_TEST
	 *   message tobe sent when dismiss loading dialog 
	 */
	public static final int MESSAGE_PAOMADENG_TEST=4;

	/**
	 * MESSAGE_PRINT_START
	 *   message tobe sent when dismiss loading dialog 
	 */
	public static final int MESSAGE_PRINT_START = 5;
	public static final int MESSAGE_PRINT_CHECK_UID = 15;
	
	/**
	 * MESSAGE_PRINT_STOP
	 *   message tobe sent when dismiss loading dialog 
	 */
	public static final int MESSAGE_PRINT_STOP = 6;
	
	public static final int MESSAGE_PRINT_END = 14;
	
	/**
	 * MESSAGE_INKLEVEL_DOWN
	 *   message tobe sent when ink level change 
	 */
	public static final int MESSAGE_INKLEVEL_CHANGE = 7;
	
	/**
	 * MESSAGE_COUNT_CHANGE
	 *   message tobe sent when count change 
	 */
	public static final int MESSAGE_COUNT_CHANGE = 8;
	
	public static final int MESSAGE_REFRESH_POWERSTAT = 9;
	
	public static final int MESSAGE_SWITCH_RFID = 10;
	
	
	public static final int MESSAGE_RFID_LOW = 11;
	
	public static final int MESSAGE_RFID_ZERO = 12;
	
	public static final int MESSAGE_RFID_ALARM = 13;
	
	public static final int MESSAGE_RECOVERY_PRINT = 16;
	
	public static final int MESSAGE_OPEN_MSG_SUCCESS = 17;
	
	public static final int MESSAGE_RFID_OFF_H7 = 18;

	public static final int MESSAGE_OPEN_GROUP = 19;

	public static final int MESSAGE_OPEN_NEXT_MSG_SUCCESS = 20;

	public static final int MESSAGE_OPEN_PREVIEW = 21;

	/**
	 * the bitmap for preview
	 */
	private Bitmap mPreBitmap;
	/**
	 * 
	 */
	public int[]	mPreBytes;
	
	/**
	 * background buffer
	 *   used for save the background bin buffer
	 *   fill the variable buffer into this background buffer so we get printing buffer
	 */
	public byte[] mBgBuffer;
	/**
	 *printing buffer
	 *	you should use this buffer for print
	 */
	public byte[] mPrintBuffer;
	
	/**
	 * 褰撳墠鎵撳嵃浠诲姟
	 */
	public List<MessageTask> mMsgTask = new ArrayList<MessageTask>();
	/**
	 * preview buffer
	 * 	you should use this buffer for preview
	 */
	public byte[] mPreviewBuffer;
	
	private boolean mFeatureCorrect = false;
	//Socket___________________________________________________________________________________________
		private Network Net;//checking net;
		private String hostip,aimip;// ip addr
		private Handler myHandler=null;//rec infor prpcess handle
		private String Commands="";// command word;
		private static final int PORT =3550; // port number;
		private volatile ServerSocket server=null; //socket service object
		private ExecutorService mExecutorService = null; //hnadle ExecutorService
		private List<Socket> mList = new ArrayList<Socket>(); //socket list
		private Map<Socket, Service> mServices = new HashMap<Socket, Service>();
		private volatile boolean flag= true;// status flag
		private String PrnComd="";//printing word
		private Printer_Database Querydb;// database class
		private Paths_Create Paths=new Paths_Create();//get and creat path class
		private String AddPaths;//create paths
		private String Scounts;//add counter
		private Stack<String> StrInfo_Stack  = new Stack<String>();// str stack infor
		private PackageInfo pi; //system infor pack
		private StringBuffer sb = new StringBuffer(); //str area word
		private HashMap<String, String> map = new HashMap<String, String>();//map area word
		private int PrinterFlag=0;
		private int SendFileFlag=0;
		private int CleanFlag=0;
		private int StopFlag=0;
		private Socket Gsocket;  
		
		//Socket___________________________________________________________________________________________
	
	public ControlTabActivity() {
		//mMsgTitle = (ExtendMessageTitleFragment)fragment;
		mCounter = 0;
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		//requestWindowFeature(Window.FEATURE_NO_TITLE);
		return inflater.inflate(R.layout.control_frame, container, false);
	}
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {	
		super.onActivityCreated(savedInstanceState);
		mIndex=0;
		mTlkList = new Vector<Vector<TlkObject>>();
		mBinBuffer = new HashMap<Vector<TlkObject>, byte[]>();
		mObjList = new ArrayList<BaseObject>();
		mContext = this.getActivity();
		mSysconfig = SystemConfigFile.getInstance(mContext);
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_REOPEN_SERIAL);
		filter.addAction(ACTION_CLOSE_SERIAL);
		filter.addAction(ACTION_BOOT_COMPLETE);
		mReceiver = new SerialEventReceiver(); 
		mContext.registerReceiver(mReceiver, filter);
		
		mMsgFile = (TextView) getView().findViewById(R.id.opened_msg_name);
		tvMsg = (TextView) getView().findViewById(R.id.tv_msg_name);
		
		mPreview = (PreviewScrollView ) getView().findViewById(R.id.sv_preview);
		
		mBtnStart = (RelativeLayout) getView().findViewById(R.id.StartPrint);
		mBtnStart.setOnClickListener(this);
		mBtnStart.setOnTouchListener(this);
		mTvStart = (TextView) getView().findViewById(R.id.tv_start);
		
		mBtnStop = (RelativeLayout) getView().findViewById(R.id.StopPrint);
		mBtnStop.setOnClickListener(this);
		mBtnStop.setOnTouchListener(this);
		mTvStop = (TextView) getView().findViewById(R.id.tv_stop);
		//mRecords = (TextView) getView().findViewById(R.id.tv_records);
		/*
		 *clean the print head
		 *this command unsupported now 
		 */
		
		mBtnClean = (RelativeLayout) getView().findViewById(R.id.btnFlush);
		mBtnClean.setOnClickListener(this);
		mBtnClean.setOnTouchListener(this);
		mTvClean = (TextView) getView().findViewById(R.id.tv_flush);
				
		mBtnOpenfile = (RelativeLayout) getView().findViewById(R.id.btnBinfile);
		mBtnOpenfile.setOnClickListener(this);
		mBtnOpenfile.setOnTouchListener(this);
		mTvOpen = (TextView) getView().findViewById(R.id.tv_binfile);

		mMsgPrev = (ImageButton) getView().findViewById(R.id.ctrl_btn_up);
		mMsgNext = (ImageButton) getView().findViewById(R.id.ctrl_btn_down);
		mMsgPrev.setOnClickListener(this);
		mMsgNext.setOnClickListener(this);

		setupViews();
		
		mTVPrinting = (TextView) getView().findViewById(R.id.tv_printState);
		mTVStopped = (TextView) getView().findViewById(R.id.tv_stopState);
		
		
		switchState(STATE_STOPPED);
		mScrollView = (HorizontalScrollView) getView().findViewById(R.id.preview_scroll);
		mllPreview = (LinearLayout) getView().findViewById(R.id.ll_preview);
		// mMsgPreview = (TextView) getView().findViewById(R.id.message_preview);
		// mMsgPreImg = (ImageView) getView().findViewById(R.id.message_prev_img);
		//
//		mPrintState = (TextView) findViewById(R.id.tvprintState);
		mtvInk = (TextView) getView().findViewById(R.id.tv_inkValue);
		mInkLevel = (TextView) getView().findViewById(R.id.ink_value);
		mInkLevel2 = (TextView) getView().findViewById(R.id.ink_value2);

		mPowerStat = (ImageView) getView().findViewById(R.id.power_value);
//		mPower = (TextView) getView().findViewById(R.id.power_state);
		mPowerV = (TextView) getView().findViewById(R.id.powerV);
		mTime = (TextView) getView().findViewById(R.id.time);
		
		
		refreshPower();
		//  鍔犺浇鎵撳嵃璁℃暟
		PrinterDBHelper db = PrinterDBHelper.getInstance(mContext);
		//mCounter = db.getCount(mContext);
		RTCDevice rtcDevice = RTCDevice.getInstance(mContext);

		// 濡傛灉鏄涓�娆″惎鍔紝鍚慠TC鐨凬VRAM鍐欏叆0
		if (PlatformInfo.PRODUCT_SMFY_SUPER3.equalsIgnoreCase(PlatformInfo.getProduct())) {
			rtcDevice.initSystemTime(mContext);
			mCounter = rtcDevice.readCounter(mContext);
			if (mCounter == 0) {
				rtcDevice.writeCounter(mContext, 0);
				db.setFirstBoot(mContext, false);
			}
		}
		/* 濡傛灉瑷疆鍙冩暩32鐖瞣n锛岃▓鏁稿櫒閲嶇疆 */
		if (mSysconfig.getParam(31) == 1) {
			mCounter = 0;
		}
		/***PG1 PG2杈撳嚭鐘舵�佷负 0x11锛屾竻闆舵ā寮�**/
		FpgaGpioOperation.clean();
		
		//Debug.d(TAG, "===>loadMessage");
		// 閫氳繃鐩戝惉绯荤粺骞挎挱鍔犺浇
		SharedPreferences p = mContext.getSharedPreferences(SettingsTabActivity.PREFERENCE_NAME, Context.MODE_PRIVATE);
		boolean loading = p.getBoolean(PreferenceConstants.LOADING_BEFORE_CRASH, false);
		/**
		 * if crash happened when load the last message, don`t load it again
		 * avoid endless loop of crash
		 */
		Debug.d(TAG, "--->loading: " + loading);
		if (!loading) {
			loadMessage();
		}
		
		
		/****鍒濆鍖朢FID****/
		mRfidManager = RFIDManager.getInstance(mContext);
		mHandler.sendEmptyMessageDelayed(RFIDManager.MSG_RFID_INIT, 1000);
		
		refreshCount();
		SocketBegin();// Beging Socket service start;
		Querydb=new Printer_Database(mContext);

	}

	@Override
	public void onResume() {
		super.onResume();
		Debug.e(TAG, "--->onResume");
	}

	private String opendTlks() {
		StringBuilder name = new StringBuilder();
		if (mMsgTask != null) {
			for (MessageTask task : mMsgTask) {
				name.append(task.getName());
				name.append("^");
			}
			if (name.length() > 0) {
				name.deleteCharAt(name.length() - 1);
			}
			return name.toString();
		}
		return null;
	}
	public void onConfigureChanged() {
		mMsgFile.setText(opendTlks());
		int heads = 1;
		tvMsg.setText(R.string.str_msg_name);
		mTvStart.setText(R.string.str_btn_print);
		mTvStop.setText(R.string.str_btn_stop);
		mTvOpen.setText(R.string.str_openfile);
		mTvClean.setText(R.string.str_btn_clean);
		mTVPrinting.setText(R.string.str_state_printing);
		mTVStopped.setText(R.string.str_state_stopped);
		mtvInk.setText(R.string.str_state_inklevel);
		if (mSysconfig.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0) {
			heads = mSysconfig.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS);
		} else {
			heads = mSysconfig.getPNozzle().mHeads;
		}
		Debug.d(TAG, "--->onConfigChanged: " + heads + "   -- " + RFIDManager.TOTAL_RFID_DEVICES);
		if (heads > RFIDManager.TOTAL_RFID_DEVICES) {
			mRfidManager = RFIDManager.getInstance(mContext,true);
			mHandler.sendEmptyMessageDelayed(RFIDManager.MSG_RFID_INIT, 1000);
		}
		onConfigChange();
	}
	
	private void setupViews() {
		if (PlatformInfo.PRODUCT_FRIENDLY_4412.equalsIgnoreCase(PlatformInfo.getProduct())) {
			mForward.setVisibility(View.GONE);
			mBackward.setVisibility(View.GONE);
		}
	}
	
	@Override
	public void onDestroy()
	{
		mContext.unregisterReceiver(mReceiver);
		super.onDestroy();

		//UsbSerial.close(mFd);
	}
	
	public void loadMessage() {
		String f = mSysconfig.getLastMsg();
		Debug.d(TAG, "--->load message: " + f );
		if (f == null || f.isEmpty() || !new File(ConfigPath.getTlkDir(f)).exists()) {
			return;
		}
		Message msg = mHandler.obtainMessage(MESSAGE_OPEN_PREVIEW);
		Bundle bundle = new Bundle();
		bundle.putString("file", f);
		msg.setData(bundle);
		mHandler.sendMessageDelayed(msg, 1000);
	}

	/**
	 * load tlk and print it after load success
	 * @param message
	 */
	public void loadAndPrint(String message) {
		Message msg = mHandler.obtainMessage(MESSAGE_OPEN_PREVIEW);
		Bundle bundle = new Bundle();
		bundle.putString("file", message);
		bundle.putBoolean("printAfterLoad", true);
		msg.setData(bundle);
		mHandler.sendMessageDelayed(msg, 1000);

	}


	public void reloadSettingAndMessage() {
		mSysconfig.init();
		loadMessage();
	}
	private void switchRfid() {
		mRfid += 1;
		int heads = mSysconfig.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0 ? mSysconfig.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) : mSysconfig.getPNozzle().mHeads;
		if (mRfid >= RFIDManager.TOTAL_RFID_DEVICES || mRfid >= heads) {
			mRfid = 0;
		}
//		Debug.d(TAG, "--->switchRfid to: " + mRfid);
		refreshInk();
		// refreshCount();
		mHandler.sendEmptyMessageDelayed(MESSAGE_SWITCH_RFID, 3000);
	}
	
	boolean mInkLow = false;
	boolean mInkZero = false;

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
	}

	private void refreshInk() {
		
		float ink = mRfidManager.getLocalInk(mRfid);
//		Debug.d(TAG, "--->refresh ink: " + mRfid + " = " + ink);
		String level = String.valueOf(mRfid + 1) + "-" + (String.format("%.1f", ink) + "%");
		
		if (!mRfidManager.isValid(mRfid)) {
			mInkLevel.setBackgroundColor(Color.RED);
			mInkLevel.setText(String.valueOf(mRfid + 1) + "--");
			
		} else if (ink > 0){
			//mInkLevel.clearAnimation();
			mInkLevel.setBackgroundColor(0x436EEE);
			mInkLevel.setText(level);
		} else {
			mInkLevel.setBackgroundColor(Color.RED);
			mInkLevel.setText(level);
			//閹栧�肩埐0鍋滄鎵撳嵃
			if (mDTransThread != null && mDTransThread.isRunning()) {
				mHandler.sendEmptyMessage(MESSAGE_PRINT_STOP);
			}
			
		}
		// Debug.e(TAG, "--->ink = " + ink + ", " + (ink <= 1.0f) + ", " + (ink > 0f));
		// Debug.e(TAG, "--->ink = " + ink + ", " + (ink <= 0f));
		if (ink <= 1.0f && ink > 0f && mInkLow == false) {
			mInkLow = true;
			mHandler.sendEmptyMessageDelayed(MESSAGE_RFID_LOW, 5000);
		} else if (ink <= 0f && mInkZero == false) {
			mInkZero = true;
			mHandler.removeMessages(MESSAGE_RFID_LOW);
			if (!Configs.IGNORE_RFID) {
				mHandler.sendEmptyMessageDelayed(MESSAGE_RFID_ZERO, 2000);
			}
			
		} else {
			mFlagAlarming = false;
		}
		refreshVoltage();
		refreshPulse();
	}
	
	private void refreshCount() {
		float count = 0;
		// String cFormat = getResources().getString(R.string.str_print_count);
		// ((MainActivity)getActivity()).mCtrlTitle.setText(String.format(cFormat, mCounter));

		RFIDDevice device = mRfidManager.getDevice(mRfid);
		if (device != null && mDTransThread != null) {
			count = device.getLocalInk() - 1;
			Debug.d(TAG, "--->count: " + count);
			count = count * mDTransThread.getInkThreshold(0) + mDTransThread.getCount();
		}
		if (count < 0) {
			count = 0;
		}
		Debug.d(TAG, "--->refreshCount: " + count);
		((MainActivity) getActivity()).setCtrlExtra(mCounter, (int) count);
	}
	
	private void setDevNo(String dev) {
		((MainActivity) getActivity()).setDevNo(dev);
	}
	
	/**
	 * 娓│瀵﹂殯鎯呮硜鐖瞤ower鍊煎湪35-44涔嬮枔锛屽皪瀵﹂殯鍊奸�茶灏嶆噳
	 */
	private void refreshPower() {
		Debug.d(TAG, "--->refreshPower");
		if (PlatformInfo.PRODUCT_SMFY_SUPER3.equalsIgnoreCase(PlatformInfo.getProduct())) {
			int power = LRADCBattery.getPower();
			Debug.d(TAG, "--->power: " + power);
			if (power >= 41) {
//				mPower.setText(String.valueOf(100));
				mPowerStat.setImageResource(R.drawable.battery100);
			} else if (power >= 38) {
//				mPower.setText(String.valueOf(75));
				mPowerStat.setImageResource(R.drawable.battery75);
			} else if (power >= 36) {
//				mPower.setText(String.valueOf(50));
				mPowerStat.setImageResource(R.drawable.battery50);
			} else if (power >= 35) {
//				mPower.setText(String.valueOf(25));
				mPowerStat.setImageResource(R.drawable.battery25);
			} else if (power >= 33) {
//				mPower.setText(String.valueOf(0));
				mPowerStat.setImageResource(R.drawable.battery0);
			} else if (power >= 20) {
				mPowerStat.setImageResource(R.drawable.battery0);
			} else {
				// mPower.setText("--");
//				mPowerStat.setImageResource(R.drawable.battery0);
				mPowerStat.setVisibility(View.GONE);
			}
			//mPowerV.setText(String.valueOf(power));
			// mTime.setText("0");
			// display Voltage & pulse width
			
			mHandler.sendEmptyMessageDelayed(MESSAGE_REFRESH_POWERSTAT, 5*60*1000);
		}
	}
	
	/**
	 * if setting param25 == on, read from RFID feature 5
	 * if setting param25 == off, read from setting param 26
	 */
	private void refreshVoltage() {
		boolean auto = false;
		if (mRfidManager == null) {
			auto = false;
		} else {
			int vol = mSysconfig.getParam(24);
			if (vol > 0) {
				auto = true;
			}
		}
		
		if (auto) {
			RFIDDevice device = mRfidManager.getDevice(0);
			int vol = device.getFeature(4);
			mPowerV.setText(String.valueOf(vol));
		} else {
			mPowerV.setText(String.valueOf(mSysconfig.getParam(25)));
		}
	}

	/**
	 * if setting param27 == on, read from RFID feature 4
	 * if setting param27 == off, read from setting param 28
	 */
	private void refreshPulse() {
		boolean auto = false;
		if (mRfidManager == null) {
			auto = false;
		} else {
			int p = mSysconfig.getParam(26);
			if (p > 0) {
				auto = true;
			}
		}
		if (auto) {
			RFIDDevice device = mRfidManager.getDevice(0);
			int pulse = device.getFeature(5);
//			Debug.d(TAG, "--->pulse: " + pulse);
			mTime.setText(String.valueOf(pulse));
		} else {
			mTime.setText(String.valueOf(mSysconfig.getParam(27)));
		}
	}


	private boolean messageNew = false;
	private void setMessage(String message) {
		mObjPath = message;
		messageNew = true;
	}

	public int testdata=0;
	public Handler mHandler = new Handler(){
		public void handleMessage(Message msg) {
			final String pcMsg = msg.getData().getString(Constants.PC_CMD, "");
			final boolean printAfterLoad = msg.getData().getBoolean("printAfterLoad", false);
			final boolean printNext = msg.getData().getBoolean("printNext", false);
			switch(msg.what)
			{
				case MESSAGE_OPEN_PREVIEW:
					String tlk = msg.getData().getString("file");
					setMessage(tlk);
					if (mPreBitmap != null) {
						BinFromBitmap.recyleBitmap(mPreBitmap);
					}
					Debug.d(TAG, "--->mObjPath: " + mObjPath);
					mPreBitmap = BitmapFactory.decodeFile(MessageTask.getPreview(mObjPath));

					dispPreview(mPreBitmap);
					mMsgFile.setText(mObjPath);
					mSysconfig.saveLastMsg(mObjPath);

					Message message = mHandler.obtainMessage(MESSAGE_OPEN_TLKFILE);
					if (printAfterLoad) {
						message.sendToTarget();
					}
					if (printNext) {
						Bundle bundle = new Bundle();
						bundle.putBoolean("printNext", printNext);
						message.setData(bundle);
						message.sendToTarget();
					}
					break;

				case MESSAGE_OPEN_TLKFILE:		//

					if (!printNext && !messageNew ) {
						mHandler.sendEmptyMessage(MESSAGE_OPEN_MSG_SUCCESS);
						break;
					}
					progressDialog();
					
//					mObjPath = msg.getData().getString("file");
					final String msgPC = msg.getData().getString(Constants.PC_CMD);

					Debug.d(TAG, "open tlk :" + mObjPath );
					//startPreview();
					if (mObjPath == null) {
						dismissProgressDialog();
						break;
					}
					
					//鏂规2锛氫粠tlk鏂囦欢閲嶆柊缁樺埗鍥剧墖锛岀劧鍚庤В鏋愮敓鎴恇uffer
					//parseTlk(f);
					//initBgBuffer();
					mMsgTask.clear();
					new Thread() {
						@Override
						public void run() {
							mMsgTask.clear();
							/**鑾峰彇鎵撳嵃缂╃暐鍥撅紝鐢ㄤ簬棰勮灞曠幇**/
							if (mObjPath.startsWith("Group-")) {   // group messages
								List<String> paths = MessageTask.parseGroup(mObjPath);
								for (String path : paths) {
									MessageTask task = new MessageTask(mContext, path);
									mMsgTask.add(task);
								}
							} else {
								MessageTask task = new MessageTask(mContext, mObjPath);
								mMsgTask.add(task);
							}
							if (printNext) {
								mHandler.sendEmptyMessage(MESSAGE_OPEN_NEXT_MSG_SUCCESS);
								return;
							}
							Message mesg = mHandler.obtainMessage(MESSAGE_OPEN_MSG_SUCCESS);
							if (msgPC != null) {
								Bundle bundle = new Bundle();
								bundle.putString(Constants.PC_CMD, msgPC);
								mesg.setData(bundle);
							}
//							mHandler.sendEmptyMessage(MESSAGE_OPEN_MSG_SUCCESS);
							mesg.sendToTarget();

						}
					}.start();

					break;
				case MESSAGE_OPEN_GROUP:
					Debug.d(TAG, "--->group");
					ArrayList<String> files = msg.getData().getStringArrayList("file");
					MessageGroupsortDialog dialog = new MessageGroupsortDialog(mContext, files);
					dialog.setOnPositiveClickedListener(new OnPositiveListener() {
						@Override
						public void onClick() {

						}

						@Override
						public void onClick(String content) {
							Debug.d(TAG, "--->group: " + content);
							// save group information & send message to handle opening
							Message msg = mHandler.obtainMessage(MESSAGE_OPEN_PREVIEW);
							Bundle b = new Bundle();
							b.putString("file", content);
							msg.setData(b);
							msg.sendToTarget();
						}
					});
					dialog.show();
					break;
				case MESSAGE_OPEN_NEXT_MSG_SUCCESS:
					if (mDTransThread == null) {
						break;
					}
					mDTransThread.resetTask(mMsgTask);
					mPreBitmap = BitmapFactory.decodeFile(MessageTask.getPreview(mObjPath));
					dispPreview(mPreBitmap);
//					refreshCount();
					mMsgFile.setText(mObjPath);

					mSysconfig.saveLastMsg(mObjPath);
					dismissProgressDialog();

					break;
				case MESSAGE_OPEN_MSG_SUCCESS:


					if (mPreBitmap != null) {
						BinFromBitmap.recyleBitmap(mPreBitmap);
					}
					//鏂规1锛氫粠bin鏂囦欢鐢熸垚buffer
					initDTThread();
					Debug.d(TAG, "--->init thread ok");
					mPreBitmap = BitmapFactory.decodeFile(MessageTask.getPreview(mObjPath));
					// mPreBitmap = mDTransThread.mDataTask.get(0).getPreview();
					
//					dispPreview(mPreBitmap);

					refreshInk();
					refreshCount();
					mMsgFile.setText(mObjPath);

					mSysconfig.saveLastMsg(mObjPath);
//					dismissProgressDialog();

					if (Configs.IGNORE_RFID) {
						mHandler.sendEmptyMessage(MESSAGE_PRINT_START);
					} else {
						mHandler.sendEmptyMessage(MESSAGE_PRINT_CHECK_UID);
					}

					if("100".equals(PrnComd))	
					{
						 msg = mHandler.obtainMessage(MESSAGE_PRINT_START);

						if (pcMsg != null) {
							Bundle bundle = new Bundle();
							bundle.putString(Constants.PC_CMD, pcMsg);
							msg.setData(bundle);
						}

						 mHandler.sendMessage(msg);

						PrnComd="";
					}
					break;
				case MESSAGE_UPDATE_PRINTSTATE:
					String text = msg.getData().getString("text");
					mPrintStatus.setText("result: "+text);
					break;
				case MESSAGE_UPDATE_INKLEVEL:
					Bundle bundle = msg.getData();
					int level = bundle.getInt("ink_level");
					mFeatureCorrect = bundle.getBoolean("feature", true);
					refreshInk();
					break;
				case MESSAGE_DISMISS_DIALOG:
					mLoadingDialog.dismiss();
					break;
				case MESSAGE_PAOMADENG_TEST:
					
					char[] data = new char[32];
					for (char i = 0; i < 15; i++) {
						data[2*i] = (char)(0x01<<i);
						data[2*i+1] = 0xffff;
					}
					data[30] = 0xff;
					data[31] = 0xff;
//					char[] data = new char[2];
//					if (testdata < 0 || testdata > 15)
//						testdata = 0;
//					data[0] = (char) (0x0001 << testdata);
//					data[1] = (char) (0x0001 << testdata);
//					testdata++;
					FpgaGpioOperation.writeData(FpgaGpioOperation.FPGA_STATE_OUTPUT,data, data.length*2);
					mHandler.sendEmptyMessageDelayed(MESSAGE_PAOMADENG_TEST, 1000);
					break;
				case MESSAGE_PRINT_CHECK_UID:

					if (mDTransThread != null && mDTransThread.isRunning()) {
						Debug.d(TAG, "--->printing...");
						handleError(R.string.str_print_printing, pcMsg);
						break;
					}
					//Debug.d(TAG, "--->initDTThread");
					ExtendInterceptor interceptor = new ExtendInterceptor(mContext);
					ExtendStat stat = interceptor.getExtend();
					boolean statChanged = false;
					if (stat != extendStat) {
						statChanged = true;
						extendStat = stat;
					}
					if (mDTransThread == null  || statChanged) {
						initDTThread();
					}
					if (mDTransThread == null) {
						handleError(R.string.str_toast_no_message, pcMsg);
						break;
					}
					Debug.d(TAG, "--->prepare buffer");
					List<DataTask> dt = mDTransThread.getData();
					int heads = 1;
					if (dt != null && dt.size() > 0) {
						heads = dt.get(0).getPNozzle().mHeads;
					}
					mRfidManager.checkUID(heads);
					break;
				case RFIDManager.MSG_RFID_CHECK_FAIL:
					handleError(R.string.toast_rfid_changed, pcMsg);
					break;
				case RFIDManager.MSG_RFID_CHECK_SUCCESS:
				case MESSAGE_PRINT_START:

					Debug.d(TAG, "--->print start");
					if (mDTransThread != null && mDTransThread.isRunning()) {
						handleError(R.string.str_print_printing, pcMsg);
						break;
					}
					if (!checkRfid()) {
						handleError(R.string.str_toast_no_ink, pcMsg);
						return;
					}
					Debug.d(TAG, "--->check rfid ok");

					if (mObjPath == null || mObjPath.isEmpty()) {
						handleError(R.string.str_toast_no_message, Constants.pcErr(pcMsg));
						break;
					}
					// reset value of counter object to system config value
					resetCounterIfNeed();
					if (!checkQRFile()) {
						handleError(R.string.str_toast_no_qrfile, Constants.pcErr(pcMsg));
						mHandler.sendEmptyMessage(MESSAGE_RFID_ALARM);
						break;
					} else {
						mFlagAlarming = false;
					}
					Debug.d(TAG, "--->checkQRFile ok");
					List<DataTask> tasks = mDTransThread.getData();
					DataTask task = tasks.get(0);
					if (!task.mTask.isPrintable()) {
						handleError(task.mTask.unPrintableTips(), pcMsg);
						break;
					}

					Debug.d(TAG, "--->clean");
					/**
					 * 鍚姩鎵撳嵃鍚庤瀹屾垚鐨勫嚑涓伐浣滐細
					 * 1銆佹瘡娆℃墦鍗帮紝  鍏堟竻绌� 锛堣鏂囦欢锛夛紝 鐒跺悗 鍙戣缃�
					 * 2銆佸惎鍔―ataTransfer绾跨▼锛岀敓鎴愭墦鍗癰uffer锛屽苟涓嬪彂鏁版嵁
					 * 3銆佽皟鐢╥octl鍚姩鍐呮牳绾跨▼锛屽紑濮嬭疆璁璅PGA鐘舵��
					 */
					
					/*鎵撳嵃杩囩▼涓姝㈠垏鎹㈡墦鍗板璞�*/
					switchState(STATE_PRINTING);
					FpgaGpioOperation.clean();
					Debug.d(TAG, "--->update settings");
					FpgaGpioOperation.updateSettings(mContext, task, FpgaGpioOperation.SETTING_TYPE_NORMAL);
					Debug.d(TAG, "--->launch thread");

					if (!mDTransThread.launch(mContext)) {
						handleError(R.string.str_toast_no_bin, pcMsg);
						break;
					}
					Debug.d(TAG, "--->finish TrheadId=" + Thread.currentThread().getId());
					handlerSuccess(R.string.str_print_startok, pcMsg);
					break;
				case MESSAGE_PRINT_STOP:
					// do nothing if not in printing state
					if (mDTransThread == null || !mDTransThread.isRunning()) {
						sendToRemote(Constants.pcOk(msg.getData().getString(Constants.PC_CMD)));
						break;
					}
					if (mDTransThread != null && !mDTransThread.isRunning()) {
						switchState(STATE_STOPPED);
						FpgaGpioOperation.clean();
						break;
					}
					FpgaGpioOperation.uninit();
					if (mDTransThread != null) {
						mDTransThread.finish();
//						mDTransThread = null;
//						initDTThread();
					}
					sendToRemote(Constants.pcOk( msg.getData().getString(Constants.PC_CMD)));
					/*鎵撳嵃浠诲姟鍋滄鍚庡厑璁稿垏鎹㈡墦鍗板璞�*/
					switchState(STATE_STOPPED);
					
					ToastUtil.show(mContext, R.string.str_print_stopok);
					FpgaGpioOperation.clean();
//					rollback();
					/* 濡傛灉鐣跺墠鎵撳嵃淇℃伅涓湁瑷堟暩鍣紝闇�瑕佽閷勭暥鍓嶅�煎埌TLK鏂囦欢涓�*/
					updateCntIfNeed();
					
					break;
				case MESSAGE_PRINT_END:
					FpgaGpioOperation.uninit();
					switchState(STATE_STOPPED);
					FpgaGpioOperation.clean();
					break;
				case MESSAGE_INKLEVEL_CHANGE:
					int devIndex = msg.arg1;
					// for (int i = 0; i < mSysconfig.getHeads(); i++) {
					mRfidManager.downLocal(devIndex);
					// }
					/*鎵撳嵃鏅備笉鍐嶅鏅傛洿鏂板ⅷ姘撮噺*/
					// refreshInk();
					// mRfidManager.write(mHandler);
					break;
				case MESSAGE_COUNT_CHANGE:
					mCounter++;
					refreshCount();
					//PrinterDBHelper db = PrinterDBHelper.getInstance(mContext);
					//db.updateCount(mContext, (int) mCounter);
					RTCDevice device = RTCDevice.getInstance(mContext);
					device.writeCounter(mContext, mCounter);
					break;
				case MESSAGE_REFRESH_POWERSTAT:
					refreshPower();
					break;
				case MESSAGE_SWITCH_RFID:
					switchRfid();
					break;
				case RFIDManager.MSG_RFID_INIT:
					mRfidManager.init(mHandler);
					break;
				case RFIDManager.MSG_RFID_INIT_SUCCESS:
					// mRfidManager.read(mHandler);
					break;
				case RFIDManager.MSG_RFID_READ_SUCCESS:
					boolean ready = true;
					Bundle bd = (Bundle) msg.getData();
					for (int i=0; i < mSysconfig.getPNozzle().mHeads; i++) {
						RFIDDevice dev = mRfidManager.getDevice(i);
						if (dev == null) {
							break;
						}
						if (dev.getLocalInk() <= 0) {
							ready = false;
							break;
						}
					}

					if (Configs.IGNORE_RFID) {

						if (repeatTimes <= 0) {
							mRfidManager.defaultInkForIgnoreRfid();
							ready = true;
						} else {
							repeatTimes--;
						}
					}
					if (!ready) {
						mHandler.sendEmptyMessageDelayed(RFIDManager.MSG_RFID_INIT, 5000);
					} else {
						mHandler.removeMessages(MESSAGE_RFID_ZERO);
						mFlagAlarming = false;
						mHandler.sendEmptyMessageDelayed(MESSAGE_RFID_OFF_H7, 2000);
					}
					if (mRfidInit == false) {
						switchRfid();
						refreshCount();
						mRfidInit = true;
					}
					break;
				case MESSAGE_RFID_OFF_H7:
					ExtGpio.writeGpio('h', 7, 0);
					break;
				case RFIDManager.MSG_RFID_WRITE_SUCCESS:
					float ink = mRfidManager.getLocalInk(0);
					refreshInk();
					break;
				case MESSAGE_RFID_LOW:
					Debug.e(TAG, "--->low: play error");
					mHandler.sendEmptyMessage(MESSAGE_RFID_ALARM);
					mHandler.sendEmptyMessageDelayed(MESSAGE_RFID_LOW, 5000);
					break;
				case MESSAGE_RFID_ZERO:
					Debug.e(TAG, "--->zero: play error");
					mHandler.sendEmptyMessage(MESSAGE_RFID_ALARM);
					mHandler.sendEmptyMessageDelayed(MESSAGE_RFID_ZERO, 2000);
					break;
				case MESSAGE_RFID_ALARM:
					mFlagAlarming = true;
					ExtGpio.writeGpio('h', 7, 1);
					if (mRfiAlarmTimes++ < 3) {
						ExtGpio.playClick();
						mHandler.sendEmptyMessageDelayed(MESSAGE_RFID_ALARM, 150);						
					} else {
						mRfiAlarmTimes = 0;
					}
					break;
				case MESSAGE_RECOVERY_PRINT:
					SharedPreferences preference = mContext.getSharedPreferences(SettingsTabActivity.PREFERENCE_NAME, Context.MODE_PRIVATE);
					boolean pCrash = preference.getBoolean("stat_before_crash", false);
					if (pCrash) {
						ToastUtil.show(mContext, R.string.str_recover_print);
						mHandler.sendEmptyMessageDelayed(MESSAGE_PRINT_START, 2000);
						preference.edit().putBoolean("stat_before_crash", false).commit();
					}
					break;
				default:
					break;
			}
		}
	};


	private void handlerSuccess(int toastRs, String pcMsg) {
		ToastUtil.show(mContext, toastRs);
		sendToRemote(Constants.pcOk(pcMsg));
		dismissProgressDialog();
	}
	/**
	 * handle print error message
	 * @param toast
	 * @param pcMsg
	 */
	private void handleError(String toast, String pcMsg) {

		ToastUtil.show(mContext, toast == null ? "failed" : toast);
		sendToRemote(Constants.pcErr(pcMsg));
		dismissProgressDialog();
	}
	/**
	 * handle print error message
	 * @param toastRs
	 * @param pcMsg
	 */
	private void handleError(int toastRs, String pcMsg) {
		if (toastRs == 0) {
			toastRs = R.string.str_toast_no_bin;
		}
		ToastUtil.show(mContext, toastRs);
		sendToRemote(Constants.pcErr(pcMsg));
		dismissProgressDialog();
	}

	private boolean checkRfid() {
		boolean ready = true;
		if (Configs.IGNORE_RFID) {
			return true;
		}
		if (mDTransThread == null) {
			return true;
		}
		//DataTask task = mDTransThread.getData();
		int heads = SystemConfigFile.getInstance(mContext).getPNozzle().mHeads;// task.getHeads();
		for (int i = 0; i < heads; i++) {
			float ink = mRfidManager.getLocalInk(i);
			if (ink <= 0) {
				ready = false;
			}
		}
		return ready;
	}
	
	private boolean checkQRFile() {
		boolean ready = true;
		if (mDTransThread == null) {
			return true;
		}
		QRReader reader = QRReader.getInstance(mContext);
		boolean qrReady = reader.isReady();
		Debug.d(TAG, "--->checkQRfile = " + qrReady);
		DataTask task = mDTransThread.getCurData();
		for (BaseObject obj : task.getObjList()) {
			if (!(obj instanceof BarcodeObject)) {
				continue;
			}
			if (!((BarcodeObject) obj).isQRCode() || !obj.getSource()) {
				continue;
			}
			ready = qrReady;
		}
		return ready;
	}

	private void resetCounterIfNeed() {
		List<DataTask> tasks = mDTransThread.getData();
		if (tasks == null) {
			return;
		}
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
		for (DataTask task : tasks) {
			if (task == null) {
				continue;
			}
			List<BaseObject> objects = task.getObjList();
			for (BaseObject object : objects) {
				if (object instanceof CounterObject) {
					((CounterObject) object).setValue(config.getParam(SystemConfigFile.INDEX_COUNTER));
				}
			}
		}
	}

	private void dispPreview(Bitmap bmp) {
		int x=0,y=0;
		int cutWidth = 0;
		float scale = 1;
		if (bmp == null) {
			return;
		}
		
//		String product = SystemPropertiesProxy.get(mContext, "ro.product.name");
//		DisplayMetrics dm = new DisplayMetrics();
//		getActivity().getWindowManager().getDefaultDisplay().getMetrics(dm);
//		Debug.d(TAG, "--->screen width: " + dm.widthPixels + " height: " + dm.heightPixels + "  dpi= " + dm.densityDpi);
		float height = mllPreview.getHeight();
		scale = (height/bmp.getHeight());
		mllPreview.removeAllViews();
			for (int i = 0;x < bmp.getWidth(); i++) {
				if (x + 1200 > bmp.getWidth()) {
					cutWidth = bmp.getWidth() - x;
				} else {
					cutWidth =1200;
				}
				Bitmap child = Bitmap.createBitmap(bmp, x, 0, cutWidth, bmp.getHeight());
				if (cutWidth * scale < 1 || bmp.getHeight() * scale < 1) {
					child.recycle();
					break;
				}
				Debug.d(TAG, "-->child: " + child.getWidth() + "  " + child.getHeight() + "   view h: " + mllPreview.getHeight());
				Bitmap scaledChild = Bitmap.createScaledBitmap(child, (int) (cutWidth*scale), (int) (bmp.getHeight() * scale), true);
				//child.recycle();
				//Debug.d(TAG, "--->scaledChild  width = " + child.getWidth() + " scale= " + scale);
				x += cutWidth; 
				ImageView imgView = new ImageView(mContext);
				imgView.setScaleType(ScaleType.FIT_XY);
//				if (density == 1) {
					imgView.setLayoutParams(new LayoutParams(scaledChild.getWidth(),scaledChild.getHeight()));
//				} else {
//					imgView.setLayoutParams(new LayoutParams(cutWidth,LayoutParams.MATCH_PARENT));
//				}
				
				imgView.setBackgroundColor(Color.WHITE);
				imgView.setImageBitmap(scaledChild);
				mllPreview.addView(imgView);
				// scaledChild.recycle();
			}
	}
	
	private int mRfiAlarmTimes = 0;
	private boolean mRfidInit = false;
	
	/**
	 * Counter & dynamic QR objects need a roll-back operation after each print-stop
	 * because these dynamic objects generate the next value after each single print finished;
	 * then, if stop printing at that time these values will step forward by "1" to the real value;
	 * a mistake will happen at the next continue printing
	 * Deprecated: move to DataTransferThread to do this  
	 */
	@Deprecated
	private void rollback() {
		if (mMsgTask == null) {
			return;
		}
		int index = mDTransThread.index();
		MessageTask task = mMsgTask.get(index);
		for (BaseObject object : task.getObjects()) {
			if (object instanceof CounterObject) {
				((CounterObject) object).rollback();
			}
		}
	}
	private void updateCntIfNeed() {
		int index = mDTransThread.index();
		if (index >= mMsgTask.size()) return;
		MessageTask task = mMsgTask.get(index);

		for (BaseObject object : task.getObjects()) {
			if (object instanceof CounterObject) {
				Message msg = new Message();
				msg.what = MainActivity.UPDATE_COUNTER;
				try {
					msg.arg1 = Integer.valueOf(((CounterObject) object).getContent());
				} catch (Exception e) {
					break;
				}
				mCallback.sendMessage(msg);
				break;
			}
		}
	}
	
	public void initDTThread() {
		
		if (mMsgTask == null) {
			return;
		}
		if (mDTransThread == null) {
			Debug.d(TAG, "--->Print thread ready");
			mDTransThread = DataTransferThread.getInstance();
			mDTransThread.setCallback(this);
		}
		Debug.d(TAG, "--->init");

		// 鍒濆鍖朾uffer
		mDTransThread.initDataBuffer(mContext, mMsgTask);
		// TLKFileParser parser = new TLKFileParser(mContext, mObjPath);
		// 璁剧疆dot count
		mDTransThread.setDotCount(mMsgTask);
		// 璁剧疆UI鍥炶皟
		mDTransThread.setOnInkChangeListener(this);
		
	}
	
	private final int STATE_PRINTING = 0;
	private final int STATE_STOPPED = 1;
	
	public void switchState(int state) {
		Debug.d(TAG, "--->switchState=" + state);
		switch(state) {
			case STATE_PRINTING:
				mBtnStart.setClickable(false);
				mTvStart.setTextColor(Color.DKGRAY);
				mBtnStop.setClickable(true);
				mTvStop.setTextColor(Color.BLACK);
				mBtnOpenfile.setClickable(false);
				mTvOpen.setTextColor(Color.DKGRAY);
				mTVPrinting.setVisibility(View.VISIBLE);
				mTVStopped.setVisibility(View.GONE);
				mBtnClean.setEnabled(false);
				mTvClean.setTextColor(Color.DKGRAY);

				// mMsgNext.setClickable(false);
				// mMsgPrev.setClickable(false);
				ExtGpio.writeGpio('b', 11, 1);
				break;
			case STATE_STOPPED:
				mBtnStart.setClickable(true);
				mTvStart.setTextColor(Color.BLACK);
				mBtnStop.setClickable(false);
				mTvStop.setTextColor(Color.DKGRAY);
				mBtnOpenfile.setClickable(true);
				mTvOpen.setTextColor(Color.BLACK);
				mTVPrinting.setVisibility(View.GONE);
				mTVStopped.setVisibility(View.VISIBLE);
				mBtnClean.setEnabled(true);
				mTvClean.setTextColor(Color.BLACK);

				// mMsgNext.setClickable(true);
				// mMsgPrev.setClickable(true);
				ExtGpio.writeGpio('b', 11, 0);
				break;
			default:
				Debug.d(TAG, "--->unknown state");
		}
	}
	
	
	public void startPreview()
	{
		Debug.d(TAG, "===>startPreview");
		
		try{
			mPreviewBuffer = Arrays.copyOf(mPrintBuffer, mPrintBuffer.length);
			BinInfo.Matrix880Revert(mPreviewBuffer);
			mPreBytes = new int[mPreviewBuffer.length*8];
			// BinCreater.bin2byte(mPreviewBuffer, mPreBytes);
			mPreview.createBitmap(mPreBytes, mBgBuffer.length/110, Configs.gDots);
			mPreview.invalidate();
			
			//mPreviewRefreshHandler.sendEmptyMessage(0);
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		
	}

	public void onCheckUsbSerial()
	{
		mSerialdev = null;
		File file = new File("/dev");
		if(file.listFiles() == null)
		{
			return ;
		}
		File[] files = file.listFiles(new PrinterFileFilter("ttyACM"));
		for(File f : files)
		{
			if(f == null)
			{
				break;
			}
			Debug.d(TAG, "file = "+f.getName());
			int fd = UsbSerial.open("/dev/"+f.getName());
			Debug.d(TAG, "open /dev/"+f.getName()+" return "+fd);
			if(fd < 0)
			{
				Debug.d(TAG, "open usbserial /dev/"+f.getName()+" fail");
				continue;
			}
			UsbSerial.close(fd);
			mSerialdev = "/dev/"+f.getName();
			break;
		}
	}

	public class SerialEventReceiver extends BroadcastReceiver
	{

		@Override
		public void onReceive(Context context, Intent intent) {
			// TODO Auto-generated method stub
			Debug.d(TAG, "******intent="+intent.getAction());
			if(ACTION_REOPEN_SERIAL.equals(intent.getAction()))
			{
				onCheckUsbSerial();
			}
			else if(ACTION_CLOSE_SERIAL.equals(intent.getAction()))
			{
				Debug.d(TAG, "******close");
				mSerialdev = null;
			}
			else if(ACTION_BOOT_COMPLETE.equals(intent.getAction()))
			{
				onCheckUsbSerial();
				byte info[] = new byte[23];
				UsbSerial.printStart(mSerialdev);
				UsbSerial.getInfo(mSerialdev, info);
				//updateInkLevel(info);
				UsbSerial.printStop(mSerialdev);
			}
		}
		
	}
	
	
	public int calculateBufsize(Vector<TlkObject> list)
	{
		int length=0;
		for(int i=0; i<list.size(); i++)
		{
			Debug.d(TAG,"calculateBufsize list i="+i);
			TlkObject o = list.get(i);
			if(o.isTextObject())	//each text object take over 16*16/8 * length=32Bytes*length
			{
				Debug.d(TAG,"content="+o.mContent);
				DotMatrixFont font = new DotMatrixFont(DotMatrixFont.FONT_FILE_PATH+o.font+".txt");
				int l = font.getColumns();				
				length = (l*o.mContent.length()+o.x) > length?(l*o.mContent.length()+o.x):length;
			}
			else if(o.isPicObject()) //each picture object take over 32*32/8=128bytes
			{
				length = (o.x+128) > length?(o.x+128):length;
			}
		}
		return length;
	}
	
	/*
	 * make set param buffer
	 * 1.  Byte 2-3,param 00,	reserved
	 * 2.  Byte 4-5,param 01,	print speed, unit HZ,43kHZ for highest
	 * 3.  Byte 6-7, param 02,	delay,unit: 0.1mmm
	 * 13. Byte 8-9, param 03,	reserved
	 * 14. Byte 10-11, param 04,triger 00 00  on, 00 01 off
	 * 15. Byte 12-13, param 05,sync  00 00  on, 00 01 off
	 * 16. Byte 14-15, param 06
	 * 17. Byte 16-17, param 07, length, unit 0.1mm
	 * 18. Byte 18-19, param 08, timer, unit ms
	 * 19. Byte 20-21, param 09, print head Temperature
	 * 20. Byte 20-21, param 10,  Ink cartridge Temperature
	 * 21. others reserved  
	 */
	public static void makeParams(Context context, byte[] params)
	{
		if(params==null || params.length<128)
		{
			Debug.d(TAG,"params is null or less than 128, realloc it");
			params = new byte[128];
		}
		SharedPreferences preference = context.getSharedPreferences(SettingsTabActivity.PREFERENCE_NAME, 0);
		int speed = preference.getInt(SettingsTabActivity.PREF_PRINTSPEED, 0);
		params[2] = (byte) ((speed>>8)&0xff);
		params[3] = (byte) ((speed)&0xff);
		int delay = preference.getInt(SettingsTabActivity.PREF_DELAY, 0);
		params[4] = (byte) ((delay>>8)&0xff);
		params[5] = (byte) ((delay)&0xff);
		int triger = (int)preference.getLong(SettingsTabActivity.PREF_TRIGER, 0);
		params[8] = 0x00;
		params[9] = (byte) (triger==0?0x00:0x01);
		int encoder = (int)preference.getLong(SettingsTabActivity.PREF_ENCODER, 0);
		params[10] = 0x00;
		params[11] = (byte) (encoder==0?0x00:0x01);
		int bold = preference.getInt(SettingsTabActivity.PREF_BOLD, 0);
		params[12] = (byte) ((bold>>8)&0xff);
		params[13] = (byte) ((bold)&0xff);
		int fixlen = preference.getInt(SettingsTabActivity.PREF_FIX_LEN, 0);
		params[14] = (byte) ((fixlen>>8)&0xff);
		params[15] = (byte) ((fixlen)&0xff);
		int fixtime= preference.getInt(SettingsTabActivity.PREF_FIX_TIME, 0);
		params[16] = (byte) ((fixtime>>8)&0xff);
		params[17] = (byte) ((fixtime)&0xff);
		int headtemp = preference.getInt(SettingsTabActivity.PREF_HEAD_TEMP, 0);
		params[18] = (byte) ((headtemp>>8)&0xff);
		params[19] = (byte) ((headtemp)&0xff);
		int resvtemp = preference.getInt(SettingsTabActivity.PREF_RESV_TEMP, 0);
		params[20] = (byte) ((resvtemp>>8)&0xff);
		params[21] = (byte) ((resvtemp)&0xff);
		int fontwidth = preference.getInt(SettingsTabActivity.PREF_FONT_WIDTH, 0);
		params[22] = (byte) ((fontwidth>>8)&0xff);
		params[23] = (byte) ((fontwidth)&0xff);
		int dots = preference.getInt(SettingsTabActivity.PREF_DOT_NUMBER, 0);
		params[24] = (byte) ((dots>>8)&0xff);
		params[25] = (byte) ((dots)&0xff);
		int resv12 = preference.getInt(SettingsTabActivity.PREF_RESERVED_12, 0);
		params[26] = (byte) ((resv12>>8)&0xff);
		params[27] = (byte) ((resv12)&0xff);
		int resv13 = preference.getInt(SettingsTabActivity.PREF_RESERVED_13, 0);
		params[28] = (byte) ((resv13>>8)&0xff);
		params[29] = (byte) ((resv13)&0xff);
		int resv14 = preference.getInt(SettingsTabActivity.PREF_RESERVED_14, 0);
		params[30] = (byte) ((resv14>>8)&0xff);
		params[31] = (byte) ((resv14)&0xff);
		int resv15 = preference.getInt(SettingsTabActivity.PREF_RESERVED_15, 0);
		params[32] = (byte) ((resv15>>8)&0xff);
		params[33] = (byte) ((resv15)&0xff);
		int resv16 = preference.getInt(SettingsTabActivity.PREF_RESERVED_16, 0);
		params[34] = (byte) ((resv16>>8)&0xff);
		params[35] = (byte) ((resv16)&0xff);
		int resv17 = preference.getInt(SettingsTabActivity.PREF_RESERVED_17, 0);
		params[36] = (byte) ((resv17>>8)&0xff);
		params[37] = (byte) ((resv17)&0xff);
		int resv18 = preference.getInt(SettingsTabActivity.PREF_RESERVED_18, 0);
		params[38] = (byte) ((resv18>>8)&0xff);
		params[39] = (byte) ((resv18)&0xff);
		int resv19 = preference.getInt(SettingsTabActivity.PREF_RESERVED_19, 0);
		params[40] = (byte) ((resv19>>8)&0xff);
		params[41] = (byte) ((resv19)&0xff);
		int resv20 = preference.getInt(SettingsTabActivity.PREF_RESERVED_20, 0);
		params[42] = (byte) ((resv20>>8)&0xff);
		params[43] = (byte) ((resv20)&0xff);
		int resv21 = preference.getInt(SettingsTabActivity.PREF_RESERVED_21, 0);
		params[44] = (byte) ((resv21>>8)&0xff);
		params[45] = (byte) ((resv21)&0xff);
		int resv22 = preference.getInt(SettingsTabActivity.PREF_RESERVED_22, 0);
		params[46] = (byte) ((resv22>>8)&0xff);
		params[47] = (byte) ((resv22)&0xff);
		int resv23 = preference.getInt(SettingsTabActivity.PREF_RESERVED_23, 0);
		params[48] = (byte) ((resv23>>8)&0xff);
		params[49] = (byte) ((resv23)&0xff);
		
	}
	
	/**
	 * the loading dialog
	 */
	public LoadingDialog mLoadingDialog;
	public Thread mProgressThread;
	public boolean mProgressShowing;
	public void progressDialog()
	{
		SharedPreferences p = mContext.getSharedPreferences(SettingsTabActivity.PREFERENCE_NAME, Context.MODE_PRIVATE);
		p.edit().putBoolean(PreferenceConstants.LOADING_BEFORE_CRASH, true).commit();
		if (mProgressShowing || (mLoadingDialog != null && mLoadingDialog.isShowing())) {
			return;
		}
		mLoadingDialog = LoadingDialog.show(mContext, R.string.strLoading);
		Debug.d(TAG, "===>show loading");
		mProgressShowing = true;
		mProgressThread = new Thread(){
			
			@Override
			public void run(){
				
				try{
					for(;mProgressShowing==true;)
					{
						Thread.sleep(2000);
					}
					Debug.d(TAG, "===>dismiss loading");
					mHandler.sendEmptyMessage(MESSAGE_DISMISS_DIALOG);
				}catch(Exception e)
				{
					
				}
			}
		};
		mProgressThread.start();
	}
	
	public void dismissProgressDialog()
	{
		mProgressShowing=false;
		SharedPreferences p = mContext.getSharedPreferences(SettingsTabActivity.PREFERENCE_NAME, Context.MODE_PRIVATE);
		p.edit().putBoolean(PreferenceConstants.LOADING_BEFORE_CRASH, false).commit();
		Debug.d(TAG, "===>dismissProgressDialog");
	}
	
	public int currentRfid = 0;
	@Override
	public void onClick(View v) {
		
		// ExtGpio.playClick();
		switch (v.getId()) {
			case R.id.StartPrint:
				mHandler.sendEmptyMessage(MESSAGE_OPEN_TLKFILE);
				break;
			case R.id.StopPrint:
				// mHandler.removeMessages(MESSAGE_PAOMADENG_TEST);
				mHandler.sendEmptyMessage(MESSAGE_PRINT_STOP);
				break;
			/*娓呮礂鎵撳嵃澶达紙涓�涓壒娈婄殑鎵撳嵃浠诲姟锛夛紝闇�瑕佸崟鐙殑璁剧疆锛氬弬鏁�2蹇呴』涓� 4锛屽弬鏁�4涓�200锛� 鍙傛暟5涓�20锛�*/
			case R.id.btnFlush:
				DataTransferThread thread = DataTransferThread.getInstance();
				thread.purge(mContext);
				break;
			case R.id.btnBinfile:
				MessageBrowserDialog dialog = new MessageBrowserDialog(mContext, OpenFrom.OPEN_PRINT, mObjPath);
				dialog.setOnPositiveClickedListener(new OnPositiveListener() {
					
					@Override
					public void onClick() {
						ArrayList<String> f = MessageBrowserDialog.getSelected();
						if (f==null || f.size() == 0) {
							return;
						}
						/** 如果选择内容为多个，表示需要新建组 */
						Message msg = mHandler.obtainMessage(MESSAGE_OPEN_PREVIEW);
						Bundle bundle = new Bundle();
						if (f.size() > 1) {
							msg = mHandler.obtainMessage(MESSAGE_OPEN_GROUP);
							bundle.putStringArrayList("file", f);
						} else {
							bundle.putString("file", f.get(0));
						}

						// bundle.putString("file", f);
						msg.setData(bundle);
						mHandler.sendMessage(msg);

					}

					@Override
					public void onClick(String content) {
						// TODO Auto-generated method stub
						
					}
					
				});
				dialog.show();
				break;
			case R.id.btn_page_forward:
				mScrollView.smoothScrollBy(-400, 0);
				break;
			case R.id.btn_page_backward:
				mScrollView.smoothScrollBy(400, 0);
				break;
			case R.id.ctrl_btn_up:
				ConfirmDialog dlg = new ConfirmDialog(mContext, R.string.message_confirm_printnext);
				dlg.setListener(new DialogListener() {
					@Override
					public void onConfirm() {
						super.onConfirm();
						loadMessage(false);
					}

				});
				dlg.show();

				break;
			case R.id.ctrl_btn_down:
				ConfirmDialog dlg1 = new ConfirmDialog(mContext, R.string.message_confirm_printnext);
				dlg1.setListener(new DialogListener() {
					@Override
					public void onConfirm() {
						super.onConfirm();
						loadMessage(true);
					}

				});
				dlg1.show();
				break;
			default:
				break;
		}
		
	}

	private void loadMessage(boolean forward) {
		String msg = null;
		if (forward) {
			msg = loadNextMsg();
		} else {
			msg = loadNPrevMsg();
		}

		Message message = mHandler.obtainMessage(MESSAGE_OPEN_PREVIEW);
		Bundle bundle = new Bundle();
		bundle.putString("file", msg);
		if (mDTransThread != null && mDTransThread.isRunning()) {
			bundle.putBoolean("printNext", true);
		}
		message.setData(bundle);

		mHandler.sendMessageDelayed(message, 100);
	}

	private String loadNextMsg() {
		File msgDir = new File(Configs.TLK_PATH_FLASH);
		String[] tlks = msgDir.list();
		Arrays.sort(tlks, new Comparator<String>() {
			@Override
			public int compare(String s, String t1) {
				try {
					if (s.startsWith("Group-") && t1.startsWith("Group-")) {
						String g1 = s.substring(6);
						String g2 = s.substring(6);
						int gi1 = Integer.parseInt(g1);
						int gi2 = Integer.parseInt(g2);
						if (gi1 > gi2) {
							return 1;
						} else if (gi1 < gi2) {
							return -1;
						} else {
							return 0;
						}
					} else if (s.startsWith("Group-")) {
						return -1;
					} else if (t1.startsWith("Group-")) {
						return 1;
					} else {
						int gi1 = Integer.parseInt(s);
						int gi2 = Integer.parseInt(t1);
						if (gi1 > gi2) {
							return 1;
						} else if (gi1 < gi2) {
							return -1;
						} else {
							return 0;
						}
					}
				} catch (Exception e) {
					return 0;
				}
			}
		});

		for (int i = 0; i < tlks.length; i++) {
			String tlk = tlks[i];
			if (tlk.equalsIgnoreCase(mObjPath)) {
				if (i + 1 < tlks.length ) {
					return tlks[i + 1];
				} else {
					return tlks[0];
				}
			}
		}
		return null;
	}

	private String loadNPrevMsg() {
		File msgDir = new File(Configs.TLK_PATH_FLASH);
		String[] tlks = msgDir.list();
		Arrays.sort(tlks, new Comparator<String>() {
			@Override
			public int compare(String s, String t1) {

				try {
					if (s.startsWith("Group-") && t1.startsWith("Group-")) {
						String g1 = s.substring(6);
						String g2 = s.substring(6);
						int gi1 = Integer.parseInt(g1);
						int gi2 = Integer.parseInt(g2);
						if (gi1 > gi2) {
							return 1;
						} else if (gi1 < gi2) {
							return -1;
						} else {
							return 0;
						}
					} else if (s.startsWith("Group-")) {
						return -1;
					} else if (t1.startsWith("Group-")) {
						return 1;
					} else {
						int gi1 = Integer.parseInt(s);
						int gi2 = Integer.parseInt(t1);
						if (gi1 > gi2) {
							return 1;
						} else if (gi1 < gi2) {
							return -1;
						} else {
							return 0;
						}
					}
				} catch (Exception e) {
					return 0;
				}
			}
		});

		for (int i = 0; i < tlks.length; i++) {
			String tlk = tlks[i];
			if (tlk.equalsIgnoreCase(mObjPath)) {
				if (i > 0 ) {
					return tlks[i - 1];
				} else {
					return tlks[tlks.length - 1];
				}
			}
		}
		return null;
	}

	@Override
	public void onInkLevelDown(int device) {
		Message message = mHandler.obtainMessage();
		message.arg1 = device;
		message.what = MESSAGE_INKLEVEL_CHANGE;
		mHandler.sendMessage(message);
	}

	@Override
	public void onInkEmpty() {
		
	}

	@Override
	public void onCountChanged() {
		mHandler.sendEmptyMessage(MESSAGE_COUNT_CHANGE);
	}

	@Override
	public boolean onTouch(View view, MotionEvent event) {
		switch(view.getId()) {
			case R.id.StartPrint:
			case R.id.StopPrint:
			case R.id.btnFlush:
			case R.id.btnBinfile:
			case R.id.btn_page_forward:
			case R.id.btn_page_backward:
				if (event.getAction() == MotionEvent.ACTION_DOWN) {
//					PWMAudio.Play();
				}
			default:
				break;
		}
		return false;
	}

	public boolean isAlarming() {
		return mFlagAlarming;
	}

	public boolean isPrinting() {
		if (mDTransThread != null) {
			return mDTransThread.isRunning();
		}
		return false;
	}
	
	
	public void setCallback(Handler callback) {
		mCallback = callback;
	}
	
	public void onConfigChange() {
		if (mDTransThread == null || mDTransThread.isRunning()) {
			return;
		}
		mDTransThread.initCount();
		refreshCount();
	}
	
	@Override
	public void OnFinished(int code) {
		Debug.d(TAG, "--->onFinished");
		mHandler.sendEmptyMessage(MESSAGE_PRINT_STOP);
		this.getActivity().runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				ToastUtil.show(mContext, R.string.str_barcode_end);	
			}
		});
		
	}
	
	private void sendToRemote(String msg) {
		try {
			PrintWriter pout = new PrintWriter(new BufferedWriter(  
                     new OutputStreamWriter(Gsocket.getOutputStream())),true); 
             pout.println(msg);
		} catch (Exception e) {
		}
		
	}
	
	//Soect_____________________________________________________________________________________________________________________________
			//通讯 开始
			private void SocketBegin()
			{
				//Net = new Network();
				int nRet = 0;
			//	if (!Net.checkNetWork(mContext)) {
				//	ToastUtil.show(mContext, "没有开启网络...!");
				//	return;
			//	}
				hostip = getLocalIpAddress(); //获取本机
				
				
				final ServerThread serverThread=new ServerThread();
				//flag=true;
				serverThread.start();//线程开始
				

				
		//接收线程处理
			myHandler =new Handler(){	
			public void handleMessage(Message msg)
				{ 
					if(msg.what==0x1234)
					{
						 String ss=msg.obj.toString();
						// RecInfo(msg.obj.toString());
					} else if (msg.what == EditMultiTabActivity.HANDLER_MESSAGE_SAVE_SUCCESS) {
						String cmd = msg.getData().getString(Constants.PC_CMD);
						sendMsg(Constants.pcOk(cmd));
					} //else if (msg.what)
					else
					{
					 String ss=msg.obj.toString();
					}
				}
				};
			}
			public static String toStringHex(String s) {  
			    byte[] baKeyword = new byte[s.length() / 2];  
			    for (int i = 0; i < baKeyword.length; i++) {  
			        try {  
			            baKeyword[i] = (byte) (0xff & Integer.parseInt(s.substring(i * 2, i * 2 + 2), 16));  
			        } catch (Exception e) {  
			            
			        }  
			    }  
			    try {  
			        s = new String(baKeyword, "utf-8");// UTF-16le:Not  
			    } catch (Exception e1) {  
			         
			    }  
			    return s;  
			}  
			
			//获取本机地址
			public static String getLocalIpAddress() {  
			        try {  
			            for (Enumeration<NetworkInterface> en = NetworkInterface  
			                            .getNetworkInterfaces(); en.hasMoreElements();) {  
			                        NetworkInterface intf = en.nextElement();  
			                       for (Enumeration<InetAddress> enumIpAddr = intf  
			                                .getInetAddresses(); enumIpAddr.hasMoreElements();) {  
			                            InetAddress inetAddress = enumIpAddr.nextElement();  
			                            if (!inetAddress.isLoopbackAddress() && !inetAddress.isLinkLocalAddress()) {  
			                            return inetAddress.getHostAddress().toString();  
			                            }  
			                       }  
			                    }  
			                } catch (SocketException ex) {  
			                    Debug.e("WifiPreference IpAddress", ex.toString());
			                }  
			             return null; 
			 }

			
			   
			//Server服务
		    class ServerThread extends Thread {  
		          
		        public void stopServer(){  
		            try {                
		                if(server!=null){                   
		                	server.close();  
		                    System.out.println("close task successed");    
		                }  
		            } catch (IOException e) {               
		                System.out.println("close task failded");          
		                }  
		        }  
		    public void run() {  
		              
		                try {
		                	server = new ServerSocket(PORT);
		                } catch (IOException e1) {  
		                    // TODO Auto-generated catch block  
		                    System.out.println("S2: Error");  
		                    e1.printStackTrace();
							return;
		                }  
		                mExecutorService = Executors.newCachedThreadPool();  //鍒涘缓涓?涓嚎绋嬫睜  
		                //System.out.println("鏈嶅姟鍣ㄥ凡鍚姩...");
		                Socket client = null;  
		                while(flag) {  
		                    try {  
		                        System.out.println("S3: Error");  
		                    client = server.accept();
		                    // set time out of Socket to 5s
		                    client.setSoTimeout(2 * 1000);
		                    //client.setSoTimeout(5000);
		                 //   System.out.println("S4: Error");  
		                    //鎶婂鎴风鏀惧叆瀹㈡埛绔泦鍚堜腑  
//		                    mList.add(client);

								Service service = new Service(client);
								stopOthers();
							mServices.put(client, service);
		                    mExecutorService.execute(service); //鍚姩涓?涓柊鐨勭嚎绋嬫潵澶勭悊杩炴帴
		                     }catch ( IOException e) {  
		                         System.out.println("S1: Error");  
		                        e.printStackTrace();  
		                    }  
		                }  
		             
		              
		        }  
		    }

		    public void sendMsg(String msg) {
				Iterator<Socket> clients = mServices.keySet().iterator();
				for (;clients.hasNext();) {
					Socket socket = clients.next();
					Service service = mServices.get(socket);
					service.sendmsg(msg);
				}
			}
		    
			public void stopOthers() {
				Iterator<Socket> keys = mServices.keySet().iterator();
				for (;keys.hasNext();) {
					Socket socket = keys.next();
					Service service = mServices.get(socket);
					service.stop();
				}
				mServices.clear();
			}

		    //线程池，子线程
		    class Service implements Runnable {  
		         private volatile boolean kk=true;  
		      
		         private BufferedReader in = null;  
		         private String msg = "";  
		           
		         public Service(Socket socket) {  
		        	 Gsocket = socket;  
		             try {  
		                 in = new BufferedReader(new InputStreamReader(socket.getInputStream()));  
		                 
		         		 	 
		         		//map=obtainSimpleInfo(mContext); 
		         		//msg=map.toString();
		         		
		                 //this.sendmsg(Querydb.QuerySqlData("select * from System"));  
		                 this.sendmsg("connected success!!!");  
		             } catch (IOException e) {  
		                 e.printStackTrace();  
		             }  
		               
		         }  

				public void stop() {
					Debug.d(TAG, "--->stop: " + Thread.currentThread().getName());
					kk = false;
				}

				private boolean isTlkReady(File tlk) {
					if (tlk == null) {
						return false;
					}
					File[] files = tlk.listFiles();
					if (files == null || files.length <= 0) {
						return false;
					}
					List<String> list = new ArrayList<String>();
					for (int i = 0; i < files.length; i++) {
						list.add(files[i].getName());
					}
					if (list.contains("1.TLK") && list.contains("1.bin")) {
						return true;
					}
					return false;
				}
		         public void run() {  
		               
		                 while(kk) {  
		                     try {  
		                        if((msg = in.readLine())!= null) {  
		                             //100是打印  
		                          //	msg= toStringHex(msg); 
		                        	Debug.d(TAG, "--->fromPc: " + msg);
		                        	if (msg.indexOf("1000") >= 0) {  // LAN Printing

										cacheBin(Gsocket, msg);
									} else if (msg.indexOf("1100") >= 0) {

		                        	    PCCommand cmd = PCCommand.fromString(msg);
		                        	    DataTransferThread.deleteLanBuffer(Integer.valueOf(cmd.content));
									} else if (msg.indexOf("1200") >= 0) {

										PCCommand cmd = PCCommand.fromString(msg);
										DataTransferThread.getInstance().resetIndex();
									} else if(msg.indexOf("100")>=0) {
										String[] Apath = msg.split("\\|");
										if (Apath == null || Apath.length < 4) {
											continue;
										}

										File msgfile = new File(Apath[3]);
										if (!isTlkReady(msgfile)) {
											sendmsg(Constants.pcErr(msg));
											return;
										}
		                            	if(PrinterFlag==0)
		                            	{
		                            		//打印赵工写好了，再测试
		                            		PrnComd="100";
		                            	    PrinterFlag=1;
		                            		StopFlag=1;
		                            		CleanFlag=0;

		                                 	mObjPath= msgfile.getName();

		                                 	Message message = mHandler.obtainMessage(MESSAGE_OPEN_TLKFILE);
		                                 	Bundle bundle = new Bundle();
		         							bundle.putString("file", mObjPath);  // f表示信息名称
											bundle.putString(Constants.PC_CMD, msg);
		         							message.setData(bundle);
		         							mHandler.sendMessage(message);
		         							// msg = mHandler.obtainMessage(MESSAGE_OPEN_TLKFILE);

		                                 	//}
		                                 //	else
		                                 	//{
		                                 		//this.sendmsg(msg+"recv success!");
		                                 	//}
		                            	}
		                            } 
		                            else if(msg.indexOf("200")>=0)
		                            {
		                            	//200是清洗
//		                            	this.sendmsg("");
		                            		CleanFlag=1;
		                            	DataTransferThread thread = DataTransferThread.getInstance();
		                				thread.purge(mContext);
		                				this.sendmsg(Constants.pcOk(msg));
		                            }
		                            else if(msg.indexOf("300")>=0)
		                            {
		                            	//300发文件
		                            	AddPaths="";
		                            	if(SendFileFlag==0)//发文件等赵工写好了，再测试
		                            	{
		                            		SendFileFlag=1;
		                            		this.sendmsg(WriteFiles(Gsocket,msg));
		                            	}
		                           
		                            }
		                            else if(msg.indexOf("400")>=0)
		                            {
		                            	//400取计数器
		                            	for(int i=0;i<7;i++)
		                            	{
		                            	sendmsg(mCounter+" |\r\nink|"+mRfidManager.getLocalInk(i));//+"|\r\n"+mMsgTask.getName()+"|\r\n");//获取INK无显示问题，赵工这地方改好，前面注示去掉就OK了
		                            	this.sendmsg(Constants.pcOk(msg));
		                            	}
		                            }
		                            else if(msg.indexOf("500")>=0)
		                            {
		                            	//500停止打印
		                            	if(StopFlag==1)
		                            	{
		                            		StopFlag=0;
		                            		PrinterFlag=0;
											Message message = mHandler.obtainMessage(MESSAGE_PRINT_STOP);
											Bundle bundle = new Bundle();
											bundle.putString(Constants.PC_CMD, msg);
											message.setData(bundle);
		                            		message.sendToTarget();
		                            		//this.sendmsg(msg+"recv success!");
		                            	
		                            	}
		                            }
		                            else if(msg.indexOf("600")>=0)
		                            {
		                           //600字符串长成所需文件
		                            	
		                    			String[] strArray = msg.split("\\|");
		                    			
		                    			StrInfo_Stack.push(strArray[3]);//用堆栈存储收的信息，先进称出;

		                    			try {
											int count = Integer.parseInt(strArray[3]);
											for (MessageTask task : mMsgTask) {
												for (BaseObject object : task.getObjects()) {
													if (object instanceof CounterObject) {
														object.setContent(strArray[3]);
													}
												}
											}
										} catch (Exception e) {
											// TODO: handle exception
											this.sendmsg(Constants.pcErr(msg));
										}
										this.sendmsg(Constants.pcOk(msg));
		                            }
		                            else if(msg.indexOf("700")>=0)
		                            {
		                           		//700
		                    			this.sendmsg(getString(R.string.str_build_tlk_start));
		                            	String[] parts = msg.split("\\|");
		                            	for (int j = 0; j < parts.length; j++) {
		                            		Debug.d(TAG, "--->parts[" + j + "] = " + parts[j]);
										}
		                            	
		                            	if (parts != null || parts.length > 4) {
		                            		MakeTlk(parts[3]);
										}
		                            }
		                            else if(msg.indexOf("800")>=0)
		                            {
		                           		//600字符串长成所需文件
		                    			if (deleteFile(msg)) {
		                            		this.sendmsg(Constants.pcOk(msg));
		                            	} else {
		                    			this.sendmsg(Constants.pcErr(msg));
		                            	}
		                            }
		                            else if(msg.indexOf("900")>=0)
		                            {
		                           		//600字符串长成所需文件
		                    			if (deleteDirectory(msg)) {
		                            		this.sendmsg(Constants.pcOk(msg));
		                            	} else {
		                            		this.sendmsg(Constants.pcErr(msg));
		                            	}
		                            } else {
		                                 Message msgLocal = new Message();  
		                                 msgLocal.what = 0x1234;  
		                                 msgLocal.obj =msg+"" ;  
		                                 Debug.i(TAG, "--->" + msgLocal.obj.toString());
                                        Debug.i(TAG, "--->" + msg);
		                                 myHandler.sendMessage(msgLocal);  
		                               
		                                 this.sendmsg(Constants.pcErr(msg));
		                            }
		                                          
								}
		                 } catch (SocketTimeoutException e) {

						 }catch (IOException e) {
		                        Debug.i(TAG, "--->socketE: " + e.getMessage());
		                        kk=false;  
		                        this.sendmsg(Constants.pcErr(msg));
		                        return;
		                    }  
		                     
		                 }  
					     Debug.d(TAG, "--->thread " + Thread.currentThread().getName() + " stop");
		             
		         }  
		         //向客户端发信息
		         public void sendmsg(String msg) {  
		            //System.out.println(msg);
					 Debug.i(TAG, "--->send: " + msg);
		             PrintWriter pout = null;  
		             try {  
		                 pout = new PrintWriter(new BufferedWriter(  
		                         new OutputStreamWriter(Gsocket.getOutputStream())),true);  
		                 pout.println(msg);  
		             }catch (IOException e) {  
		                 e.printStackTrace();  
		             }  
		      }  
		       

		}

		// cache bin
		private void cacheBin(Socket socket, String message) {
			Debug.i(TAG, "--->cacheBin: " + message);
			PCCommand cmd = PCCommand.fromString(message);
			int length = Integer.valueOf(cmd.size);
			byte[] buffer = new byte[length];
			int total = 0;
			int timeout = 5;
			if (length <= 16) {
			    this.sendMsg(Constants.pcErr(message));
			    return;
            }
			char[] remoteBin = new char[(length - 16) / 2];
			try {
				InputStream stream = socket.getInputStream();
				while (true) {
					try {

						Thread.sleep(2000);
						int r = stream.read(buffer);

						break;
					} catch (Exception ex) {
//						this.sendMsg(Constants.pcErr(message));
						Debug.e(TAG, "--->e: " + ex.getMessage());
						timeout--;
						if (timeout ==0 ) break;
					}
				}
			} catch (Exception e) {
				this.sendMsg(Constants.pcErr(message));
			}
			if (timeout == 0) {
				this.sendMsg(Constants.pcErr(message));
			} else {
				for (int i = 0; i < remoteBin.length; i++) {
					remoteBin[i] = (char) (buffer[2 * i + 16] << 8 + buffer[2 * i + 16 + 1]);
				}
				DataTransferThread.setLanBuffer(Integer.valueOf(cmd.content), remoteBin);

                this.sendMsg(Constants.pcOk(message));
            }

//			try {
//				InputStream stream = socket.getInputStream();
//
//				char[] remoteBin = new char[length / 2];
//				int total = 0;
//				int timeout = 3;
//				Debug.i(TAG, "--->stream.length:" + length);
//
//				while (true) {
//
//					try {
//						int read = 0;
//						read = stream.read(buffer);
//
//						int start = total == 0 ? 16 / 2 : 0;
//						for (int i = start; i < read / 2; i++) {
//							remoteBin[i] = (char) (buffer[2 * i] << 8 + buffer[2 * i + 1]);
//						}
//						total += read;
//					} catch (Exception e) {
//						timeout--;
//						if (timeout <= 0) {
//							break;
//						}
//						Debug.e(TAG, "--->stream read e: " + e.getMessage());
////					this.sendMsg(Constants.pcErr(message));
//						try {
//							Thread.sleep(2000);
//						} catch (Exception exception) { }
//
//					}
//				}
//				DataTransferThread.setLanBuffer(Integer.valueOf(cmd.content), remoteBin);
//
//				this.sendMsg(Constants.pcOk(message));
//
//			} catch (Exception ex ) {}


		}
		//获取设备信息
		    private HashMap<String, String> obtainSimpleInfo(Context context){
				//HashMap<String, String> map = new HashMap<String, String>();
				PackageManager mPackageManager = context.getPackageManager();
				PackageInfo mPackageInfo = null;
				try {
					mPackageInfo = mPackageManager.getPackageInfo(context.getPackageName(), PackageManager.GET_ACTIVITIES);
				} catch (NameNotFoundException e) {
					e.printStackTrace();
				}
				 PackageManager pm = mContext.getPackageManager();
			        try {
						pi = pm.getPackageInfo(mContext.getPackageName(), PackageManager.GET_ACTIVITIES);
					} catch (NameNotFoundException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}

			   
			       

			        
			 
				map.put("versionName", mPackageInfo.versionName);
				map.put("versionCode", "_" + mPackageInfo.versionCode);
				map.put("Build_version", "_" + Build.VERSION.RELEASE);
				
				map.put("CPU ABI", "_" + Build.CPU_ABI);
			    map.put("Vendor", "_" + Build.MANUFACTURER);
				map.put("MODEL", "_" + Build.MODEL);
				map.put("SDK_INT", "_" + Build.VERSION.SDK_INT);
				map.put("PRODUCT", "_" +  Build.PRODUCT);
				
				return map;
			}
			//接收信息，并写文件	
			private String WriteFiles(Socket socket,String msg ) {
				
		        if (socket == null)
		        {
		            return "";
		        }
		       
		        InputStream in=null;
				FileOutputStream file = null;
				try {
		            //
		 
		        	String savePath=msg.substring(msg.indexOf("/")+1,msg.lastIndexOf("/"));

		        	String[] Apath = savePath.split("\\/");
		        	 
		        	String TmpFiles=msg.substring(msg.lastIndexOf("/"));
		        	TmpFiles=TmpFiles.substring(TmpFiles.indexOf("/")+1,TmpFiles.indexOf("|"));

		        	String TmpsavePath= Paths.CreateDir(msg);
		        	       
		        
		        	savePath=TmpsavePath+TmpFiles;
				 	InputStream inb=null;
				 	AddPaths="";
			        	inb = socket.getInputStream();
				    	
			        	
			        file = new FileOutputStream(savePath, false);
				
					byte[] buffer = new byte[8192];
				
					int size = -1;
				
					int totalReceived = 0;
					while (true) {
						int read = 0;
						try {
							if (inb != null) {
								read = inb.read(buffer);
							}
						} catch (SocketTimeoutException e) {
							Debug.d(TAG, "--->SocketTimeout: " + read);
							if (totalReceived != 0) {
								read = -1;
							}

						}
						//passedlen += read;
						if (read == -1) {
							break;
						}
						Debug.d(TAG, "--->read: " + read);
						totalReceived += read;
						//下面进度条本为图形界面的prograssBar做的，这里如果是打文件，可能会重复打印出一些相同的百分比
						//System.out.println("文件接收了" +  (passedlen * 100/ len) + "%\n");
						file.write(buffer, 0, read);
					}

				file.flush();
				file.close();

			} catch(Exception e){
				return Constants.pcErr(msg);
			}
		    SendFileFlag=0;
		 	return "000-ok: " + msg;
	}
	private void MakeTlk(String msg)
	{
		Debug.d(TAG, "--->msg: " + msg);
		File file = new File(msg);
		if (file == null) {
			return;
		}
		String tlk = file.getAbsolutePath();
		String Name = file.getParentFile().getName();
		if (!tlk.endsWith("TLK")) {
			return;
		}
		Debug.d(TAG, "--->tlk: " + tlk + "   Name = " + Name);
		MessageForPc message = new MessageForPc(mContext, tlk,Name);

		message.reCreate(mContext, myHandler, msg);
	}
	public boolean deleteFile(String filePath) {
		//delete file
		//getPath2();
	    File file = new File(filePath.substring(filePath.indexOf("/"), filePath.lastIndexOf("/")));
	    //file.setExecutable(true,false); 
	   // file.setReadable(true,false); 
	    //file.setWritable(true,false);
	    if(file.exists()) {
	    if(file.isFile()){
	       file.delete();
	       System.gc();
	       return true; 
	        }
	       
	}
	    return false;
	}
	    /**
	     * 删除文件夹以及目录下的文件
	     * @param   filePath 被删除目录的文件路径
	     * @return  目录删除成功返回true，否则返回false
	     */
	    public boolean deleteDirectory(String filePath) {
	    boolean flag = false;
			Debug.d(TAG, "--->filePath: " + filePath);
	        //如果filePath不以文件分隔符结尾，自动添加文件分隔符
	    filePath=filePath.substring(filePath.indexOf("/"), filePath.lastIndexOf("/"));
	        if (!filePath.endsWith(File.separator)) {
	            filePath = filePath + File.separator;
	        }
			Debug.d(TAG, "--->filePath: " + filePath);
	        File dirFile = new File(filePath);
	        if (!dirFile.exists() || !dirFile.isDirectory()) {
	            return false;
	        }
	        flag = true;
	        File[] files = dirFile.listFiles();
	        //遍历删除文件夹下的所有文件(包括子目录)
	        for (int i = 0; i < files.length; i++) {
	            if (files[i].isFile()) {
	            //删除子文件
	                flag = DeleteFolderFile(files[i].getAbsolutePath());
	                if (!flag) break;
	            } else {
	            //删除子目录
	                flag = deleteDirectory(files[i].getAbsolutePath());
	                if (!flag) break;
	            }
	        }
	        if (!flag) return false;
	        //删除当前空目录
	        return dirFile.delete();
	    }

	    /**
	     *  根据路径删除指定的目录或文件，无论存在与否
	     *@param filePath  要删除的目录或文件
	     *@return 删除成功返回 true，否则返回 false。
	     */
	    public boolean DeleteFolder(String filePath) {
	    File file = new File(filePath);
	        if (!file.exists()) {
	            return false;
	        } else {
	            if (file.isFile()) {
	            // 为文件时调用删除文件方法
	                return DeleteFolderFile(filePath);
	            } else {
	            // 为目录时调用删除目录方法
	                return deleteDirectory(filePath);
	            }
	        }
	    }
	    public boolean  DeleteFolderFile(String filePath) {
	    	//delete file
	    	//getPath2();
	        File file = new File(filePath);
	        //file.setExecutable(true,false); 
	       // file.setReadable(true,false); 
	        //file.setWritable(true,false);
	        if(file.exists()) {
	        if(file.isFile()){
	           file.delete();
	           System.gc();
	           return true; 
	            }
	           
	    }
	        return false;
	    }
	    public String getPath2() {
			String sdcard_path = null;
			String sd_default = Environment.getExternalStorageDirectory()
					.getAbsolutePath();
			Log.d("text", sd_default);
			if (sd_default.endsWith("/")) {
				sd_default = sd_default.substring(0, sd_default.length() - 1);
			}
			// 得到路径
			try {
				Runtime runtime = Runtime.getRuntime();
				Process proc = runtime.exec("mount");
				InputStream is = proc.getInputStream();
				InputStreamReader isr = new InputStreamReader(is);
				String line;
				BufferedReader br = new BufferedReader(isr);
				while ((line = br.readLine()) != null) {
					if (line.contains("secure"))
						continue;
					if (line.contains("asec"))
						continue;
					if (line.contains("fat") && line.contains("/mnt/")) {
						String columns[] = line.split(" ");
						if (columns != null && columns.length > 1) {
							if (sd_default.trim().equals(columns[1].trim())) {
								continue;
							}
							sdcard_path = columns[1];
						}
					} else if (line.contains("fuse") && line.contains("/mnt/")) {
						String columns[] = line.split(" ");
						if (columns != null && columns.length > 1) {
							if (sd_default.trim().equals(columns[1].trim())) {
								continue;
							}
							sdcard_path = columns[1];
						}
					}
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			Log.d("text", sdcard_path);
			return sdcard_path;
		}

	    public void onComplete() {
			String msg=mCounter+" \r\nink"+mRfidManager.getLocalInk(0)+"\r\n"+mObjPath+"\r\n";
			Debug.d(TAG, "--->onComplete: msg = " + msg);
			PrintWriter pout = null;
			this.sendMsg("1000|1");
//	        try {
//	            pout = new PrintWriter(new BufferedWriter(  
//	                   new OutputStreamWriter(Gsocket.getOutputStream())),true);  
//	             pout.println(msg);  
//	         }catch (IOException e) {  
//	             e.printStackTrace();  
//	         }  
		}
		public void onPrinted() {
	    	this.sendMsg("1000|2");
		}

	static char[] sRemoteBin;
	//Socket________________________________________________________________________________________________________________________________
	
}

