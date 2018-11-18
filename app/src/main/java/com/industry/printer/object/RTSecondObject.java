package com.industry.printer.object;

import java.text.SimpleDateFormat;
import java.util.Date;

import com.industry.printer.Utils.Debug;

import android.content.Context;

public class RTSecondObject extends BaseObject {

	public RTSecondObject(Context context, float x) {
		super( context, BaseObject.OBJECT_TYPE_RT_SECOND, x);
		// TODO Auto-generated constructor stub
	}

	@Override
	public String getContent()
	{
		SimpleDateFormat dateFormat = new SimpleDateFormat("ss");
		mContent = dateFormat.format(new Date());
		return mContent;
	}
	

	public String toString()
	{
		float prop = getProportion();
		String str="";
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
		str += mId+"^";
		str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getXEnd()*2 * prop, 5)+"^";
		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
		str += BaseObject.intToFormatString(0, 1)+"^";
		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
		str += "000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^" + mFont + "^000^000";
		Debug.d(TAG,"file string ["+str+"]");
		return str;
	}
}
