package com.industry.printer.data;

import java.io.CharArrayReader;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Vector;
import java.lang.System;

import android.content.Context;
import android.graphics.Bitmap;
import android.text.TextUtils;

import com.industry.printer.BinInfo;
import com.industry.printer.MessageTask;
import com.industry.printer.FileFormat.QRReader;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.interceptor.ExtendInterceptor;
import com.industry.printer.interceptor.ExtendInterceptor.ExtendStat;
import com.industry.printer.object.BarcodeObject;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.DynamicText;
import com.industry.printer.object.HyperTextObject;
import com.industry.printer.object.JulianDayObject;
import com.industry.printer.object.LetterHourObject;
import com.industry.printer.object.MessageObject;
import com.industry.printer.object.RealtimeDate;
import com.industry.printer.object.RealtimeHour;
import com.industry.printer.object.RealtimeMinute;
import com.industry.printer.object.RealtimeMonth;
import com.industry.printer.object.RealtimeObject;
import com.industry.printer.object.RealtimeSecond;
import com.industry.printer.object.RealtimeYear;
import com.industry.printer.object.ShiftObject;
import com.industry.printer.object.TLKFileParser;
import com.industry.printer.object.WeekDayObject;
import com.industry.printer.object.WeekOfYearObject;
import com.industry.printer.object.data.SegmentBuffer;

import org.apache.http.util.CharArrayBuffer;


/**
 * 用于生成打印数据和预览图
 * @author zhaotongkai
 *
 */
public class DataTask {
	
	public static final String TAG = DataTask.class.getSimpleName();
	
	public Context	mContext;
	public ArrayList<BaseObject> mObjList;
	public MessageTask mTask;

	private ExtendStat mExtendStat;
	/**
	 * background buffer
	 *   used for save the background bin buffer
	 *   fill the variable buffer into this background buffer so we get printing buffer
	 */
	public char[] mBgBuffer;
	public char[] mPrintBuffer;
	public char[] mBuffer;
	
	private int mDots = 0;
	private int[] mDotsEach = new int[8];
	
	public boolean isReady = true;
	
	/**
	 * 背景的binInfo，
	 */
	public BinInfo mBinInfo;
	/**
	 * 保存所有变量bin文件的列表；在prepareBackgroudBuffer时解析所有变量的bin文件
	 * 然后保存到这个列表中，以便在填充打印buffer时直接从这个binInfo的列表中读取相应变量buffer
	 * 无需重新解析bin文件，提高效率
	 */
	public HashMap<BaseObject, BinInfo> mVarBinList;
	
	public DataTask(Context context, MessageTask task) {
		mContext = context;
		init(task);
	}
	
	public void setTask(MessageTask task) {
		if (task == null) {
			return;
		}
		init(task);
	}
	
	private void init(MessageTask task) {
		mTask = task;
		isReady = true;
		if (task != null) {
			mObjList = task.getObjects();
		}
		
		ExtendInterceptor interceptor = new ExtendInterceptor(mContext);
		mExtendStat = interceptor.getExtend();
		mDots = 0;
		mVarBinList = new HashMap<BaseObject, BinInfo>();
	}
	/**
	 * prepareBackgroudBuffer
	 * parse the 1.bin, and then read the file content into mBgBuffer, one bit extends to one byte
	 */
	public boolean prepareBackgroudBuffer()
	{
		if (mTask == null) {
			return false;
		}
		
		/**记录当前打印的信息路径**/
		mBinInfo = new BinInfo(ConfigPath.getBinAbsolute(mTask.getName()), mTask, mExtendStat);
		if (mBinInfo == null) {
			Debug.e(TAG, "--->binInfo null");
			return false;
		}
		mBgBuffer = mBinInfo.getBgBuffer();
		
		if (mBgBuffer == null) {
			return false;
		}
		
		Debug.d(TAG, "--->bgbuffer = " + mBgBuffer.length);
		mPrintBuffer = new char[mBgBuffer.length];
		return true;
	}
// H.M.Wang 2020-6-16 追加是否保存print.bin标记，以控制保存行为
	public char[] getPrintBuffer(boolean bSave) {
		return getPrintBuffer(false, bSave);
	}
// End of H.M.Wang 2020-6-16 追加是否保存print.bin标记，以控制保存行为

