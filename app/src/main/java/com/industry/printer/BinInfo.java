package com.industry.printer;

import java.io.ByteArrayInputStream;
import java.io.CharArrayReader;
import java.io.CharArrayWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Logger;

import org.apache.http.util.ByteArrayBuffer;

import android.R.integer;
import android.R.menu;
import android.content.Context;
import android.graphics.Bitmap;

import com.industry.printer.MessageTask.MessageType;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.interceptor.ExtendInterceptor.ExtendStat;

/**
 * @author zhaotongkai
 *
 */
public class BinInfo {
	public static final String TAG="BinInfo";

	/* column extension ratio, only useful under big dot printer*/
	private static int columnExtension = 1;
	private File mFile;
	private FileInputStream mFStream;
	
	private MessageTask mTask;

	private ExtendStat mExtend;
	private boolean isExtended;

	/**bin文件的总列数**/
	public int mColumn;

	/**bin文件总长度**/
	public int mLength;
	
	/** 是否需要对列进行补偿 */
	public boolean mNeedFeed = false;
	
	/**打印头类型，单头，双头，三头和四头， 默认为单头*/
	public int mType = 1;
	
	/**每个打印头每列的字节数*/
	public int mBytesPerH;
	
	/**每个打印头每列的双字节数*/
	public int mCharsPerH;
	
	/**每个打印头每列的字节数*/
	public int mBytesPerHFeed;
	
	/**每个打印头每列的双字节数*/
	public int mCharsPerHFeed;
	
	/**bin文件每列的总字节数**/
	public int mBytesPerColumn;
	
	/**bin文件每列的总字符数**/
	public int mCharsPerColumn;
	
	/**补偿之后的列总字节数*/
	public int mBytesFeed;
	
	/**补偿之后的列总双字节数*/
	public int mCharsFeed;
	
	/**变量的每个字符所占列数**/
	public int mColPerElement;
	
	/**bin文件的字节缓存**/
	public byte[] mBufferBytes;
	
	/**bin文件的字符缓存**/
	public char[] mBufferChars;
	
	public byte[] mBuffer;
	public ByteArrayInputStream mCacheStream;

	public int mVarCount;


	public BinInfo(String file) {
		this(file, null, null);
		Debug.d(TAG, "===>binFile: " + file);
	}
	
	public BinInfo(String file, ExtendStat extension) {
		this(file, null, extension);
		Debug.d(TAG, "===>binFile: " + file);
	}

	public BinInfo(String file, MessageTask task, ExtendStat extension)
	{
		this(file, task, 10, extension);
	}
	
	public BinInfo(String file, MessageTask task, int varCount, ExtendStat extension) {
		mColumn = 0;
		mVarCount = varCount;
		mBufferBytes = null;
		mBufferChars = null;
		mTask = task;
		mType = task != null ? task.getHeads() : 1;
		mExtend = extension;
		isExtended = false;
		/**读取文件头信息**/
		
		mFile = new File(file);
		try {
			mFStream = new FileInputStream(mFile);
			mBuffer = new byte[mFStream.available()];
			mFStream.read(mBuffer);
			mFStream.close();
			Debug.d(TAG, "--->buffer.size=" + mBuffer.length);
			resolve();
		} catch (Exception e) {
			Debug.d(TAG, ""+e.getMessage());
		}
	}
	

	
	public BinInfo(InputStream stream, int type) {
		mColumn = 0;
		mBufferBytes = null;
		mBufferChars = null;
		if (type <=0 || type > 4) {
			mType = 1;
		} else {
			mType = type;
		}
		/**读取文件头信息**/
		try {
			// mFStream = new FileInputStream(mFile);
			mBuffer = new byte[stream.available()];
			stream.read(mBuffer);
			// mFStream.close();
			Debug.d(TAG, "--->buffer.size=" + mBuffer.length);
			resolve();
		} catch (Exception e) {
			Debug.d(TAG, ""+e.getMessage());
		}
	}
	
