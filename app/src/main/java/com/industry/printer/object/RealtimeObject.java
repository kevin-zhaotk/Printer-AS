package com.industry.printer.object;

import java.util.Vector;

import com.industry.printer.MessageTask;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;

import android.R;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;

public class RealtimeObject extends BaseObject {

	private static final String TAG = RealtimeObject.class.getSimpleName();
	
	public final static long MS_DAY = 24*60*60*1000;
	public static final long MS_PER_HOUR = 60*60*1000;
	public static final long MS_PER_MINUTE = 60*1000;
	public String mFormat; /* yyyy-mm-dd hh:nn for example*/
	public Vector<BaseObject> mSubObjs;

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
		Debug.d(TAG, ">>>setFormat Format: " + format);
		parseFormat();
//		setWidth(mXcor_end - mXcor);
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
				!str.startsWith("YYYY", i) &&
//				!str.startsWith("AAAA", i) &&
//				!str.startsWith("RRRR", i) &&
//				!str.startsWith("RR", i) &&
				!str.startsWith("AA", i) &&
				!str.startsWith("MM", i) &&
				!str.startsWith("DD", i) &&
//				!str.startsWith("GG", i) &&
				!str.startsWith("HH", i) &&
				!str.startsWith("NN", i) &&
// H.M.Wang 2020-8-6 增加SS秒时间格式
//                !str.startsWith("mm", i) &&
//                !str.startsWith("ss", i) &&
				!str.startsWith("SS", i))
// End of H.M.Wang 2020-8-6 增加SS秒时间格式
			{
				i += 1;
				continue;
			}
			Debug.d(TAG, "str="+str+", i="+i);
			if(i>0)
			{
				o = new TextObject(mContext, this, x);
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

//			if(str.startsWith("YYYY", i) || str.startsWith("AAAA", i))
			if(str.startsWith("YYYY", i))
			{
				o = new RealtimeYear(mContext, this, x,true);
				mSubObjs.add(o);
				i += 4;
//			} else if(str.startsWith("YY", i) || str.startsWith("AA", i)) {
			} else if(str.startsWith("YY", i)) {
				o = new RealtimeYear(mContext, this, x,false);
				mSubObjs.add(o);
				i += 2;
			} else if(str.startsWith("MM", i)) {
				o = new RealtimeMonth(mContext, this, x);
				mSubObjs.add(o);
				i += 2;
			}
//			else if(str.startsWith("DD", i) || str.startsWith("GG", i))
			else if(str.startsWith("DD", i))
			{
				o = new RealtimeDate(mContext, this, x);
				mSubObjs.add(o);
				i += 2;
			}
			else if(str.startsWith("HH", i))
			{
// H.M.Wang 2019-9-24 追加小时的所属信息
				o = new RealtimeHour(mContext, this, x);
//				o = new RealtimeHour(mContext, x);
				mSubObjs.add(o);
				i += 2;
			}
// H.M.Wang 2020-8-6 增加SS秒时间格式
//			else if(str.startsWith("NN", i) || str.startsWith("mm", i)) {
			else if(str.startsWith("NN", i)) {
// End of H.M.Wang 2020-8-6 增加SS秒时间格式
// H.M.Wang 2019-9-24 追加分钟的所属信息
				o = new RealtimeMinute(mContext, this, x);
//				o = new RealtimeMinute(mContext, x);
				mSubObjs.add(o);
				i += 2;
			}
// H.M.Wang 2020-8-6 增加SS秒时间格式
//			else if(str.startsWith("SS", i) || str.startsWith("ss", i))
			else if(str.startsWith("SS", i))
			{
				o = new RealtimeSecond(mContext, this, x);
				mSubObjs.add(o);
				i += 2;
// End of H.M.Wang 2020-8-6 增加SS秒时间格式
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
			Debug.d(TAG, "parseFormat -> str:" + str +"; x="+x);
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
//		Debug.d(TAG, "++++>" + getX() + "   " + getXEnd() + "  width=" + mBitmap.getWidth());
		for(BaseObject o : mSubObjs)
		{
			Debug.d(TAG, "++++>id:" + o.mId + ", x=" + (o.getX() - getX()));
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
		Debug.d(TAG, "getBgBitmap scaleW="+scaleW+", scaleH="+scaleH);
		Debug.d(TAG, "getBgBitmap width="+Math.round(mXcor_end * scaleW - mXcor * scaleW)+", mHeight="+Math.round(mHeight * scaleH));
		// meature();
		Bitmap bmp = Bitmap.createBitmap(Math.round(mXcor_end * scaleW - mXcor * scaleW) , Math.round(mHeight * scaleH), Configs.BITMAP_CONFIG);
		//System.out.println("getBitmap width="+width+", height="+height+ ", mHeight="+mHeight);
		mCan = new Canvas(bmp);
		mCan.drawColor(Color.WHITE);
		for(BaseObject o : mSubObjs)
		{
			Debug.d(TAG, "--->obj: " + o.mId + "; o.x: " + o.getX() + "; x: " + getX());
			//constant
			if(o instanceof TextObject)
			{
				Bitmap b = o.makeBinBitmap(context, o.getContent(), Math.round(o.getWidth() * scaleW), Math.round(o.getHeight() * scaleH), o.getFont());
				if (b == null) {
					continue;
				}
				mCan.drawBitmap(b, Math.round(o.getX() * scaleW - getX() * scaleW), 0, mPaint);
				Debug.d(TAG, "drawBitmap: x=" + Math.round((o.getX() * scaleW)) + "; y=" + Math.round(o.getY() * scaleH) + "; w= " + b.getWidth() + "; h= " + b.getHeight());
			}
			else	//variable
			{
				// o.drawVarBitmap();
			}
		}
		return bmp;
	}

	@Deprecated
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
			Debug.d(TAG, "Content:" + object.getContent() + "; mWidth:" + object.getWidth() + "; mRatio:" + mRatio);
			object.setWidth(object.getWidth() * mRatio);
//			object.setWidth(object.getWidth());
			x = x + object.getWidth();
		}
		mXcor_end = x;
		Debug.d(TAG, "Toal mXcor_end:" + mXcor_end + "; mXcor:" + mXcor);
		setWidth(mXcor_end - mXcor);
	};
	
	
	@Override
	public void setHeight(float size)
	{
		super.setHeight(size);
		size = getHeight();
		Debug.d(TAG, "--->setHeight: " + size);
		float x = getX();
		if(mSubObjs == null)
			return;
		for(BaseObject o : mSubObjs)
		{
			// o.setX(x);
			o.setHeight(size);
// H.M.Wang 2020-1-6 调整高度的时候，不主动按比例调整宽度
//			o.resizeByHeight();
// End of H.M.Wang 2020-1-6 调整高度的时候，不主动按比例调整宽度

			// x = o.getXEnd();
		}
		// setWidth(x - getX());
		isNeedRedraw = true;
		this.meature();
	}
	