	public char[] getPrintBuffer(boolean isPreview, boolean bSave) {
		Debug.d(TAG, "--->getPrintBuffer");
		if (mBgBuffer == null) {
			return null;
		}
		Debug.d(TAG, "--->getPrintBuffer  111" );
		CharArrayReader cReader = new CharArrayReader(mBgBuffer);

		// H.M.Wang 将1.bin写入打印缓冲区，然后将v*.bin写入缓冲区
		try {
// H.M.Wang 2020-7-27 由于32DN会对mPrintBuffer进行扩容，如果这里不重新生成，则会使用扩容后的容量，再次被扩容，这使得打印缓冲区持续放大
			mPrintBuffer = new char[mBgBuffer.length];
// End of H.M.Wang 2020-7-27 由于32DN会对mPrintBuffer进行扩容，如果这里不重新生成，则会使用扩容后的容量，再次被扩容，这使得打印缓冲区持续放大
			cReader.read(mPrintBuffer);
			if (isNeedRefresh()) {
				// 将v*.bin写入缓冲区
				refreshVariables(isPreview);
			}
		} catch (IOException e) {
			Debug.d(TAG, "--->e : " + e.getMessage());
		}
// H.M.Wang 2020-6-30 这段代码可能会在isPreview=true时，导致后面的处理不能进行，应该注释掉
//		if (isPreview) {
//			return mPrintBuffer;
//		}
// End of H.M.Wang 2020-6-30 这段代码可能会在isPreview=true时，导致后面的处理不能进行

// H.M.Wang 2020-7-23 追加32DN打印头时的移位处理
		if(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_32DN) {
//			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
			mPrintBuffer = bitShiftFor32DN();
//			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
//			Debug.d(TAG, mTask.getPath() + "/print.bin");
//			BinCreater.saveBin(mTask.getPath() + "/print.bin", mPrintBuffer, 64);
		}
// End of H.M.Wang 2020-7-23 追加32DN打印头时的移位处理

// H.M.Wang 2020-8-17 追加32SN打印头
		if(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_32SN) {
//			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
			mPrintBuffer = bitShiftFor32SN();
//			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
//			Debug.d(TAG, mTask.getPath() + "/print.bin");
//			BinCreater.saveBin(mTask.getPath() + "/print.bin", mPrintBuffer, 64);
		}
// End of H.M.Wang 2020-8-17 追加32SN打印头

		// H.M.Wang 追加下列8行
//		if(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_64_DOT && Configs.getMessageShift(3) == 1) {
		if(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
//			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
			mPrintBuffer = evenBitShiftFor64Dot();
//			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
//			Debug.d(TAG, mTask.getPath() + "/print.bin");
//			BinCreater.saveBin(mTask.getPath() + "/print.bin", mPrintBuffer, 64);
		}

// H.M.Wang 2020-9-6 取消64SN的打印缓冲区转换
//// H.M.Wang 2020-8-26 追加64SN打印头
//		if(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_64SN) {
////			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
//			mPrintBuffer = bitShiftFor64SN();
////			Debug.d(TAG, "mPrintBuffer.length = " + mPrintBuffer.length);
////			Debug.d(TAG, mTask.getPath() + "/print.bin");
////			BinCreater.saveBin(mTask.getPath() + "/print.bin", mPrintBuffer, 64);
//		}
// End of H.M.Wang 2020-8-26 追加64SN打印头
// End of H.M.Wang 2020-9-6 取消64SN的打印缓冲区转换

//		BinCreater.saveBin("/mnt/sdcard/print1.bin", mPrintBuffer, 32);
		Debug.d(TAG, "--->BytesPerColumn: " + mBinInfo.mBytesPerColumn);
//		if (mBinInfo.mBytesPerColumn == 4)  {
//			evenBitShift();
//		} // else{
			/*完成平移/列变换得到真正的打印buffer*/
		rebuildBuffer();
		// }
		//BinCreater.Bin2Bitmap(mPrintBuffer);
		/*test bin*/
		/*
		byte[] buffer = new byte[mBgBuffer.length * 2];
		for (int i = 0; i < buffer.length/2; i++) {
			buffer[2*i] = (byte)(mBgBuffer[i] & 0x00ff);
			buffer[2*i+1] = (byte) (((int) mBgBuffer[i])/256 & 0x00ff);
		}
		BinCreater.saveBin("/mnt/usbhost1/print.bin", buffer, 32);
		*/
		/*test bin*/
		Debug.d(TAG, "--->buffer = " + mBuffer.length);

// H.M.Wang 2020-4-18 从DataTransferThread移至此
        if(bSave) {
            FileUtil.deleteFolder("/mnt/sdcard/print.bin");
			BinCreater.saveBin("/mnt/sdcard/print.bin", mBuffer, mBinInfo.mBytesPerHFeed * 8 * mTask.getNozzle().mHeads);
        }
// End of H.M.Wang 2020-4-18 从DataTransferThread移至此

// H.M.Wang 2020-4-18 追加12.7R5头
// H.M.Wang 2020-5-9 12.7R5d打印头类型不参与信息编辑，因此不通过信息的打印头类型判断其是否为12.7R5的信息，而是通过参数来规定现有信息的打印行为
        SystemConfigFile sysconf = SystemConfigFile.getInstance(mContext);
//		Debug.d(TAG, "Params = " + sysconf.getParam(SystemConfigFile.INDEX_HEAD_TYPE));
//		Debug.d(TAG, "Nozzle Index = " + PrinterNozzle.MessageType.NOZZLE_INDEX_12_7_R5);
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//		if(sysconf.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_12_7_R5) {
		if(sysconf.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X48 ||
			sysconf.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X50) {
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头

//		if(mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_12_7_R5) {
// End of H.M.Wang 2020-5-9 12.7R5d打印头类型不参与信息编辑，因此不通过信息的打印头类型判断其是否为12.7R5的信息，而是通过参数来规定现有信息的打印行为
			CharArrayBuffer caBuf = new CharArrayBuffer(0);
			int orgCharsOfHead = mBinInfo.mCharsPerHFeed * mTask.getNozzle().mHeads;
			int orgCols = mBuffer.length / orgCharsOfHead;
			char[] empty = new char[orgCharsOfHead];
			Arrays.fill(empty, (char)0x0000);

// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
			int maxColNumPerUnit = 0;
			if(sysconf.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X48) {
				maxColNumPerUnit = PrinterNozzle.R6X48_MAX_COL_NUM_EACH_UNIT;
			} else if(sysconf.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X50) {
				maxColNumPerUnit = PrinterNozzle.R6X50_MAX_COL_NUM_EACH_UNIT;
			}

			for(int i=0; i<PrinterNozzle.R6_PRINT_COPY_NUM; i++) {
				for(int k=0; k<maxColNumPerUnit; k++) {
					for(int j=0; j<PrinterNozzle.R6_HEAD_NUM; j++) {
						if((j % 2 == 0 && i == PrinterNozzle.R6_PRINT_COPY_NUM - 1) || 	// 单数行最后一列
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
						   (j % 2 != 0 && i == 0) ||    									// 双数行第一列
						   (k >= orgCols)) {												// 原始块中宽度不足部分
							caBuf.append(empty, 0, orgCharsOfHead);
						} else {
							caBuf.append(mBuffer, k * orgCharsOfHead, orgCharsOfHead);
						}
					}
				}
			}

			mBuffer = caBuf.toCharArray();

            if(bSave) {
                FileUtil.deleteFolder("/mnt/sdcard/printR5x6.bin");
                BinCreater.saveBin("/mnt/sdcard/printR5x6.bin", mBuffer, mBinInfo.mBytesPerHFeed * 8 * mTask.getNozzle().mHeads * PrinterNozzle.R6_HEAD_NUM);
            }
		}
// End of H.M.Wang 2020-4-18 追加12.7R5头

// H.M.Wang 2020-10-23 计算点数从DataTransferThread移到这里
		calDots();
// End of H.M.Wang 2020-10-23 计算点数从DataTransferThread移到这里

		return mBuffer;
	}

// H.M.Wang 2020-10-23 计算点数从DataTransferThread移到这里
	private void calDots(){
		Debug.d(TAG, "GetPrintDots Start Time: " + System.currentTimeMillis());
// H.M.Wang 2020-10-18 重新开放打印前计算墨点数
		int[] dots = NativeGraphicJni.GetPrintDots(mPrintBuffer, mPrintBuffer.length, getInfo().mCharsPerHFeed, getPNozzle().mHeads);

		int totalDot = 0;
		for (int j = 0; j < dots.length; j++) {
			// H.M.Wang 2019-10-11 获得的点数乘2
			SystemConfigFile config = SystemConfigFile.getInstance(mContext);
			final int headIndex = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
			final PrinterNozzle head = PrinterNozzle.getInstance(headIndex);

			if (head != PrinterNozzle.MESSAGE_TYPE_16_DOT &&
                head != PrinterNozzle.MESSAGE_TYPE_32_DOT &&
                head != PrinterNozzle.MESSAGE_TYPE_32DN &&
                head != PrinterNozzle.MESSAGE_TYPE_32SN &&
                head != PrinterNozzle.MESSAGE_TYPE_64SN &&
                head != PrinterNozzle.MESSAGE_TYPE_64_DOT) {
                dots[j] *= 2;
			} else {
				dots[j] *= 200;
			}
			totalDot += dots[j];
		}
		setDots(totalDot);
		setDotsEach(dots);
// End of H.M.Wang 2020-10-18 重新开放打印前计算墨点数
		Debug.d(TAG, "GetPrintDots Done Time: " + System.currentTimeMillis());
	}
// End of H.M.Wang 2020-10-23 计算点数从DataTransferThread移到这里

