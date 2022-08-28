package com.industry.printer.object;

import android.content.Context;

import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.R;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.hardware.FpgaGpioOperation;

public class MessageObject extends BaseObject {
	private static final String TAG = MessageObject.class.getSimpleName();

	public int mDots = 0;
	public int[] mDotPer = new int[8];
//	public int mType;
// H.M.Wang 2021-4-11 修改该变量为整形
//	public boolean mHighResolution;
    public int mPrintDpi;
// H.M.Wang 2021-4-11 修改该变量为整形

	public PrinterNozzle mPNozzle;
	
	public static final int PIXELS_PER_MM = 12;		// 最好设置为 152 / 12.7 更加准确（11.96850393700787）
	public static final float[] mBaseList = {1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5, 5.5f, 6, 6.5f, 
											7, 7.5f, 8, 8.5f, 9, 9.5f, 10, 10.5f, 11, 11.5f, 12, 12.7f};
	public static final float[] mBaseList_9mm = {1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5, 5.5f, 6, 6.5f,7, 7.5f, 8, 8.5f, 9};
	public static final float[] mBaseList_16 = {1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5, 5.5f, 6, 6.5f,
											7, 7.5f, 8, 8.5f, 9, 9.5f, 10, 10.5f, 11, 11.5f, 12, 12.5f, 
											13, 13.5f, 14, 14.5f, 15, 15.5f, 16, 16.3f};
	
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体
// H.M.Wang 2020-4-15 追加"5x5"字体
	public static final String[] mDotSizes = {
		"5x5", "7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R"
	};

// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
	public static final String[] mDot_32_Size = {
		"5x5", "7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R", "19x13", "21x14", "32"
	};

    // H.M.Wang 追加下列3行
    public static final String[] mDot_64_Size = {
		"5x5", "7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R", "19x13", "21x14", "24", "32", "64"
    };
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
// End of H.M.Wang 2020-4-15 追加"5x5"字体
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体

// H.M.Wang 2021-8-16 追加96DN头
	public static final String[] mDot_96_Size = {
		"5x5", "7x6", "10x8", "12x9", "14x10", "16x12", "16@L", "16@R", "19x13", "21x14", "24", "32", "64", "96"
	};
// End of H.M.Wang 2021-8-16 追加96DN头

	public MessageObject(Context context,  float x) {
		super(context, BaseObject.OBJECT_TYPE_MsgName, x);
		//mIndex = index;
		Debug.d(TAG, "--->MessageObject: " + context.getResources());
		String name = (String)context.getResources().getString(R.string.object_msg_name);
		mContent = name;
		// mType = 0;
		mPNozzle = PrinterNozzle.getInstance(0);
// H.M.Wang 2021-4-6 暂时修改为true，以达到300dpi的目的
//		mHighResolution = false;
// H.M.Wang 2021-4-11 修改该变量为整形
//		mHighResolution = true;
        mPrintDpi = FpgaGpioOperation.DPI_VERSION_150;
// End of H.M.Wang 2021-4-11 修改该变量为整形
// End of H.M.Wang 2021-4-6 暂时修改为true，以达到300dpi的目的
	}
	
