package com.industry.printer.object;

import java.io.File;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.industry.printer.ImageConverter;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFromBitmap;

public class GraphicObject  extends BaseObject{

	public static String TAG="GraphicObject";
	
	public Bitmap mBitmap;
	private String mImage;
	
	public GraphicObject(Context context) {
		super(context, BaseObject.OBJECT_TYPE_GRAPHIC, 0);
		// TODO Auto-generated constructor stub
	}
	
	public GraphicObject(Context context, float x) {
		super(context, BaseObject.OBJECT_TYPE_GRAPHIC, x);
		// TODO Auto-generated constructor stub
	}

	public void setImage(String file)
	{
		File f = new File(file);
		if( !f.isFile() || !isPic(file))
		{
			Debug.d(TAG, "please select a correct file");
			return;
		}
		mImage = f.getPath();
		Debug.d(TAG, "setImage file: "+file);
		mContent = f.getName();
		mBitmap = BitmapFactory.decodeFile(file);
		// mBitmap = ImageConverter.convertGreyImg(b);
		// BinFromBitmap.recyleBitmap(b);
		PrinterNozzle type = mTask.getNozzle();
		if (mBitmap != null) {
			mWidth = mBitmap.getWidth()/type.getScaleW();
			mHeight = mBitmap.getHeight()/type.getScaleH();
		}
		Debug.d(TAG, "setImage w= " + mWidth + " h= " + mHeight);
	}

	// H.M.Wang 插入图片时宽度缩减一半的问题
	@Override
	public void setHeight(String size)
	{
		float height = mTask.getMsgObject().getPixels(size);

		Debug.d(TAG, "====> setHeight: mWidth = " + mWidth + "; mHeight = " + mHeight + "; height = " + height);
		float ratio = height / mHeight;
//		setHeight(height);
//		Debug.d(TAG, "====> setHeight: oldHeight = " + oldHeight);
		setWidth(mWidth * ratio);
		setHeight(height);
		Debug.d(TAG, "====> setHeight: mWidth = " + mWidth + "; mHeight = " + mHeight);
	}
/*	H.M.Wang 撤销这个函数，改为采用上面的函数
	public void setHeight(float size) {
		float oldHeight = mHeight;
		super.setHeight(size);
		setWidth(mWidth * mHeight / oldHeight);
		Debug.d(TAG, "mWidth = " + mWidth + "; mHeight = " + mHeight);
	}
*/
	public String getImage() {
		return mContent;
	}


	// H.M.Wang 2019-9-27 追加该函数，用来在条码/二维码设置字号的时候，恢复到原始比例状态，并且根据新的高计算新的宽
	@Override
	public void resizeByHeight() {
		if(null != mBitmap) {
			mWidth = mBitmap.getWidth() * mHeight / mBitmap.getHeight();
		}
	}

	public Bitmap getScaledBitmap(Context context)
	{
		if (mBitmap != null) {
			return Bitmap.createScaledBitmap(mBitmap, (int)mWidth, (int)mHeight, false);
		}
		return mBitmap;
	}
	
	/**
	 * 
	 */
	public Bitmap makeBinBitmap(Context ctx, String content, int ctW, int ctH, String font) {
		if (mBitmap != null) {
			return Bitmap.createScaledBitmap(mBitmap, ctW, ctH, false);
		}
		return null;
	}
	
	public static String[] pic_formats={".png", ".jpg", ".jpeg", ".bmp"};
	
	public static boolean isPic(String f)
	{
		if(f==null)
			return false;
		int index = f.lastIndexOf(".");
		if(index<0 || index >= f.length())
			return false;
		String suffix = f.substring(index);
		for(String s : pic_formats)
		{
			if(s.equalsIgnoreCase(suffix))
				return true;
		}
		return false;
	}
	
	/**
	 * do some extra process after the message saved success
	 * for graphic object: copy picture files to message-own directory
	 */
	public void afterSave() {
		if (mImage == null) {
			return;
		}
		// String srcPath = Configs.CONFIG_PATH_FLASH + Configs.PICTURE_SUB_PATH + "/" + mContent;
		String dstPath = Configs.TLK_PATH_FLASH  + "/" + mTask.getName() + "/" + mContent;
		if (mImage.equalsIgnoreCase(dstPath)) {
			return;
		}
		FileUtil.copyFile(mImage, dstPath);
	}
	
	@Override
	public Bitmap getpreviewbmp() {
		Debug.d(TAG, "--->getpreviewbmp w= " + mWidth + " h= " + mHeight);
// H.M.Wang 2019-9-27 追加判断是否已经被回收
		if (mBitmap == null || mBitmap.isRecycled()) {
			getScaledBitmap(mContext);
		}
//		if (mBitmap == null) {
//			Debug.e(TAG, "--->no image");
//			return null;
//		}
		return Bitmap.createScaledBitmap(mBitmap, (int)mWidth, (int)mHeight, false);
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
			.append(BaseObject.intToFormatString(mContent.length(), 3))
			.append("^")
			.append("000^000^000^")
			.append(mContent)
			.append("^00000000^00000000^00000000^00000000^0000^0000^0000^000^000");
			
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
//		str += BaseObject.intToFormatString(mContent.length(), 3)+"^";
//		
//		str += "000^000^000^";
//		str += mContent + "^00000000^00000000^00000000^00000000^0000^0000^0000^000^000";
		String str = builder.toString();
		System.out.println("file string ["+str+"]");
		return str;
	}
}
