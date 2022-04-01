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

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.NativeGraphicJni;
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
	// H.M.Wang 2019-12-20 修改为浮点数
	public float mColPerElement;
	
	/**bin文件的字节缓存**/
	public byte[] mBufferBytes;
	
	/**bin文件的字符缓存**/
	public char[] mBufferChars;
	
	public byte[] mBuffer;
	public ByteArrayInputStream mCacheStream;

	public int mVarCount;
	private byte[] feed;


	public BinInfo(String file) {
		this(file, null, null);
		///./...Debug.d(TAG, "===>binFile: " + file);
	}
	
	public BinInfo(String file, ExtendStat extension) {
		this(file, null, extension);
		///./...Debug.d(TAG, "===>binFile: " + file);
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
			///./...Debug.d(TAG, "--->buffer.size=" + mBuffer.length);
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
			///./...Debug.d(TAG, "--->buffer.size=" + mBuffer.length);
			resolve();
		} catch (Exception e) {
			Debug.d(TAG, ""+e.getMessage());
		}
	}
	
	public BinInfo(Context ctx, Bitmap bmp, int type, ExtendStat extend) {
// H.M.Wang 2022-4-1 追加mType的参数设置，该参数从函数的参数中传递，否则，生成多头数据会出现乱码
		if (type <=0 || type > 4) {
			mType = 1;
		} else {
			mType = type;
		}
// End of H.M.Wang 2022-4-1 追加mType的参数设置，该参数从函数的参数中传递，否则，生成多头数据会出现乱码

		mExtend = extend;
		BinFileMaker m = new BinFileMaker(ctx);
		// BinCreater.saveBitmap(bmp, "bar.png");

		// H.M.Wang 追加一个是否移位的参数
// H.M.Wang 2022-4-1 根据头数设置参数，原来的固定为1是错误的
//		m.extract(bmp, 1, false);
		m.extract(bmp, mType, false);
// End of H.M.Wang 2022-4-1 根据头数设置参数，原来的固定为1是错误的

		mBuffer = m.getBuffer();
		ByteArrayBuffer buffer = new ByteArrayBuffer(0);
		byte[] header = new byte[BinCreater.RESERVED_FOR_HEADER];
		int width = bmp.getWidth();
		///./...Debug.d(TAG, "--->width=" + width);
		header[2] = (byte) (width & 0x0ff);
    	header[1] = (byte) ((width>>8) & 0x0ff);
    	header[0] = (byte) ((width>>16) & 0x0ff);
		///./...Debug.d(TAG, "--->header= " + header[0] + "  " + header[1] + "  " + header[2]);
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
		///./...Debug.d(TAG, "--->header= " + head[0] + "  " + head[1] + "  " + head[2]);
		//bin文件总长度
		mLength = mCacheStream.available();
		///./...Debug.d(TAG, "--->mLength=" + mLength + "  column = " + mColumn);
		
		
		//文件的总字节数/总列数 = 每列的字节数
		mBytesPerColumn = mLength/mColumn;
		///./...Debug.d(TAG, "--->mBytesPerColumn =" + mBytesPerColumn);
		//文件的总字符数/总列数/2 = 每列的字符数
		mCharsPerColumn = mBytesPerColumn/2;
		/*如果mBytesPerColumn不是type的整数倍，说明这个bin文件不是一个合法的bin文件
		 *那么我们不会保证打印结果是否正确，所以这里不需要容错
		 */

		mBytesPerH = mBytesPerColumn/mType;

		///./...Debug.d(TAG, "--->mBytesPerH =" + mBytesPerH + ", type=" + mType);
		mCharsPerH = mBytesPerH/2;
		/* 如果每列的字节数为奇数则 +1 变为偶数， 以便于FPGA处理*/
		if (mBytesPerH%2 != 0) {
			mNeedFeed = true;
		}

		/** 计算补偿后的字节数和双字节数 */
		if (mNeedFeed) {
			mBytesPerHFeed = mBytesPerH + 1;
			mBytesFeed = mBytesPerColumn + mType;
			///./...Debug.d(TAG, "--->117 mBytesPerHFeed =" + mBytesPerHFeed + ", mBytesPerFeed=" + mBytesFeed);
		} else {
			mBytesPerHFeed = mBytesPerH;
			mBytesFeed = mBytesPerColumn;
			///./...Debug.d(TAG, "--->120 mBytesPerHFeed =" + mBytesPerHFeed + ", mBytesPerFeed=" + mBytesFeed);
		}
		mCharsPerHFeed = mBytesPerHFeed/2;
		mCharsFeed = mBytesFeed/2;

		//通过文件后缀是否带有v判断是否为变量的bin文件
		if (mFile != null && mFile.getName().contains("v")) {
			if (mVarCount <= 0) {
				mVarCount = 10;
			}
			///./...Debug.d(TAG, "===>varCount: " + mVarCount);
			// H.M.Wang 2019-12-20 修改为浮点数
			mColPerElement = 1.0f * mColumn/mVarCount;
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
		return mBufferChars;
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

			// 由于原来在填充打印缓冲区的背景区时，会对25.4xn类型的打印头进行切割和移位操作，这个操作从2019-8-21日期转为保存1.bin时进行，因此这里的切割移位功能取消，JNI作了相应的修改
			// H.M.Wang 注释掉JAVA填充背景图的实现部分，转到JNI实现
//			Debug.d(TAG, "getBgBuffer Start = " + System.currentTimeMillis());

			byte[] bufferBytes = new byte[mLength];
			mCacheStream.read(bufferBytes, 0, mLength);

			mBufferChars = NativeGraphicJni.GetBgBuffer(
				bufferBytes,
				mLength + feed,
				mBytesFeed,
				mBytesPerHFeed,
				mBytesPerH,
/*				( null != mTask &&
				 (mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
				  mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
				  mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
				  mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR)),*/
				mColumn,
				mType
			);
/*
			mBufferBytes = new byte[mLength + feed];
			mBufferChars = new char[(mLength + feed)/2];
			if(mBufferBytes == null || mBufferChars == null) {
				Debug.e(TAG, "--->null");
				return null;
			}

			// H.M.Wang 追加下列9行。为了25.4xn喷头调整实际的喷头字节数
			int bytesPerH = mBytesPerH;

			if( null != mTask &&
			   (mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
				mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
				mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
				mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR) ) {
				bytesPerH -= 2;
			}

			// int bytesPer = mBytesPerColumn + (mNeedFeed==true? mType : 0);
			// 从当前位置读取mBytesPerH个字节到背景buffer中，由于需要处理多头情况，所以要注意在每个头的列尾要注意补偿问题（双字节对齐
			for(int i=0; i < mColumn; i++) {
//                Debug.d(TAG, "i = " + i+ "; mColumn = " + mColumn);
				byte tempByte = 0x00, bufferBypte = 0x00;
				int skipBytes = 0;
				for (int j = 0; j < mType; j++) {
//                    Debug.d(TAG, "j = " + j+ "; mType = " + mType);
//					mCacheStream.read(mBufferBytes, i*mBytesFeed + j*mBytesPerHFeed, mBytesPerH);

					//	每个25.4的头打印像素位308点，需要38.5个字节，为了处理最后的半个字节，设置缺省一次读取38个字节，304位，但鉴于这种请开给你
					//	需要对第一和第三头多取一个字节，将最后一个字节的前4位留用，后4位用在第二和第四头使用，放置在其数据的前部，因此第二和第四头
					//	的每个字节均需要进行4位的移位处理。因此，对于每一列后部需要跳过的空白区，第一和第三头仅需要跳过1个字节，第二和第四头需要跳
					//	过两个字节，整体根据头的数量，跳过上述字节的合计数量字节数

					// H.M.Wang 追加下列27行。为了25.4xn喷头调整实际的喷头字节数
					if( null != mTask &&
					   (mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
						mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
						mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
						mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR) ) {
						if(j%2 == 0) {
							mCacheStream.read(mBufferBytes, i*mBytesFeed + j*mBytesPerHFeed, bytesPerH + 1);
							tempByte = mBufferBytes[i*mBytesFeed + j*mBytesPerHFeed + bytesPerH];
							mBufferBytes[i*mBytesFeed + j*mBytesPerHFeed + bytesPerH] &= 0x0f;
							tempByte >>= 4;
							tempByte &= 0x0f;
//							Debug.d(TAG, "tempByte = " + tempByte);
							skipBytes += 1;
						} else {
							mCacheStream.read(mBufferBytes, i*mBytesFeed + j*mBytesPerHFeed, bytesPerH);
							for(int k=0; k<bytesPerH; k++) {
								bufferBypte = mBufferBytes[i*mBytesFeed + j*mBytesPerHFeed + k];
								tempByte |= (byte)((bufferBypte << 4) & 0xf0);
								mBufferBytes[i*mBytesFeed + j*mBytesPerHFeed + k] = tempByte;
								tempByte = (byte)((bufferBypte >> 4) & 0x0f);
							}
							mBufferBytes[i*mBytesFeed + j*mBytesPerHFeed + bytesPerH] = tempByte;
							skipBytes += 2;
						}
					} else {
						mCacheStream.read(mBufferBytes, i*mBytesFeed + j*mBytesPerHFeed, bytesPerH);
					}
				}
				// H.M.Wang 追加下列7行。为了25.4xn喷头调整实际的喷头字节数
				if( null != mTask &&
				   (mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
					mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
					mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
					mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR) ) {
					mCacheStream.skip(skipBytes);
				}
			}
	    	//mFStream.close();
			// 如果是奇数列在每列最后添加一个byte
			
	    	//把byte[]存为char[]
	    	for(int i = 0; i < mBufferChars.length; i++) {
	    		mBufferChars[i] = (char) (((char)(mBufferBytes[2*i+1] << 8) & 0x0ff00) | (mBufferBytes[2*i] & 0x0ff)); 
	    	}
			Debug.d(TAG, "getBgBuffer End = " + System.currentTimeMillis());
*/
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

    // H.M.Wang 2019-12-4 取消索引表查表方式
    //// H.M.Wang 2019-10-27 追加串口来的数据索要检索的字符索引表
	//private static byte[] SERRIAL_CODES = new byte[] {
	//	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	//	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
	//	0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
	//	0x55, 0x56, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	//	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
	//	0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
	//	0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
	//	0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	//	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	//	0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63,
	//	0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
	//	0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	//	0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E
	//};
	// End --------------------------------------------------

    public synchronized char[] getVarBuffer(String var, boolean flagClearZero, boolean asciiIndex)
    {
    	int n;
		byte[] feed = {0};

		///./...Debug.d(TAG, "===>String: " + var);

		// H.M.Wang 追加下列11行。为计数器清楚前置0
		byte[] blank = new byte[mBytesPerH];
		for(int i=0; i<mBytesPerH; i++) {
			blank[i] = 0;
		}
		boolean bFillBlank = false;
		boolean bSkipThisZero = false;
		boolean bClearZero = false;
		SystemConfigFile mConfigFile = SystemConfigFile.getInstance();
		if(null != mConfigFile && mConfigFile.getParam(SystemConfigFile.INDEX_CLEAR_ZERO) == 1) {
			bClearZero = true;
		}

		ByteArrayBuffer ba = new ByteArrayBuffer(0);

		///./...Debug.d(TAG, "===>mColPerElement:" + mColPerElement + ", mBytesPerH=" + mBytesPerH + ", type=" + mType);

   		for(int i=0; i<var.length(); i++)
   		{
   			String v = var.substring(i, i+1);

			// H.M.Wang 2019-12-4 如果计数器变量为普通变量则按原来的处理方式处理，如果是从串口或者网络来的数据则直接以ASCII码作为索引
			if(asciiIndex) {
				byte[] code = v.getBytes();
				n = code[0];
			} else {
				try {
					n = Integer.parseInt(v);

					// H.M.Wang 追加下列6行。为计数器清除前置0
					if (bClearZero && n == 0 && !bSkipThisZero && i < var.length() - 1 && flagClearZero) {
						bFillBlank = true;
					} else {
						bFillBlank = false;
						bSkipThisZero = true;
					}

				} catch (Exception e) {
					n = (int) v.charAt(0) - (int) "A".charAt(0);
				}
			}

			// H.M.Wang2019-12-4 取消查表，改为直接用ASCII码访问
			//for(int k=0; k<SERRIAL_CODES.length; k++) {
			//	if(code[0] == SERRIAL_CODES[k]) {
			//		n = k;
			//		break;
			//	}
			//}

			// H.M.Wang 追加下列6行。为计数器清楚前置0
			if(bClearZero && n == 0 && !bSkipThisZero && i<var.length()-1 && flagClearZero) {
				bFillBlank = true;
			} else {
				bFillBlank = false;
				bSkipThisZero = true;
			}

			if (mBuffer == null || mBuffer.length < (n+1) * mColPerElement * mBytesPerColumn) {
				bFillBlank = true;
			}
			// End ----------------------------------

//   			Debug.d(TAG, "n == " + n);
   			/* 如果每列的字节数为单数，则需要在每列尾部补齐一个字节 */
   			for (int k = 0; k < (int)mColPerElement; k++) {
   				for (int j = 0; j < mType; j++) {
					// H.M.Wang 注释掉1行
//					ba.append(mBuffer, n*mColPerElement * mBytesPerColumn + 16 + k * mBytesPerColumn + j * mBytesPerH, mBytesPerH);

					// H.M.Wang 追加下列5行。为计数器清楚前置0
					if(bFillBlank) {
						ba.append(blank, 0, mBytesPerH);
					} else {
						// H.M.Wang 2019-12-20 浮点运算后取整
						// 2020-5-30 由于vbin可能是10位的，也可能是128位的，如果一个128位的变量去参照10位的vbin就会产生溢出，再次追加一个判断，如果超出范围则填充空
						try{
							ba.append(mBuffer, (int)(n*mColPerElement) * mBytesPerColumn + 16 + k * mBytesPerColumn + j * mBytesPerH, mBytesPerH);
						} catch (IndexOutOfBoundsException e) {
							ba.append(blank, 0, mBytesPerH);
						}
						// End of 2020-5-30 由于vbin可能是10位的，也可能是128位的，如果一个128位的变量去参照10位的vbin就会产生溢出，再次追加一个判断，如果超出范围则填充空
					}

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

		// H.M.Wang 2019-9-24 将打印缓冲区全部置白
		for(int i=0; i<mBufferChars.length; i++) {
			mBufferChars[i] = 0xffff;
		}

   		///./...Debug.d(TAG, "--->char len = " + mBufferChars.length + "   bytes: " + mBufferBytes.length);
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
		for (int k = 0; k < bits * (int) mColPerElement; k++) {
			for (int j = 0; j < mType; j++) {
				// H.M.Wang 2019-12-20 浮点运算后取整
   				ba.append(mBuffer, (int)(offset * mColPerElement) * mBytesPerColumn + 16 + k * mBytesPerColumn + j * mBytesPerH, mBytesPerH);
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

    public synchronized char[] getVarBufferByAscii(String text) {
		int n;
		byte[] feed = {0};

		ByteArrayBuffer ba = new ByteArrayBuffer(0);
		for(int i=0; i<text.length(); i++)
		{
//			String v = text.substring(i, i+1);
			int ascii = text.charAt(i);
			n = ascii - 0x20;
			if (mBuffer == null || mBuffer.length < (n+1) * mColPerElement * mBytesPerColumn) {
				continue;
			}
			///./...Debug.d(TAG, "===>mColPerElement:" + mColPerElement + ", mBytesPerH=" + mBytesPerH + ", type=" + mType);
			/* 如果每列的字节数为单数，则需要在每列尾部补齐一个字节 */
			for (int k = 0; k < (int)mColPerElement; k++) {
				for (int j = 0; j < mType; j++) {
					// H.M.Wang 2019-12-20 浮点运算后取整
					ba.append(mBuffer, (int)(n*mColPerElement) * mBytesPerColumn + 16 + k * mBytesPerColumn + j * mBytesPerH, mBytesPerH);
					if (mNeedFeed) {
						ba.append(feed, 0, 1);
					}
					// Debug.d(TAG, "===>offset:" + (n*mColPerElement * mBytesPerColumn + 16 + k * mBytesFeed + j * mBytesPerH) + " ,mBytesPerH=" + mBytesPerH);
				}
			}
		}
		mBufferBytes = ba.toByteArray();

		//把byte[]存为char[]
		if (mBufferChars == null) {
			mBufferChars = new char[mBufferBytes.length/2];
		}
		if (mExtend != null && mExtend.getScale() > 1) {
			mBufferChars = new char[mBufferBytes.length/2];
		}
		///./...Debug.d(TAG, "--->char len = " + mBufferChars.length + "   bytes: " + mBufferBytes.length);
		for(int i = 0; i < mBufferChars.length; i++) {
			mBufferChars[i] = (char) ((char)((mBufferBytes[2*i+1] << 8) & 0x0ff00) | (mBufferBytes[2*i] & 0x0ff));
		}

		// row extension
		expend();

		// column extension
//    	expendColumn(8);
		return extract();
	}
    
    public static void overlap(byte[] dst, byte[] src, int x, int high)
    {
    	int len = src.length;
		// column extension, adjust the x coordinate
		x = x * columnExtension;
    	if(dst.length < x*high +src.length)
    	{
    		///./...Debug.d(TAG, "dst buffer no enough space!!!!");
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
    		///./...Debug.d(TAG, "dst buffer no enough space!!!! dst.len=" + dst.length + " , src=" + src.length + " , pos=" + x*high);
    		len = dst.length - x*high;
    		//return;
    	}
    	///./...Debug.d(TAG, "--->high = " + high);
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
    	///./...Debug.d(TAG, "--->cover: " + dst.length + "  " + x + "  " + high + "  " + src.length);
    	if(dst.length < x*high + src.length)
    	{
    		///./...Debug.d(TAG, "dst buffer no enough space!!!! dst.len=" + dst.length + " , src=" + src.length + " , pos=" + x*high);
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
    	///./...Debug.d(TAG, "===>Matrix880 : buffer len:"+buffer.length);
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
    	///./...Debug.d(TAG, "===>Matrix880Revert : buffer len:"+buffer.length);
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

//	public static int pcount = 0;
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

//		BinCreater.saveBin("/mnt/sdcard/print_var_" + pcount + "_org.bin", mBufferChars, mCharsFeed * 16);

		///./...Debug.d(TAG, "--->expend scale : " + scale + " length: " + mBufferChars.length);
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
				///./...Debug.d(TAG, "--->j = " + j + "  column = " + column);
				for (int i = 0; i < column; i++) {
// H.M.Wang 2020-1-3 解决1带多情况计算不准确的问题
//					reader.read(mBufferChars, (i*scale + j) * mCharsPerHFeed, mCharsPerHFeed);
					reader.read(mBufferChars, (i*scale + j) * mCharsFeed, mCharsFeed);
// End of H.M.Wang 2020-1-3 解决1带多情况计算不准确的问题
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

//		BinCreater.saveBin("/mnt/sdcard/print_var_" + pcount++ + "_dst.bin", mBufferChars, mCharsFeed * scale * 16);
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