	private boolean isNeedRebuild() {
		MessageObject object = mTask.getMsgObject();
		PrinterNozzle nozzle = object.getPNozzle();
		for (int i = 0; i < nozzle.mHeads; i++) {
			int shift = nozzle.shiftEnable ? Configs.getMessageShift(i) : 0;
			if (shift > 0 ) {
				return true;
			}
			int mirror = nozzle.mirrorEnable ? Configs.getMessageDir(i) : SystemConfigFile.DIRECTION_NORMAL;
			if (mirror == SystemConfigFile.DIRECTION_REVERS) {
				return true;
			}

		}
		int revert = 0;
		SystemConfigFile sysconf = SystemConfigFile.getInstance(mContext);
		if (nozzle.reverseEnable) {

			if (sysconf.getParam(14) > 0) {
				revert |= 0x01;
			}
			if (sysconf.getParam(15) > 0) {
				revert |= 0x02;
			}
			if (sysconf.getParam(20) > 0) {
				revert |= 0x04;
			}
			if (sysconf.getParam(21) > 0) {
				revert |= 0x08;
			}
		}
		if (revert > 0 ) {
			return true;
		}
		int rotate = nozzle.rotateAble ? sysconf.getParam(35): 0;
		if (rotate > 0) {
			return  true;
		}
		return false;
	}

