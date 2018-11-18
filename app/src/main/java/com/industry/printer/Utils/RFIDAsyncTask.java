package com.industry.printer.Utils;

import com.industry.printer.ThreadPoolManager;
import com.printer.corelib.RFIDData;
import com.industry.printer.hardware.RFIDDevice;


public class RFIDAsyncTask implements Runnable {

	public static volatile RFIDAsyncTask mInstance;
	private int mFd;
	private RFIDData mCmd;
	private RfidCallback mCallback;
	
	
	public static RFIDAsyncTask execute(int fd, RFIDData data, RfidCallback callback) {
		if (mInstance == null) {
			synchronized (RFIDAsyncTask.class) {
				if (mInstance == null) {
					mInstance = new RFIDAsyncTask(fd, data, callback);
				}
			}
		} else {
			mInstance.put(fd, data, callback);
		}
		ThreadPoolManager.mRfidThread.execute(mInstance);
		return mInstance;
	}
	
	public RFIDAsyncTask(int fd, RFIDData data, RfidCallback callback) {
		put(fd, data, callback);
	}
	
	public void put(int fd, RFIDData data, RfidCallback callback) {
		mFd = fd;
		mCmd = data;
		mCallback = callback;
	}
	
	@Override  
    public void run() {
		
		Debug.print(RFIDDevice.RFID_DATA_SEND, mCmd.mTransData);
		byte[] readin = null;
		int writed = RFIDDevice.write(mFd, mCmd.transferData(), mCmd.getLength());
		readin = RFIDDevice.read(mFd, 64);
		Debug.print(RFIDDevice.RFID_DATA_RECV, readin);
		RFIDData response = parseResponse(readin);
		if (mCallback != null) {
			mCallback.onFinish(response);
		}
	}
	
	private RFIDData parseResponse(byte[] value) {
		
		if (value == null || value.length == 0) {
			return null;
		}
		RFIDData rfidData = new RFIDData(value, true);
		return rfidData;
	}
	
	public interface RfidCallback {
		public void onFinish(RFIDData data);
	}
}
