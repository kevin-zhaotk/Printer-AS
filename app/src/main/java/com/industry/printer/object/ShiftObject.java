package com.industry.printer.object;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.BinFromBitmap;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

public class ShiftObject extends BaseObject {
	private static final String TAG = ShiftObject.class.getSimpleName();

	private static final int SHIFT_MAX = 4;

	public int mBits;
	public int mShifts[];
	public String mValues[];

	public ShiftObject(Context context, float x) {
		super(context, BaseObject.OBJECT_TYPE_SHIFT, x);
		mIndex = 0;
		mShifts = new int[SHIFT_MAX];
		mShifts[0] = 0000;
		mShifts[1] = 600;
		mShifts[2] = 1200;
		mShifts[3] = 1800;
		mValues = new String[SHIFT_MAX];
		mValues[0] = "1";
		mValues[1] = "2";
		mValues[2] = "3";
		mValues[3] = "4";
		mBits = 1;
	}

	public ShiftObject(Context context, BaseObject parent, float x) {
		this(context, x);
		mParent = parent;
	}

	public void setShift(int shift, String time) {
		if (shift >= SHIFT_MAX ||
				shift < 0 ||
				time == null ||
				!checkNum(time))
			return;
		try {
			int i = Integer.parseInt(time);
			if (i < 0 || i > 2400)
				return;
			mShifts[shift] = Integer.parseInt(time);
		} catch (Exception e) {
			return;
		}
		Debug.d(TAG, "--->shift: " + shift + "---time: " + time);
	}

	public int getShift(int shift) {
		if (shift < 0 || shift >= SHIFT_MAX)
			return 0;
		return mShifts[shift];
	}


	public void setValue(int shift, String val) {
		Debug.d(TAG, "===>setValue shift: " + shift + "  val: " + val + "  bit: " + mBits);
		if (shift >= SHIFT_MAX || shift < 0 || val == null/* || !checkNumandLetter(val)*/)
			return;

		if (val.length() > mBits) {
			mValues[shift] = val.substring(val.length() - mBits);
		} else if (val.length() < mBits) {
			mValues[shift] = "";
			for (int i = 0; i < mBits - val.length(); i++) {
				mValues[shift] += " ";
			}
			mValues[shift] += val;
		} else {
			mValues[shift] = val;
		}
		Debug.d(TAG, "===>shift: " + shift + "  value: " + mValues[shift]);
	}

	public void setBits(int bits) {
		mBits = bits;
		String tStr = BaseObject.intToFormatString(0, mBits);
		setWidth(mPaint.measureText(tStr));
		for(int i=0; i<4; i++) {
			mValues[i] = ((" " + mValues[i]).substring(Math.max(mValues[i].length() + 1 - mBits, 0)));
		}
	}

	public int getBits() {
		return mBits;
	}

	public String getValue(int shift) {
		if (shift >= SHIFT_MAX || shift < 0)
			return null;
		Debug.d(TAG, "===>value: " + mValues[shift]);
		return mValues[shift];
	}

	public int getShiftIndex() {
		int i = 0;
		Calendar c = Calendar.getInstance();
		int hour = c.get(Calendar.HOUR_OF_DAY) * 100;

		for (i = 0; i < SHIFT_MAX - 1; i++) {
			if (hour >= mShifts[i] && hour < mShifts[i + 1]) {
				break;
			}
		}
		Debug.d(TAG, "===>index: " + i);
		return i;
	}

	@Override
	public String getMeatureString() {
		return (mBits == 2 ? "SS" : "S");
	}

	@Override
	public void setContent(String content) {
		super.setContent((" " + content).substring(Math.max(content.length()+1-mBits,0)));
	}