	public BinInfo(Context ctx, Bitmap bmp, ExtendStat extend) {
		mExtend = extend;
		BinFileMaker m = new BinFileMaker(ctx);
		// BinCreater.saveBitmap(bmp, "bar.png");
		m.extract(bmp, 1);
		mBuffer = m.getBuffer();
		ByteArrayBuffer buffer = new ByteArrayBuffer(0);
		byte[] header = new byte[BinCreater.RESERVED_FOR_HEADER];
		int width = bmp.getWidth();
		Debug.d(TAG, "--->width=" + width);
		header[2] = (byte) (width & 0x0ff);
    	header[1] = (byte) ((width>>8) & 0x0ff);
    	header[0] = (byte) ((width>>16) & 0x0ff);
		Debug.d(TAG, "--->header= " + header[0] + "  " + header[1] + "  " + header[2]);
    	// BinCreater.saveBin("/mnt/usbhost0/bar.bin", mBuffer, 19*8);
    	buffer.append(header, 0, header.length);
    	buffer.append(mBuffer, 0, mBuffer.length);
    	mBuffer = buffer.buffer();
    	resolve();
	}
	
	private synchronized void resolve() {
		byte[] head = new byte[BinCreater.RESERVED_FOR_HEADER];
		/*把bin文件内容读入内存*/
		mCacheStream = new ByteArrayInputStream(mBuffer);

		mCacheStream.read(head, 0, BinCreater.RESERVED_FOR_HEADER);
		mColumn =  (head[0]&0xff) << 16 | (head[1] & 0xff)<<8 | (head[2]&0xff);
		Debug.d(TAG, "--->header= " + head[0] + "  " + head[1] + "  " + head[2]);
		//bin文件总长度
		mLength = mCacheStream.available();
		Debug.d(TAG, "--->mLength=" + mLength + "  column = " + mColumn);
		
		
		//文件的总字节数/总列数 = 每列的字节数
		mBytesPerColumn = mLength/mColumn;
		Debug.d(TAG, "--->mBytesPerColumn =" + mBytesPerColumn);
		//文件的总字符数/总列数/2 = 每列的字符数
		mCharsPerColumn = mBytesPerColumn/2;
		/*如果mBytesPerColumn不是type的整数倍，说明这个bin文件不是一个合法的bin文件
		 *那么我们不会保证打印结果是否正确，所以这里不需要容错
		 */
		mBytesPerH = mBytesPerColumn/mType;
		Debug.d(TAG, "--->mBytesPerH =" + mBytesPerH + ", type=" + mType);
		mCharsPerH = mBytesPerH/2;
		/* 如果每列的字节数为奇数则 +1 变为偶数， 以便于FPGA处理*/
		if (mBytesPerH%2 != 0) {
			mNeedFeed = true;
		}

		/** 计算补偿后的字节数和双字节数 */
		if (mNeedFeed) {
			mBytesPerHFeed = mBytesPerH + 1;
			mBytesFeed = mBytesPerColumn + mType;
			Debug.d(TAG, "--->117 mBytesPerHFeed =" + mBytesPerHFeed + ", mBytesPerFeed=" + mBytesFeed);
		} else {
			mBytesPerHFeed = mBytesPerH;
			mBytesFeed = mBytesPerColumn;
			Debug.d(TAG, "--->120 mBytesPerHFeed =" + mBytesPerHFeed + ", mBytesPerFeed=" + mBytesFeed);
		}
		mCharsPerHFeed = mBytesPerHFeed/2;
		mCharsFeed = mBytesFeed/2;
		//通过文件后缀是否带有v判断是否为变量的bin文件
		if (mFile != null && mFile.getName().contains("v")) {
			if (mVarCount <= 0) {
				mVarCount = 10;
			}
			Debug.d(TAG, "===>varCount: " + mVarCount);
			mColPerElement = mColumn/mVarCount;
		} else {
			mColPerElement = 0;
		}


	}

	private boolean isVarBuffer() {
		if(mFile != null && mFile.getName().contains("v")) {
			return true;
		} else {
			return false;
		}
	}
	
	/**
	 * extract real print buffer from 1.bin file
	 * 1.bin is a 600dpi resolution based buffer, each kind of print-header should extract print buffer   
	 */
	private synchronized char[] extract() {
		int hType = mTask == null ? MessageType.MESSAGE_TYPE_12_7_S : mTask.getHeadType();
		return mBufferChars;
//		if (hType == MessageType.MESSAGE_TYPE_12_7_S)	{
//    		char[] rBuffer = new char[mBufferChars.length/4];
//    		for (int j = 0; j < rBuffer.length; j++) {
//    			int curRow = j / mCharsPerHFeed;
//				rBuffer[j] = mBufferChars[4*curRow*mCharsPerHFeed + j%mCharsPerHFeed];
//			}
//    		Debug.d(TAG, "--->rBuffer.length: " + rBuffer.length);
//    		return rBuffer;
//    	} else {
//    		return mBufferChars;
//    	}
		
	}
	public int getCharsPerColumn() {
		return mCharsPerColumn;
	}
	
