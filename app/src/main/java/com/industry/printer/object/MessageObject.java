package com.industry.printer.object;

import android.content.Context;

import com.industry.printer.MessageTask.MessageType;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;

public class MessageObject extends BaseObject {

	public int mDots = 0;
	public int[] mDotPer = new int[8];
//	public int mType;
	public boolean mHighResolution;

	public PrinterNozzle mPNozzle;
	
	public static final int PIXELS_PER_MM = 12;
	public static final float[] mBaseList = {1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5, 5.5f, 6, 6.5f, 
											7, 7.5f, 8, 8.5f, 9, 9.5f, 10, 10.5f, 11, 11.5f, 12, 12.7f};
	public static final float[] mBaseList_16 = {1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5, 5.5f, 6, 6.5f, 
											7, 7.5f, 8, 8.5f, 9, 9.5f, 10, 10.5f, 11, 11.5f, 12, 12.5f, 
											13, 13.5f, 14, 14.5f, 15, 15.5f, 16, 16.3f};
	
	
	public static final String[] mDotSizes = {
		"7x6" , "16x12"
	};

	public static final String[] mDot_32_Size = {
			"7x6" , "16x12", "32 Dots"
	};
	
	public MessageObject(Context context,  float x) {
		super(context, BaseObject.OBJECT_TYPE_MsgName, x);
		//mIndex = index;
		Debug.d(TAG, "--->MessageObject: " + context.getResources());
		String name = (String)context.getResources().getString(R.string.object_msg_name);
		mContent = name;
		// mType = 0;
		mPNozzle = PrinterNozzle.getInstance(0);
		mHighResolution = false;
	}
	
	public void setType(int i)
	{
		String[] printer =	mContext.getResources().getStringArray(R.array.strPrinterArray);
		if(i<0 || i>printer.length)
			return ;
//		mType = i;
		mPNozzle = PrinterNozzle.getInstance(i);
	}
	
	public void setType(String type) {
		String[] printer =	mContext.getResources().getStringArray(R.array.strPrinterArray);
		for (int i=0; i<printer.length; i++) {
			if (printer[i].equals(type)) {
//				mPNozzle.mType = i;
				mPNozzle = PrinterNozzle.getInstance(i);
				break;
			}
		}
	}

	private void initDotCount() {
		mDots = 0;
		for (int i = 0; i < mDotPer.length; i++) {
			mDotPer[i] = 0;
		}
	}

	public PrinterNozzle getPNozzle() {
		return mPNozzle;
	}

	public int getType() {
		return mPNozzle.mType;
	}
	

	
	public void setHighResolution(boolean resolution) {
		mHighResolution = resolution;
	}
	
	public void setHighResolution(int resolution) {
		mHighResolution = resolution == 0 ? false : true;
	}
	
	public boolean getResolution() {
		return mHighResolution;
	}
	
	public String getPrinterName() {
		String[] printer =	mContext.getResources().getStringArray(R.array.strPrinterArray);
		if (printer == null || printer.length == 0) {
			return "";
		}
		return mPNozzle.mType >= printer.length ? printer[0] : printer[mPNozzle.mType];
	}
	
	public void setDotCount(int count) {
		mDots = count;		
	}
	
	public void setDotCountPer(int[] count) {
		Debug.d(TAG, "--->setDotCountPer: " + count.length);
		for (int i = 0; i < count.length; i++) {
			Debug.d(TAG, "--->setDotCountPer: count[" + i + "]=" + count[i]);
			if (mDotPer.length < i) {
				break;
			}
			mDotPer[i] = count[i];
		}
	}

