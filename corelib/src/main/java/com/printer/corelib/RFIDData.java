package com.printer.corelib;

import java.nio.ByteBuffer;

import org.apache.http.util.ByteArrayBuffer;


import android.R.string;

public class RFIDData {
	
	/*设置模块工作在 ISO14443 TYPE A 模式*/
	public static final byte RFID_CMD_SETTYPE = 0x3A;
	public static final byte RFID_DATA_TYPEA = 0x41;
	/*Mifare one/Ultralight 卡寻卡：*/
	public static final byte RFID_CMD_SEARCH = 0x46;
	public static final byte RFID_DATA_CARDWAKEUP = 0x26;
	public static final byte RFID_DATA_CARDALL = 0x52;
	/*Ultralight 卡选卡*/
	public static final byte RFID_CMD_SELECTCARD = 0x33;
	/*Ultralight 卡读卡*/
	public static final byte RFID_CMD_READCARD = 0x4b;
	/*Ultralight 卡写卡*/
	public static final byte RFID_CMD_WRITECARD = 0x35;
	/*Mifare one 卡防冲突*/
	public static final byte RFID_CMD_AVOIDCONFLICT = 0x47;
	public static final byte RFID_DATA_AVOIDCONFLICT = 0x04;
	
	private static final byte mIdentificator = 0x10;
	private static final byte mHeader = 0x02;
	private static final byte mTailer = 0x03;
	/*已经插入了数据辨识符（0x10）*/
	public byte[] mTransData;
	/*过滤掉数据辨识符（0x10）的真实数据*/
	public byte[] mRealData;
	
	private int mDatalen;
	
	/*数据包内容*/
	private byte mAddress[];
	private byte mLength;
	private byte mCommand;
	private byte[] mData;
	private byte mCheckCode;
	private byte mResult;
	
	/**
	 * 通过命令字和数据域构造数据
	 * @param cmd 命令字
	 * @param data 数据域
	 */
	public RFIDData(byte cmd, byte[] data) {
		mAddress = new byte[2];
		mAddress[0] = 0x00;
		mAddress[1] = 0x00;
		mLength = 0x00;
		mCheckCode = 0x00;
		mCommand = cmd;
		mData = data;
		//Debug.d("", "===>cmd:"+cmd +", mCommand:"+mCommand);
		
		ByteArrayBuffer buffer = new ByteArrayBuffer(0);
		buffer.append(mHeader);
		buffer.append(mAddress[0]);
		buffer.append(mAddress[1]);
		buffer.append(mLength);
		buffer.append(mCommand);
		buffer.append(mData, 0, mData.length);
		buffer.append(mCheckCode);
		buffer.append(mTailer);
		mRealData = buffer.toByteArray();
//		for (int i=0; i<mRealData.length; i++) {
//			Debug.d("", "===>mRealData:"+mRealData[i]);
//			
//		}
		//计算长度字
		mLength = (byte) (mRealData.length-4);
		mRealData[3] = mLength;
		//计算校验字
		for (int i = 1; i < mRealData.length-2; i++) {
			mCheckCode += mRealData[i];	
			mRealData[mRealData.length-2] = mCheckCode;
		}
		getTransferData();
	}
	/**
	 * 构造RFID数据
	 * @param data
	 * @param isfilter 如果为true 表示data为插入了数据辨识符的传输数据，如果为false 表示data为过滤掉数据辨识符的真实数据
	 */
	public RFIDData(byte[] data, boolean isfilter) {
		if (isfilter) {
			mTransData = data;
			getRealData();
			// Debug.d("", "---------11-------");
			// Debug.print(mRealData);
			// Debug.d("", "---------11-------");
			if (mRealData == null) {
				return ;
			}
			//计算长度字
			mLength = (byte) (mRealData.length-5);
			// Debug.d("", "-------len: "+mLength);
			// mRealData[3] = mLength;
			//计算校验字
			for (int i = 1; i < mRealData.length-2; i++) {
				mCheckCode += mRealData[i];	
			}
			// Debug.d("", "---------22-------");
			// Debug.print(mRealData);
			// Debug.d("", "---------22-------");
		} else {
			mRealData = data;
			getTransferData();
		}
	}
	