	public int getBytesPerColumn() {
		return mBytesPerColumn;
	}
	
	public int getBytesFeed() {
		return mBytesFeed;
	}
	
	public int getCharsFeed() {
		return mCharsFeed;
	}
	
	public int getBytesPerH() {
		return mBytesPerH;
	}
	
	public int getBytesPerHFeed() {
		return mBytesPerHFeed;
	}
	
	public int getCharsPerH() {
		return mCharsPerH;
	}
	
	public int getCharsPerHFeed() {
		return mCharsPerHFeed;
	}
	
    public synchronized char[] getBgBuffer()
    {
    	if (mLength <= 0) {
			return null;
		}
    	
		try {
			/*计算整个buffer需要补偿的字节数*/
			int feed = (mNeedFeed==true?mColumn*mType : 0);

			mBufferBytes = new byte[mLength + feed];
			mBufferChars = new char[(mLength + feed)/2];
			if(mBufferBytes == null || mBufferChars == null) {
				Debug.e(TAG, "--->null");
				return null;
			}
				
			// int bytesPer = mBytesPerColumn + (mNeedFeed==true? mType : 0);
			/** 从当前位置读取mBytesPerH个字节到背景buffer中，由于需要处理多头情况，所以要注意在每个头的列尾要注意补偿问题（双字节对齐）*/
			for(int i=0; i < mColumn; i++) {
				for (int j = 0; j < mType; j++) {
					mCacheStream.read(mBufferBytes, i*mBytesFeed + j*mBytesPerHFeed, mBytesPerH);
				}
			}
	    	//mFStream.close();
			/* 如果是奇数列在每列最后添加一个byte */
			
	    	//把byte[]存为char[]
	    	for(int i = 0; i < mBufferChars.length; i++) {
	    		mBufferChars[i] = (char) (((char)(mBufferBytes[2*i+1] << 8) & 0x0ff00) | (mBufferBytes[2*i] & 0x0ff)); 
	    	}
	    	// 根据喷头类型抽取实际的打印buffer
	    	
		} catch (Exception e) {
			Debug.d(TAG, "--->e: " + e.getMessage());
		} finally {
			
		}
		
		// deal with extension, 1 to 2,4,6,8; 2 to 4, 6, 8;
		// row extension
		expend();

		// column extension
//		expendColumn(8);

    	return extract(); // bmp.createScaledBitmap(bmp, columns, 150, true);
    }
    
    public synchronized char[] getVarBuffer(String var)
    {
    	int n;
    	byte[] feed = {0};

    	ByteArrayBuffer ba = new ByteArrayBuffer(0);
   		for(int i=0; i<var.length(); i++)
   		{
   			String v = var.substring(i, i+1);
   			try {
   				n = Integer.parseInt(v);
			} catch (Exception e) {
				n = (int)v.charAt(0) - (int)"A".charAt(0);
			}
   			
   			Debug.d(TAG, "===>mColPerElement:" + mColPerElement + ", mBytesPerH=" + mBytesPerH + ", type=" + mType);
   			/* 如果每列的字节数为单数，则需要在每列尾部补齐一个字节 */
   			for (int k = 0; k < mColPerElement; k++) {
   				for (int j = 0; j < mType; j++) {
   	   				ba.append(mBuffer, n*mColPerElement * mBytesPerColumn + 16 + k * mBytesPerColumn + j * mBytesPerH, mBytesPerH);
   	   	   			if (mNeedFeed) {
   	   					ba.append(feed, 0, 1);
   	   				}
   	   	   			// Debug.d(TAG, "===>offset:" + (n*mColPerElement * mBytesPerColumn + 16 + k * mBytesFeed + j * mBytesPerH) + " ,mBytesPerH=" + mBytesPerH);
   				}
			}
   		}
   		mBufferBytes = ba.toByteArray();
   		/*
   		for (int i = 0; i < mBufferBytes.length; i++) {
			if ( i!=0 && i%16 == 0) {
				System.out.println();
			}
			System.out.print(mBufferBytes[i]);
			System.out.print("  ");
		}*/
   		//把byte[]存为char[]
   		if (mBufferChars == null) {
   			mBufferChars = new char[mBufferBytes.length/2];
   		}
   		if (mExtend != null && mExtend.getScale() > 1) {
			mBufferChars = new char[mBufferBytes.length/2];
		}
   		Debug.d(TAG, "--->char len = " + mBufferChars.length + "   bytes: " + mBufferBytes.length);
    	for(int i = 0; i < mBufferChars.length; i++) {
    		mBufferChars[i] = (char) ((char)((mBufferBytes[2*i+1] << 8) & 0x0ff00) | (mBufferBytes[2*i] & 0x0ff)); 
    	}

    	// row extension
   		expend();

    	// column extension
//    	expendColumn(8);
    	return extract();
    }
    