	public void setType(int i)
	{
		String[] printer =	mContext.getResources().getStringArray(R.array.strPrinterArray);
		if(i<0 || i>printer.length)
			return ;
//		mType = i;
		mPNozzle = PrinterNozzle.getInstance(i);
// H.M.Wang 2020-5-9 12.7R5d打印头类型强制修改为12.7类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//		if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7_R5) {
		if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_R6X48 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_R6X50) {
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
			mPNozzle = PrinterNozzle.MESSAGE_TYPE_12_7;
		}
// End of H.M.Wang 2020-5-9 12.7R5d打印头类型强制修改为12.7类型

// H.M.Wang 2021-3-6 追加E6X48,E6X50头
		if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X48 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X50) {
			mPNozzle = PrinterNozzle.MESSAGE_TYPE_9MM;
		}
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
		if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X1) {
			mPNozzle = PrinterNozzle.MESSAGE_TYPE_9MM;
		}
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
		if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X48 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X50) {
			mPNozzle = PrinterNozzle.MESSAGE_TYPE_9MM;
		}
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
	}
	
	public void setType(String type) {
		String[] printer =	mContext.getResources().getStringArray(R.array.strPrinterArray);
		for (int i=0; i<printer.length; i++) {
			if (printer[i].equals(type)) {
//				mPNozzle.mType = i;
				mPNozzle = PrinterNozzle.getInstance(i);
// H.M.Wang 2020-5-9 12.7R5d打印头类型强制修改为12.7类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//				if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7_R5) {
				if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_R6X48 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_R6X50) {
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
					mPNozzle = PrinterNozzle.MESSAGE_TYPE_12_7;
				}
// End of H.M.Wang 2020-5-9 12.7R5d打印头类型强制修改为12.7类型
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
				if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X48 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X50) {
					mPNozzle = PrinterNozzle.MESSAGE_TYPE_9MM;
				}
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
				if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X1) {
					mPNozzle = PrinterNozzle.MESSAGE_TYPE_9MM;
				}
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
				if(mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X48 || mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X50) {
					mPNozzle = PrinterNozzle.MESSAGE_TYPE_9MM;
				}
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
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
// H.M.Wang 2021-4-6 暂时修改为true，以达到300dpi的目的。取消中途设置
//		mHighResolution = resolution;
// End of H.M.Wang 2021-4-6 暂时修改为true，以达到300dpi的目的。取消中途设置
	}

// H.M.Wang 2021-4-11 修改该变量为整形
	public void setPrintDpi(int resolution) {
// H.M.Wang 2021-4-6 暂时修改为true，以达到300dpi的目的。取消中途设置
//		mHighResolution = resolution == 0 ? false : true;
        mPrintDpi = (resolution == FpgaGpioOperation.DPI_VERSION_NONE ? FpgaGpioOperation.DPI_VERSION_150 : resolution);
// End of H.M.Wang 2021-4-6 暂时修改为true，以达到300dpi的目的。取消中途设置
	}
	
	public int getPrintDpi() {
//		return mHighResolution;
        return mPrintDpi;
	}
// End of H.M.Wang 2021-4-11 修改该变量为整形

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
// H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
//				.append("^000^000^000^000^")
				.append("^" + Configs.GetDpiVersion() + "^000^000^")
