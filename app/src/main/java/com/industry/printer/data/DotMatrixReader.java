package com.industry.printer.data;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import org.apache.http.util.ByteArrayBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.google.zxing.common.BitArray;
import com.google.zxing.common.BitMatrix;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

public class DotMatrixReader {

	private static final String TAG = DotMatrixReader.class.getSimpleName(); 
	public static DotMatrixReader mInstance;
	
	private File mDotFile;
	private InputStream mReader;
	
	public static DotMatrixReader getInstance(Context context) {
		if (mInstance == null) {
			mInstance = new DotMatrixReader(context);
		}
		return mInstance;
	}

	public DotMatrixReader(Context context) {
		
		try {
			if (new File(ConfigPath.getFont()).exists()) {
				mReader = new FileInputStream(ConfigPath.getFont());
				mReader.mark(0);
				mReader.reset();
				Debug.e(TAG, "11111===>DotMatrixReader");
			} else {
				mReader = context.getAssets().open("dotmatrix/HZK16");
				mReader.mark(0);
				mReader.reset();
				Debug.e(TAG, "2222==>DotMatrixReader");
			}
			
		} catch (FileNotFoundException e) {
			Debug.e(TAG, "===>Excpetion:"+e.getMessage());
		} catch (IOException e) {
			Debug.e(TAG, "===>Excpetion:"+e.getMessage());
		}
	}
	
	/**
	 * 根据国标码查询字库点阵
	 * @param inCodes
	 * @return
	 */
	public byte[] getDotMatrix(char[] inCodes) {
		
		int offset = 0;
		byte[] buffer = new byte[32];
		ByteArrayBuffer matrix = new ByteArrayBuffer(0);
		for (int i = 0; i < inCodes.length; i++) {
			if (isAscii(inCodes[i])) {
				offset = getOffsetByAscii(inCodes[i]);
			} else {
				offset = getOffsetByGBCode(inCodes[i]);
			}
			try {
				mReader.reset();
				mReader.skip(offset);
				mReader.read(buffer);
				Debug.d(TAG, "----------------------");
				//Debug.d(TAG, "===>code:"+Integer.toHexString(inCodes[i])+"   offset:"+offset);
				//Debug.print(buffer);
				Debug.d(TAG, "----------------------");
				columnTransferSmallend(buffer);
//				Debug.print(buffer);
				Debug.d(TAG, "----------------------");
				matrix.append(expendTo32Bit(buffer), 0, 64);
				
			} catch (IOException e) {
			}
			
		}
		return matrix.toByteArray();
	}
	
	public int getDotCount(byte[] dots) {
		int count = 0;
		for (int i = 0; i < dots.length; i++) {
			for (int j = 0; j < 8; j++) {
				if ((dots[i] & (0x01<< j)) > 0) {
					count++;
				}
			}
		}
		return count;
	}
	/**
	 * 把字库中取出的行点阵转换成列点阵,高字节在前
	 * @param matrix
	 */
	private void columnTransferBigend(byte[] matrix) {
		if (matrix == null || matrix.length != 32) {
			return;
		}
		byte[] trans = new byte[32];

		for (int i = 0; i < trans.length; i++) {
			for (int j = 0; j < 8; j++) {
				byte data = (byte)((matrix[j*2 + i/16 + (i%2)*16]) & 0x0ff);
				data = (byte) (data  & (0x01 <<(7-(i/2)%8))); 
				// Debug.d(TAG, "i="+i+", j="+j+"-->"+data);
				if ( data != 0) {
					trans[i] |= (0x01 << (7-j));
				}
			}
		}
		Debug.print("", trans);
		ByteBuffer b = ByteBuffer.wrap(trans);
		b.get(matrix);
	}
	
	/**
	 * 把字库中取出的行点阵转换成列点阵，低字节在前，与点阵显示工具对应
	 * @param matrix
	 */
	private void columnTransferSmallend(byte[] matrix) {
		if (matrix == null || matrix.length != 32) {
			return;
		}
		byte[] trans = new byte[32];

		for (int i = 0; i < trans.length; i++) {
			for (int j = 0; j < 8; j++) {
				byte data = (byte)((matrix[j*2 + i/16 + (i%2)*16]) & 0x0ff);
				data = (byte) (data  & (0x01 <<(7-(i/2)%8))); 
				// Debug.d(TAG, "i="+i+", j="+j+"-->"+data);
				if ( data != 0) {
					trans[i] |= (0x01 << j);
				}
			}
		}
		Debug.print("", trans);
		ByteBuffer b = ByteBuffer.wrap(trans);
		b.get(matrix);
	}
	
	private byte[] expendTo32Bit(byte[] sixteen) {
		if (sixteen == null) {
			return null;
		}
		byte[] bit_32 = new byte[64];
		for (int i = 0; i < sixteen.length; i+=2) {
			bit_32[2*i] = sixteen[i];
			bit_32[2*i+1] = sixteen[i+1];
		}
		return bit_32;
	}
	
	/**
	 * 数字和字符通过ascii码计算字库偏移量
	 * 计算公式：offset = (94*(3-1)+(*(str+i)-0x30+16-1))*200L 
	 * @param ascii
	 * @return 偏移量
	 */
	private int getOffsetByAscii(char ascii) {
		int offset = (94*(3-1)+(ascii-0x30+16-1))*32;
		return offset;
	}
	
	/**
	 * 汉字通过国标码计算字库偏移量
	 * 计算公式 offset =(94*(qh-1)+(wh-1))*32;
	 * @param gbk 国标码
	 * @return 偏移量
	 */
	private int getOffsetByGBCode(char gbk) {
		Debug.d(TAG, "--->gbk:"+Integer.toHexString(gbk));
		int quCode=0, weiCode=0; 
		quCode = (gbk>>8)&0x00ff;
		weiCode = gbk & 0x00ff;
		Debug.d(TAG, "--->gbk qu:"+Integer.toHexString(quCode)+" , wei:"+Integer.toHexString(weiCode));
		return (94*(quCode-1)+(weiCode-1))*32;
	}
	
	private boolean isAscii(char c) {
		return (c < 0xA0);
	}
	
	
}