	public void refreshVariables(boolean prev)
	{
		float scaleW = 2, scaleH = 1;
		String substr=null;
		char[] var;
		if(mObjList==null || mObjList.isEmpty())
			return;
		int heads = mTask.getHeads() == 0 ? 1 : mTask.getHeads();
		SystemConfigFile config = SystemConfigFile.getInstance(mContext);
		ExtendInterceptor interceptor = new ExtendInterceptor(mContext);
		ExtendStat stat = interceptor.getExtend();
		float div = (float) (2.0/heads);
		MessageObject msg = mTask.getMsgObject();
		// Debug.d(TAG, "+++++type:" + msg.getType());
//		if (msg != null && (msg.getType() == MessageType.MESSAGE_TYPE_1_INCH || msg.getType() == MessageType.MESSAGE_TYPE_1_INCH_FAST)) {
//			div = 1;
//			scaleW = 1;
//			scaleH = 0.5f;
//		} else if (msg != null && (msg.getType() == MessageType.MESSAGE_TYPE_1_INCH_DUAL || msg.getType() == MessageType.MESSAGE_TYPE_1_INCH_DUAL_FAST)) {
//			div = 0.5f;
//			scaleW = 0.5f;
//			scaleH = 0.25f;
//		} else if (msg != null && msg.getType() == MessageType.MESSAGE_TYPE_16_DOT) {
//			div = 152f/16f;
//		}
//		/**if high resolution message, do not divide width by 2 */
//		if (msg.getResolution()) {
//			Debug.d(TAG, "--->High Resolution");
//			scaleW = scaleW/2;
//			div = div/2;
//		}

		PrinterNozzle headType = mTask.getNozzle();

		if (headType == PrinterNozzle.MESSAGE_TYPE_1_INCH) {
// H.M.Wang 修改
//			div = 1;
			div = 1.0f * 304/308;
			scaleW = 1;
			scaleH = 0.5f;
		} else if (headType == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL) {
// H.M.Wang 修改
//			div = 0.5f;
			div = 1.0f * 304/308/2;
			scaleW = 0.5f;
			scaleH = 0.25f;

		// H.M.Wang 追加下列8行
		} else if (headType == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE) {
// H.M.Wang 修改
//			div = 0.3333333333f;
			div = 1.0f * 304/308/3;
			scaleW = 0.3333333333f;
			scaleH = 0.1666666667f;
		} else if (headType == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR) {
// H.M.Wang 修改
//			div = 0.25f;
			div = 1.0f * 304/308/4;
			scaleW = 0.25f;
			scaleH = 0.125f;

		} else if (headType == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
			div = 152f/16f;
			scaleW = 152f/16;
			scaleH = 152f/16;
// H.M.Wang 2020-7-23 追加32DN打印头
//		} else if (headType == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-8-17 追加32SN打印头
//		} else if (headType == PrinterNozzle.MESSAGE_TYPE_32_DOT || headType == PrinterNozzle.MESSAGE_TYPE_32DN) {
		} else if (headType == PrinterNozzle.MESSAGE_TYPE_32_DOT || headType == PrinterNozzle.MESSAGE_TYPE_32DN || headType == PrinterNozzle.MESSAGE_TYPE_32SN) {
// End of H.M.Wang 2020-8-17 追加32SN打印头
			div = 152f/32f;
			scaleW = 152f/32;
			scaleH = 152f/32;
// End of H.M.Wang 2020-7-23 追加32DN打印头

		// H.M.Wang 追加下列两行
// H.M.Wang 2020-8-26 追加64SN打印头
//		} else if (headType == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		} else if (headType == PrinterNozzle.MESSAGE_TYPE_64_DOT || headType == PrinterNozzle.MESSAGE_TYPE_64SN) {
// H.M.Wang 2020-8-26 追加64SN打印头
			div = 152f/64f;
			scaleW = 152f/64;
			scaleH = 152f/64;
		}
		/**if high resolution message, do not divide width by 2 */
		if (msg.getResolution()) {
			Debug.d(TAG, "--->High Resolution");
			scaleW = scaleW/2;
			div = div/2;
		}
//		div = div/stat.getScale();
		Debug.d(TAG, "-----scaleW = " + scaleW + " div = " + div);
		//mPreBitmap = Arrays.copyOf(mBg.mBits, mBg.mBits.length);
		for(BaseObject o:mObjList)
		{
			Debug.d(TAG, "Name " + o.mName);
			if (o instanceof BarcodeObject) {
// H.M.Wang 2020-7-31 该判断的内容相当于非动态二维码不继续执行后续代码，但是由于存在超文本，可能内容会发生变化
//				Debug.d(TAG, "+++++++++++++>source: " + o.getSource());
//				/* 如果二維碼從QR文件中讀 */
//				if (!o.getSource()) {
//					continue;
//				}
// End of H.M.Wang 2020-7-31 该判断的内容相当于非动态二维码不继续执行后续代码，但是由于存在超文本，可能内容会发生变化
// 2019-12-18 H.M.Wang 当数据源为外部的时候，不去读取内部的QR文件。并且修改content的设置方式，原来的方式如果不从reader读数据，则可能就是“123456789”，新的方式是如果读并且读到，则设置，否则跳过
//				String content = "123456789";
//				if (!prev) {
// H.M.Wang 2020-5-18 由于数据源参数索引使用立即数，未及时发现修改，改为固定变量索引
//				if (!prev && SystemConfigFile.getInstance().getParam(40) == SystemConfigFile.DATA_SOURCE_FILE) {
				if (!prev && SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FILE) {
// End of H.M.Wang 2020-5-18 由于数据源参数索引使用立即数，未及时发现修改，改为固定变量索引
					QRReader reader = QRReader.getInstance(mContext);
					String content = reader.read();

					if (TextUtils.isEmpty(content)) {
						isReady = false;
						continue;
					}
					o.setContent(content);
				}
// End.
				// Bitmap bmp = o.getScaledBitmap(mContext);
				Bitmap bmp = ((BarcodeObject)o).getPrintBitmap((int)(o.getWidth()/scaleW), mBinInfo.getBytesFeed()*8, (int)(o.getWidth()/scaleW), (int)(o.getHeight()/scaleH), (int)o.getY());
				Debug.d(TAG,"--->cover barcode w = " + o.getWidth() + "  h = " + o.getHeight() + " total=" + (mBinInfo.getBytesFeed()*8) + " " + (o.getWidth()/scaleW) + " " + (o.getHeight()/scaleH));
				// BinCreater.saveBitmap(bmp, "bar.png");
				BinInfo info = new BinInfo(mContext, bmp, mExtendStat);

				BinInfo.overlap(mPrintBuffer, info.getBgBuffer(), (int)(o.getX()/div), info.getCharsFeed() * stat.getScale());
				continue;
// H.M.Wang 2020-5-22 串口数据启用DynamicText，取消代用CounterObject
            } else if(o instanceof DynamicText) {
				Debug.d(TAG, "--->object index=" + o.getIndex());

                Bitmap bmp = ((DynamicText)o).getPrintBitmap((int)(o.getWidth()/scaleW), (int)(o.getHeight()/scaleH), headType.getHeight());
                BinInfo info = new BinInfo(mContext, bmp, mExtendStat);
                BinInfo.overlap(mPrintBuffer, info.getBgBuffer(), (int)(o.getX()/div), info.getCharsFeed() * stat.getScale());

/*
				BinInfo info = null;
				if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_LAN ||
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2 ||
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3 ||
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_SCANER1 ||
// H.M.Wang 2020-6-9 追加串口6协议
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_6 ||
// End of H.M.Wang 2020-6-9 追加串口6协议
// H.M.Wang 2020-7-17 追加串口7协议
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_7 ||
// End of H.M.Wang 2020-7-17 追加串口7协议
// H.M.Wang 2020-6-29 追加网络快速打印数据源
					SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_FAST_LAN ) {
// H.M.Wang 2020-6-29 追加网络快速打印数据源
					info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, 128, mExtendStat);
					var = info.getVarBuffer(o.getContent(), true, true);
				} else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_4 ) {
					info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					var = info.getVarBuffer(o.getContent(), true, false);
				} else {
					info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					var = info.getVarBuffer(o.getContent(), true, false);
				}

				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), info.getCharsFeed() * stat.getScale());
*/
            } else if(o instanceof CounterObject)
			{
// 2019-12-17 H.M.Wang 彻底取消原来的事先保存与Object对应的BinInfo的做法，因为修改为通过useSerialContent()来判断是何种打印方式的话，每次都有可能发生变化
				// H.M.Wang 2019-10-27 修改。适应从串口来的打印数据
				// End ------------------------------------------
//				BinInfo info = mVarBinList.get(o);
				BinInfo info = null;
				Debug.d(TAG, "--->object index=" + o.getIndex());
//				if (info == null) {
				// H.M.Wang 2019-12-19 追加多种协议支持
				// H.M.Wang 2019-12-5 为对应串口打印时，vbin的元素个数不是传统计数器的10位，而是128位，做了区分
/*					if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_LAN ||
						SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_1 ||
						SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_2 ||
						SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_3 ) {
						info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, 128, mExtendStat);
						var = info.getVarBuffer(((CounterObject) o).getRemoteContent(), true, true);
					} else if(SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_DATA_SOURCE) == SystemConfigFile.DATA_SOURCE_RS231_4 ) {
						info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
						var = info.getVarBuffer(((CounterObject) o).getRemoteContent(), true, false);
					} else {*/
						info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
// H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整	，因此生成打印缓冲区的时候，只要取内容即可
//				var = info.getVarBuffer(prev? ((CounterObject) o).getContent() : ((CounterObject) o).getNext(), true, false);
						var = info.getVarBuffer(o.getContent(), true, false);
// End of H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整	，因此生成打印缓冲区的时候，只要取内容即可
/*					}*/
//					info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					// End. 2019-12-5 -----------
//					mVarBinList.put(o, info);
//				}

				// H.M.Wang 2019-12-4 修改变量缓冲区获取方式，如果是串口，则按ASCII进行索引，如果是普通变量则按原来处理方式处理
				// 2019-12-17 H.M.Wang 追加对使用的数据源区分
//				if(SystemConfigFile.getInstance().getParam(40) == 1) {
//				if(((CounterObject) o).useSerialContent()) {
//					var = info.getVarBuffer(((CounterObject) o).getRemoteContent(), true, true);
//				} else {
//					var = info.getVarBuffer(prev? ((CounterObject) o).getContent() : ((CounterObject) o).getNext(), true, false);
//				}
				// End. .......................H.M.Wang 2019-12-4

//				BinCreater.saveBin("/sdcard/" + o.getIndex() + ".bin", var, info.getCharsPerHFeed()*16);

				// Debug.d(TAG, "--->object x=" + o.getX()/div);

				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), info.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-5-22 串口数据启用DynamicText，取消代用CounterObject
			}
			else if(o instanceof RealtimeObject) {

				Vector<BaseObject> rt = ((RealtimeObject) o).getSubObjs();

				for (BaseObject rtSub : rt) {
					if (rtSub instanceof RealtimeYear) {
						substr = ((RealtimeYear) rtSub).getContent();
					} else if (rtSub instanceof RealtimeMonth) {
						substr = ((RealtimeMonth) rtSub).getContent();
						//continue;
					} else if (rtSub instanceof RealtimeDate) {
						substr = ((RealtimeDate) rtSub).getContent();
						//continue;
					} else if (rtSub instanceof RealtimeHour) {
						substr = ((RealtimeHour) rtSub).getContent();
					} else if (rtSub instanceof RealtimeMinute) {
						substr = ((RealtimeMinute) rtSub).getContent();
// H.M.Wang 2020-8-6 增加SS秒时间格式
					} else if (rtSub instanceof RealtimeSecond) {
						substr = ((RealtimeSecond) rtSub).getContent();
// End of H.M.Wang 2020-8-6 增加SS秒时间格式
					} else
						continue;
					BinInfo info = mVarBinList.get(rtSub);
					if (info == null) {
						info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), rtSub.getIndex()), mTask, mExtendStat);
						mVarBinList.put(rtSub, info);
					}
					var = info.getVarBuffer(substr, false, false);
					//BinCreater.saveBin("/mnt/usbhost1/v" + o.getIndex() + ".bin", var, info.mBytesPerHFeed*8);
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
					BinInfo.overlap(mPrintBuffer, var, (int) (rtSub.getX() / div), info.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
					Debug.d(TAG, "--->real x=" + rtSub.getX() / div);
//					BinCreater.saveBin("/sdcard/" + o.getIndex() + substr + ".bin", var, info.getCharsFeed() * stat.getScale() * 16);
				}
// H.M.Wang 2020-2-17 追加HyperText控件
			} else if(o instanceof HyperTextObject) {
				Vector<BaseObject> htObjs = ((HyperTextObject) o).getSubObjs();

				for (BaseObject htObj : htObjs) {
					if (htObj instanceof RealtimeYear) {
						substr = ((RealtimeYear) htObj).getContent();
					} else if (htObj instanceof RealtimeMonth) {
						substr = ((RealtimeMonth) htObj).getContent();
					} else if (htObj instanceof RealtimeDate) {
						substr = ((RealtimeDate) htObj).getContent();
					} else if (htObj instanceof RealtimeHour) {
						substr = ((RealtimeHour) htObj).getContent();
					} else if (htObj instanceof RealtimeMinute) {
						substr = ((RealtimeMinute) htObj).getContent();
					} else if (htObj instanceof RealtimeSecond) {
						substr = ((RealtimeSecond) htObj).getContent();
					} else if (htObj instanceof ShiftObject) {
// H.M.Wang 2020-2-24 超文本班次打印崩溃问题解决
//						substr = ((ShiftObject) htObj).getContent();
// End of H.M.Wang 2020-2-24 超文本班次打印崩溃问题解决
					} else if (htObj instanceof WeekDayObject) {
						substr = ((WeekDayObject) htObj).getContent();
					} else if (htObj instanceof WeekOfYearObject) {
						substr = ((WeekOfYearObject) htObj).getContent();
					} else if (htObj instanceof CounterObject) {
// H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整	，因此生成打印缓冲区的时候，只要取内容即可
						substr = htObj.getContent();
// End of H.M.Wang 2020-7-2 调整计数器增量策略，在打印完成时调整	，因此生成打印缓冲区的时候，只要取内容即可
					} else
						continue;
					BinInfo info = mVarBinList.get(htObj);
					if (info == null) {
						info = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), htObj.getIndex()), mTask, mExtendStat);
						mVarBinList.put(htObj, info);
					}
