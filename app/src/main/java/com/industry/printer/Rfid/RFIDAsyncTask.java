package com.industry.printer.Rfid;

import com.industry.printer.ThreadPoolManager;
import com.industry.printer.Utils.Debug;
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
/*
		int writed = RFIDDevice.write(mFd, mCmd.transferData(), mCmd.getLength());
// H.M.Wang 2022-4-24 如果操作太快，read函数可能会超时返回null，导致后续处理异常。修改为如果空则一直等待
//		readin = RFIDDevice.read(mFd, 64);
		do {
			readin = RFIDDevice.read(mFd, 64);
		} while(null == readin);
// End of H.M.Wang 2022-4-24 如果操作太快，read函数可能会超时返回null，导致后续处理异常。修改为如果空则一直等待
*/

// H.M.Wang 2022-5-12 修改读写逻辑，如果读失败，超时返回，则最多等待5次，每次等待100ms，作为一个尝试循环。如果失败，再次发送写命令，后重新开始读尝试循环，最多3次
		try {
			for(int i=0; i<3; i++) {
				int writed = RFIDDevice.write(mFd, mCmd.transferData(), mCmd.getLength());
				for(int j=0; j<5; j++) {
					readin = RFIDDevice.read(mFd, 64);
					if(null != readin) break;
					Thread.sleep(100);
				}
				if(null != readin) break;
				Thread.sleep(1000);
			}
		} catch(InterruptedException e) {

		}
// End of H.M.Wang 2022-5-12 修改读写逻辑，如果读失败，超时返回，则最多等待5次，每次等待100ms，作为一个尝试循环。如果失败，再次发送写命令，后重新开始读尝试循环，最多3次

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
