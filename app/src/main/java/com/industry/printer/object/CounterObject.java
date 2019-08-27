package com.industry.printer.object;

import com.industry.printer.MainActivity;
import com.industry.printer.R;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.cache.FontCache;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;
import android.renderscript.Sampler.Value;
import android.util.Log;
import com.industry.printer.hardware.RTCDevice;

public class CounterObject extends BaseObject {

	private static final String TAG_STRING = CounterObject.class.getSimpleName();
	
	public int mBits;
	public boolean mDirection;
	public int mStart;
	public int mEnd;
	public int mValue;
	public int mStepLen;
	// 计数器初始值对应设置里10个计数器值的编号
	public int mCounterIndex;
	//public int mCurVal;
	
	public CounterObject(Context context, float x) {
		super(context, BaseObject.OBJECT_TYPE_CNT, x);
		mStart=0;
		mStepLen=1;
		mDirection = true;
		setBits(5);
	}

	public void setBits(int n)
	{
		mBits = n;
		mValue = 1;
		setContent( BaseObject.intToFormatString(mValue, mBits));
		mEnd = (int) Math.pow(10, mBits) -1;
	}
	
//	@Override
//	public void setContent(String content) {
//		super.setContent(content);
//		
//	}
	
	public int getBits()
	{
		return mBits;
	}
	
	public void setStart(int begin)
	{
		mStart = begin;
	}
	
	public int getStart()
	{
		return mStart;
	}
	
	public void setEnd(int end)
	{
		mEnd = end;
	}
	
	public int getEnd()
	{
		return mEnd;
	}
	
	public void setRange(int start, int end) {

		mStart = start;
		mEnd = end;
		if (start <= end) {
			mDirection = true;
		} else {
			mDirection = false;

		}
		Debug.i(TAG, "--->setRange: " + start + ", " + end + ", d: " + mDirection);
	}

	public String getDirection()
	{
		String[] directions = mContext.getResources().getStringArray(R.array.strDirectArray);
		return mDirection ? directions[0] : directions[1];
	}
	
	public void setSteplen(int step)
	{
		if(step < 1) {
			mStepLen = 1;
		} else {
			mStepLen = step;
		}
	}
	
	public int getSteplen()
	{
		return mStepLen;
	}
	
	public int getValue()
	{
		return mValue;
	}
	
		
	public void setValue(int value)
	{
		if( mStart < mEnd) {
			if(value < mStart || value> mEnd) {
				mValue = mStart;
			}
			else {
				mValue = value;
			}
		} else {
			if (value > mStart || value < mEnd) {
				mValue = mStart;
			} else {
				mValue = value;
			}
		}
		mContent = BaseObject.intToFormatString(mValue, mBits);
	}

	@Override
	public void setContent(String content) {
		try{
			Debug.d(TAG, "--->setContent content="+content);
			int value = Integer.parseInt(content);
			Debug.d(TAG, "setContent value="+value);
			setValue(value);
		} catch (Exception e) {
			mValue = mStart;
			Debug.d(TAG, "--->setContent exception: " + e.getMessage());
		}
		mContent = BaseObject.intToFormatString(mValue, mBits);
//		Debug.d(TAG, "setContent content="+content+", value="+mValue+", mMax="+mMax);
	}

	public String getNext()
	{
		Debug.d(TAG, "--->getNext mContent="+mContent+", mValue="+mValue+", mSteplen=" + mStepLen + " direction=" + mDirection);

		if(mDirection)	//increase
		{
			if(mValue+mStepLen > mEnd || mValue < mStart)
				mValue=mStart;
			else
				mValue += mStepLen;
		}
		else	//decrease
		{
			if(mValue-mStepLen < mEnd)
				mValue=mStart;
			else
				mValue -= mStepLen;
		}

		String value =mContent;
		setContent( BaseObject.intToFormatString(mValue, mBits));
		Debug.d(TAG, "getNext mContent="+mContent+", mValue="+mValue);
		SystemConfigFile.getInstance(mContext).setParam(mCounterIndex + SystemConfigFile.INDEX_COUNT_1, Integer.valueOf(value));
		return value;
	}

