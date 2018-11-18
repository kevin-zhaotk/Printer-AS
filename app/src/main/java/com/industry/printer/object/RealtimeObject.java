package com.industry.printer.object;

import java.util.Vector;

import com.industry.printer.MessageTask;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.data.DotMatrixReader;
import com.industry.printer.data.InternalCodeCalculater;

import android.R;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.text.format.Time;
import android.util.Log;

public class RealtimeObject extends BaseObject {

	private static final String TAG = RealtimeObject.class.getSimpleName();
	
	public final static long MS_DAY = 24*60*60*1000;
	public static final long MS_PER_HOUR = 60*60*1000;
	public static final long MS_PER_MINUTE = 60*1000;
	public String mFormat; /* yyyy-mm-dd hh:nn for example*/
	public Vector<BaseObject> mSubObjs;
	public int mOffset;
	
	public RealtimeObject(Context context,  float x) {
		super(context, BaseObject.OBJECT_TYPE_RT, x);
		//Time t = new Time();
		Debug.d(TAG, ">>>RealtimeObject mcontext: " + mContext);
		mSubObjs = new Vector<BaseObject>();
		mOffset = 0;
		setFormat("YY-MM-DD");
	}

	public void setFormat(String format)
	{
		if(format==null || (mFormat!= null &&mFormat.equals(format)))
			return;
		Debug.d(TAG, ">>>setFormat mcontext: " + mContext);
		mFormat = format;
		parseFormat();
		super.setWidth(mXcor_end - mXcor);
		isNeedRedraw = true;
		setTask(mTask);
	}
	
	public String getFormat()
	{
		return mFormat;
	}
	
	public void parseFormat()
	{
		int i=0;
		float x = getX();
		Debug.d(TAG, "parseFormat x="+x);
		String str = mFormat.toUpperCase();
		BaseObject o=null;
		mSubObjs.clear();
		for(;str != null && str.length()>0;)
		{
			if(	!str.startsWith("YY", i) &&
					!str.startsWith("AA", i) &&
					!str.startsWith("MM", i) &&
					!str.startsWith("DD", i) &&
					!str.startsWith("GG", i) &&
					!str.startsWith("HH", i) &&
					!str.startsWith("NN", i))
			{
				i += 1;
				continue;
			}
			Debug.d(TAG, "str="+str+", i="+i);
			if(i>0)
			{
				o = new TextObject(mContext, x);
				o.setContent(str.substring(0, i));
				mSubObjs.add(o);
				
				/*树莓系统通过点阵字库计算坐标，每个字模列宽为16bit*/ 
				if (PlatformInfo.isBufferFromDotMatrix()==1) {
					x = x + o.getContent().length() * 16;
				} 
				/*通过bitmap提取点阵的系统用下面的计算方法*/
				else {
					x = o.getXEnd();
				}
			}
			
			if(str.startsWith("YY", i) || str.startsWith("AA", i))
			{
				o = new RealtimeYear(mContext, this, x,false);
				mSubObjs.add(o);
				i += 2;
			}
			else if(str.startsWith("MM", i))
			{
				o = new RealtimeMonth(mContext, this, x);
				mSubObjs.add(o);
				i += 2;
			}
			else if(str.startsWith("DD", i) || str.startsWith("GG", i))
			{
				o = new RealtimeDate(mContext, this, x);
				mSubObjs.add(o);
				i += 2;
			}
			else if(str.startsWith("HH", i))
			{
				o = new RealtimeHour(mContext, x);
				mSubObjs.add(o);
				i += 2;
			}
			else if(str.startsWith("NN", i))
			{
				o = new RealtimeMinute(mContext, x);
				mSubObjs.add(o);
				i += 2;
			} else {
				continue;
			}
			
			/*树莓系统通过点阵字库计算坐标，每个字模列宽为16bit*/ 
			if (PlatformInfo.isBufferFromDotMatrix()==1) {
				x = x + o.getContent().length() * 16;
			} 
			/*通过bitmap提取点阵的系统用下面的计算方法*/
			else {
				x = o.getXEnd();
			}
			str = str.substring(i);
			i=0;
		}
		mXcor_end = x;
		setWidth(mXcor_end - getX());
	}
	
	public void setIndex(int index)
	{
		mIndex = index++;
		for (BaseObject object : mSubObjs) {
			object.setIndex(index++);
		}
	}
	
