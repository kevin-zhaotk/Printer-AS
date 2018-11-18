package com.industry.printer.data;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import org.apache.http.util.ByteArrayBuffer;

import com.industry.printer.R;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Bitmap.Config;
import android.widget.Toast;

public class BinCreater {
	
	public static final String TAG="BinCreater";
	
	//public static final String FILE_PATH="/storage/external_storage/sda1";
	public static final int RESERVED_FOR_HEADER=16;
	
	
	public int mBmpBytes[];
	public byte mBinBits[];
	
	/**
	 * buffer中每一列的高度（每一列的点阵数，即行数）, mWidth:点阵buffer的宽度（即列数）
	 */
	public int mHeight;
	public int mWidth;
	
	public int mHeighEachHead;
	
	public BinCreater() {
		mHeight = 0;
		mWidth = 0;
	}
	
	public int[] extract(Bitmap bitmap, int head) {
		Debug.e(TAG, "--->please Override this Method in Child class");
		return null;
	}
	
	public int extract(String text) {
		Debug.e(TAG, "--->please Override this Method in Child class");
		return 0;
	}
    /*
     * height - pixes per column
     */
    public void swap()
    {
    	byte tmp;
    	if (mBinBits == null || mBinBits.length <= 0) {
    		return;
		}
    	int bytes = mHeight%8==0?mHeight/8:mHeight/8+1;
    	int width = mBinBits.length/bytes;
    	for(int i=0;i<mBinBits.length/2; i++)
		{
			tmp = mBinBits[2*i+1];
    		mBinBits[2*i+1] = mBinBits[2*i];
    		mBinBits[2*i] = tmp;
		}
    	
    }

    
    public void saveBytes(int[] map, String fileName)
    {
    	try{
    		//File file = new File("/storage/external_storage/sda1", "2.bin");
        	File file = new File(Configs.USB_ROOT_PATH, fileName);
    		FileOutputStream fs = new FileOutputStream(file);
    		//ByteArrayOutputStream barr = new ByteArrayOutputStream();
    		for(int i=0; i< map.length; i++)
    			fs.write(map[i]);
    		fs.flush();
    		fs.close();
    	}catch(Exception e)
    	{
    		Debug.d(TAG, "Exception: "+e.getMessage());
    		//return false;
    	}
    	//return true;
        
	}
    
    
    public boolean byte2bit(int height)
    {
    	if (mBmpBytes == null) {
    		return false;
		}
    	//int width= mBmpBytes.length/880;
    	int rows=mBmpBytes.length/height;
    	if(mBmpBytes == null || mBinBits==null || mBinBits.length < mBmpBytes.length/8)
    	{
    		Debug.d(TAG, "There is no enough space for store bmpbits");
    		return false;
    	}
    	Debug.d(TAG, "rows="+rows+",cols="+height);
    	//int bytesPercol = height%8==0 ? height/8 : (height/8+1);
    	mBinBits[2] = (byte) (rows & 0x0ff);
    	mBinBits[1] = (byte) ((rows>>8) & 0x0ff);
    	mBinBits[0] = (byte) ((rows>>16) & 0x0ff);
    	
    	
    	/*
    	mBinBits[8] = (byte) (colEach & 0x0ff);
    	mBinBits[7] = (byte) ((colEach>>8) & 0x0ff);
    	mBinBits[6] = (byte) ((colEach>>16) & 0x0ff);
    	*/
    	for(int k=0; k<rows; k++)
    	{
    		for(int i=0; i<height; i++)
    		{
    			//Debug.d(TAG, "mBmpBytes["+(i*width+k)+"]=0x"+Integer.toHexString( mBmpBytes[i*width+k]));
        			if((mBmpBytes[i*rows+k] & 0x00ffffff) != 0)
        			{
        				mBinBits[(k*height)/8 +i/8+RESERVED_FOR_HEADER] |= 0x01<<(i%8);
        			}
        			else
        			{
        				mBinBits[(k*height)/8 +i/8+RESERVED_FOR_HEADER] &= ~(0x01<<(i%8));
        			}
        			if(k<3)
        				Debug.d(TAG, "mBytes["+(i*rows+k)+"] <--> "+"mBinBits["+((k*height)/8 +i/8+RESERVED_FOR_HEADER)+"]["+(i%8)+"]");	
    		}
    	}
    	return true;
    }
    
    
    public boolean saveBin(String f, int width)
    {
    	byte head[]=new byte[16];
    	Debug.d(TAG, "+++++++++++++saveBin");
    	Debug.d(TAG, "saveBin f="+f+", width="+width);
    	/*save column-width*/
    	head[2] = (byte) (width & 0x0ff);
    	head[1] = (byte) ((width>>8) & 0x0ff);
    	head[0] = (byte) ((width>>16) & 0x0ff);
    	
    	try{
    		File file = new File(f);
    		FileOutputStream fs = new FileOutputStream(f);
    		ByteArrayOutputStream barr = new ByteArrayOutputStream();
    		barr.write(head);
    		barr.write(mBinBits,0,mBinBits.length);
    		barr.writeTo(fs);
    		fs.flush();
    		fs.close();
    		barr.close();
    	}catch(Exception e)
    	{
    		Debug.d(TAG, "Exception: "+e.getMessage());
    		return false;
    	}
    	Debug.d(TAG, "+++++++++++++saveBin");
    	return true;
    }
    
