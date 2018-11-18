package com.industry.printer.hardware;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ThreadPoolExecutor;

import android.R.integer;
import android.content.Context;
import android.graphics.Bitmap.Config;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.industry.printer.DataTransferThread;
import com.industry.printer.ThreadPoolManager;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.RFIDAsyncTask.RfidCallback;
import com.printer.corelib.RFIDData;

public class RFIDManager implements RfidCallback{
	
	private static final String TAG = RFIDManager.class.getSimpleName();
	
	private static volatile RFIDManager	mInstance=null;
	private List<RFIDDevice> mRfidDevices = new ArrayList<RFIDDevice>();
	private int mCurrent=0;
	private int mLiveHeads = 1;
	private RFIDDevice mDevice;
	private Handler mCallback;

	
	public static int TOTAL_RFID_DEVICES = 8;
	
	public static final int MSG_RFID_INIT_SUCCESS = 101;
	public static final int MSG_RFID_INIT_FAIL = 102;
	public static final int MSG_RFID_WRITE_SUCCESS = 103;
	public static final int MSG_RFID_WRITE_FAIL = 104;
	public static final int MSG_RFID_READ_SUCCESS = 105;
	public static final int MSG_RFID_READ_FAIL = 106;
	public static final int MSG_RFID_INIT = 107;
	public static final int MSG_RFID_CHECK_FAIL = 108;
	public static final int MSG_RFID_CHECK_SUCCESS = 109;
	
	public static final int MSG_RFID_SWITCH_DEVICE = 1;
	public static final int MSG_RFID_INIT_NEXT = 2;
	

	public static final int MSG_RFID_CHECK_SWITCH_DEVICE = 3;
	public static final int MSG_RFID_CHECK_NEXT = 4;
	public static final int MSG_RFID_CHECK_COMPLETE = 5;
	