	public void setOffset(int offset) {
		mOffset = offset;
	}
	
	public int getOffset() {
		return mOffset;
	}
	 
	@Override
	public Bitmap getScaledBitmap(Context context)
	{
		// Debug.d(TAG, "--->getBitmap width="+(mXcor_end - mXcor)+", mHeight="+mHeight);
		
		if (!isNeedRedraw) {
			return mBitmap;
		}

		/* 如果需要重新繪製，先計算新的尺寸 */
		if (mXcor_end - mXcor == 0) {
			meature();
		}
		mContent = "";
		Debug.d(TAG, "--->getScaledBitmap xEnd: " + mXcor_end + " x="+ mXcor + "  height=" + mHeight);
		// meature();
		
//		mBitmap = Bitmap.createBitmap((int)(mXcor_end - mXcor) , (int)mHeight, Configs.BITMAP_CONFIG);
//		mCan = new Canvas(mBitmap);
//		Log.d(TAG, "++++>" + getX() + "   " + getXEnd() + "  width=" + mBitmap.getWidth());
		for(BaseObject o : mSubObjs)
		{
			Log.d(TAG, "++++>id:" + o.mId + ", x=" + (o.getX() - getX()));
			//Bitmap b = o.getScaledBitmap(context);
			//mCan.drawBitmap(b, o.getX()-getX(), 0, mPaint);
			mContent += o.getContent();
		}
		mBitmap = super.getScaledBitmap(context);
		isNeedRedraw = false;
		return mBitmap;
	}
	
	
	public Bitmap getBgBitmap(Context context, float scaleW, float scaleH)
	{
		Debug.d(TAG, "getBitmap width="+(mXcor_end - mXcor)+", mHeight="+mHeight);
		// meature();
		Bitmap bmp = Bitmap.createBitmap((int)(mXcor_end * scaleW - mXcor * scaleW) , (int)(mHeight * scaleH), Configs.BITMAP_CONFIG);
		//System.out.println("getBitmap width="+width+", height="+height+ ", mHeight="+mHeight);
		mCan = new Canvas(bmp);
		mCan.drawColor(Color.WHITE);
		for(BaseObject o : mSubObjs)
		{
			Debug.d(TAG, "--->obj: " + o.mId);
			//constant 
			if(o instanceof TextObject)
			{
				Bitmap b = o.makeBinBitmap(context, o.getContent(), (int)(o.getWidth() * scaleW), (int)(o.getHeight() * scaleH), o.getFont());
				if (b == null) {
					continue;
				}
				mCan.drawBitmap(b, (int)(o.getX() * scaleW - getX() * scaleW), 0, mPaint);
			}
			else	//variable
			{
				// o.drawVarBitmap();
			}
		}
		return bmp;
	}

	@Override
	public int drawVarBitmap() {
		int dot = 0;
		for (BaseObject o : mSubObjs) {
			if (o instanceof TextObject) {
				continue;
			}
			dot += o.drawVarBitmap();
		}
		return dot;
	}
	
	@Override
	public int makeVarBin(Context ctx, float scaleW, float scaleH, int dstH) {
		int dot = 0;
		for(BaseObject o : mSubObjs)
		{
			Debug.d(TAG, "--->obj: " + o.mId);
			if(o instanceof TextObject)
			{
			}
			else	//variable
			{
				dot += o.makeVarBin(ctx, scaleW, scaleH, dstH);
			}
		}
		return dot;
	}
	
	@Override
	protected void meature() {
		float x = mXcor;
		for (int i = 0; i < mSubObjs.size(); i++) {
			BaseObject object = mSubObjs.get(i);
			object.meature();
			object.setX(x);
			x = x + object.getWidth();
		}
		mXcor_end = x;
		setWidth(mXcor_end - mXcor);
	};
	
	
	@Override
	public void setHeight(float size)
	{
		super.setHeight(size);
		size = getHeight();
		Debug.d(TAG, "--->setHeight: " + size);
		float x = getX();
		isNeedRedraw = true;
		if(mSubObjs == null)
			return;
		for(BaseObject o : mSubObjs)
		{
			// o.setX(x);
			o.setHeight(size);
			o.resizeByHeight();
			
			// x = o.getXEnd();
		}
		// setWidth(x - getX());
	}
	
