package com.industry.printer.object;

import android.content.Context;

import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;

public class MessageObject extends BaseObject {
	private static final String TAG = MessageObject.class.getSimpleName();

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
	
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体
	public static final String[] mDotSizes = {
		"7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R"
	};

	public static final String[] mDot_32_Size = {
		"7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R", "32"
	};

    // H.M.Wang 追加下列3行
    public static final String[] mDot_64_Size = {
        "7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R", "24", "32", "64"
    };
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体

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
		return mPNozzle.mIndex >= printer.length ? printer[0] : printer[mPNozzle.mIndex];
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
            // H.M.Wang 修改下列两行
//            int ratio = (mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT) ? 1 : 2 ;
            int ratio = (mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT) ? 1 : 2 ;

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

		Debug.d(TAG, "toString = [" + builder.toString() + "]");

		return builder.toString();
	}
	
	
	/**
	 * 根据喷头类型返回支持的字号
	 * @return
	 */
	public String[] getDisplayFSList() {
		String[] size = new String[mBaseList.length];
		Debug.d(TAG, "--->getDisplayFSList mPNozzle.mType = " + mPNozzle.mType);
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7) { //single
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i]); 
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_25_4
				|| mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH) { //dual
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 2); 
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_38_1) {// triple
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 3);
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_50_8  || mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL) { // four
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 4);
			}
		} else  if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
			size = new String[mDotSizes.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = mDotSizes[i];
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
			size = new String[mDot_32_Size.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = mDot_32_Size[i];
			}

        // H.M.Wang 追加下列5行
        } else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
            size = new String[mDot_64_Size.length];
            for (int i = 0; i < size.length; i++) {
                size[i] = mDot_64_Size[i];
            }

		// H.M.Wang 追加下列8行
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE) {
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 6);
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR) {
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList[i] * 8);
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
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7) {
			return h;
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_25_4 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH) {
			return h/2;
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_38_1) {
			return h/3;
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_50_8 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL) {
			return h/4;

		// H.M.Wang 追加下列4行
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE) {
			return h/6;
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR) {
			return h/8;

		} else if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT )//addbylk 喷头类型
		{
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            if (size.equalsIgnoreCase(mDotSizes[0])) {
				h = 12.7f / 2;
			} else if (size.equalsIgnoreCase(mDotSizes[1])) {
				h = 12.7f * 10 / 16;
			} else if (size.equalsIgnoreCase(mDotSizes[2])) {
				h = 12.7f * 12 / 16;
			} else if (size.equalsIgnoreCase(mDotSizes[3])) {
				h = 12.7f * 14 / 16;
//			} else if (size.equalsIgnoreCase(mDotSizes[4]) || size.equalsIgnoreCase(mDotSizes[5]) || size.equalsIgnoreCase(mDotSizes[6])) {
			} else {	// 与上面等效
				h = 12.7f;
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			}
		} else  if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT ) { //addbylk
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			if (size.equalsIgnoreCase(mDot_32_Size[0])) {
				h = 12.7f / 4;
			} else if (size.equalsIgnoreCase(mDot_32_Size[1])) {
				h = 12.7f / 2 * 10 / 16;
			} else if (size.equalsIgnoreCase(mDot_32_Size[2])) {
				h = 12.7f / 2 * 12 / 16;
			} else if (size.equalsIgnoreCase(mDot_32_Size[3])) {
				h = 12.7f / 2 * 14 / 16;
			} else if (size.equalsIgnoreCase(mDot_32_Size[4]) || size.equalsIgnoreCase(mDot_32_Size[5]) || size.equalsIgnoreCase(mDot_32_Size[6])) {
				h = 12.7f / 2;
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			} else {
				h = 12.7f;
			}
//			return h;

        // H.M.Wang 追加下列9行
        } else  if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT ) {
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			if (size.equalsIgnoreCase(mDot_64_Size[0])) {
				h = 12.7f / 8;
			} else if (size.equalsIgnoreCase(mDot_64_Size[1])) {
				h = 12.7f / 8 * 2 * 10 / 16;
			} else if (size.equalsIgnoreCase(mDot_64_Size[2])) {
				h = 12.7f / 8 * 2 * 12 / 16;
			} else if (size.equalsIgnoreCase(mDot_64_Size[3])) {
                h = 12.7f / 8 * 2 * 14 / 16;
			} else if (size.equalsIgnoreCase(mDot_64_Size[4]) || size.equalsIgnoreCase(mDot_64_Size[5]) || size.equalsIgnoreCase(mDot_64_Size[6])) {
				h = 12.7f / 8 * 2;
			} else if (size.equalsIgnoreCase(mDot_64_Size[7])) {
				h = 12.7f / 8 * 3;
			} else if (size.equalsIgnoreCase(mDot_64_Size[8])) {
				h = 12.7f / 8 * 4;
            } else {
                h = 12.7f;
            }
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
		}
		Debug.d(TAG, "--->h: " + h + ", type=" + mPNozzle.mType);
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

		sizelist= mBaseList;