	@Override
	public String getContent() {
		int i=0;
		Calendar c = Calendar.getInstance();
		int hour = c.get(Calendar.HOUR_OF_DAY) * 100;
		for (i = 0; i < 3; i++) {
			if (hour >= mShifts[i] && hour < mShifts[i+1]) {
				break;
			}
		}
		Debug.d(TAG, "--->shift Value: " + getValue(i));
		String value = getValue(i);

		setContent(value);
		return super.getContent();
	}

// H.M.Wang 2020-1-21 追加预览图生成函数
	@Override
	public Bitmap getpreviewbmp()
	{
		Bitmap bitmap;
		mPaint.setTextSize(getfeed());
		mPaint.setAntiAlias(true); //
		mPaint.setFilterBitmap(true); //
/*
		boolean isCorrect = false;
		// Debug.d(TAG,"--->getBitmap font = " + mFont);
		for (String font : mFonts) {
			if (font.equals(mFont)) {
				isCorrect = true;
				break;
			}
		}
		if (!isCorrect) {
			mFont = DEFAULT_FONT;
		}

		try {
			mPaint.setTypeface(FontCache.get(mContext, mFont));
		} catch (Exception e) {
			Debug.e(TAG, e.getMessage());
		}

		int width = (int)mPaint.measureText(getMeatureString());

		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		if (mWidth == 0) {
			setWidth(width);
		}
*/
		bitmap = Bitmap.createBitmap((int)mWidth, (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);

		mCan = new Canvas(bitmap);
		Paint.FontMetrics fm = mPaint.getFontMetrics();
		mPaint.setColor(Color.BLUE);

		StringBuilder sb = new StringBuilder();
		for(int i=0; i<mBits; i++) {
			sb.append('S');
		}

		mCan.drawText(sb.toString(), 0, mHeight-fm.descent, mPaint);

		Bitmap result = Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);
		if(result != bitmap) {
			BinFromBitmap.recyleBitmap(bitmap);
		}

		return result;
	}
// End of H.M.Wang 2020-1-21 追加预览图生成函数

// H.M.Wang 2020-2-6 修改生成Shift的位图
	@Override
	public Bitmap getScaledBitmap(Context context)
	{
		int i=0,index=0;
		Debug.d(TAG,"getScaledBitmap  mWidth = " + mWidth + ", mHeight = " + mHeight);
		SimpleDateFormat dateFormat = new SimpleDateFormat("HHmm");
		int date = 0;
		try {
			date = Integer.parseInt(dateFormat.format(new Date()));
		} catch (Exception e) {
			Debug.e(TAG, "--->" + e.getMessage());
		}
		Debug.d(TAG, "date="+date);
		for(i=0; i<4; i++)
		{
			if(!isValidShift(i))
			{
				index = i-1;
				break;
			}
			//Debug.d(TAG, "mShifts["+i+"]="+mShifts[i]+", mShifts[i+1]="+mShifts[i+1]+", isValidShift(i+1)="+ isValidShift(i+1));
			if(date > mShifts[i] && isValidShift(i+1) && date < mShifts[i+1])
			{
				index =i;
				break;
			}
			else
				continue;
		}
		Debug.d(TAG, "index = " + index);

		if(index>4 || index<0)
			index=0;

		setContent(mValues[index]);

		mBitmap = super.getScaledBitmap(context);
		isNeedRedraw = false;

		return mBitmap;
	}
// End of H.M.Wang 2020-2-6 修改生成Shift的位图

// H.M.Wang 2020-2-5 追加Shift生成Vbin
    @Override
    public int makeVarBin(Context ctx, float scaleW, float scaleH, int dstH) {
		int dots[] = new int[1];
		int singleW;
		Paint paint = new Paint();
		int height = Math.round(mHeight * scaleH);
		paint.setTextSize(height);
		paint.setAntiAlias(true); //去除锯齿
		paint.setFilterBitmap(true); //对位图进行滤波处理

		try {
			paint.setTypeface(FontCache.get(ctx, mFont));
		} catch (Exception e) {

		}

//		Debug.d(TAG, "SaveTime: - Start makeVarBin : " + System.currentTimeMillis());
		int width = Math.round(paint.measureText("8"));
		Paint.FontMetrics fm = paint.getFontMetrics();

		PrinterNozzle head = mTask.getNozzle();

		// H.M.Wang 修改下列两行
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
			head == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
			head == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
			head == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
// H.M.Wang 2020-8-26 追加64SN打印头
			head == PrinterNozzle.MESSAGE_TYPE_64SN ||
// End of H.M.Wang 2020-8-26 追加64SN打印头
			head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
			singleW = width;
		} else {
			singleW = Math.round(mWidth * scaleW/mContent.length());
		}

		/*draw 0-9 totally 10 digits Bitmap*/

		/** if message isn`t high resolution, divid by 2 because the buffer bitmap is halfed, so the variable buffer should be half too*/
		MessageObject msgObj = mTask.getMsgObject();
		if (!msgObj.getResolution() ) {
			singleW = singleW / msgObj.getPNozzle().getFactorScale();
		}

		Debug.d(TAG, "--->singleW=" + singleW);

		/* 最終生成v.bin使用的bitmap */
		Bitmap gBmp = Bitmap.createBitmap(singleW * 2 * 5, dstH, Configs.BITMAP_CONFIG);
		Canvas gCan = new Canvas(gBmp);
		gCan.drawColor(Color.WHITE);	/*white background*/

