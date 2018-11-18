package com.industry.printer.object.data;

import java.io.BufferedReader;
import java.io.CharArrayReader;
import java.io.IOException;

import org.apache.http.util.CharArrayBuffer;

import android.content.Context;
import android.util.Log;

import com.industry.printer.BinInfo;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.BinCreater;
import com.industry.printer.hardware.RFIDDevice;
import com.industry.printer.hardware.RFIDManager;

public class SegmentBuffer {
	
	public static final String TAG = SegmentBuffer.class.getSimpleName();
	
	/**
	 * 打印送数方向 0表示从右往左（buffer的正常顺序）
	 * 	        1表示从左往右（buffer要进行倒序）
	 */
	public static final int DIRECTION_NORMAL = 0;
	public static final int DIRECTION_REVERS = 1;
	
	public int mColumns;
	public int mHight;
	public int mType;
	public CharArrayBuffer mBuffer;
	private Context mContext;
	
	private RFIDDevice mRfid;
	/**
	 * 从BinInfo中提取指定打印头的buffer数据，比如该TLK支持3头打印，从中取出第二个打印头的buffer数据
	 * @param info 从bin文件生成的BinInfo对象
	 * @param type  打印头类型 见 MessageType
	 */
	public SegmentBuffer(Context ctx, char[] info, int type, int heads, int ch) {
		this(ctx, info, type, heads, ch, DIRECTION_NORMAL);
	}
	
	public SegmentBuffer(Context ctx, char[] info, int type, int heads, int ch, int direction) {
		this(ctx, info, type, heads, ch, direction, 0);
	}

	public SegmentBuffer(Context ctx, char[] info, int type, int heads, int ch, int direction, int shift) {
		this(ctx, info, type, heads, ch, direction, shift, 0, 0);
	}

	/**
	 * 从BinInfo中取出的buffer是原始的数据，没有进行补偿和偏移转换计算
	 * 所有的处理工作都是在SegmentBuffer中完成
	 * @param info 
	 * @param type 打印头索引
	 * @param heads 打印头数量
	 * @param ch 补偿后的列高（双字节数）
	 * @param direction 数据方向
	 * @param shift  顺移列数
	 * @param revert 按位反转
	 * @param rotate 旋转
	 */
	private SegmentBuffer(Context ctx, char[] info, int type, int heads, int ch, int direction, int shift, int revert, int rotate) {
		mType = type;
		mBuffer = new CharArrayBuffer(0);
		char feed = 0x0;
		mContext = ctx;
		/*计算info的总列数*/
		mColumns = info.length/ch;	
		/*计算每个打印头的高度*/
		mHight = ch/heads;
		Debug.d(TAG, "--->mHight=" + mHight + ",  columns=" + mColumns );
		Debug.d(TAG, "--->ch=" + ch + ", direction=" + direction + ", shift=" + shift);
		/*计算当前打印头的起始*/
		int start = mHight * type;
		
		/*打印起始位平移shift列*/
		for (int j = 0; j < mHight * shift; j++) {
			mBuffer.append(feed);
		}
		Debug.d(TAG, "--->mBuffer.length=" + mBuffer.length() );
		for (int i = 0; i < mColumns; i++) {
			
			if (direction == DIRECTION_NORMAL) {
				mBuffer.append(info, i * ch + start, mHight);
			} else if (direction == DIRECTION_REVERS) {
				mBuffer.append(info, (mColumns-i-1) * ch + start, mHight);
			}
		}
		/*原始列数+偏移列数=该buffer的总列数*/
		mColumns += shift;
		Debug.d(TAG, "--->mBuffer.length=" + mBuffer.length() );
		reverse(revert);
		Debug.d(TAG, "--->mBuffer.length=" + mBuffer.length() );
// 旋转功能已废弃
//		rotate(rotate);

		mRfid = RFIDManager.getInstance(mContext).getDevice(mType);
	}