// End of H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
				.append(BaseObject.intToFormatString(mDots, 7))
				.append("^00000000^00000000^00000000^0000^0000^0000^000^")
				.append(mContent);
		}
		else
		{
            // H.M.Wang 修改下列两行
//            int ratio = (mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT) ? 1 : 2 ;
// H.M.Wang 2020-7-23 追加32DN打印头
//			int ratio = (mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT) ? 1 : 2 ;
			int ratio =(mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
// H.M.Wang 2022-5-27 追加32x2头类型
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_32X2 ||
// End of H.M.Wang 2022-5-27 追加32x2头类型
// H.M.Wang 2020-8-26 追加64SN打印头
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN ||
// End of H.M.Wang 2020-8-26 追加64SN打印头
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT ||
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2022-8-19 追加 （H.M.Wang 2021-8-16 追加96DN头） 时的遗漏
						mPNozzle == PrinterNozzle.MESSAGE_TYPE_96DN
// End of H.M.Wang 2022-8-19 追加 （H.M.Wang 2021-8-16 追加96DN头） 时的遗漏
			) ? 1 : 2 ;

//			str += mId+"^";
//			str += "00000^00000^00000^00000^0^000^";
//			str += BaseObject.intToFormatString(mPNozzle.mType,3) + "^000^000^000^000^";
//			str += BaseObject.intToFormatString(dotCount()*2, 7)+"^00000000^00000000^00000000^0000^0000^0000^000^"+mContent;
//			Debug.d(TAG, "file string ["+str+"]");		
			builder.append("^00000^00000^00000^00000^0^000^")
				.append(BaseObject.intToFormatString(mPNozzle.mType,3))
// H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
//				.append("^000^")
				.append("^" + Configs.GetDpiVersion() + "^")
// End of H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
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
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
//		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM) {
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X48 ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X50 ||
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X48 ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X50 ||
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X1) {
			size = new String[mBaseList_9mm.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = String.valueOf(mBaseList_9mm[i]);
			}
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7) { //single
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
// H.M.Wang 2020-7-23 追加32DN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-8-17 追加32SN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN) {
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32SN) {
// End of H.M.Wang 2020-8-17 追加32SN打印头
// End of H.M.Wang 2020-7-23 追加32DN打印头
			size = new String[mDot_32_Size.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = mDot_32_Size[i];
			}

        // H.M.Wang 追加下列5行
// H.M.Wang 2020-8-26 追加64SN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// H.M.Wang 2022-5-27 追加32x2头类型
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN) {
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32X2) {
// End of H.M.Wang 2022-5-27 追加32x2头类型
// End of H.M.Wang 2020-8-26 追加64SN打印头
            size = new String[mDot_64_Size.length];
            for (int i = 0; i < size.length; i++) {
                size[i] = mDot_64_Size[i];
            }
// H.M.Wang 2021-9-20 (H.M.Wang 2021-8-16 追加96DN头) 追加遗漏内容
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_96DN) {
			size = new String[mDot_96_Size.length];
			for (int i = 0; i < size.length; i++) {
				size[i] = mDot_96_Size[i];
			}
// End of H.M.Wang 2021-9-20 (H.M.Wang 2021-8-16 追加96DN头) 追加遗漏内容
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
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
//		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM) {
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X48 ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X50 ||
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X48 ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X50 ||
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X1) {
			return h * 12.7f / 9;
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_12_7) {
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
// H.M.Wang 2020-4-15 追加"5x5"字体
			if (size.equalsIgnoreCase(mDotSizes[0])) {
				h = 12.7f * 5 / 16;
			} else if (size.equalsIgnoreCase(mDotSizes[1])) {
				h = 12.7f * 8 / 16;
			} else if (size.equalsIgnoreCase(mDotSizes[2])) {
				h = 12.7f * 10 / 16;
			} else if (size.equalsIgnoreCase(mDotSizes[3])) {
				h = 12.7f * 12 / 16;
			} else if (size.equalsIgnoreCase(mDotSizes[4])) {
				h = 12.7f * 14 / 16;
//			} else if (size.equalsIgnoreCase(mDotSizes[4]) || size.equalsIgnoreCase(mDotSizes[5]) || size.equalsIgnoreCase(mDotSizes[6])) {
			} else {	// 与上面等效
				h = 12.7f * 16 / 16;
// End of H.M.Wang 2020-4-15 追加"5x5"字体
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			}
// H.M.Wang 2020-7-23 追加32DN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT ) { //addbylk
// H.M.Wang 2020-8-17 追加32SN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN) {
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32SN) {
// Enf of H.M.Wang 2020-8-17 追加32SN打印头
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
// H.M.Wang 2020-4-15 追加"5x5"字体
			if (size.equalsIgnoreCase(mDot_32_Size[0])) {
				h = 12.7f * 5 / 32;
			} else if (size.equalsIgnoreCase(mDot_32_Size[1])) {
				h = 12.7f * 8 / 32;
			} else if (size.equalsIgnoreCase(mDot_32_Size[2])) {
				h = 12.7f * 10 / 32;
			} else if (size.equalsIgnoreCase(mDot_32_Size[3])) {
				h = 12.7f * 12 / 32;
			} else if (size.equalsIgnoreCase(mDot_32_Size[4])) {
				h = 12.7f * 14 / 32;
			} else if (size.equalsIgnoreCase(mDot_32_Size[5]) || size.equalsIgnoreCase(mDot_32_Size[6]) || size.equalsIgnoreCase(mDot_32_Size[7])) {
				h = 12.7f * 16 / 32;
// End of H.M.Wang 2020-4-15 追加"5x5"字体
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size.equalsIgnoreCase(mDot_32_Size[8])) {
				h = 12.7f * 19 / 32;
			} else if (size.equalsIgnoreCase(mDot_32_Size[9])) {
				h = 12.7f * 21 / 32;
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else {
				h = 12.7f * 32 / 32;
			}
//			return h;

        // H.M.Wang 追加下列9行
// H.M.Wang 2020-8-26 追加64SN打印头
//		} else  if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT ) {
// H.M.Wang 2022-5-27 追加32x2头类型
//		} else  if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN) {
		} else  if ( mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32X2) {
// End of H.M.Wang 2022-5-27 追加32x2头类型
// End of H.M.Wang 2020-8-26 追加64SN打印头
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
// H.M.Wang 2020-4-15 追加"5x5"字体
			if (size.equalsIgnoreCase(mDot_64_Size[0])) {
				h = 12.7f * 5 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[1])) {
				h = 12.7f * 8 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[2])) {
				h = 12.7f * 10 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[3])) {
				h = 12.7f * 12 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[4])) {
                h = 12.7f * 14 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[5]) || size.equalsIgnoreCase(mDot_64_Size[6]) || size.equalsIgnoreCase(mDot_64_Size[7])) {
				h = 12.7f * 16 / 64;
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size.equalsIgnoreCase(mDot_64_Size[8])) {
				h = 12.7f * 19 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[9])) {
				h = 12.7f * 21 / 64;
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size.equalsIgnoreCase(mDot_64_Size[10])) {
				h = 12.7f * 24 / 64;
			} else if (size.equalsIgnoreCase(mDot_64_Size[11])) {
				h = 12.7f * 32 / 64;
            } else {
                h = 12.7f * 64 / 64;
            }