	@Override
	public void setHeight(String size)
	{
		int height = mTask.getMsgObject().getPixels(size);
		setHeight(height);
	}
	
	public void wide() {
		float x = getX();
		for(BaseObject o : mSubObjs) {
			o.setX(x);
			o.setWidth(o.getWidth() + 1);
			x = o.getXEnd();
		}
		setWidth(x - getX());
		isNeedRedraw = true;
	}
	
	public void narrow() {
		float x = getX();
		for(BaseObject o : mSubObjs) {
			o.setX(x);
			o.setWidth(o.getWidth() - 1);
			x = o.getXEnd();
		}
		setWidth(x - getX());
		isNeedRedraw = true;
	}
	
	@Override
	public void setX(float x)
	{
		//float i;
		super.setX(x);
		if(mSubObjs == null)
			return;
		for(BaseObject o : mSubObjs)
		{
			o.setX(x);
			x = o.getXEnd();
			Debug.d(TAG, "===>x=" + o.getX() + ",  end=" + x);
		}
	}
	
	@Override
	public void setY(float y)
	{
		super.setY(y);
		if(mSubObjs == null)
			return;
		for(BaseObject o : mSubObjs)
		{
			o.setY(y);
		}
	}
	
	@Override
	public void setSelected(boolean s)
	{
		super.setSelected(s);
		if(mSubObjs == null)
				return;
		for(BaseObject o : mSubObjs)
		{
			o.setSelected(s);
		}
	}
	
	@Override
	public void setFont(String font) {
		super.setFont(font);
		if(mSubObjs == null)
				return;
		for(BaseObject o : mSubObjs)
		{
			o.setFont(font);
		}
		isNeedRedraw = true;
		// meature();
	}
	
	@Override
	public String getContent() {
		mContent = "";
		
		for (BaseObject object : mSubObjs) {
			mContent += object.getContent();
		}
		return mContent;
	}
	
	
	@Override
	public void generateVarbinFromMatrix(String f) {
		for (BaseObject object : getSubObjs()) {
			if (object.mId.equals(BaseObject.OBJECT_TYPE_TEXT)) {
				continue;
			}
			object.generateVarbinFromMatrix(f);
			
		}
	}
	
	/**
	 * 设置当前object所在的Task
	 * @param task
	 */
	@Override
	public void setTask(MessageTask task) {
		mTask = task;
		for (BaseObject obj : mSubObjs) {
			obj.setTask(task);
		}
	}
	
	public String toString()
	{
		float prop = getProportion();
		String str="";
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
		str += mId+"^";
		str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
		str += BaseObject.floatToFormatString(getXEnd()*2  * prop, 5)+"^";
		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
		str += BaseObject.intToFormatString(0, 1)+"^";
		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
		//str += BaseObject.intToFormatString(mContent.length(), 3)+"^";
		str += "000^000^000^000^000^";
		str += BaseObject.intToFormatString(mOffset, 5) + "^00000000^00000000^00000000^0000^0000^" + mFont + "^000^"+mFormat;
		System.out.println("file string ["+str+"]");
		return str;
	}
	
	public Vector<BaseObject> getSubObjs()
	{
		return mSubObjs;
	}
//////addbylk 
	@Override	 
	public Bitmap getpreviewbmp()
	{
		// Debug.d(TAG, "--->getBitmap width="+(mXcor_end - mXcor)+", mHeight="+mHeight);
		Bitmap bitmap;

		/* 如果需要重新繪製，先計算新的尺寸 */
		if (mXcor_end - mXcor == 0) {
	//		meature();
		}
		Debug.e(TAG, "=============--->getScaledBitmap xEnd: " + mXcor_end + " x="+ mXcor + "  height=" + mHeight);
		// meature();
		
		bitmap = Bitmap.createBitmap((int)(mXcor_end - mXcor) , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		mCan = new Canvas(bitmap);
		Log.d(TAG, "++++>x: " + getX() + "   " + getXEnd() + "  width=" + bitmap.getWidth());
		for(BaseObject o : mSubObjs)
		{
			Bitmap b = o.getpreviewbmp();
			Debug.e(TAG, "============>id:" + o.mId + ", x=" + o.getX() + "  width = " + b.getWidth());
			mCan.drawBitmap(b, o.getX()-getX(), 0, mPaint);
		}
		return bitmap;
	}

}