	/**
	 *
	 * @param pattern
	 * 	pattern 是位域操作，每个bit代表一个值
	 * 		bit0: 1头反转标志
	 * 		bit1: 2头反转标志
	 * 		bit2: 3头反转标志
	 * 		bit3: 4头反转标志
	 * 	bit0|bit1|bit2|bit3 == 1111   按32bit反转
	 * 	bit0 bit1== 11  按16bit反转
	 * 	bit2 bit3== 11  按16bit反转
	 *
	 */
	public void reverse(int pattern) {

		Debug.d(TAG, "--->reverse: " + pattern);
		if ((pattern & 0x0f) == 0x00) {
			return;
		}
		Debug.d(TAG, "--->mBuffer.length=" + mBuffer.length() );
		int realLen = mBuffer.length();
		char[] buffer = mBuffer.buffer();
		Debug.d(TAG, "--->buffer.length: " + buffer.length);
		mBuffer = new CharArrayBuffer(realLen);

		// 4头整体反转
		if (pattern == 0x0f) {
			for (int i = 0; i < realLen/2; i++) {
				int source = buffer[2 * i] | buffer[2*i +1];
				mBuffer.append(source);
			}
			return;
		}

		for (int i = 0; i < realLen; i++) {
			// 1-2头数据
			if (i % 2 == 0) {
				// 1-2反转
				if ((pattern & 0x03) == 0x03) {
					char source = buffer[i];
					mBuffer.append(revert(source));
				} else if ((pattern & 0x03) == 0x01) {		//仅1头反转
					byte low = (byte)(buffer[i] & 0x0ff);
					char output = (char)(buffer[i] & 0x0ff00);
					Debug.d(TAG, "--->output: " + Integer.toHexString(output));
					output |= revert(low) & 0x0ff;
					Debug.d(TAG, "--->input: 0x" + Integer.toHexString(buffer[i]) +  "   output: 0x" + Integer.toHexString(output));
					mBuffer.append(output);
				} else if ((pattern & 0x03) == 0x02) {		//仅2头反转
					byte high = (byte) ((buffer[i] & 0x0ff00) >> 8);
					char output = (char) (buffer[i] & 0x0ff);
					output |= revert(high) << 8 & 0x0ff00;
					mBuffer.append(output);
				} else {
					mBuffer.append(buffer[i]);
				}
			} else {	// 3-4头数据
				// 3-4反转
				if ((pattern & 0x0C) == 0x0C) {
					char source = buffer[i];
					mBuffer.append(revert(source));
				} else if ((pattern & 0x0C) == 0x04) {		//仅3头反转
					byte low = (byte)(buffer[i] & 0x0ff);
					char output = (char)(buffer[i] & 0x0ff00);
					output |= revert(low) & 0x0ff;
					mBuffer.append(output);
				} else if ((pattern & 0x0C) == 0x08) {		//仅4头反转
					byte high = (byte) ((buffer[i] & 0x0ff00) >> 8);
					char output = (char) (buffer[i] & 0x0ff);
					output |= revert(high) << 8 & 0x0ff00;
					mBuffer.append(output);
				} else {
					mBuffer.append(buffer[i]);
				}
			}
		}
		Debug.d(TAG, "--->after revert buffer.length: " + mBuffer.length());
	}

	/**
	 * 螺旋排列
	 * 1. 由slant 参数控制（这个参数已有，因该是1-30 范围）
	 * 2.   =1 ，bin 增加 15列， bit0 不动，  bit1 移到下一列（第二列）， bit N 移到第n+1 列
	 * 3. =2 ，  bin增加16列， bitN移到N+2列。
	 * 4. =3 ，  bin增加17列， bitN移到N+3列。
	 * @param shift
	 */
	private void rotate(int shift) {
		if (shift <= 0) {
			return;
		}
//		char[] origin = {
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF,
//				0xFFFF, 0xFFFF
//		};

		mColumns += 15 + shift - 1;
		char[] origin = mBuffer.buffer();
//		StringBuffer sb = new StringBuffer();
//		for (int i = 0; i < 2*16; i++) {
//
//			for (int j = 0; j < origin.length/2; j++) {
//				int r = origin[2*j + (i/16)] & (0x0001 << i);
//				//System.out.print(r > 0 ? 1 : 0);
//				sb.append(r > 0 ? 1 : 0);
//				sb.append(" ");
//			}
//			Log.i("XXX", sb.toString());
//			sb.delete(0, sb.length());
//		}
//		mBuffer = new CharArrayBuffer(origin.length + (14+shift)*2);

		mBuffer = new CharArrayBuffer(0);
		Debug.d(TAG, "--->origin.len= " + origin.length + "   shift = " + shift);
		char[] buffer = new char[origin.length + (14+shift)*2];
		for (int column = 0 ; column < buffer.length/2; column++) { //列数
			for (int row = 0 ; row < 16; row++) {
				int index = column - row - shift + 1;
				if ( index >= 0 && index * 2 < origin.length) {
					//Debug.d(TAG, "--->index*2= " + index*2 + "   column=" + column*2 + "  row= " + row);
					buffer[column * 2] |= origin[index * 2] & (0x0001 << row);

				} else {
//					Debug.d(TAG, "--->index= " + index);
				}
			}
//			Debug.d(TAG, "--->buffer[" + column*2 + "] = " + Integer.toHexString(buffer[column*2]));
			
		}
		mBuffer.append(buffer, 0, buffer.length);
//		Debug.i(TAG, "array = " + mBuffer.length() + "   buffer = " + buffer.length);
//		StringBuffer sb = new StringBuffer();
//		for (int i = 0; i < 2*16; i++) {
//			for (int j = 0; j < buffer.length/2; j++) {
//				int r = buffer[2*j + (i/16)] & (0x0001 << i);
//				sb.append(r > 0 ? 1 : 0);
//				sb.append("");
//			}
//			Debug.i("XXX", sb.toString());
//			sb.delete(0, sb.length());
//		}

	}