// End of H.M.Wang 2020-4-15 追加"5x5"字体
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
// H.M.Wang 2022-8-19 追加 （H.M.Wang 2021-8-16 追加96DN头） 时的遗漏
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_96DN) {
			if (size.equalsIgnoreCase(mDot_96_Size[0])) {
				h = 12.7f * 5 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[1])) {
				h = 12.7f * 8 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[2])) {
				h = 12.7f * 10 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[3])) {
				h = 12.7f * 12 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[4])) {
				h = 12.7f * 14 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[5]) || size.equalsIgnoreCase(mDot_64_Size[6]) || size.equalsIgnoreCase(mDot_64_Size[7])) {
				h = 12.7f * 16 / 96;
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size.equalsIgnoreCase(mDot_96_Size[8])) {
				h = 12.7f * 19 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[9])) {
				h = 12.7f * 21 / 96;
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size.equalsIgnoreCase(mDot_96_Size[10])) {
				h = 12.7f * 24 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[11])) {
				h = 12.7f * 32 / 96;
			} else if (size.equalsIgnoreCase(mDot_96_Size[12])) {
				h = 12.7f * 64 / 96;
			} else {
				h = 12.7f * 96 / 96;
			}
// End of H.M.Wang 2022-8-19 追加 （H.M.Wang 2021-8-16 追加96DN头） 时的遗漏
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
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
//		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM) {
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X48 ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X50 ||
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X48 ||
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X50 ||
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
			mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X1) {
			sizelist = mBaseList_9mm;
		}
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

// H.M.Wang 2020-8-8 在每个判断高度得if语句里面，准确计算的数值后面都+1，目的是为了解决在保存时，由于四舍五入会导致数值偏差，再次读入时会发生错误。如：
		if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
