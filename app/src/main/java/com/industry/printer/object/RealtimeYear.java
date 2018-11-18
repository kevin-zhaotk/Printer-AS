package com.industry.printer.object;

import java.util.Calendar;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.Utils.Configs;
/*
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Color;
*/

import android.R.color;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Paint.FontMetrics;
import android.graphics.Typeface;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;

import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;
import android.text.format.Time;
import android.util.Log;

public class RealtimeYear extends BaseObject {

	public static final String TAG="RealtimeYear";
	public String mFormat;
	public int mOffset;
	public RealtimeObject mParent;
	
	public RealtimeYear(Context context, float x, boolean f) {
		super(context, BaseObject.OBJECT_TYPE_DL_YEAR, x);
		mOffset = 0;
		mParent = null;
		Time t = new Time();
		t.set(System.currentTimeMillis());
		if(!f)
		{
			mFormat="YY";
			setContent(BaseObject.intToFormatString(t.year%100, 2));
		}
		else if(f)
		{
			mFormat="YYYY";
			setContent(BaseObject.intToFormatString(t.year, 4));
		}
		System.out.println("<<<RealtimeYear");
	}
	
	public RealtimeYear(Context context, RealtimeObject parent, float x, boolean f) {
		this(context, x ,f);
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
		
		if(mFormat.length()==2)
			setContent(BaseObject.intToFormatString(t.year%100, 2));
		else if(mFormat.length()==4)
			setContent(BaseObject.intToFormatString(t.year, 4));
		Debug.d(TAG, ">>getContent, "+mContent);
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
		Debug.d(TAG, "file string ["+str+"]");
		return str;
	}
//////addbylk 
	@Override	 
	public Bitmap getpreviewbmp()
	{
		Bitmap bitmap;
	    Paint Paint; 
		Paint = new Paint();
		Paint.setTextSize(getfeed());
		Paint.setAntiAlias(true);//.setAntiAlias(true);   
		Paint.setFilterBitmap(true); 
	
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
			Paint.setTypeface(FontCache.getFromExternal(mFont));
		} catch (Exception e) {}

		String str_new_content="";
		str_new_content =	mContent;	
		
		str_new_content =	str_new_content.replace('0', 'Y');		
		str_new_content =	str_new_content.replace('1', 'Y');	
		str_new_content =	str_new_content.replace('2', 'Y');	
		str_new_content =	str_new_content.replace('3', 'Y');	
		str_new_content =	str_new_content.replace('4', 'Y');	
		str_new_content =	str_new_content.replace('5', 'Y');	
		str_new_content =	str_new_content.replace('6', 'Y');	
		str_new_content =	str_new_content.replace('7', 'Y');	
		str_new_content =	str_new_content.replace('8', 'Y');	
		str_new_content =	str_new_content.replace('9', 'Y');	
		//Debug.e(TAG, "--->content: " + getContent() + "  width=" + width);

		int width = (int)Paint.measureText(str_new_content);
		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);

		Canvas can = new Canvas(bitmap);
		FontMetrics fm = Paint.getFontMetrics();

		Paint.setColor(Color.BLUE);

		can.drawText(str_new_content , 0, mHeight-fm.descent, Paint);
	
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
	}
}
