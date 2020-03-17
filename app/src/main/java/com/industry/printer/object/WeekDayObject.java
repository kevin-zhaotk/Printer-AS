package com.industry.printer.object;

import java.util.Calendar;
import java.util.Locale;

import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.Utils.Configs;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.provider.ContactsContract.Contacts.Data;

public class WeekDayObject extends BaseObject {
	private static final String TAG = WeekDayObject.class.getSimpleName();

	public int mBits;

	public WeekDayObject(Context context, float x) {
		super(context, OBJECT_TYPE_WEEKDAY, x);
		Calendar c = Calendar.getInstance();
		int dat = c.get(Calendar.DAY_OF_WEEK);
		mBits = 1;
		setContent(String.valueOf(dat));
	}

	public WeekDayObject(Context context) {
		this(context, 0);
	}

	public WeekDayObject(Context context, BaseObject parent, float x) {
		this(context, x);
		mParent = parent;
	}

	public void setBits(int bits ) {
		if(bits != 1 && bits != 2) return;

		if(mBits != bits) {
			mBits = bits;

			String cnt = getContent();
			setWidth(mPaint.measureText(cnt));
			setContent(mContent);
		}
	}

	@Override
	public String getMeatureString() {
		return (mBits == 2 ? "00" : "0");
	}

	@Override
	public void setContent(String content) {
		super.setContent(("0" + content).substring(Math.max(content.length()+1-mBits,0)));
	}

	@Override
	public String getContent() {
		Calendar c = Calendar.getInstance();
		int dat = c.get(Calendar.DAY_OF_WEEK);
		if (dat == 1) {
			dat = 7;
		} else {
			dat = dat - 1;
		}

		setContent("" + dat);
		return super.getContent();
	}
	
	
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
////		for (String font : mFonts) {
////			if (font.equals(mFont)) {
////				isCorrect = true;
////				break;
////			}
////		}
////		if (!isCorrect) {
////			mFont = DEFAULT_FONT;
////		}
		try {
			Paint.setTypeface(FontCache.get(mContext, mFont));
		} catch (Exception e) {}
		
		int width = (int)Paint.measureText("D");
		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		if (mWidth == 0) {
			setWidth(width);
		}
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
	 
		Canvas can = new Canvas(bitmap);
		FontMetrics fm = Paint.getFontMetrics();
		
		Paint.setColor(Color.BLUE); 
	
		
		 
		String str_new_content="D";	
		can.drawText(str_new_content , 0, mHeight-fm.descent, Paint);
	
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
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
				.append("000^000^000^000^000^00000000^00000000^00000000^00000000^" + (mParent == null ? "0000" : String.format("%03d", mParent.mIndex)) + "^0000^")
//				.append("000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^")
				.append(mFont)
				.append("^000^000");
				
		String str = builder.toString();
//		String str="";
//		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		str += mId+"^";
//		str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getXEnd()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//		str += BaseObject.intToFormatString(0, 1)+"^";
//		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//		str += "000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^0000^000^000";
		Debug.d(TAG, "toString = [" + str + "]");
//		Debug.d(TAG,"file string ["+str+"]");
		return str;
	}
	
	
}