    /**
     * 保存bin文件
     * @param f bin文件路径
     * @param width bin文件总列数，单位byte
     * @param single bin文件单列高度，单位bit
     */
    public boolean saveBin(String f, int width, int single)
    {
    	byte head[]=new byte[16];
    	
    	/*save column-width*/
    	head[2] = (byte) (width & 0x0ff);
    	head[1] = (byte) ((width>>8) & 0x0ff);
    	head[0] = (byte) ((width>>16) & 0x0ff);
    	    	
    	/*save width of single element*/
    	head[5] = (byte) (single & 0x0ff);
    	head[4] = (byte) ((single>>8) & 0x0ff);
    	head[3] = (byte) ((single>>16) & 0x0ff);
    	
    	Debug.d(TAG, "+++++++++++++saveBin Var");
    	Debug.d(TAG, "saveBin f="+f+", width="+width+" ,single="+single);
    	try{
    		File file = new File(f);
    		FileOutputStream fs = new FileOutputStream(f);
    		ByteArrayOutputStream barr = new ByteArrayOutputStream();
    		barr.write(head);
    		barr.write(mBinBits,0,mBinBits.length);
    		barr.writeTo(fs);
    		fs.flush();
    		fs.close();
    		barr.close();
    	}catch(Exception e)
    	{
    		Debug.d(TAG, "Exception: "+e.getMessage());
    		return false;
    	}
    	Debug.d(TAG, "+++++++++++++saveBin var");
    	return true;
    }
    
    /**
     * 保存bin文件
     * @param f bin文件存放路径
     * @param dots 点阵buffer
     * @param single bin文件单列高度，单位bit
     * @return 保存成功返回true，保存失败返回false
     */
    public static boolean saveBin(String f, byte[] dots, int single) {
    	int bytesPerCol = 0;
    	bytesPerCol = single%8==0? single/8 : (single/8+1);
    	int columns = dots.length/bytesPerCol;
    	Debug.d(TAG, "--->saveBin columns:" + columns);
    	try {
    		
    		File file = new File(f);
    		Debug.d(TAG, "--->saveBin f:" + file.getAbsoluteFile());
    		if (!file.exists() && !file.createNewFile()) {
				Debug.d(TAG, "===>error: create bin file failed");
				return false;
			}
			FileOutputStream stream = new FileOutputStream(file);
			byte head[]=new byte[16];
	    	head[2] = (byte) (columns & 0x0ff);
	    	head[1] = (byte) ((columns>>8) & 0x0ff);
	    	head[0] = (byte) ((columns>>16) & 0x0ff);
	    	
	    	/*save width of single element*/
	    	head[5] = (byte) (single & 0x0ff);
	    	head[4] = (byte) ((single>>8) & 0x0ff);
	    	head[3] = (byte) ((single>>16) & 0x0ff);
	    	stream.write(head);
			stream.write(dots);
			stream.flush();
			stream.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			Debug.d(TAG, "===>saveBin err:"+e.getMessage());
			return false;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Debug.d(TAG, "===>saveBin err:"+e.getMessage());
			return false;
		}
    	return true;
    }
    
