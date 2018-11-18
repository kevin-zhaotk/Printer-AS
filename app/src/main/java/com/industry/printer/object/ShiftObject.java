package com.industry.printer.object;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import com.industry.printer.Utils.Debug;

import android.content.Context;
import android.graphics.Bitmap;

public class ShiftObject extends BaseObject {
	public final String TAG="ShiftObject";
	
	private static final int SHIFT_MAX = 4;
	
	public int mBits;
	public int mShifts[];
	public String mValues[];
	public ShiftObject(Context context, float x) {
		super( context, BaseObject.OBJECT_TYPE_SHIFT, x);
		mBits = 1;
		mShifts = new int[SHIFT_MAX];
		mShifts[0]=800;
		mShifts[1]=1400;
		mShifts[2]=2200;
		mShifts[3]=1800;
		mValues = new String[SHIFT_MAX];
		mValues[0] = "1";
		mValues[1] = "2";
		mValues[2] = "3";
		mValues[3] = "4";		
	}
	
	public void setShift(int shift, String time)
	{
		if(shift >=SHIFT_MAX || 
				shift<0 || 
				time==null || 
				!checkNum(time) )
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
	
	public int getShift(int shift)
	{
		if(shift < 0 || shift >= SHIFT_MAX)
			return 0;
		return mShifts[shift];
	}
	
	
	public void setValue(int shift, String val)
	{
		Debug.d(TAG, "===>setValue shift: " + shift + "  val: " + val + "  bit: " + mBits);
		if(shift >= SHIFT_MAX || shift<0 || val==null || !checkNumandLetter(val))
			return;
		
		if (val.length() > mBits) {
			mValues[shift]  = val.substring(val.length() - mBits);
		} else if (val.length() < mBits) {
			mValues[shift] = "";
			for (int i = 0; i < mBits - val.length(); i++) {
				mValues[shift] += "0";
			}
			mValues[shift] += val;
		} else {
			mValues[shift] = val;
		}
		Debug.d(TAG, "===>shift: " + shift + "  value: " + mValues[shift]);
	}
	
	public void setBits(int bits ) {
		mBits = bits;
	}
	
	public int getBits() {
		return mBits;
	}
	
	public String getValue(int shift)
	{
		if(shift >= SHIFT_MAX || shift<0)
			return null;
		Debug.d(TAG, "===>value: " + mValues[shift]);
		return mValues[shift];
	}
	
	public int getShiftIndex() {
		int i = 0;
		Calendar c = Calendar.getInstance();
		int hour = c.get(Calendar.HOUR_OF_DAY) * 100;
		
		for (i = 0; i < SHIFT_MAX-1; i++) {
			if (hour >= mShifts[i] && hour < mShifts[i+1]) {
				break;
			}
		}
		Debug.d(TAG, "===>index: " + i);
		return i;
	}
	
	@Override
	public String getContent() {
		int i=0;
		Calendar c = Calendar.getInstance();
		int hour = c.get(Calendar.HOUR_OF_DAY) * 100;
		for (i = 0; i < 4; i++) {
			if (hour >= mShifts[i] && hour < mShifts[i+1]) {
				break;
			}
		}
		Debug.d(TAG, "--->shift Value: " + getValue(i));
		return getValue(i);
	}
	
	@Override
	public Bitmap getScaledBitmap(Context context)
	{
		int i=0,index=0;
		Debug.d(TAG,"getScaledBitmap  mWidth="+mWidth+", mHeight="+mHeight);
		SimpleDateFormat dateFormat = new SimpleDateFormat("HHmm");
		int date = 0;
		try {
			date = Integer.parseInt(dateFormat.format(new Date()));
		} catch (Exception e) {
			Debug.e(TAG, "--->" + e.getMessage());
		}
		Debug.d(TAG, "date="+date);
		for(i=0; i<5; i++)
		{
			if(!isValidShift(i))
			{
				index = i-1;
				break;
			}
			//Debug.d(TAG, "mShifts["+i+"]="+mShifts[i]+", mShifts[i+1]="+mShifts[i+1]+", isValidShift(i+1)="+ isValidShift(i+1));
			if(date > mShifts[i] && isValidShift(i+1) && date<mShifts[i+1])
			{
				index =i;
				break;
			}
			else
				continue;
		}
		Debug.d(TAG, "index="+index);
		if(index>4 || index<0)
			index=0;
		setContent(mValues[index]);
		return Bitmap.createScaledBitmap(getBitmap(context), (int)mWidth, (int)mHeight, true);
	}
	
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
		str += mValues[0]+"^"+mValues[1]+"^"+mValues[2]+"^"+mValues[3]+"^";
		str += mShifts[0]+"^"+mShifts[1]+"^"+mShifts[2]+"^"+mShifts[3]+"^"+"00000^150" + "^";
		str += mFont+"^" + "000" + "^" + "1";
		Debug.d(TAG,"counter string ["+str+"]");
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
	
	public boolean isValidShift(int i)
	{
		if(i<0 || i >= SHIFT_MAX)
		{
			Debug.d(TAG, "invalide i="+i);
			return false;
		}
		else if(i==0 && mShifts[i]>=0 && mShifts[i] <= 2400)
		{
			Debug.d(TAG, "valide i="+i);
			return true;
		}
		else if(i < SHIFT_MAX && mShifts[i]>=0 && mShifts[i] <= 2400 && (mShifts[i] > mShifts[i-1]))
		{
			Debug.d(TAG, "valide i="+i);
			return true;
		}			
		else
			return false;
	}
}
