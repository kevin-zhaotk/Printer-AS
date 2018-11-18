package com.industry.printer.object.data;

import java.io.File;
import java.io.FileOutputStream;

import com.industry.printer.MessageTask;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.object.BaseObject;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;

public class BitmapWriter {

	public static final String TAG = BitmapWriter.class.getSimpleName();
	
	public static BitmapWriter mInstance;
	
	/*对象实际高度到图片高度的比例
	 * Bitmap高 = object mHeight * SCALE
	 */
	public static final double SCALE = 0.5;
	private static final int BITMAP_FIX_HEIGHT = 152;
	public Context mContext;
	private Paint	mPaint;
	
	
	public static BitmapWriter getInstance(Context context) {
		if (mInstance == null) {
			mInstance = new BitmapWriter(context);
		}
		return mInstance;
	}
	
	public BitmapWriter(Context context) {
		mPaint = new Paint();
		mPaint.setTextSize(BITMAP_FIX_HEIGHT);
	}
	
	public Bitmap make(BaseObject object) {
		// 暂时不支持字体
		// mPaint.setTypeface(FontCache.get(context, "fonts/"+mFont));
		int width = (int)mPaint.measureText(object.getContent());
		int height = (int)mPaint.getTextSize();
		
		Bitmap bmp = Bitmap.createBitmap(width , height, Configs.BITMAP_CONFIG);
		Canvas mCan = new Canvas(bmp);
		mCan.drawText(object.getContent(), 0, height-5, mPaint);
		return bmp;
	}
	
	public void makeVariable(BaseObject object)
	{
		int singleW; //the width value of each char
		MessageTask task = object.getTask();
		int width = (int)mPaint.measureText("8");
		int height = (int)mPaint.getTextSize();
		/*draw Bitmap of single digit*/
		Bitmap bmp = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
		Canvas can = new Canvas(bmp);
		
		/*draw 0-9 totally 10 digits Bitmap*/
		singleW = width;
		Bitmap gBmp = Bitmap.createBitmap(singleW*10, height, Configs.BITMAP_CONFIG);
		Canvas gCan = new Canvas(gBmp);
		gCan.drawColor(Color.WHITE);	/*white background*/
		for(int i =0; i<=9; i++)
		{
			/*draw background to white firstly*/
			can.drawColor(Color.WHITE);
			can.drawText(String.valueOf(i), 0, height-5, mPaint);
			gCan.drawBitmap(bmp, i*bmp.getWidth(), height - 5, mPaint);
			BinFromBitmap.recyleBitmap(bmp);
		}
		BinFromBitmap.recyleBitmap(bmp);
		BinFileMaker maker = new BinFileMaker(mContext);
		maker.save(ConfigPath.getVBinAbsolute(task.getName(), object.getIndex()));
		//
		BinFromBitmap.recyleBitmap(gBmp);
	}
	
	
	/**
     * ����bmp��ָ����path��·���µ�ָ���ļ���picName�������pathΪnull���򱣴浽Ĭ��·��
     * @param bmp
     * @param path
     * @param picName
     */
    public static void saveBitmap(Bitmap bmp, String path, String picName)
    {
        if (path == null) {
            path = Configs.USB_ROOT_PATH;
        }
        File f = new File(path, picName);
        //File f = new File("/storage/external_storage/sda1", picName);
        if(f.exists())
        {
            f.delete();
        }
        try{
            FileOutputStream out = new FileOutputStream(f);
            bmp.compress(CompressFormat.PNG, 90, out);
            out.flush();
            out.close();
            Debug.d(TAG, "PNG save ok");
        }catch(Exception e)
        {
            Debug.d(TAG, "save failed: "+e.getMessage());
        }
    }
}