//		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7) {
//			h = size/PIXELS_PER_MM;
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_25_4 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH) {
//			h = 2 * (size/PIXELS_PER_MM);
//			type = 2;
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_38_1) {
//			h = 3 * (size/PIXELS_PER_MM);
//			type = 3;
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_50_8 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL) {
//			h = 4 * (size/PIXELS_PER_MM);
//			type = 4;
//		} else
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
			if (size <= 152f/2) {
                return mDotSizes[0];
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
//			} else {
//				return mDotSizes[1];
            } else if(size <= 152f * 10 / 16) {
                return mDotSizes[1];
            } else if(size <= 152f * 12 / 16) {
                return mDotSizes[2];
            } else if(size <= 152f * 14 / 16) {
                return mDotSizes[3];
            } else {
//				return mDotSizes[4];
                return mDotSizes[4] + "," + mDotSizes[5] + "," + mDotSizes[6];
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT) {

			if (size <= 152f/4) {
				return mDot_32_Size[0];
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            } else if (size <= 152f/2 * 10 / 16) {
                return mDot_32_Size[1];
            } else if (size <= 152f/2 * 12 / 16) {
                return mDot_32_Size[2];
            } else if (size <= 152f/2 * 14 / 16) {
                return mDot_32_Size[3];
            } else if (size <= 152f/2) {
//				return mDot_32_Size[1];
//                return mDot_32_Size[4];
				return mDot_32_Size[4] + "," + mDot_32_Size[5] + "," + mDot_32_Size[6];
			} else {
//				return mDot_32_Size[2];
//				return mDot_32_Size[5];
				return mDot_32_Size[7];
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			}

        // H.M.Wang 追加下列10行
        } else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT) {

            if (size <= 152f/8) {
                return mDot_64_Size[0];
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            } else if (size <= 152f/8*2 * 10 / 16) {
                return mDot_64_Size[1];
            } else if (size <= 152f/8*2 * 12 / 16) {
                return mDot_64_Size[2];
            } else if (size <= 152f/8*2 * 14 / 16) {
                return mDot_64_Size[3];
			} else if (size <= 152f/8*2) {
//				return mDot_64_Size[1];
//                return mDot_64_Size[4];
				return mDot_64_Size[4] + "," + mDot_64_Size[5] + "," + mDot_64_Size[6];
			} else if (size <= 152f/8*3) {
//				return mDot_64_Size[2];
				return mDot_64_Size[7];
			} else if (size <= 152f/8*4) {
//				return mDot_64_Size[3];
				return mDot_64_Size[8];
            } else {
//				return mDot_64_Size[4];
				return mDot_64_Size[9];
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            }

        } else {
// H.M.Wang 2019-9-29 去除ScaleH，否则会由于其采用了308/304而使得计算结果被放大
//			h = mPNozzle.getScaleH() * size/PIXELS_PER_MM;
			type = mPNozzle.factor();
			h = type * size/PIXELS_PER_MM;
		}
		for (int i = 0; i < sizelist.length; i++) {
			if ((h > type * sizelist[i] - 0.3) && (h < type * sizelist[i] + 0.3)) {
				h = sizelist[i] * type;
				break;
			}
		}

// H.M.Wang 2019-9-29 保留小数点后1位
		return String.format("%.1f", h);
//		return String.valueOf(h);
	}
}