	/*
	public String toString()
	{
		StringBuilder builder = new StringBuilder(mId);
		
		builder.append("^")
				.append("00000^00000^00000^00000^0^000^")
				.append(BaseObject.intToFormatString(mPNozzle.mType,3))
				.append("^")
				.append(BaseObject.boolToFormatString(mHighResolution, 3))
				.append("^000^000^000^")
				.append(BaseObject.intToFormatString(mDots*2, 7))
				.append("^00000000^00000000^00000000^0000^0000^0000^000^")
				.append(mContent);
				
		String str = builder.toString();		
//		str += mId+"^";
//		str += "00000^00000^00000^00000^0^000^";
//		str += BaseObject.intToFormatString(mPNozzle.mType,3) + "^000^000^000^000^";
//		str += BaseObject.intToFormatString(mDots*2, 7)+"^00000000^00000000^00000000^0000^0000^0000^000^"+mContent;
		Debug.d(TAG, "file string ["+str+"]");
		return str;
	}
	*/
	public String toString()  //addbylk xxx/30
	{			
		String str="";
		StringBuilder builder = new StringBuilder(mId);
		if(PlatformInfo.isBufferFromDotMatrix()!=0) //adfbylk
		{

//			str += mId+"^";
//			str += "00000^00000^00000^00000^0^000^";
//			str += BaseObject.intToFormatString(mPNozzle.mType,3) + "^000^000^000^000^";
//			str += BaseObject.intToFormatString(mDots*220, 7)+"^00000000^00000000^00000000^0000^0000^0000^000^"+mContent;
//			Debug.d(TAG, "file string ["+str+"]");
//			
			builder.append("^");
			builder.append("00000^00000^00000^00000^0^000^")
				.append(BaseObject.intToFormatString(mPNozzle.mType,3))
				.append("^000^000^000^000^")
				.append(BaseObject.intToFormatString(mDots, 7))
				.append("^00000000^00000000^00000000^0000^0000^0000^000^")
				.append(mContent);
		}
		else
		{
			int ratio = (mPNozzle.mType == MessageType.MESSAGE_TYPE_16_DOT || mPNozzle.mType == MessageType.MESSAGE_TYPE_32_DOT) ? 1 : 2 ;
//			str += mId+"^";
//			str += "00000^00000^00000^00000^0^000^";
//			str += BaseObject.intToFormatString(mPNozzle.mType,3) + "^000^000^000^000^";
//			str += BaseObject.intToFormatString(dotCount()*2, 7)+"^00000000^00000000^00000000^0000^0000^0000^000^"+mContent;
//			Debug.d(TAG, "file string ["+str+"]");		
			builder.append("^00000^00000^00000^00000^0^000^")
				.append(BaseObject.intToFormatString(mPNozzle.mType,3))
				.append("^000^")
				.append(intToFormatString(mDotPer[0] * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[1] * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[2] * ratio, 7))
				.append("^")
				.append(BaseObject.intToFormatString(mDots * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[3] * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[4] * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[5] * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[6] * ratio, 7))
				.append("^")
				.append(intToFormatString(mDotPer[7] * ratio, 7))
				.append("^0000^000^")
				.append(mContent);
		}
		
		return builder.toString();
	}
	
	
	/**
	 * 根据喷头类型返回支持的字号
	 * @return
	 */
	public String[] getDisplayFSList() {
		String[] size = new String[mBaseList.length];
		Debug.d(TAG, "--->getDisplayFSList mPNozzle.mType = " + mPNozzle.mType);
		if (mPNozzle.mType == MessageType.MESSAGE_TYPE_12_7 || mPNozzle.mType == MessageType.MESSAGE_TYPE_12_7_S) { //single
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i]); 
			}
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_25_4 || mPNozzle.mType == MessageType.MESSAGE_TYPE_33
				|| mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH
				|| mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_FAST) { //dual
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 2); 
			}
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_38_1) {// triple
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 3);
			}
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_50_8  || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_DUAL || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_DUAL_FAST) { // four
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 4);
			}
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_16_3) { // four
			size = new String[mBaseList_16.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList_16[i]);
			}

		} else  if ( mPNozzle.mType == MessageType.MESSAGE_TYPE_16_DOT) {
			size = new String[mDotSizes.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = mDotSizes[i];
			}
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_32_DOT) {
			size = new String[mDot_32_Size.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = mDot_32_Size[i];
			}
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_NOVA) {
			// size = new String[mBaseList_16.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i]);
			}
		}
		return size;
	}
	
	/**
	 * 根据喷头类型和高度计算实际的字号
	 * 点阵字库单独处理：实际高度bit
	 * 		如： 7bit点阵，	计算得到的实际字号是  152
	 * 			 16bit点阵，	计算得到的实际字号是  304
	 * @param size
	 * @return
	 */
	public float getRealFontsize(String size) {
		float h=1;
		Debug.d(TAG, "--->size: " + size);
		try {
			h = Float.parseFloat(size);
			
		} catch(Exception e) {
			Debug.d(TAG, "--->exception: " + e.getMessage());
		}
		Debug.d(TAG, "--->h: " + h + ", type=" + mPNozzle.mType);
		if (mPNozzle.mType == MessageType.MESSAGE_TYPE_12_7 || mPNozzle.mType == MessageType.MESSAGE_TYPE_12_7_S) {
			return h;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_25_4 || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_FAST) {
			return h/2;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_38_1) {
			return h/3;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_50_8 || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_DUAL || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_DUAL_FAST) {
			return h/4;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_16_3) {
			return h*12.7f/16.3f;
		}
		else if ( mPNozzle.mType == MessageType.MESSAGE_TYPE_16_DOT )//addbylk 喷头类型
		{
			if (size.equalsIgnoreCase("7x6")) {
				h = 6.4f;
			} else if (size.equalsIgnoreCase("16x12")) {
				h = 12.7f;
			}  
			
		} else  if ( mPNozzle.mType == MessageType.MESSAGE_TYPE_32_DOT ) { //addbylk
			if (size.equalsIgnoreCase("7x6")) {
				h = 3.2f;
			} else if (size.equalsIgnoreCase("16x12")) {
				h = 6.4f;
			} else {
				h = 12.7f;
			}
			return h;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_NOVA) {
			return h;
		}
		return h;
	}
	
	public int getPixels(String size) {
		float h = getRealFontsize(size);
		
		return (int)(h * PIXELS_PER_MM);
	}
	
	public String getDisplayFs(float size) {
		float h = 0;
		int type = 1;
		Debug.d(TAG, "--->getDisplayFs: " + size);
		float[] sizelist;
		if (mPNozzle.mType == MessageType.MESSAGE_TYPE_16_3) {
			sizelist= mBaseList_16;
		} else {
			sizelist= mBaseList;
		}
		if (mPNozzle.mType == MessageType.MESSAGE_TYPE_12_7 || mPNozzle.mType == MessageType.MESSAGE_TYPE_12_7_S) {
			h = size/PIXELS_PER_MM;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_25_4 || mPNozzle.mType == MessageType.MESSAGE_TYPE_33 || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_FAST ) {
			h = 2 * (size/PIXELS_PER_MM);
			type = 2;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_38_1) {
			h = 3 * (size/PIXELS_PER_MM);
			type = 3;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_50_8 || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_DUAL || mPNozzle.mType == MessageType.MESSAGE_TYPE_1_INCH_DUAL_FAST) {
			h = 4 * (size/PIXELS_PER_MM);
			type = 4;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_16_3) {
			h = size/PIXELS_PER_MM * 1.3f;
			
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_NOVA) {
			h = size/PIXELS_PER_MM;
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_16_DOT) {
			if (size <= 152/2) {
				return mDotSizes[0]; 
			} else {
				return mDotSizes[1];
			}
			
		} else if (mPNozzle.mType == MessageType.MESSAGE_TYPE_32_DOT) {

			if (size == 38) {
				return mDot_32_Size[0];
			} else if (size == 76) {
				return mDot_32_Size[1];
			} else {
				return mDot_32_Size[2];
			}

		} else {
			h = size/PIXELS_PER_MM;
		}
		for (int i = 0; i < sizelist.length; i++) {
			if ((h > type * sizelist[i] - 0.3) && (h < type * sizelist[i] + 0.3)) {
				h = sizelist[i] * type;
				break;
			}
		}
		return String.valueOf(h);
	}
}
