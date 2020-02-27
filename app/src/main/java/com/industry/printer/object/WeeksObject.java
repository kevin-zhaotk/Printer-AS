package com.industry.printer.object;

import com.industry.printer.Utils.Debug;

import android.content.Context;

public class WeeksObject extends BaseObject {
	private static final String TAG = WeeksObject.class.getSimpleName();

	public WeeksObject(Context context, float x) {
		super(context, OBJECT_TYPE_WEEKS, x);
	}

	public String toString()
	{
		float prop = getProportion();
		String str="";
		str += mId+"^";
		str += BaseObject.floatToFormatString(getX() * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getY() * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getXEnd() * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getYEnd() * prop, 5)+"^";
		str += BaseObject.intToFormatString(0, 1)+"^";
		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
		str += "000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^0000^000^000";
		Debug.d(TAG, "toString = [" + str + "]");
//		Debug.d(TAG,"file string ["+str+"]");
		return str;
	}
}
