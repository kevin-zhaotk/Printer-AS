package com.industry.printer.object;

import java.util.Calendar;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;
import android.os.SystemClock;
import android.text.format.Time;
import android.util.Log;

public class RealtimeMonth extends BaseObject {

	public static final String TAG="RealtimeMonth";
	public int mOffset;
	public RealtimeObject mParent;
	
	public RealtimeMonth(Context context, float x) {
		super(context, BaseObject.OBJECT_TYPE_DL_MON, x);
		Time t = new Time();
		mOffset = 0;
		mParent = null;
		t.set(System.currentTimeMillis());
		setContent(BaseObject.intToFormatString(t.month+1, 2));
	}
	
	public RealtimeMonth(Context context, RealtimeObject parent, float x) {
		this(context, x);
		mParent = parent;
	}

	@Override
	public String getContent()
	{
		if (mParent != null) {
			mOffset = mParent.getOffset();
		}
		Time t = new Time();
		
		t.set(System.currentTimeMillis() + mOffset * RealtimeObject.MS_DAY - timeDelay());
		setContent(BaseObject.intToFormatString(t.month+1, 2));
		Log.d(TAG, ">>getContent, "+mContent);
		return mContent;
	}
	
	public String toString()
	{
		float prop = getProportion();
		StringBuilder builder = new StringBuilder(mId);
		
		builder.append("^")
				.append(BaseObject.floatToFormatString(getX() * prop, 5))
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
				.append("000^000^000^000^000^")
				.append(mParent == null? "00000":BaseObject.intToFormatString(mParent.getOffset(), 5))
				.append("^00000000^00000000^00000000^0000^0000^")
				.append(mFont)
				.append("^000^000");
		
		String str = builder.toString();
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		str += mId+"^";
//		str += BaseObject.floatToFormatString(getX() * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getXEnd() * prop, 5)+"^";
//		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
//		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//		str += BaseObject.intToFormatString(0, 1)+"^";
//		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//		//str += BaseObject.intToFormatString(mContent.length(), 3)+"^";
//		str += "000^000^000^000^000^";
//		str += mParent == null? "00000":BaseObject.intToFormatString(mParent.getOffset(), 5);
//		str += "^00000000^00000000^00000000^0000^0000^" + mFont + "^000^000";
		System.out.println("file string ["+str+"]");
		return str;
	}
//////addbylk 
	@Override	 
	public Bitmap getpreviewbmp()
	{	Debug.d(TAG, "1===== " + getContent() );
		Bitmap bitmap;
		mPaint.setTextSize(getfeed());
		mPaint.setAntiAlias(true);   
		mPaint.setFilterBitmap(true); 
	
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
			mPaint.setTypeface(FontCache.getFromExternal(mFont));
		} catch (Exception e) {}

		String str_new_content="";
		str_new_content =	mContent;	
		
		str_new_content =	str_new_content.replace('0', 'M');		
		str_new_content =	str_new_content.replace('1', 'M');	
		str_new_content =	str_new_content.replace('2', 'M');	
		str_new_content =	str_new_content.replace('3', 'M');	
		str_new_content =	str_new_content.replace('4', 'M');	
		str_new_content =	str_new_content.replace('5', 'M');	
		str_new_content =	str_new_content.replace('6', 'M');	
		str_new_content =	str_new_content.replace('7', 'M');	
		str_new_content =	str_new_content.replace('8', 'M');	
		str_new_content =	str_new_content.replace('9', 'M');


		int width = (int)mPaint.measureText(str_new_content);
		Debug.d(TAG, "--->content: " + str_new_content + "  width=" + width);
		
//		Debug.d(TAG, "2===== " + getContent() );
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
		mCan = new Canvas(bitmap);
		FontMetrics fm = mPaint.getFontMetrics();
		mPaint.setColor(Color.BLUE);

		mCan.drawText(str_new_content , 0, mHeight-fm.descent, mPaint);
	
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
	}	
}
