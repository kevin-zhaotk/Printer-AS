package com.industry.printer.object;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.Configs;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint.Style;

public class RectObject extends BaseObject {

	public int mLineType;
	public RectObject(Context context, float x) {
		super(context, BaseObject.OBJECT_TYPE_RECT,x);
		setWidth(100);
		setHeight(50);
		setLineWidth(5);
		setLineType(0);
		mPaint.setStyle(Style.STROKE);
	}


	public void setLineType(int type)
	{
		mLineType=type;
	}
	
	public void setLineType(String type)
	{
		String[] lines = mContext.getResources().getStringArray(R.array.strLineArray);
		for (int i = 0; i < lines.length; i++) {
			if (lines[i].equals(type)) {
				mLineType = i;
				break;
			}
		}
	}
	
	public int getLineType()
	{
		return mLineType;
	}
	
	public String getLineName() {
		String[] lines = mContext.getResources().getStringArray(R.array.strLineArray);
		if (mLineType < 0 || mLineType >= lines.length) {
			return null;
		}
		return lines[mLineType];
	}
	
	@Override
	public Bitmap getScaledBitmap(Context context)
	{
		float adjust = mLineWidth/2;
		Bitmap bmp = Bitmap.createBitmap((int)mWidth , (int)mHeight, Configs.BITMAP_CONFIG);
		mCan = new Canvas(bmp);
		mCan.drawRect(adjust, adjust, mWidth-adjust, mHeight-adjust, mPaint);
		//can.drawText("text", 0, 4, p);
		//mCan.save();
		return bmp;
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
				.append(BaseObject.floatToFormatString(getLineWidth(), 3))
				.append("^")
				.append(BaseObject.intToFormatString(getLineType(), 3))
				.append("^")
				.append("000^000^000^00000000^00000000^00000000^00000000^0000^0000^0000^000^000");
		
		String str = builder.toString();
//		String str="";
//		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		str += mId+"^";
//		str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getXEnd()*2 * prop, 5)+"^";
//		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
//		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//		str += BaseObject.intToFormatString(0, 1)+"^";
//		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//		str += BaseObject.floatToFormatString(getLineWidth(), 3)+"^";
//		str += BaseObject.intToFormatString(getLineType(), 3)+"^";
//		str += "000^000^000^00000000^00000000^00000000^00000000^0000^0000^0000^000^000";
		Debug.d(TAG,"file string ["+str+"]");
		return str;
	}
}