    /*班次變量特殊處理，生成v.bin時固定爲兩位有效位，如果shift的bit爲1，那前面補0，
	 *所以，shift變量的v.bin固定爲8位，如果bit=1，需要跳過前面的0*/
    public synchronized char[] getVarBuffer(int shift, int bits)
    {
    	int n, offset=0;
    	byte[] feed = {0};
    	if (shift * bits > 9) {
			return null;
		}
    	if (bits == 1) {
    		offset = 2 * shift +1;
    	} else {
    		offset = bits * shift;
    	}
    	ByteArrayBuffer ba = new ByteArrayBuffer(0);
		/* 如果每列的字节数为单数，则需要在每列尾部补齐一个字节 */
		for (int k = 0; k < bits * mColPerElement; k++) {
			for (int j = 0; j < mType; j++) {
   				ba.append(mBuffer, offset * mColPerElement * mBytesPerColumn + 16 + k * mBytesPerColumn + j * mBytesPerH, mBytesPerH);
   	   			if (mNeedFeed) {
   					ba.append(feed, 0, 1);
   				}
   	   			// Debug.d(TAG, "===>offset:" + (n*mColPerElement * mBytesPerColumn + 16 + k * mBytesFeed + j * mBytesPerH) + " ,mBytesPerH=" + mBytesPerH);
			}
		}
   		mBufferBytes = ba.toByteArray();
   		//把byte[]存为char[]
   		if (mBufferChars == null) {
   			mBufferChars = new char[mBufferBytes.length/2];
   		}
    	for(int i = 0; i < mBufferChars.length; i++) {
    		mBufferChars[i] = (char) ((char)((mBufferBytes[2*i+1] << 8) & 0x0ff00) | (mBufferBytes[2*i] & 0x0ff)); 
    	}

    	// row entension
    	expend();

   		// column extension
//   		expendColumn(8);
    	return extract();
    }
    
    
    public static void overlap(byte[] dst, byte[] src, int x, int high)
    {
    	int len = src.length;
		// column extension, adjust the x coordinate
		x = x * columnExtension;
    	if(dst.length < x*high +src.length)
    	{
    		Debug.d(TAG, "dst buffer no enough space!!!!");
    		len = dst.length - x*high;
    		//return;
    	}
    	for(int i = 0; i < len; i++)
    	{
    		dst[x*high+i] |= src[i];
    	}
    }
    /**
     * 疊加方式
     * @param dst
     * @param src
     * @param x
     * @param high
     */
    public static void overlap(char[] dst, char[] src, int x, int high)
    {
    	int len = src.length;
		// column extension, adjust the x coordinate
		x = x * columnExtension;

    	if(dst.length < x*high + src.length)
    	{
    		Debug.d(TAG, "dst buffer no enough space!!!! dst.len=" + dst.length + " , src=" + src.length + " , pos=" + x*high);
    		len = dst.length - x*high;
    		//return;
    	}
    	Debug.d(TAG, "--->high = " + high);
    	int  matrix = PlatformInfo.isBufferFromDotMatrix();
    	for(int i=0; i< len; i++)
    	{
    		if (x*high + i < 0) {
				continue;
			}
    		if (matrix==1) {
    			dst[x*high + i] = src[i];
    		} else {
    			dst[x*high + i] |= src[i];
    		}
    	}
    }
    