// H.M.Wang 2020-2-24 超文本班次打印崩溃问题解决
					if (htObj instanceof ShiftObject) {
						var = info.getVarBuffer(((ShiftObject)htObj).getShiftIndex(), ((ShiftObject)htObj).getBits());
					} else {
						var = info.getVarBuffer(substr, false, false);
					}
// End of H.M.Wang 2020-2-24 超文本班次打印崩溃问题解决
					//BinCreater.saveBin("/mnt/usbhost1/v" + o.getIndex() + ".bin", var, info.mBytesPerHFeed*8);
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
					BinInfo.overlap(mPrintBuffer, var, (int) (htObj.getX() / div), info.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
					Debug.d(TAG, "--->real x=" + htObj.getX() / div);
//					BinCreater.saveBin("/sdcard/" + o.getIndex() + substr + ".bin", var, info.getCharsFeed() * stat.getScale() * 16);
				}
// End of H.M.Wang 2020-2-17 追加HyperText控件
			}
			else if(o instanceof JulianDayObject)
			{
				String vString = ((JulianDayObject)o).getContent();
				BinInfo varbin= mVarBinList.get(o);
				if (varbin == null) {
					varbin = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					mVarBinList.put(o, varbin);
				}
				Debug.d(TAG, "--->real x=" + o.getX()+ ", div-x=" + o.getX()/div );
				var = varbin.getVarBuffer(vString, false, false);
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), varbin.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高

			} else if (o instanceof ShiftObject) {
				/*班次變量特殊處理，生成v.bin時固定爲兩位有效位，如果shift的bit爲1，那前面補0，
				 *所以，shift變量的v.bin固定爲8位，如果bit=1，需要跳過前面的0*/
				int shift = ((ShiftObject)o).getShiftIndex();
				Debug.d(TAG, "--->shift ******: " + shift);
				BinInfo varbin= mVarBinList.get(o);
				if (varbin == null) {
					varbin = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					mVarBinList.put(o, varbin);
				}
				// Debug.d(TAG, "--->real x=" + o.getX()+ ", div-x=" + o.getX()/div );
				var = varbin.getVarBuffer(shift, ((ShiftObject)o).getBits());
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), varbin.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
			} else if (o instanceof LetterHourObject) {
				BinInfo varbin= mVarBinList.get(o);
				if (varbin == null) {
					varbin = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, 24, mExtendStat);
					mVarBinList.put(o, varbin);
				}
				String t = ((LetterHourObject) o).getContent();
				var = varbin.getVarBuffer(t, false, false);
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), varbin.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
			} else if (o instanceof WeekOfYearObject) {
				BinInfo varbin= mVarBinList.get(o);
				if (varbin == null) {
					varbin = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					mVarBinList.put(o, varbin);
				}
				String t = ((WeekOfYearObject) o).getContent();
				var = varbin.getVarBuffer(t, false, false);
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), varbin.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
			}  else if (o instanceof WeekDayObject) {
				BinInfo varbin= mVarBinList.get(o);
				if (varbin == null) {
					varbin = new BinInfo(ConfigPath.getVBinAbsolute(mTask.getName(), o.getIndex()), mTask, mExtendStat);
					mVarBinList.put(o, varbin);
				}
				String t = ((WeekDayObject) o).getContent();
				var = varbin.getVarBuffer(t, false, false);
// H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
				BinInfo.overlap(mPrintBuffer, var, (int)(o.getX()/div), varbin.getCharsFeed() * stat.getScale());
// End of H.M.Wang 2020-1-2 添加 * stat.getScale()以调整1带多时的高度，info.getCharsFeed()只是取一个头的高
			} else
			{
				Debug.d(TAG, "not Variable object");
			}

		}
	}

	
	
	public ArrayList<BaseObject> getObjList() {
		return mObjList;
	}
	
	
	public void setDots(int dots) {
		Debug.d(TAG, "--->dotcount: " + dots);
		mDots = dots;
	}
	
	public void setDotsEach(int[] dots) {
		if (dots == null) {
			return;
		}
		
		for (int i = 0; i < dots.length; i++) {
			Debug.d(TAG, "--->setDotsEach: dots[" + i + "]=" + dots[i]);
			if (mDotsEach.length <= i) {
				break;
			}
			mDotsEach[i] = dots[i];
		}
	}
	
	
	public int getDots() {
		return mDots;
	}
	
	public int getDots(int index) {
		Debug.d(TAG, "--->getDots: " + mDotsEach[index]);
		if (index >= mDotsEach.length) {
			return 0;
		}
		return mDotsEach[index];
	}
	
	public boolean isNeedRefresh() {
		
		if (mObjList == null || mObjList.isEmpty()) {
			return false;
		}
		
		for(BaseObject o:mObjList)
		{
			if((o instanceof CounterObject)
					|| (o instanceof RealtimeObject)
// H.M.Wang 2020-7-31 条码支持超文本，因此需要每次更新，具体是否更新看超文本时候有变化
					|| (o instanceof BarcodeObject)
// End of H.M.Wang 2020-7-31 条码支持超文本，因此需要每次更新，具体是否更新看超文本时候有变化
// H.M.Wang 2020-2-16 追加HyperText控件
					|| (o instanceof HyperTextObject)
// End of H.M.Wang 2020-2-16 追加HyperText控件
// H.M.Wang 2020-5-22 追加DynamicText控件
					|| (o instanceof DynamicText)
// End of H.M.Wang 2020-5-22 追加DynamicText控件
					|| (o instanceof JulianDayObject)
					|| (o instanceof ShiftObject)
					|| (o instanceof LetterHourObject)
					|| (o instanceof WeekOfYearObject)
					|| (o instanceof WeekDayObject)
					|| o.getSource())
			{
				return true;
			}
		}
		return false;
	}
	
	/**
	 * 对buffer进行左右移动变换，生成真正的打印数据
	 */
	public void rebuildBuffer() {

		if (!isNeedRebuild()) {
			mBuffer = mPrintBuffer;
		}
		MessageObject object = mTask.getMsgObject();;
//		ArrayList<SegmentBuffer> buffers = new ArrayList<SegmentBuffer>();
//		for (BaseObject msg : mTask.getObjects()) {
//			if (msg instanceof MessageObject) {
//				object = msg;
//				break;
//			}
//		}
		if (object == null) {
			return;
		}
		/*分头处理*/
		int heads = 1;
//		if (object != null) {
			heads = mTask.getHeads();
//		}
		ExtendInterceptor interceptor = new ExtendInterceptor(mContext);
		if (interceptor.getExtend() != ExtendStat.NONE) {
			heads = interceptor.getExtend().activeNozzleCount();
		}
		Debug.d(TAG, "--->type=" + heads);

// H.M.Wang 2020-3-3 修改生成偏移，镜像以及倒置的算法
/*
		for (int i = 0; i < heads; i++) {
			int revert = 0x00;
			int shift = object.getPNozzle().shiftEnable ? Configs.getMessageShift(i) : 0;
			int mirror = object.getPNozzle().mirrorEnable ? Configs.getMessageDir(i) : SegmentBuffer.DIRECTION_NORMAL;
			SystemConfigFile sysconf = SystemConfigFile.getInstance(mContext);
			if (object.getPNozzle().reverseEnable) {

				if (sysconf.getParam(14) > 0) {
					revert |= 0x01;
				}
				if (sysconf.getParam(15) > 0) {
					revert |= 0x02;
				}
				if (sysconf.getParam(22) > 0) {
					revert |= 0x04;
				}
				if (sysconf.getParam(23) > 0) {
					revert |= 0x08;
				}
			}

			int rotate = sysconf.getParam(35);

//			if (object.getPNozzle().shiftEnable) {
//				buffers.add(new SegmentBuffer(mContext, mPrintBuffer, i, heads, mBinInfo.getCharsFeed(), SegmentBuffer.DIRECTION_NORMAL, 0));
//			} else {
//				buffers.add(new SegmentBuffer(mContext, mPrintBuffer, i, heads, mBinInfo.getCharsFeed(), Configs.getMessageDir(i), Configs.getMessageShift(i)));
//			}
			buffers.add(new SegmentBuffer.Builder(mContext, mPrintBuffer)
					.type(i)
					.heads(heads)
					.ch(mBinInfo.getCharsFeed())
					.direction(mirror)
					.shift(shift)
					.revert(revert)
					.rotate(rotate)
					.build());
		}

		int columns=0;
		int hight = 0;
		for (SegmentBuffer segmentBuffer : buffers) {
			columns = segmentBuffer.getColumns() > columns?segmentBuffer.getColumns():columns;
			hight = segmentBuffer.mHight * buffers.size();
		}
		Debug.d(TAG, "--->columns: " + columns + "  hight: " + hight);
		mBuffer = new char[columns * hight];
		for (int j=0; j < columns; j++) {
			for (SegmentBuffer buffer : buffers) {
				buffer.readColumn(mBuffer, j, j*hight + buffer.mHight * buffer.mType);
			}
		}
*/
		int offsetDiv = 1;

// H.M.Wang 2020-7-23 追加32DN打印头
//		if(object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_16_DOT || object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_32_DOT || object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		if( object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
			object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
			object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
			object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
// H.M.Wang 2020-8-26 追加64SN打印头
			object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_64SN ||
// End of H.M.Wang 2020-8-26 追加64SN打印头
			object.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
			heads = 4;		// 16点，32点和64点，在这里假设按4个头来算，主要是为了就和当前的实现逻辑
			offsetDiv = 6;	// 打字机位移量除6
		}

		int[] shifts = new int[heads];
		int[] mirrors = new int[heads];
		for (int i = 0; i < heads; i++) {
			shifts[i] = (object.getPNozzle().shiftEnable ? Configs.getMessageShift(i) : 0) / offsetDiv;
			mirrors[i] = object.getPNozzle().mirrorEnable ? Configs.getMessageDir(i) : SystemConfigFile.DIRECTION_NORMAL;
		}

		int revert = 0x00;
		SystemConfigFile sysconf = SystemConfigFile.getInstance(mContext);
		if (object.getPNozzle().reverseEnable) {
			if (sysconf.getParam(14) > 0) {
				revert |= 0x01;
			}
			if (sysconf.getParam(15) > 0) {
				revert |= 0x02;
			}
			if (sysconf.getParam(22) > 0) {
				revert |= 0x04;
			}
			if (sysconf.getParam(23) > 0) {
				revert |= 0x08;
			}
		}

		BufferRebuilder br = new BufferRebuilder(mPrintBuffer, mBinInfo.getCharsFeed(), heads);
		br.mirror(mirrors)
		  .shift(shifts)
		  .reverse(revert);
		mBuffer = br.getCharBuffer();