	/**
	 * 根据未添加辨识符的数据得到添加辨识符的数据
	 */
	private void getTransferData() {
		ByteArrayBuffer buffer = new ByteArrayBuffer(0);
		if( mRealData == null || mRealData.length <= 0) {
			return;
		}
		//添加帧头
		buffer.append(mRealData[0]);
		//数据包内容
		for (int i = 1; i < mRealData.length-1; i++) {
			if (mRealData[i] == mIdentificator || mRealData[i] == mHeader || mRealData[i] == mTailer) {
				// Debug.d("", "===>data:"+0x10);
				buffer.append((byte)0x10);
				// Debug.d("", "===>data:"+mRealData[i]);
				buffer.append(mRealData[i]);
			} else {
				buffer.append(mRealData[i]);
				// Debug.d("", "===>data:"+mRealData[i]);
			}
		}
		buffer.append(mRealData[mRealData.length-1]);
		//Debug.d("", "===>data:"+mRealData[mRealData.length-1]);
		mTransData = buffer.toByteArray();
		
	}
	
	/**
	 * 根据添加辨识符的数据得到未添加辨识符的数据
	 */
	private void getRealData() {
		ByteArrayBuffer buffer = new ByteArrayBuffer(0);
		if( mTransData == null || mTransData.length <= 0) {
			return;
		}
		//添加帧头
		buffer.append(mTransData[0]);
		//数据包内容
		for (int i = 1; i < mTransData.length-1; i++) {
			if (mTransData[i] == mIdentificator) {
				i++;
				buffer.append(mTransData[i]);
			} else {
				buffer.append(mTransData[i]);
			}
		}
		buffer.append(mTransData[mTransData.length-1]);
		
		mRealData = buffer.toByteArray();
	}
	
	public short[] transferData() {
		short[] data = new short[mTransData.length/2+1];
		for (int i = 0; i < mTransData.length; i++) {
			// Debug.d("", "===>mTransData["+i+"]:"+Integer.toHexString(mTransData[i]));
			if (i%2 == 0) {
				data[i/2] = 0;
				data[i/2] = (short) (mTransData[i] & 0x0ff);
			} else {
				data[i/2] |= (mTransData[i]<<8 & 0x0ffff);
			}
			// Debug.d("", "===>data["+i/2+"]:"+Integer.toHexString(data[i/2]));
		}
		mDatalen = mTransData.length;
		return data;
	}
	
	public int getLength() {
		return mDatalen;
	}
	
	public byte getCommand() {
		if (mRealData != null && mRealData.length > 4) {
			return (byte) (mRealData[4] & 0x0ff);
		}
		return 0x00;
	}
	
	public byte[] getData() {
		
		if (mRealData == null || mRealData.length < 4) {
			return null;
		}
		/*由于 readblock返回数据中没有长度字节，因此暂时不进行长度校验
		mLength = mRealData[3];
		Debug.d("", "--->data length: "+mLength+",  real len: "+mRealData.length);
		if (mLength == 0 || (mLength + 5) != mRealData.length) {
			return null;
		}*/
		ByteBuffer buffer = ByteBuffer.wrap(mRealData);
		mData = new byte[mRealData.length - 7];
		if (mRealData[3] == 0x4B) {
			buffer.position(4);
		} else {
			buffer.position(5);
		}
		buffer.get(mData, 0, mRealData.length - 7);
		
		Debug.print("RFID-DATA:", mData);
		return mData;
	}
	
	
	@Override
	public String toString() {
		String data = "real: ";
		for (int i=0; i<mRealData.length; i++) {
			data += " " + String.format("0x%1$02x", mRealData[i]);
		}
		data += " , trans:";
		for (int i=0; i<mTransData.length; i++) {
			data += " " + String.format("0x%1$02x", mTransData[i]);
		}
		return data;
	}
}