	/**
	 * source按bits参数指定的位数进行反转,比如：source=20(0x14), bits = 8 -> 0x28
	 * @param source
	 * @return
	 */
	private byte revert(byte source) {

		byte output = 0;
		for (int i = 0; i <= 6; i++) {
			if ((source & (0x01 << i)) > 0) {
				output |= 0x01 << (6 - i);
			}
		}
		if ((source & 0x080) > 0) {
			output |= 0x080;
		}

		//Debug.d(TAG, "--->revertByte: 0x" + Integer.toHexString(output & 0x0ff));
		return (byte)(output & 0x0ff);
	}

	private char revert(char source) {
		char output = 0;
		for (int i = 0; i < 16; i++) {
			if ((source & (0x01 << i)) > 0) {
				output |= 0x01 << (15 - i);
			}
		}
//		Debug.d(TAG, "--->revertChar: 0x" + Integer.toHexString(output & 0x0ffff));
		return (char)(output&0x0ffff);
	}

	private int revert(int source) {
		int output = 0;
		for (int i = 0; i < 32; i++) {
			if ((source & (0x01 << i)) > 0) {
				output |= 0x01 << (31 - i);
			}
		}
		return output;
	}


	public void readColumn(char[] buffer, int col, int offset) {
		//如果當前打印頭的鎖無效，則直接返回全零buffer（即該頭無輸出）
		/*
		if (mRfid == null || mRfid.getLocalInk() > 0) {
			for (int i = 0; i < mHight; i++) {
				buffer[offset+i] = 0;
			}
			return;
		}*/
		CharArrayReader reader = new CharArrayReader(mBuffer.buffer());
		// Debug.d(TAG, "--->col=" + col + ", mColumns=" + mColumns);
		if (col < mColumns) {
			try {
				reader.skip(col * mHight);
				reader.read(buffer, offset, mHight);
			} catch (IOException e) {
				e.printStackTrace();
			}
			
		}
	}
	
	public int getColumns() {
		if (mHight <= 0) {
			return 0;
		}
		return mBuffer.length()/mHight;
	}


	public static class Builder {

		private Context context;
		private char[] buffer;

		private int type;
		private int heads;
		private int ch;
		private int direction;
		private int shift;
		private int revert;
		private int rotate;

		public Builder(Context ctx, char[] info) {
			context = ctx;
			buffer = info;
		}

		public Builder type(int type) {
			this.type = type;
			return this;
		}

		public Builder heads(int heads) {
			this.heads = heads;
			return this;
		}
		public Builder ch(int ch) {
			this.ch = ch;
			return this;
		}

		public Builder direction(int direction) {
			this.direction = direction;
			return this;
		}
		public Builder shift(int shift) {
			this.shift = shift;
			return this;
		}

		public Builder revert(int revert) {
			this.revert = revert;
			return this;
		}
		public Builder rotate(int rotate) {
			this.rotate = rotate;
			return this;
		}

		public SegmentBuffer build() {
			return new SegmentBuffer(context, buffer, type, heads, ch, direction, shift, revert, rotate);
		}
	}
}