    /**
     * 覆蓋方式
     * @param dst
     * @param src
     * @param x
     * @param high
     */
    public static void cover(char[] dst, char[] src, int x, int high) {
    	int len = src.length;
		// column extension, adjust the x coordinate
		x = x * columnExtension;
    	Debug.d(TAG, "--->cover: " + dst.length + "  " + x + "  " + high + "  " + src.length);
    	if(dst.length < x*high + src.length)
    	{
    		Debug.d(TAG, "dst buffer no enough space!!!! dst.len=" + dst.length + " , src=" + src.length + " , pos=" + x*high);
    		len = dst.length - x*high;
    		//return;
    	}
    	for(int i=0; i< len; i++)
    	{
    		if (x*high + i < 0) {
				continue;
			}
    		dst[x*high + i] = src[i];
    	}
    }
    /**
     * 该函数用于对打印buffer进行字节变换，生成880设备的打印buffer
     * 顺序是byte0+ Byte55, byte1+byte56
     * @param buffer 待转换的buffer
     */
    public static void Matrix880(byte[] buffer){
    	byte[] tmp= new byte[110];
    	Debug.d(TAG, "===>Matrix880 : buffer len:"+buffer.length);
    	for(int i=0; i< buffer.length/(Configs.gDots/8); i++){
    		for(int j=0; j<Configs.gDots/(2*8); j++){
    			tmp[2*j] = buffer[i*(Configs.gDots/8)+j];
    			tmp[2*j+1] = buffer[i*(Configs.gDots/8)+j+55]; 
    		}
    		for(int j=0; j<Configs.gDots/8; j++){
    			buffer[i*(Configs.gDots/8)+j] = tmp[j];
    		}
    	}
    }
    
    /**
     * 该函数用于将列变换得到的打印buffer进行逆变换，从而得到预览buffer
     * @param buffer 打印buffer
     */
    public static void Matrix880Revert(byte[] buffer) {
    	byte[] tmp= new byte[110];
    	Debug.d(TAG, "===>Matrix880Revert : buffer len:"+buffer.length);
    	for(int i=0; i< buffer.length/(Configs.gDots/8); i++){
    		for(int j=0; j<Configs.gDots/(2*8); j++){
    			tmp[j] = buffer[i*(Configs.gDots/8)+2*j];
    			tmp[j+55] = buffer[i*(Configs.gDots/8)+2*j+1]; 
    		}
    		for(int j=0; j<Configs.gDots/8; j++){
    			buffer[i*(Configs.gDots/8)+j] = tmp[j];
    		}
    	}
    }


	/**
	 * 1 to multi function
	 */
	private void expend() {
    	if (mExtend == null) {
    		return;
		}
		int scale = mExtend.getScale();
    	if (scale == 1) {
    		return;
		}
    	// BinCreater.saveBin("/mnt/sdcard/print_var.bin", mBufferChars, mCharsPerHFeed * 16);
    	Debug.d(TAG, "--->expend scale : " + scale + " length: " + mBufferChars.length);
		char[] buffer = mBufferChars;
    	mBufferChars = new char[buffer.length * scale];
    	
		CharArrayReader reader = new CharArrayReader(buffer);

		int column = mColumn;
		if (isVarBuffer()) {
			column = buffer.length/mCharsFeed;
		}
		try {
			
			for (int j = 0; j < scale; j++) {
				reader.mark(0);
				Debug.d(TAG, "--->j = " + j + "  column = " + column);
				for (int i = 0; i < column; i++) {
					reader.read(mBufferChars, (i*scale + j) * mCharsPerHFeed, mCharsPerHFeed);
				}
				reader.reset();
			}
			
		} catch (Exception e) {
			Debug.d(TAG, "--->e: " + e.getMessage());
		}
		if (!isVarBuffer()) {
    		mBytesPerColumn *= scale;
    		mCharsPerColumn *= scale;
    		mBytesFeed *= scale;
    		mCharsFeed *= scale;
    		mCharsPerHFeed *= scale;
    		mBytesPerHFeed *= scale;
		}
//		BinCreater.saveBin("/mnt/sdcard/print_bg1.bin", mBufferChars, mCharsPerHFeed * scale * 16);
	}


	/**
	 * expend along horizontal space, 1 column to 2/3 or any columns
	 * big dot machine
	 * extend buffer to 8 times filled with 0
	 */
	public void expendColumn(int extension) {

		if (mTask == null || mTask.getNozzle() == null) {
			return;
		}
		if (!mTask.getNozzle().buffer8Enable || !Configs.BUFFER_8) {
			columnExtension = 1;
			return;
		}
		CharArrayWriter writer = new CharArrayWriter();

		for (int i = 0; i < mColumn; i++) {
			writer.write(mBufferChars, mCharsFeed * i, mCharsFeed);
			for (int j = 0; j < mCharsFeed * (extension - 1); j++) {
				writer.write(0x00);
			}
		}
		mColumn = mColumn * extension;
		columnExtension = extension;
		mBufferChars = writer.toCharArray();
	}
}