// End of H.M.Wang 2020-3-3 修改生成偏移，镜像以及倒置的算法

		int slant = SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_SLANT);
		Debug.d(TAG, "--->slant: " + slant);
		expendColumn(mBuffer, br.getColumnNum(), slant);
	}
	/**
	 * 双列喷嘴用的，  以后不用了，   替换为旋转逻辑
	 * 对齐设定, 只针对32Bit × N的buffer，其他buffer不处理
	 * 1.  原来在buffer的总长度， 增加N 列。 
	 * 2.原buffer列中第X列，所有偶数bit，   0,2,4。。。 34 bit，  后移到第n+X列。
	 * 例如·， 
	 * a. 如果设为 0，  就是现在的buffer , 完全没变化。 
	 * b. 如果设为4，  则 buffer 增加 4 列，  16 B。 
     * 例如：  没4B 为一列， 
	 * 第0 列的 的偶数bit （在0-3  字节中，）会 移到 新buffer的第（0+4）= 列的偶数bit， （在16-19Ｂ）。　
	 */
	@Deprecated
	public void evenBitShift() {
		int shift = Configs.getEvenShift();
		mBuffer = new char[mPrintBuffer.length + shift  * 2];
		CharArrayReader cReader = new CharArrayReader(mPrintBuffer);
		try {
			cReader.read(mBuffer);
		} catch (IOException e) {
			e.printStackTrace();
		}
		/**/
		char next=0;
		char cur = 0;
		int columns = mBinInfo.mColumn + shift;
		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < 2; j++) {
				cur = mBuffer[(columns-1-i)*2 + j];
				int col = shift + i + 1; 
				if (col >= columns) {
					next = 0;
				} else {
					next = mBuffer[2*(columns - col) + j];
				}
				cur = (char) (cur & 0x0AAAA);
				next = (char) (next & 0x05555);
				// System.out.println("--->cur&a0a0=" + String.valueOf((int)cur) + ",  next&a0a0=" + String.valueOf((int)next));
				mBuffer[(columns-1-i)*2 + j] = (char) (cur | next);
				// System.out.println("--->buffer[" + ((columns-1-i)*2 + j) + "]=" + String.valueOf((int)mBuffer[(columns-1-i)*2 + j]));
			}
			
		}
	}

	/* H.M.Wang
		64DOT喷头双列的时候，每个Byte的1，3，5，7Bit向后位移4个字节
	 */
	public char[] evenBitShiftFor64Dot() {
		int COLUMNS_TO_SHIFT= 4;
		int CHARS_PER_COLOMN = 4;
		char[] buffer = new char[mPrintBuffer.length + COLUMNS_TO_SHIFT * CHARS_PER_COLOMN];	// 每行增加4个字节，共增加32个字节(16个char)

		for (int i = 0; i < mBinInfo.mColumn; i++) {
			for (int j = 0; j < CHARS_PER_COLOMN; j++) {
				buffer[i * CHARS_PER_COLOMN + j] |= (char)(mPrintBuffer[i * CHARS_PER_COLOMN + j] & 0xaaaa);
				buffer[(i + COLUMNS_TO_SHIFT) * CHARS_PER_COLOMN + j] |= (char)(mPrintBuffer[i * CHARS_PER_COLOMN + j] & 0x5555);
			}
		}
		return buffer;
	}

