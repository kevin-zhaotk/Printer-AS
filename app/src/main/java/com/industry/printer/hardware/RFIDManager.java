package com.industry.printer.hardware;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.industry.printer.ThreadPoolManager;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Rfid.RFIDAsyncTask.RfidCallback;
import com.industry.printer.Rfid.RFIDData;

public class RFIDManager implements RfidCallback, IInkDevice {
	
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

				Debug.d(TAG, "MSG_RFID_SWITCH_DEVICE");

				if (mDevice.getLocalInk() > 0) {
					mHandler.sendEmptyMessageDelayed(MSG_RFID_SWITCH_DEVICE, 200);
					break;
				}
				ExtGpio.rfidSwitch(mCurrent);
				mHandler.sendEmptyMessageDelayed(MSG_RFID_INIT_NEXT, 200);	
				break;
			case MSG_RFID_INIT_NEXT:
				Debug.d(TAG, "MSG_RFID_INIT_NEXT");

				if (mDevice.getLocalInk() > 0) {
					mHandler.sendEmptyMessage(MSG_RFID_SWITCH_DEVICE);
					break;
				}
				if (RFIDDevice.isNewModel) {
					mDevice.autoSearch(false);      // RFID_CMD_AUTO_SEARCH
				} else {
					mDevice.lookForCards(false);    // RFID_CMD_SEARCHCARD
				}
				// mDevice.connect();
				break;
				