	public void rollback() {
		Debug.d(TAG, "--->rollback value: " + mValue);
		if(!mDirection)	//increase
		{
			if(mValue+mStepLen > mStart || mValue < mEnd)
				mValue=mStart;
			else
				mValue += mStepLen;
		}
		else	//decrease
		{
			if(mValue-mStepLen < mStart)
				mValue=mStart;
			else
				mValue -= mStepLen;
		}
		setContent( BaseObject.intToFormatString(mValue, mBits));
		Debug.d(TAG, "--->rollback mContent="+mContent+", mValue="+mValue);
	}
	
	public String toString()
	{
		float prop = getProportion();
		StringBuilder builder = new StringBuilder(mId);
		int v = 0;
		try {
			v = Integer.parseInt(mContent);
		} catch ( Exception e) {

		}
		builder.append("^")
				.append(BaseObject.floatToFormatString(getX()*prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getY()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getXEnd() * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getYEnd()*2 * prop, 5))
				.append("^")
				.append(BaseObject.intToFormatString(0, 1))
				.append("^")
				.append(BaseObject.boolToFormatString(mDragable, 3))
				.append("^")
				.append(BaseObject.intToFormatString(mBits, 3))
				.append("^")
				.append("000^000^000^000^")
				.append(BaseObject.intToFormatString(mStart, 8))
				.append("^")
				.append(BaseObject.intToFormatString(mEnd, 8))
				.append("^")
				.append(BaseObject.intToFormatString( mStepLen, 8))
				.append("^")
				.append(mCounterIndex)
				.append("^")
				.append("0000^0000^")
				.append(mFont)
				.append("^000^000");
				
				
		String str = builder.toString();
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		str += mId+"^";
//		str += BaseObject.floatToFormatString(getX()*prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getXEnd() * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//		str += BaseObject.intToFormatString(0, 1)+"^";
//		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//		str += BaseObject.intToFormatString(mBits, 3)+"^";
//		str += "000^000^000^000^";
//		str += BaseObject.intToFormatString(mMax, 8)+"^";
//		str += BaseObject.intToFormatString(mMin, 8)+"^";
//		str += BaseObject.intToFormatString(Integer.parseInt(mContent) , 8)+"^";
//		str += "00000000^0000^0000^" + mFont + "^000^000";
		System.out.println("counter string ["+str+"]");
		return str;
	}
//////add by lk 
	@Override	 
	public Bitmap getpreviewbmp()
	{
		Bitmap bitmap;
		mPaint.setTextSize(getfeed());
		mPaint.setAntiAlias(true); //  
		mPaint.setFilterBitmap(true); //
	
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
		} catch (Exception e) {}
		
		int width = (int)mPaint.measureText(getContent());
		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		if (mWidth == 0) {
			setWidth(width);
		}
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
		mCan = new Canvas(bitmap);
		FontMetrics fm = mPaint.getFontMetrics();
		mPaint.setColor(Color.BLUE);//���� ���� �� λͼ �� Ϊ ��ɫ 
	 
		String str_new_content = mContent;
		str_new_content =	str_new_content.replace('0', 'c');	
		
		str_new_content =	str_new_content.replace('1', 'c');	
		str_new_content =	str_new_content.replace('2', 'c');	
		str_new_content =	str_new_content.replace('3', 'c');	
		str_new_content =	str_new_content.replace('4', 'c');	
		str_new_content =	str_new_content.replace('5', 'c');	
		str_new_content =	str_new_content.replace('6', 'c');	
		str_new_content =	str_new_content.replace('7', 'c');	
		str_new_content =	str_new_content.replace('8', 'c');	
		str_new_content =	str_new_content.replace('9', 'c');	
		
		
		mCan.drawText(str_new_content , 0, mHeight-fm.descent, mPaint);
	
		Bitmap result = Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);
		BinFromBitmap.recyleBitmap(bitmap);
		return result;
	}	
	
}