// H.M.Wang 2020-7-23 追加32DN打印头时的移位处理
	public char[] bitShiftFor32DN() {
		int COLUMNS_TO_SHIFT= 4;
		int CHARS_PER_COLOMN = 2;
		char[] buffer = new char[(mPrintBuffer.length + COLUMNS_TO_SHIFT * CHARS_PER_COLOMN) * 2];	// 每行增加4个字节，共增加16个字节(8个char),并且每列16bit后空余16bit，相当于数据区翻倍
		Arrays.fill(buffer, (char)0x0000);
		for (int i = 0; i < mBinInfo.mColumn; i++) {
			char d1 = 0x0000;
			char d2 = 0x0000;
			for (int j=CHARS_PER_COLOMN-1; j>=0; j--) {
				char odd = (char)(mPrintBuffer[i * CHARS_PER_COLOMN + j] & 0x5555);
				char even = (char)(mPrintBuffer[i * CHARS_PER_COLOMN + j] & 0xaaaa);
				for(int k=0; k<8; k++) {
					d1 *= 2;
					if(((odd << (2*k+1)) & 0x8000) == 0x8000) {
						d1++;
					}
					d2 *= 2;
					if(((even << (2*k)) & 0x8000) == 0x8000) {
						d2++;
					}
				}
			}
			buffer[2 * i * CHARS_PER_COLOMN] = d1;
			buffer[2 * i * CHARS_PER_COLOMN + 1] = 0x0000;
			buffer[(2 * (i + COLUMNS_TO_SHIFT) + 1) * CHARS_PER_COLOMN] = d2;
			buffer[(2 * (i + COLUMNS_TO_SHIFT) + 1) * CHARS_PER_COLOMN+1] = 0x0000;
		}
		return buffer;
	}
