package com.industry.printer.object;

import java.util.Calendar;


import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.cache.FontCache;
import com.industry.printer.Utils.Configs;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;

public class WeekOfYearObject extends BaseObject{
	private static final String TAG = WeekOfYearObject.class.getSimpleName();

	public WeekOfYearObject(Context ctx) {
		this(ctx, 0);
		init();
	}
	
	public WeekOfYearObject(Context ctx, float x) {
		super(ctx, BaseObject.OBJECT_TYPE_WEEKOFYEAR, x);
	}

	public WeekOfYearObject(Context context, BaseObject parent, float x) {
		this(context, x);
		mParent = parent;
	}

	private void init() {
		Calendar calendar = Calendar.getInstance();
		int week = calendar.get(Calendar.WEEK_OF_YEAR);
		setContent(BaseObject.intToFormatString(week, 2));
		Debug.d(TAG, "--->week of year: " + week);
	}

	@Override
	public String getMeatureString() {
		return "00";
	}

	@Override
	public String getContent() {
		Calendar calendar = Calendar.getInstance();
		int week = calendar.get(Calendar.WEEK_OF_YEAR);
		mContent = BaseObject.intToFormatString(week, 2);
		return mContent;
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
		String str_new_content="W";	
		for (int i = 1; i < mContent.length(); i++) {
			str_new_content += "W";
		}
			
		int width = (int)Paint.measureText(str_new_content);
		if (mWidth == 0) {
			setWidth(width);
		}
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
	 
		Canvas can = new Canvas(bitmap);
		FontMetrics fm = Paint.getFontMetrics();
		Paint.setColor(Color.BLUE); 
		can.drawText(str_new_content , 0, mHeight-fm.descent, Paint);
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
	}
	
	@Override
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
//				.append("000^000^000^000^000^000")
//				.append("^00000000^00000000^00000000^0000^0000^")
				.append(mFont)
				.append("^000^000");
		
		String str = builder.toString();
		Debug.d(TAG, "toString = [" + str + "]");
//		Debug.d(TAG, "file string ["+str+"]");
		return str;
	}
}