			case MSG_RFID_CHECK_SWITCH_DEVICE:
				mDevice.removeListener(RFIDManager.this);
				Debug.d(TAG, "--->dev: " + mCurrent + "  ink:" + mDevice.getLocalInk());
				mCurrent++;
// H.M.Wang 2021-3-16 修改不检测特征码的问题
//				if (mCurrent >= mLiveHeads) {
				if (mCurrent >= mLiveHeads && mDevice.isValid()) {
// End of H.M.Wang 2021-3-16 修改不检测特征码的问题
					Debug.d(TAG, "--->rfid check success");
					mCallback.sendEmptyMessage(MSG_RFID_CHECK_SUCCESS);
					break;
				}
// H.M.Wang 2021-3-16 修改不检测特征码的问题
//				if (mCurrent >= mRfidDevices.size()) {
				if (mCurrent >= mRfidDevices.size() || !mDevice.isValid()) {
// End of H.M.Wang 2021-3-16 修改不检测特征码的问题
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
// H.M.Wang 2021-11-9 尝试修改checkUID的方法，不去读(0x21命令，而是跑自动寻卡命令，目的是获取SN）
				mDevice.autoSearch(new RfidCallback() {
					@Override
					public void onFinish(RFIDData data) {
						byte[] orgSN = mDevice.mSN.clone();
						mDevice.parseAutosearch(data);
						if(Arrays.equals(orgSN, mDevice.mSN)) {
							mHandler.sendEmptyMessageDelayed(MSG_RFID_CHECK_SWITCH_DEVICE, 200);
						} else {
							Message msg = mHandler.obtainMessage(MSG_RFID_CHECK_COMPLETE);
							msg.arg1 = 0;
							msg.sendToTarget();
						}
					}
				});
// End of H.M.Wang 2021-11-9 尝试修改checkUID的方法，不去读(0x21命令，而是跑自动寻卡命令，目的是获取SN）

//				if (RFIDDevice.isNewModel) {
//					mDevice.readBlock(RFIDDevice.SECTOR_UUID, RFIDDevice.BLOCK_UUID, RFIDDevice.RFID_DATA_MIFARE_KEY_A);    // RFID_CMD_READ_VERIFY
//				} else {
//					mDevice.keyVerfication(RFIDDevice.SECTOR_UUID, RFIDDevice.BLOCK_UUID, RFIDDevice.RFID_DATA_MIFARE_KEY_A);   // RFID_CMD_MIFARE_KEY_VERIFICATION
//				}
				break;
			case MSG_RFID_CHECK_COMPLETE:
// H.M.Wang 2021-3-16 修改不检测特征码的问题
//				if (msg.arg1 > 0) {
				if (msg.arg1 > 0 && mDevice.isValid()) {
// End of H.M.Wang 2021-3-16 修改不检测特征码的问题
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

//		if (configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS) > 0) {
//			TOTAL_RFID_DEVICES = configFile.getParam(SystemConfigFile.INDEX_SPECIFY_HEADS);
//			Debug.d(TAG, "--->heads: " + TOTAL_RFID_DEVICES);
//		} else {
//			TOTAL_RFID_DEVICES = configFile.getPNozzle().mHeads;
//			Debug.d(TAG, "--->heads: " + TOTAL_RFID_DEVICES);
//		}
		TOTAL_RFID_DEVICES = configFile.getPNozzle().mHeads;

		// 根据参数38调整减锁数量
		TOTAL_RFID_DEVICES *= configFile.getHeadFactor();

	}

	/** implement IInkDevice*/
	@Override
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

	/** implement IInkDevice*/
	@Override
	public void switchRfid(final int i) {
		final RFIDDevice device = mRfidDevices.get(i);
		device.setReady(false);
		ThreadPoolManager.mThreads.execute(new Runnable() {
			
			@Override
			public void run() {
				Debug.e(TAG, "--->switch");
				ExtGpio.rfidSwitch(i);
				try {
					Thread.sleep(200);
				} catch (Exception e) {
				}
				
			}
		});
	}

	/** implement IInkDevice*/
	@Override
	public float getLocalInk(int dev) {
		Debug.d(TAG, "---> enter getLocalInk()");

		if (dev >= mRfidDevices.size()) {
			return 0;
		}
		RFIDDevice device = mRfidDevices.get(dev);
		if (device == null) {
			return 0;
		}
		int max = device.getMax();
		if (max <= 0 && Configs.READING) {
// H.M.Wang 2020-2-25 修改max值
//            max = 370;
            max = 2000;
// End of H.M.Wang 2020-2-25 修改max值
		}
		float ink = device.getLocalInk();
		if (max <= 0) {
			return 0;
		} else if (max < ink) {
			return 100;
		}
//// 临时取消		return (ink*100/max);
//		Debug.d(TAG, "[2020-3-16 Check] Ink: " + ink + "Max: " + max);
		return ink;
	}

    @Override
    public float getLocalInkPercentage(int dev) {
        if (dev >= mRfidDevices.size()) {
            return 0;
        }
        RFIDDevice device = mRfidDevices.get(dev);
        if (device == null) {
            return 0;
        }
        int max = device.getMax();
        if (max <= 0 && Configs.READING) {
// H.M.Wang 2020-2-25 修改max值
//            max = 370;
            max = 2000;
// End of H.M.Wang 2020-2-25 修改max值
        }
        float ink = device.getLocalInk();
        if (max <= 0) {
            return 0;
        } else if (max < ink) {
            return 100;
        }

        return (ink*100/max);
    }

	/** implement IInkDevice*/
	@Override
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

	/** implement IInkDevice*/
	@Override
	public boolean isValid(int dev) {
		if (dev >= mRfidDevices.size()) {
			return false;
		}
		if (Configs.READING) {
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
//					mHandler.sendEmptyMessageDelayed(MSG_RFID_CHECK_SWITCH_DEVICE, 200);
					String b = Integer.toHexString(data.getData()[0] & 0x0ff);
					Debug.d(TAG, "--->checkUID: " + b);
					if (!"ff".equals(b)) {
						mHandler.sendEmptyMessageDelayed(MSG_RFID_CHECK_SWITCH_DEVICE, 200);
					} else {
						Message msg = mHandler.obtainMessage(MSG_RFID_CHECK_COMPLETE);
						msg.arg1 = 0;
						msg.sendToTarget();
					}
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

					String b = Integer.toHexString(data.getData()[0] & 0x0ff);
					Debug.d(TAG, "--->checkUID: " + b);
					if (!"ff".equals(b)) {
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

	/** implement IInkDevice*/
	@Override
	public boolean checkUID(int heads) {
		mLiveHeads = heads;
		mCurrent = 0;
		ExtGpio.rfidSwitch(mCurrent);
		Debug.d(TAG, "--->Print execute check UUID");
		try {
			Thread.sleep(200);
		} catch (Exception e) {
		}
		mDevice = mRfidDevices.get(mCurrent);
		mDevice.removeListener(this);
		mDevice.addLisetener(this);
		mHandler.sendEmptyMessage(MSG_RFID_CHECK_NEXT);

		return true;
	}


	/** implement IInkDevice*/
	@Override
	public void defaultInkForIgnoreRfid() {
		for (int i=0; i < mRfidDevices.size(); i++) {
			RFIDDevice dev = getDevice(i);
			if (dev == null) {
				continue;
			}

			Debug.d(TAG, "defaultInkForIgnoreRfid");

			if (dev.getLocalInk() <= 0) {
				dev.setLocalInk(185);
			}
		}
	}

	/**
	 * 获取设备Feature信息
	 * @param device
	 * @param index
	 * @return
	 */

	@Override
	public int getFeature(int device, int index) {
		RFIDDevice dev = getDevice(device);
		if (dev == null) return 0;
		int vol = dev.getFeature(index);
		return vol;
	}
}