		/*draw Bitmap of single digit*/
		Bitmap bmp = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
		Canvas can = new Canvas(bmp);

		for(int i=0; i<4; i++) {
			can.drawColor(Color.WHITE);
			if(mBits == 1) {
//				can.drawText("0", 0, height - fm.descent, paint);
			} else {
				can.drawText(mValues[i].substring(0,1), 0, height - fm.descent, paint);
			}
			gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, height, false), (i * 2) * singleW, Math.round(getY() * scaleH), paint);

			can.drawColor(Color.WHITE);
			can.drawText(mValues[i].substring(mBits-1, mBits), 0, height - fm.descent, paint);
			gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, height, false), (i * 2 + 1) * singleW, Math.round(getY() * scaleH), paint);
		}

		BinFromBitmap.recyleBitmap(bmp);

		BinFileMaker maker = new BinFileMaker(mContext);

		// H.M.Wang 追加一个是否移位的参数。修改喷头数
		dots = maker.extract(Bitmap.createScaledBitmap(gBmp, gBmp.getWidth(), dstH, false), head.mHeads,
				(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
						mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
						mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
						mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR));

		Debug.d(TAG, "--->id: " + mId + " index:  " + mIndex);
		maker.save(ConfigPath.getVBinAbsolute(mTask.getName(), mIndex));
		//
		BinFromBitmap.recyleBitmap(gBmp);
		/*根據變量內容的實際長度計算點數*/
		dots[0] = (dots[0]* getContent().length()/10) + 1;

//		Debug.d(TAG, "SaveTime: - End makeVarBin : " + System.currentTimeMillis());
		return dots[0];
    }

    private String to3bitsString(String src) {
		String str = "000" + src;
		return str.substring(str.length() - 3);
	}

// End of H.M.Wang 2020-2-5 追加Shift生成Vbin

    public String toString()
	{
		float prop = getProportion();
		String str="";
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
		str += mId+"^";
		str += BaseObject.floatToFormatString(getX() * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getXEnd() * prop, 5)+"^";
		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
		str += BaseObject.intToFormatString(0, 1)+"^";
		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
		str += BaseObject.intToFormatString(mBits, 3)+"^";
		str += to3bitsString(mValues[0])+"^"+to3bitsString(mValues[1])+"^"+to3bitsString(mValues[2])+"^"+to3bitsString(mValues[3])+"^";
//		str += BaseObject.intToFormatString(mShifts[0],8)+"^"+BaseObject.intToFormatString(mShifts[1],8)+"^"+BaseObject.intToFormatString(mShifts[2],8)+"^"+BaseObject.intToFormatString(mShifts[3],8)+"^"+"00000^150" + "^";
		str += BaseObject.intToFormatString(mShifts[0],8)+"^"+BaseObject.intToFormatString(mShifts[1],8)+"^"+BaseObject.intToFormatString(mShifts[2],8)+"^"+BaseObject.intToFormatString(mShifts[3],8)+"^";
		str += (mParent == null ? "0000" : String.format("%03d", mParent.mIndex)) + "^150" + "^";
		str += mFont+"^" + "000" + "^" + "1";
		Debug.d(TAG, "toString = [" + str + "]");
//		Debug.d(TAG,"shift string ["+str+"]");
		return str;
	}
	
	
	public boolean checkNum(String args){
		Pattern p=Pattern.compile("^[0-9]*"); 
		Matcher m=p.matcher(args);
		if(m.matches())
		return true;
		else
		return false;
	}
	
	public boolean checkNumandLetter(String args)
	{
		Pattern p = Pattern.compile("^([0-9]|[a-z]|[A-Z])*");
		Matcher m = p.matcher(args);
		if(m.matches())
			return true;
		else
			return false;
	}
	
	public boolean isValidShift(int idx)
	{
		if(idx<0 || idx >= SHIFT_MAX)
		{
			Debug.e(TAG, "Invalid i = " + idx);
			return false;
		}
		else if(idx==0 && mShifts[idx]>=0 && mShifts[idx] <= 2400)
		{
			Debug.d(TAG, "Valid i = " + idx);
			return true;
		}
		else if(idx < SHIFT_MAX && mShifts[idx]>=0 && mShifts[idx] <= 2400 && (mShifts[idx] > mShifts[idx-1]))
		{
			Debug.d(TAG, "Valid i = " + idx);
			return true;
		}			
		else
			return false;
	}
}