// End of H.M.Wang 2020-7-23 追加32DN打印头时的移位处理

// H.M.Wang 2020-8-17 追加32SN打印头
	public char[] bitShiftFor32SN() {
		char[] buffer = new char[mPrintBuffer.length * 2];			// 每16位原内容之后插入16位的空格
		Arrays.fill(buffer, (char)0x0000);
		for (int i=0; i<mPrintBuffer.length; i++) {
			buffer[2 * i] = mPrintBuffer[i];
			buffer[2 * i + 1] = 0x0000;
		}
		return buffer;
	}
// End of H.M.Wang 2020-8-17 追加32SN打印头

// H.M.Wang 2020-8-26 追加64SN打印头
public char[] bitShiftFor64SN() {
	char[] buffer = new char[mPrintBuffer.length * 2];			// 每32位原内容之后插入32位的空格
	Arrays.fill(buffer, (char)0x0000);
	for (int i=0; i<mPrintBuffer.length; i+=2) {
		buffer[2 * i] = mPrintBuffer[i];
		buffer[2 * i + 1] = mPrintBuffer[i + 1];
		buffer[2 * i + 2] = 0x0000;
		buffer[2 * i + 3] = 0x0000;
	}
	return buffer;
}
// End of H.M.Wang 2020-8-26 追加64SN打印头

	public BinInfo getInfo() {
		return mBinInfo;
	}
	
	/**
	 * 用於清洗的buffer
	 * @return
	 */
	public char[] preparePurgeBuffer(String bin) {
		InputStream stream;
		try {
			stream = mContext.getAssets().open(bin);
			mBinInfo = new BinInfo(stream, 1);
			char[] buffer = mBinInfo.getBgBuffer();
			stream.close();
			char[] rb = new char[buffer.length * 12];
			for(int i = 0; i < 12; i++) {
				System.arraycopy(buffer, 0, rb, i * buffer.length, buffer.length -1);
			}
			return rb;
//			return buffer;
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return null;
	}
	
//	public int getHeads() {
//		return mTask.getHeads();
//	}
	
	public PrinterNozzle getPNozzle() {
		return mTask.getNozzle();
	}
	
	public int getBufferHeightFeed() {
		if (mBinInfo == null) {
			return 0;
		}
		return mBinInfo.mCharsPerHFeed;
	}
	
	public int getBufferColumns() {
		if (mBinInfo == null) {
			return 0;
		}
		return mBinInfo.mColumn;
	}
	
	public Bitmap getPreview() {
		char[] preview = getPrintBuffer(true);
		if (preview == null) {
			return null;
		}
		// String path = "/mnt/usbhost1/prev.bin";
		// BinCreater.saveBin(path, preview, getInfo().mBytesPerHFeed*8*getHeads());
		Debug.d(TAG, "--->column=" + mBinInfo.mColumn + ", charperh=" + mBinInfo.mCharsPerHFeed);
		return BinFromBitmap.Bin2Bitmap(preview, mBinInfo.mColumn, mBinInfo.mCharsFeed*16);
	}

	/**
	 * expend along horizontal space, 1 column to 32 or any columns
	 * big dot machine
	 * extend buffer to 8 times filled with 0
	 * 只针对大字机buffer处理：
	 * 	1、slant = 100，将buffer拓宽32（@link Configs.CONST_EXPAND）倍；
	 * 		详细描述：
	 * 			1、在每两列之间插入31列空白列
	 * 			2、总列数变为 Nx32
	 *
	 * 	2、slant > 100，在拓宽的基础上将buffer进行逐列偏移
	 * 		详细描述：shift = slant - 100；如果shift == 0，不执行旋转操作
	 * 				第0行，不动
	 * 				第1行，向右偏移shift列
	 * 				第2行，向右偏移（2 * shift）列
	 * 				...
	 * 				第n行，向右偏移（n * shift）列
	 */
	public void expendColumn(char[] buffer, int columns, int slant) {

		if (mTask == null || mTask.getNozzle() == null || !mTask.getNozzle().buffer8Enable) {
			return;
		}
		int extension = 0;
		int shift = 0;
		Debug.d(TAG, "--->slant: " + slant);
		if (slant >= 100 ) {
			extension = Configs.CONST_EXPAND;
			shift = slant - 100;
		} else {
			return;
		}
		// CharArrayWriter writer = new CharArrayWriter();
		Debug.d(TAG, "--->extension: " + extension + " shift: " + shift);
		int charsPerColumn = buffer.length/columns;
		int columnH = charsPerColumn * 16;
		int afterColumns = columns * Configs.CONST_EXPAND + (shift > 0 ? shift * (columnH - 1) : 0);
		// buffer extend 8 times - a temperary buffer
		char[] buffer_8 = new char[columns * Configs.CONST_EXPAND * charsPerColumn];
		
		// the  final extension and shift buffer
		// mBuffer = new char[afterColumns * charsPerColumn];
		Debug.d(TAG, "--->charsPerColumn: " + charsPerColumn + "  columnH: " + columnH + "  afterColumns: " + afterColumns + "  buffer.len: " + mBuffer.length);
		// 8 times extension buffer
		for (int i = 0; i < buffer.length/charsPerColumn; i++) {
			for (int j = 0; j < charsPerColumn; j++) {
				buffer_8[i * Configs.CONST_EXPAND * charsPerColumn + j] = buffer[i * charsPerColumn + j];
			}
		}
		if (shift == 0) {
			mBuffer = buffer_8;
			return;
		}

		// shift operation
		char[] shiftBuffer = new char[afterColumns * charsPerColumn];
		for (int i = 0; i < columns * Configs.CONST_EXPAND; i++) {
			for (int j = 0; j < columnH; j++) {
				int rowShift = shift * j;
				int bit = j%16;
				char data = buffer_8[i * charsPerColumn + j/16];
				if ((data & (0x0001<< bit)) != 0) {
					int index = (i+rowShift)*charsPerColumn + j/16;

					shiftBuffer[index] |= (0x0001<< bit);
				}	
			}
		}
		int realColumns = afterColumns;
		for (int i = afterColumns - 1; i > 0; i--) {
			if (shiftBuffer[charsPerColumn * i] != 0 || shiftBuffer[charsPerColumn *i + 1] != 0) {
				break;
			}
			realColumns--;
		}
		if (realColumns + 8 < afterColumns) {
			realColumns += 8;
		}

		mBuffer = Arrays.copyOf(shiftBuffer, realColumns * charsPerColumn);
	}

}