package com.industry.printer.object;

import java.util.Calendar;

import com.industry.printer.MessageTask.MessageType;
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
import android.graphics.Rect;
import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;

public class LetterHourObject extends BaseObject {

	private static final String TAG = LetterHourObject.class.getSimpleName();
	
	private static final String[] HOUR_LETTER = {"A","B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P",
												"Q", "R", "S", "T", "U", "V", "W", "X"};
	public LetterHourObject(Context ctx, float x) {
		super(ctx, BaseObject.OBJECT_TYPE_LETTERHOUR, x);
		super.setContent("H");
	}
	
	@Override
	public void setContent(String content) {
		
		
	}
	
	@Override
	public String getContent() {
		long time = System.currentTimeMillis();
		
		Calendar calendar = Calendar.getInstance();
		calendar.setTimeInMillis(time);
		int hour = calendar.get(Calendar.HOUR_OF_DAY);
		if (hour < 0 || hour >= HOUR_LETTER.length) {
			hour = 0;
		}
		return HOUR_LETTER[hour];
		
	}
	@Override	 
	public Bitmap getScaledBitmap(Context context) {
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
			mPaint.setTypeface(FontCache.getFromExternal(mFont));
		} catch (Exception e) {}
		int width = (int)mPaint.measureText("W");
		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		if (mWidth == 0) {
			setWidth(width);
		}
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
		mCan = new Canvas(bitmap);
		FontMetrics fm = mPaint.getFontMetrics();
		mPaint.setColor(Color.BLUE);
		mCan.drawText(getContent() , 0, mHeight-fm.descent, mPaint);
	
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
	}
	
	@Override
	public int makeVarBin(Context ctx, float scaleW, float scaleH, int dstH) {
		int dots[];
		int singleW;
		Paint paint = new Paint();
		int height = (int) (mHeight * scaleH);
		paint.setTextSize(height);
		paint.setAntiAlias(true); //去除锯齿  
		paint.setFilterBitmap(true); //对位图进行滤波处理
		//buffer減半，因此scaleW要除以2
		scaleW =scaleW / 2;
		
		try {
			paint.setTypeface(FontCache.getFromExternal(mFont));
		} catch (Exception e) {
			
		}
		
		int width = (int)paint.measureText("H");
		FontMetrics fm = paint.getFontMetrics();
		
		/*draw Bitmap of single digit*/
		Bitmap bmp = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
		Canvas can = new Canvas(bmp);
		
		/*draw 0-9 totally 10 digits Bitmap*/
		singleW = (int)(mWidth * scaleW);
		Debug.d(TAG, "--->singleW=" + singleW);
		
		/* 最終生成v.bin使用的bitmap */
		Bitmap gBmp = Bitmap.createBitmap(singleW*HOUR_LETTER.length, dstH, Configs.BITMAP_CONFIG);
		Canvas gCan = new Canvas(gBmp);
		
		gCan.drawColor(Color.WHITE);	/*white background*/
		for(int i =0; i< HOUR_LETTER.length; i++)
		{
			can.drawColor(Color.WHITE);
			can.drawText(HOUR_LETTER[i], 0, height - fm.descent, paint);
			gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, height, false), i*singleW, (int)getY() * scaleH, paint);
		}
		BinFromBitmap.recyleBitmap(bmp);
		
		BinFileMaker maker = new BinFileMaker(mContext);
		dots = maker.extract(gBmp, 1);
		Debug.d(TAG, "--->id: " + mId + " index:  " + mIndex);
		maker.save(ConfigPath.getVBinAbsolute(mTask.getName(), mIndex));
		//
		BinFromBitmap.recyleBitmap(gBmp);
		/*根據變量內容的實際長度計算點數*/
		dots[0] = (dots[0]* getContent().length()/24) + 1;
		return dots[0];
	}
	

//////addbylk 
	@Override	 
	public Bitmap getpreviewbmp()
	{	Debug.e(TAG, "1===== " + getContent() );
		Bitmap bitmap;
		mPaint.setTextSize(getfeed());
		mPaint.setAntiAlias(true); //ȥ����  
		mPaint.setFilterBitmap(true); //��λͼ�����˲�����
	
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
		
		int width = (int)mPaint.measureText(getContent());

		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		if (mWidth == 0) {
			setWidth(width);
		}
		Debug.d(TAG, "2===== " + getContent() );
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
		mCan = new Canvas(bitmap);
		FontMetrics fm = mPaint.getFontMetrics();
		Debug.d(TAG, "3===== " + getContent() );
		
				
		mCan.drawText(mContent , 0, mHeight-fm.descent, mPaint);
	
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
	}
	
	public String toString()
	{
		float prop = getProportion();
		StringBuilder builder = new StringBuilder(mId);
		
		builder.append("^")
				.append(BaseObject.floatToFormatString(getX()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getY()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getXEnd()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getYEnd()*2 * prop, 5))
				.append("^")
				.append(BaseObject.intToFormatString(0, 1))
				.append("^")
				.append(BaseObject.boolToFormatString(mDragable, 3))
				.append("^")
				.append("000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^")
				.append(mFont)
				.append("^000^")
				.append(mContent);
		
		String str = builder.toString();
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		str += mId+"^";
//		str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getXEnd()*2 * prop, 5)+"^";
//		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
//		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//		str += BaseObject.intToFormatString(0, 1)+"^";
//		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//		str += "000"+"^";
//		str += "000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^" + mFont + "^000^"+mContent;
		System.out.println("file string ["+str+"]");
		return str;
	}
	
}