// H.M.Wang 2020-4-15 追加"5x5"字体
			if (size <= 152f * 5 / 16 + 1) {
				return mDotSizes[0];
			} else if (size <= 152f * 8 / 16 + 1) {
				return mDotSizes[1];
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
//			} else {
//				return mDotSizes[1];
            } else if(size <= 152f * 10 / 16 + 1) {
                return mDotSizes[2];
            } else if(size <= 152f * 12 / 16 + 1) {
                return mDotSizes[3];
            } else if(size <= 152f * 14 / 16 + 1) {
                return mDotSizes[4];
            } else {
//				return mDotSizes[4];
                return mDotSizes[5] + "," + mDotSizes[6] + "," + mDotSizes[7];
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			}
// H.M.Wang 2020-7-23 追加32DN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-8-17 追加32SN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN) {
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_32_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32DN || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32SN) {
// End of H.M.Wang 2020-8-17 追加32SN打印头
// End of H.M.Wang 2020-7-23 追加32DN打印头

			if (size <= 152f * 5 / 32 + 1) {
				return mDot_32_Size[0];
			} else if (size <= 152f * 8 / 32 + 1) {
				return mDot_32_Size[1];
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            } else if (size <= 152f * 10 / 32 + 1) {
                return mDot_32_Size[2];
            } else if (size <= 152f * 12 / 32 + 1) {
                return mDot_32_Size[3];
            } else if (size <= 152f * 14 / 32 + 1) {
                return mDot_32_Size[4];
            } else if (size <= 152f * 16 / 32 + 1) {
				return mDot_32_Size[5] + "," + mDot_32_Size[6] + "," + mDot_32_Size[7];
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size <= 152f * 19 / 32 + 1) {
				return mDot_32_Size[8];
			} else if (size <= 152f * 21 / 32 + 1) {
//			} else if (size <= 152f * 21.1f / 32 + 1) {	// 【已经取消21.1f的处理方式，改为+1】应该是21，但是由于设置高的时候，取得RealPixel=12.7x21/32=8.334375，
														// 再乘以PIXELS_PER_MM(12)= 100.0125，取整=100。而这里计算的152x21/32=99.75，
														// 出现误差，导致判断错误。所以只好设21.1。实际上这两个方向的计算因为不是完全反运算，
														// 因此产生误差难免，应该采用完全封闭的反运算才合理
				// 出现问题的路径是：
				// ObjectInfoDialog.java(564) mObject.setHeight(mHighEdit.getText().toString());
				// BaseObject.java(901) setHeight(String size); 其中结算RealPixels得到100
				// BaseObject.java(748) setHeight(float size)；
				// BaseObject.java(788) tuningHeightOfSpecialHeadtype()；
				// BaseObject.java(907) getDisplayHeight()；
				// MessageObject.java(404) getDisplayFs()
				// 进入到本段时，size=100，但是计算的152f * 21.1f / 32=99.750，因此size <= 152f * 21.1f / 32就不成立了
				// 经过分析，下面是所有情况的计算值，由于有取整操作，因此，只有32点和64点的21x14会出问题
				// 12.7	5	16	12	47.0 		152	5	16	47.500
				// 12.7	8	16	12	76.0 		152	8	16	76.000
				// 12.7	10	16	12	95.0 		152	10	16	95.000
				// 12.7	12	16	12	114.0	 	152	12	16	114.000
				// 12.7	14	16	12	133.0	 	152	14	16	133.000
				// 12.7	16	16	12	152.0	 	152	16	16	152.000

				// 12.7	5	32	12	23.0 		152	5	32	23.750
				// 12.7	8	32	12	38.0 		152	8	32	38.000
				// 12.7	10	32	12	47.0 		152	10	32	47.500
				// 12.7	12	32	12	57.0 		152	12	32	57.000
				// 12.7	14	32	12	66.0 		152	14	32	66.500
				// 12.7	16	32	12	76.0 		152	16	32	76.000
				// 12.7	19	32	12	90.0 		152	19	32	90.250
				// 12.7	21	32	12	100.0 		152	21	32	99.750 --> 有问题
				// 12.7	32	32	12	152.0 		152	32	32	152.000

				// 12.7	5	64	12	11.0 		152	5	64	11.875
				// 12.7	8	64	12	19.0 		152	8	64	19.000
				// 12.7	10	64	12	23.0 		152	10	64	23.750
				// 12.7	12	64	12	28.0 		152	12	64	28.500
				// 12.7	14	64	12	33.0 		152	14	64	33.250
				// 12.7	16	64	12	38.0 		152	16	64	38.000
				// 12.7	19	64	12	45.0 		152	19	64	45.125
				// 12.7	21	64	12	50.0 		152	21	64	49.875 --> 有问题
				// 12.7	24	64	12	57.0 		152	24	64	57.000
				// 12.7	32	64	12	76.0 		152	32	64	76.000
				// 12.7	64	64	12	152.0 		152	64	64	152.000

				return mDot_32_Size[9];
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else {
				return mDot_32_Size[10];
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			}

        // H.M.Wang 追加下列10行
// H.M.Wang 2020-8-26 追加64SN打印头
//		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// H.M.Wang 2022-5-27 追加32x2头类型
//        } else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN) {
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_64_DOT || mPNozzle == PrinterNozzle.MESSAGE_TYPE_64SN || mPNozzle == PrinterNozzle.MESSAGE_TYPE_32X2) {
// End of H.M.Wang 2022-5-27 追加32x2头类型
// End of H.M.Wang 2020-8-26 追加64SN打印头
			if (size <= 152f * 5 / 64 + 1) {
				return mDot_64_Size[0];
			} else if (size <= 152f * 8 / 64 + 1) {
                return mDot_64_Size[1];
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            } else if (size <= 152f * 10 / 64 + 1) {
                return mDot_64_Size[2];
            } else if (size <= 152f * 12 / 64 + 1) {
                return mDot_64_Size[3];
            } else if (size <= 152f * 14 / 64 + 1) {
                return mDot_64_Size[4];
			} else if (size <= 152f * 16 / 64 + 1) {
				return mDot_64_Size[5] + "," + mDot_64_Size[6] + "," + mDot_64_Size[7];
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size <= 152f * 19 / 64 + 1) {
				return mDot_64_Size[8];
			} else if (size <= 152f * 21 / 64 + 1) {
//			} else if (size <= 152f * 21.1f / 64 + 1) {
				return mDot_64_Size[9];
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (size <= 152f * 24 / 64 + 1) {
				return mDot_64_Size[10];
			} else if (size <= 152f * 32 / 64 + 1) {
				return mDot_64_Size[11];
            } else {
				return mDot_64_Size[12];
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            }
// End of H.M.Wang 2020-8-8 在每个判断高度得if语句里面，准确计算的数值后面都+1，目的是为了解决在保存时，由于四舍五入会导致数值偏差，再次读入时会发生错误。如：
// H.M.Wang 2021-9-16 追加 （H.M.Wang 2021-8-16 追加96DN头） 时的遗漏
		} else if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_96DN) {
			if (size <= 152f * 5 / 96 + 1) {
				return mDot_96_Size[0];
			} else if (size <= 152f * 8 / 96 + 1) {
				return mDot_96_Size[1];
			} else if (size <= 152f * 10 / 96 + 1) {
				return mDot_96_Size[2];
			} else if (size <= 152f * 12 / 96 + 1) {
				return mDot_96_Size[3];
			} else if (size <= 152f * 14 / 96 + 1) {
				return mDot_96_Size[4];
			} else if (size <= 152f * 16 / 96 + 1) {
				return mDot_96_Size[5] + "," + mDot_96_Size[6] + "," + mDot_96_Size[7];
			} else if (size <= 152f * 19 / 96 + 1) {
				return mDot_96_Size[8];
			} else if (size <= 152f * 21 / 96 + 1) {
//			} else if (size <= 152f * 21.1f / 64 + 1) {
				return mDot_96_Size[9];
			} else if (size <= 152f * 24 / 96 + 1) {
				return mDot_96_Size[10];
			} else if (size <= 152f * 32 / 96 + 1) {
				return mDot_96_Size[11];
			} else if (size <= 152f * 64 / 96 + 1) {
				return mDot_96_Size[12];
			} else {
				return mDot_96_Size[13];
			}
// End of H.M.Wang 2021-9-16 追加 （H.M.Wang 2021-8-16 追加96DN头） 时的遗漏
        } else {
// End of H.M.Wang 2020-4-15 追加"5x5"字体
// H.M.Wang 2019-9-29 去除ScaleH，否则会由于其采用了308/304而使得计算结果被放大
//			h = mPNozzle.getScaleH() * size/PIXELS_PER_MM;
			type = mPNozzle.factor();
			h = type * size/PIXELS_PER_MM;
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
//			if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM) {
			if (mPNozzle == PrinterNozzle.MESSAGE_TYPE_9MM ||
				mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X48 ||
				mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X50 ||
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
				mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X48 ||
				mPNozzle == PrinterNozzle.MESSAGE_TYPE_E5X50 ||
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
				mPNozzle == PrinterNozzle.MESSAGE_TYPE_E6X1) {
				h = type * size/152*9;
			}
		}
		for (int i = 0; i < sizelist.length; i++) {
			if ((h > type * sizelist[i] - 0.3) && (h < type * sizelist[i] + 0.3)) {
				h = sizelist[i] * type;
				break;
			}
		}

// H.M.Wang 2019-9-29 保留小数点后1位
//  H.M.Wang 2019-6-24 西班牙语时，通过String.format("%.1f", h)转换会把6.0转为6,0。强制转回来
//		return String.format("%.1f", h);	// 似乎不需要这个转换
//		Debug.d(TAG, "String.valueOf(h) = " + String.valueOf(h) + "; size=" + size + "; h=" + h);
		return String.valueOf(h);
//  End of H.M.Wang 2019-6-24 西班牙语时，通过String.format("%.1f", h)转换会把6.0转为6,0。强制转回来
	}
}