	private Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch(msg.what) {
			case MSG_RFID_SWITCH_DEVICE:
				mDevice.removeListener(RFIDManager.this);
				Debug.d(TAG, "--->dev: " + mCurrent + "  ink:" + mDevice.getLocalInk());
				mCurrent++;
				if (mCurrent >= TOTAL_RFID_DEVICES) {
					Debug.d(TAG, "--->rfid init success");
					mCallback.sendEmptyMessageDelayed(MSG_RFID_READ_SUCCESS, 100);
					break;
				}
				mDevice = mRfidDevices.get(mCurrent);
				mDevice.addLisetener(RFIDManager.this);
				if (mDevice.getLocalInk() > 0) {
					mHandler.sendEmptyMessageDelayed(MSG_RFID_SWITCH_DEVICE, 200);
					break;
				}
				ExtGpio.rfidSwitch(mCurrent);
				mHandler.sendEmptyMessageDelayed(MSG_RFID_INIT_NEXT, 200);	
				break;
			case MSG_RFID_INIT_NEXT:
				if (mDevice.getLocalInk() > 0) {
					mHandler.sendEmptyMessage(MSG_RFID_SWITCH_DEVICE);
					break;
				}
				if (RFIDDevice.isNewModel) {
					mDevice.autoSearch(false);
				} else {
					mDevice.lookForCards(false);
				}
				// mDevice.connect();
				break;
				
			case MSG_RFID_CHECK_SWITCH_DEVICE:
				mDevice.removeListener(RFIDManager.this);
				Debug.d(TAG, "--->dev: " + mCurrent + "  ink:" + mDevice.getLocalInk());
				mCurrent++;
				if (mCurrent >= mLiveHeads) {
					Debug.d(TAG, "--->rfid check success");
					mCallback.sendEmptyMessageDelayed(MSG_RFID_CHECK_SUCCESS, 100);
					break;
				}
				if (mCurrent >= mRfidDevices.size()) {
					Debug.d(TAG, "--->rfid check failure");
					mCallback.sendEmptyMessageDelayed(MSG_RFID_CHECK_FAIL, 100);
					break;
				}
				mDevice = mRfidDevices.get(mCurrent);
				mDevice.addLisetener(RFIDManager.this);
				
				ExtGpio.rfidSwitch(mCurrent);
				mHandler.sendEmptyMessageDelayed(MSG_RFID_CHECK_NEXT, 200);	
				
				break;
			case MSG_RFID_CHECK_NEXT:
				if (RFIDDevice.isNewModel) {
					mDevice.readBlock(RFIDDevice.SECTOR_UUID, RFIDDevice.BLOCK_UUID, RFIDDevice.RFID_DATA_MIFARE_KEY_A);
				} else {
					mDevice.keyVerfication(RFIDDevice.SECTOR_UUID, RFIDDevice.BLOCK_UUID, RFIDDevice.RFID_DATA_MIFARE_KEY_A);
				}
				break;
			case MSG_RFID_CHECK_COMPLETE:
				if (msg.arg1 > 0) {
					mCallback.sendEmptyMessageDelayed(MSG_RFID_CHECK_SUCCESS, 100);
				} else {
					mCallback.sendEmptyMessageDelayed(MSG_RFID_CHECK_FAIL, 100);
				}
				
				break;
			}
		}
	};
	
	public static RFIDManager getInstance(Context ctx, boolean reInit) {
		if (reInit) {
			mInstance = null;
		}
		return getInstance(ctx);
	}
	public static RFIDManager getInstance(Context ctx) {
		if (mInstance == null) {
			synchronized (RFIDManager.class) {
				if (mInstance == null) {
					Debug.d(TAG, "--->new RfidManager");
					mInstance = new RFIDManager(ctx);
				}
			}
		}
		return mInstance;
	}
	
	public RFIDManager(Context ctx) {
		SystemConfigFile configFile = SystemConfigFile.getInstance(ctx);
		if (configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0) {
			TOTAL_RFID_DEVICES = configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS);
			Debug.d(TAG, "--->heads: " + TOTAL_RFID_DEVICES);
		} else {
			TOTAL_RFID_DEVICES = configFile.getHeads();
			Debug.d(TAG, "--->heads: " + TOTAL_RFID_DEVICES);
		}
	}
	
	
	public void init(final Handler callback) {
		mCallback = callback;
		Debug.d(TAG, "--->init");
		mCurrent = 0;
		ExtGpio.rfidSwitch(mCurrent);

		try {
			Thread.sleep(1000);
		} catch (Exception e) {
		}
		
		if (mRfidDevices.size() != TOTAL_RFID_DEVICES) {
			mRfidDevices.clear();
			for (int i = 0; i < TOTAL_RFID_DEVICES; i++) {
				RFIDDevice device = new RFIDDevice();
				mRfidDevices.add(device);
			}
		}
		
		mDevice = mRfidDevices.get(mCurrent);
		mDevice.addLisetener(this);
		mHandler.sendEmptyMessage(MSG_RFID_INIT_NEXT);
		// mDevice.connect();
		/*
		ThreadPoolManager.mThreads.execute(new Runnable() {
			
			@Override
			public void run() {
				
				for (int i = 0; i < TOTAL_RFID_DEVICES; i++) {
					Debug.d(TAG, "--->RFID init: " + i);
					// 初始化卡
					ExtGpio.rfidSwitch(i);
					// 等待1s，确保RFID建立稳定链接
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					RFIDDevice device = new RFIDDevice();
					device.init();
					mRfidDevices.add(device);
					Debug.d(TAG, "--->rfid=" + i + "  isReady? " + mRfidDevices.get(i).isReady());
				}
				
				callback.sendEmptyMessage(MSG_RFID_INIT_SUCCESS);
			}
		});		
		*/
	}
	
	public void checkRfid() {
		
	}
	
	public void switchRfid(final int i) {
		final RFIDDevice device = mRfidDevices.get(i);
		device.setReady(false);
		ThreadPoolManager.mThreads.execute(new Runnable() {
			
			@Override
			public void run() {
				Debug.e(TAG, "--->switch");
				ExtGpio.rfidSwitch(i);
				try {
					Thread.sleep(500);
				} catch (Exception e) {
				}
				
			}
		});
	}
	
	public float getLocalInk(int dev) {
		if (dev >= mRfidDevices.size()) {
			return 0;
		}
		RFIDDevice device = mRfidDevices.get(dev);
		if (device == null) {
			return 0;
		}
		int max = device.getMax();
		if (max <= 0 && Configs.IGNORE_RFID) {
			max = 370;
		}
		float ink = device.getLocalInk();
		if (max <= 0) {
			return 0;
		} else if (max < ink) {
			return 100;
		}
		return (ink*100/max);
	}
	
	public void downLocal(int dev) {
		if (dev >= mRfidDevices.size()) {
			return ;
		}
		RFIDDevice device = mRfidDevices.get(dev);
		if (device == null) {
			return ;
		}
		device.down();
	}
	
	public boolean isReady(int dev) {
		if (dev >= mRfidDevices.size()) {
			return false;
		}
		RFIDDevice device = mRfidDevices.get(dev);
		return device.isReady();
	}
	
	public boolean isValid(int dev) {
		if (dev >= mRfidDevices.size()) {
			return false;
		}
		if (Configs.IGNORE_RFID) {
			return true;
		}
		RFIDDevice device = mRfidDevices.get(dev);
		return device.isValid();
	}
	
	public RFIDDevice getDevice(int index) {
		if (index >= mRfidDevices.size()) {
			return null;
		}
		return mRfidDevices.get(index);
	}

	@Override
	public void onFinish(RFIDData data) {
		if (data == null) {
			Debug.d(TAG, "--->rfid response null");
			mHandler.sendEmptyMessageDelayed(MSG_RFID_SWITCH_DEVICE, 2000);
			return;
		}
		Debug.d(TAG, "--->rfid: " + Integer.toHexString(data.getCommand()));
		switch(data.getCommand()) {
			case RFIDDevice.RFID_CMD_CONNECT:
				mDevice.lookForCards(false);
				break;
			case RFIDDevice.RFID_CMD_SEARCHCARD:
				if (RFIDDevice.isNewModel) {
					mDevice.autoSearch(false);
				} else {
					mDevice.avoidConflict(false);
				}
				break;
			case RFIDDevice.RFID_CMD_MIFARE_CONFLICT_PREVENTION:
				if (!mDevice.selectCard(mDevice.mSN, false)) {
					mHandler.sendEmptyMessage(MSG_RFID_SWITCH_DEVICE);
				}
				break;
			case RFIDDevice.RFID_CMD_MIFARE_CARD_SELECT:
				mDevice.keyVerfication(RFIDDevice.SECTOR_INK_MAX, RFIDDevice.BLOCK_INK_MAX, mDevice.mRFIDKeyA);
				break;
			case RFIDDevice.RFID_CMD_AUTO_SEARCH:
				if (!mDevice.getReady()) {
					mHandler.sendEmptyMessageDelayed(MSG_RFID_SWITCH_DEVICE, 200);
				} else {
					mDevice.readBlock(RFIDDevice.SECTOR_INK_MAX, RFIDDevice.BLOCK_INK_MAX, mDevice.mRFIDKeyA);
				}
				break;
			case RFIDDevice.RFID_CMD_READ_VERIFY:
				if (mDevice.getState() == RFIDDevice.STATE_RFID_MAX_READY) {
					mDevice.readBlock(RFIDDevice.SECTOR_FEATURE, RFIDDevice.BLOCK_FEATURE, mDevice.mRFIDKeyA);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_FEATURE_READY) {
					mDevice.readBlock(RFIDDevice.SECTOR_INKLEVEL, RFIDDevice.BLOCK_INKLEVEL, mDevice.mRFIDKeyA);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_VALUE_READY) {
					mDevice.readBlock(RFIDDevice.SECTOR_COPY_INKLEVEL, RFIDDevice.BLOCK_COPY_INKLEVEL, mDevice.mRFIDKeyA);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_BACKUP_READY) {
					mHandler.sendEmptyMessageDelayed(MSG_RFID_SWITCH_DEVICE, 200);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_UUID_READY) {
					mHandler.sendEmptyMessageDelayed(MSG_RFID_CHECK_SWITCH_DEVICE, 200);
				}
				break;
			case RFIDDevice.RFID_CMD_MIFARE_KEY_VERIFICATION:
				byte[] rfid = data.getData();
				// mDevice.isCorrect(rfid);
				if (mDevice.getState() == RFIDDevice.STATE_RFID_MAX_KEY_VERFIED) {
					mDevice.readBlock(RFIDDevice.SECTOR_INK_MAX, RFIDDevice.BLOCK_INK_MAX);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_FEATURE_KEY_VERFYED) {
					mDevice.readBlock(RFIDDevice.SECTOR_FEATURE, RFIDDevice.BLOCK_FEATURE);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_VALUE_KEY_VERFYED) {
					mDevice.readBlock(RFIDDevice.SECTOR_INKLEVEL, RFIDDevice.BLOCK_INKLEVEL);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_BACKUP_KEY_VERFYED) {
					mDevice.readBlock(RFIDDevice.SECTOR_COPY_INKLEVEL, RFIDDevice.BLOCK_COPY_INKLEVEL);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_UUID_KEY_VERFYING) {
					mDevice.readBlock(RFIDDevice.SECTOR_UUID, RFIDDevice.BLOCK_UUID);
				}
				break;
			case RFIDDevice.RFID_CMD_MIFARE_READ_BLOCK:
				if (mDevice.getState() == RFIDDevice.STATE_RFID_MAX_READY) {
					mDevice.keyVerfication(RFIDDevice.SECTOR_FEATURE, RFIDDevice.BLOCK_FEATURE, mDevice.mRFIDKeyA);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_FEATURE_READY) {
					mDevice.keyVerfication(RFIDDevice.SECTOR_INKLEVEL, RFIDDevice.BLOCK_INKLEVEL, mDevice.mRFIDKeyA);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_VALUE_READY) {
					mDevice.keyVerfication(RFIDDevice.SECTOR_COPY_INKLEVEL, RFIDDevice.BLOCK_COPY_INKLEVEL, mDevice.mRFIDKeyA);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_BACKUP_READY) {
					mHandler.sendEmptyMessageDelayed(MSG_RFID_SWITCH_DEVICE, 200);
				} else if (mDevice.getState() == RFIDDevice.STATE_RFID_UUID_READY) {
					if (mDevice.checkUID(data.getData())) {
						mHandler.sendEmptyMessageDelayed(MSG_RFID_CHECK_SWITCH_DEVICE, 200);
					} else {
						Message msg = mHandler.obtainMessage(MSG_RFID_CHECK_COMPLETE);
						msg.arg1 = 0;
						msg.sendToTarget();
					}
					
				}
				break;
			default:
				break;
		}
	}
	
	public boolean checkUID(int heads) {
		mLiveHeads = heads;
		mCurrent = 0;
		ExtGpio.rfidSwitch(mCurrent);

		try {
			Thread.sleep(500);
		} catch (Exception e) {
		}
		mDevice = mRfidDevices.get(mCurrent);
		mDevice.removeListener(this);
		mDevice.addLisetener(this);
		mHandler.sendEmptyMessage(MSG_RFID_CHECK_NEXT);
		return true;
	}


	public void defaultInkForIgnoreRfid() {
		for (int i=0; i < mRfidDevices.size(); i++) {
			RFIDDevice dev = getDevice(i);
			if (dev == null) {
				continue;
			}
			if (dev.getLocalInk() <= 0) {
				dev.setLocalInk(185);
			}
		}
	}
}