    /**
     * 保存bin文件
     * @param f bin文件存放路径
     * @param dots 点阵buffer
     * @param single bin文件单列高度，单位bit
     * @return 保存成功返回true，保存失败返回false
     */
    public static boolean saveBin(String f, char[] c_dots, int single) {
    	int bytesPerCol = 0;
    	bytesPerCol = single%8==0? single/8 : (single/8+1);
    	byte[] dots = new byte[c_dots.length * 2];
    	for(int i=0; i<c_dots.length; i++ ) {
    		dots[2*i] = (byte) (c_dots[i] & 0x0ff);
    		dots[2*i +1] = (byte) ((c_dots[i] >> 8) & 0x0ff);
    	}
    	int columns = dots.length/bytesPerCol;
    	Debug.d(TAG, "--->saveBin columns:" + columns + "   bytesPerCol=" + bytesPerCol + "  dot.len= " + dots.length);
    	try {
    		
    		File file = new File(f);
    		if (file.exists()) {
				file.delete();
			}
    		Debug.d(TAG, "--->saveBin f:" + file.getAbsoluteFile());
    		if (!file.exists() && !file.createNewFile()) {
				Debug.d(TAG, "===>error: create bin file failed");
				return false;
			}
			FileOutputStream stream = new FileOutputStream(file);
			byte head[]=new byte[16];
	    	head[2] = (byte) (columns & 0x0ff);
	    	head[1] = (byte) ((columns>>8) & 0x0ff);
	    	head[0] = (byte) ((columns>>16) & 0x0ff);
	    	
	    	/*save width of single element*/
	    	head[5] = (byte) (single & 0x0ff);
	    	head[4] = (byte) ((single>>8) & 0x0ff);
	    	head[3] = (byte) ((single>>16) & 0x0ff);
	    	stream.write(head);
			stream.write(dots);
			stream.flush();
			stream.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			Debug.d(TAG, "===>saveBin err:"+e.getMessage());
			return false;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Debug.d(TAG, "===>saveBin err:"+e.getMessage());
			return false;
		}
    	return true;
    }
    
    
    public boolean saveBin(String f) {
    	int bytesPerCol = 0;
    	bytesPerCol = mHeight%8==0? mHeight/8 : (mHeight/8+1);
    	int columns = mBinBits.length/bytesPerCol;
    	try {
    		
    		File file = new File(f);
    		Debug.d(TAG, "--->saveBin f:" + file.getAbsoluteFile());
    		if (!file.exists() && !file.createNewFile()) {
				Debug.d(TAG, "===>error: create bin file failed");
				return false;
			}
			FileOutputStream stream = new FileOutputStream(file);
			byte head[]=new byte[16];
	    	head[2] = (byte) (columns & 0x0ff);
	    	head[1] = (byte) ((columns>>8) & 0x0ff);
	    	head[0] = (byte) ((columns>>16) & 0x0ff);
	    	
	    	/*save width of single element*/
	    	head[5] = (byte) (mHeight & 0x0ff);
	    	head[4] = (byte) ((mHeight>>8) & 0x0ff);
	    	head[3] = (byte) ((mHeight>>16) & 0x0ff);
	    	stream.write(head);
			stream.write(mBinBits);
			stream.flush();
			stream.close();
		} catch (FileNotFoundException e) {
			Debug.e(TAG, "===>saveBin err:"+e.getMessage());
			return false;
		} catch (IOException e) {
			Debug.e(TAG, "===>saveBin err:"+e.getMessage());
			return false;
		}
    	return true;
    }
    

}