//	@Override
//	public void setHeight(String size)
//	{
//		float height = 1.0f * mTask.getMsgObject().getPixels(size);
//		this.setHeight(height);
//	}

// H.M.Wang 2020-9-3 在BaseObject类中的draw函数会重新计算变量的宽度。如果没有这个函数，会以当前内容作为参考文本，如果是RealtimeObject。原来是按着"YY-MM-DD"计算的宽度，后来就按"20-09-03"计算了，宽度发生变化，结果会发生内容覆盖
//	追加此函数以避免
	@Override
	public String getMeatureString() {
		StringBuilder sb = new StringBuilder();
		for (BaseObject o : mSubObjs) {
			sb.append(o.getMeatureString());
		}
		return sb.toString();
	}
// End of H.M.Wang 2020-9-3 在BaseObject类中的draw函数会重新计算变量的宽度。如果没有这个函数，会以当前内容作为参考文本，如果是RealtimeObject。原来是按着"YY-MM-DD"计算的宽度，后来就按"20-09-03"计算了，宽度发生变化，结果会发生内容覆盖
//	追加此函数以避免

	// H.M.Wang 2019-9-25 该类对象重新根据高度重新设置宽度的时候，主要根据子项目的内容来设置，跟自己的内容无关
	@Override
	public void resizeByHeight() {
		float x = super.getX();

		for(BaseObject o : mSubObjs)
		{
			o.setX(x);
			o.resizeByHeight();
			x = o.getXEnd();
		}
		setWidth(x - getX());
	}

	// H.M.Wang 修改。取消原来的子元素均等加减1的缩放方法，改为均等缩放
	public void wide() {
		float x = getX();
		float ratio = (getWidth() + 5) / getWidth();
		mRatio *= ratio;

		for(BaseObject o : mSubObjs) {
			o.setX(x);
			o.setWidth(o.getWidth() * ratio);
			x = o.getXEnd();
		}
		setWidth(x - getX());
		isNeedRedraw = true;
	}
/*
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
*/
	public void narrow() {
		float x = getX();
		float ratio = (getWidth() - 5) / getWidth();
		mRatio *= ratio;

		for(BaseObject o : mSubObjs) {
			o.setX(x);
			o.setWidth(o.getWidth() * ratio);
			x = o.getXEnd();
		}
		setWidth(x - getX());
		isNeedRedraw = true;
	}
/*
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
*/
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
			Debug.d(TAG, "===>content=" + o.getContent() + ", x=" + o.getX() + ",  end=" + x);
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
		meature();
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
		Debug.d(TAG, "toString = [" + str + "]");
//		System.out.println("file string ["+str+"]");
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
		Debug.d(TAG, "++++>x: " + getX() + "   " + getXEnd() + "  width=" + bitmap.getWidth());
		for(BaseObject o : mSubObjs)
		{
			Bitmap b = o.getpreviewbmp();
			Debug.e(TAG, "============>id:" + o.mId + ", x=" + o.getX() + "  width = " + b.getWidth());
			mCan.drawBitmap(b, o.getX()-getX(), 0, mPaint);
		}
		return bitmap;
	}

}
