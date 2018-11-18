package com.industry.printer.Rfid;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.RFIDAsyncTask.RfidCallback;
import com.printer.corelib.RFIDData;
import com.industry.printer.hardware.RFIDDevice;
import com.industry.printer.hardware.RFIDManager;

import android.R.bool;
import android.R.integer;
import android.content.Context;
import android.os.SystemClock;

public class RfidTask implements RfidCallback{
	
	private String TAG = RfidTask.class.getSimpleName();
	
	
	public static final int STATE_IDLE = 0;
	public static final int STATE_PROCESSING = 1;
	public static final int STATE_SYNCED = 2;
	
	private Context mContext;
	private int mIndex=0;
	private long mTimeStamp = 0;
	private int mState = 0;
	
	public RfidTask(Context ctx) {
		mState = STATE_IDLE;
		mContext = ctx;
		mTimeStamp = SystemClock.elapsedRealtime();
	}
	
	public RfidTask(int index, Context ctx) {
		this(ctx);
		mIndex = index;
		clearStat();
	}
	
	public void setIndex(int index) {
		mIndex = index;
	}
	
	public synchronized void onLoad() {
		clearStat();
		RFIDManager manager = RFIDManager.getInstance(mContext);
		RFIDDevice dev = manager.getDevice(mIndex);
		if (dev != null) {
			dev.addLisetener(this);
		}
	}
	
	public synchronized void onUnload() {
		RFIDManager manager = RFIDManager.getInstance(mContext);
		RFIDDevice dev = manager.getDevice(mIndex);
		if (dev != null) {
			dev.removeListener(this);
		}
	}
	/**
	 * 清除状态，为下次写入做准备
	 */
	public void clearStat() {
		mTimeStamp = SystemClock.elapsedRealtime();
		mState = STATE_IDLE;
		RFIDManager manager = RFIDManager.getInstance(mContext);
		RFIDDevice dev = manager.getDevice(mIndex);
		if (dev != null) {
			dev.setState(RFIDDevice.STATE_RFID_CONNECTED);
		}
	}
	
	public int getStat() {
		return mState;
	}
	
	public boolean isIdle() {
		return mState == STATE_IDLE;
	}
	
	public long getLast() {
		return mTimeStamp;
	}
	
	public void execute() {
		Debug.d(TAG, "--->execute index=" + mIndex + "  state=" + mState);
		RFIDManager manager = RFIDManager.getInstance(mContext);
		RFIDDevice dev = manager.getDevice(mIndex);
		
		mState = STATE_PROCESSING;
		Debug.d(TAG, "--->dev.state= " + (dev == null ? "" : dev.getState()));
		if (dev == null) {
			return ;
		}
		switch (dev.getState()) {
			case RFIDDevice.STATE_RFID_CONNECTED:
				if(RFIDDevice.isNewModel) {
					dev.writeInk(false);
				} else {
					dev.lookForCards(true);
				}
				break;
			case RFIDDevice.STATE_RFID_SERACH_OK:
				dev.avoidConflict(true);
				break;
			case RFIDDevice.STATE_RFID_AVOIDCONFLICT:
				dev.selectCard(dev.mSN, true);
				break;
			case RFIDDevice.STATE_RFID_SELECTED:
				boolean res = dev.keyVerify(false, true);
				break;
			case RFIDDevice.STATE_RFID_VALUE_KEY_VERFYED:
				dev.writeInk(false);
				break;
			case RFIDDevice.STATE_RFID_VALUE_SYNCED:
				Debug.d(TAG, "--->isNew: " + RFIDDevice.isNewModel);
				if (RFIDDevice.isNewModel) {
					dev.writeInk(true);
				} else {
					dev.keyVerify(true, true);
				}
				break;
			case RFIDDevice.STATE_RFID_BACKUP_KEY_VERFYED:
				dev.writeInk(true);
				mTimeStamp = SystemClock.elapsedRealtime();
				break;
			case RFIDDevice.STATE_RFID_BACKUP_SYNCED:
				mState = STATE_SYNCED;
				dev.setState(RFIDDevice.STATE_RFID_CONNECTED);
				break;
			case RFIDDevice.STATE_RFID_UUID_READY:
				dev.setState(RFIDDevice.STATE_RFID_CONNECTED);
				break;
			default:
				break;
		}
	}

	@Override
	public void onFinish(RFIDData data) {
		byte[] rfid;
		RFIDManager manager = RFIDManager.getInstance(mContext);
		RFIDDevice dev = manager.getDevice(mIndex);
		if (data == null) {
			mState = STATE_SYNCED;
			dev.setState(RFIDDevice.STATE_RFID_CONNECTED);
			return;
		}
		Debug.d(TAG, "--->onFinish: 0x" + Integer.toHexString(data.getCommand()));
		switch (data.getCommand()) {
			case RFIDDevice.RFID_CMD_CONNECT:
			case RFIDDevice.RFID_CMD_SEARCHCARD:
			case RFIDDevice.RFID_CMD_AUTO_SEARCH:
			case RFIDDevice.RFID_CMD_MIFARE_CONFLICT_PREVENTION:
			case RFIDDevice.RFID_CMD_MIFARE_CARD_SELECT:
			case RFIDDevice.RFID_CMD_READ_VERIFY:
			case RFIDDevice.RFID_CMD_MIFARE_KEY_VERIFICATION:
			case RFIDDevice.RFID_CMD_WRITE_VERIFY:
				if (dev.getState() == RFIDDevice.STATE_RFID_BACKUP_SYNCED) {
					mState = STATE_SYNCED;
					Debug.d(TAG, "--->state=" + mState);
				}
				break;
			case RFIDDevice.RFID_CMD_MIFARE_WRITE_BLOCK:
				if (dev.getState() == RFIDDevice.STATE_RFID_BACKUP_SYNCED) {
					mState = STATE_SYNCED;
					Debug.d(TAG, "--->state=" + mState);
					dev.setState(RFIDDevice.STATE_RFID_CONNECTED);
				} else if (dev.getState() == RFIDDevice.STATE_RFID_VALUE_SYNCED) {
					
				}
				
				break;
			default:
				break;
		}
		
	}
}