/*
喷头		字体		字号		高度(画面、内部，152基数)
16DOT
		5x5		5x5			152f * 5 / 16
		4		7x6			152f * 8 / 16
		10		10x8		152f * 10 / 16
		12		12x9		152f * 12 / 16
		14		14x10		152f * 14 / 16
		7		16x12		152f * 16 / 16
		7L		16@L		152f * 16 / 16
		7R		16@R		152f * 16 / 16

32DOT
		5x5		5x5			152f * 5 / 32
		4		7x6			152f * 8 / 32
		10		10x8		152f * 10 / 32
		12		12x9		152f * 12 / 32
		14		14x10		152f * 14 / 32
		7		16x12		152f * 16 / 32
		7L		16@L		152f * 16 / 32
		7R		16@R		152f * 16 / 32
		19		19x13		152f * 19 / 32
		21		21x14		152f * 21 / 32
				32			152f * 32 / 32

64DOT
		5x5		5x5			152f * 5 / 64
		4		7x6			152f * 8 / 64
		10		10x8		152f * 10 / 64
		12		12x9		152f * 12 / 64
		14		14x10		152f * 14 / 64
		7		16x12		152f * 16 / 64
		7L		16@L		152f * 16 / 64
		7R		16@R		152f * 16 / 64
		19		19x13		152f * 19 / 64
		21		21x14		152f * 21 / 64
				24			152f * 24 / 64
				32			152f * 32 / 64
				64			152f * 64 / 64

96DOT
		5x5		5x5			152f * 5 / 96
		4		7x6			152f * 8 / 96
		10		10x8		152f * 10 / 96
		12		12x9		152f * 12 / 96
		14		14x10		152f * 14 / 96
		7		16x12		152f * 16 / 96
		7L		16@L		152f * 16 / 96
		7R		16@R		152f * 16 / 96
		19		19x13		152f * 19 / 96
		21		21x14		152f * 21 / 96
				24			152f * 24 / 96
				32			152f * 32 / 96
				64			152f * 64 / 96
				96			152f * 96 / 96
 */