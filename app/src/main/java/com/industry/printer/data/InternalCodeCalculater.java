package com.industry.printer.data;

import java.io.UnsupportedEncodingException;
import java.nio.CharBuffer;

import android.database.CharArrayBuffer;

import com.industry.printer.Utils.Debug;

public class InternalCodeCalculater {
	
	public static InternalCodeCalculater mInstance;
	
	public static InternalCodeCalculater getInstance() {
		if (mInstance == null) {
			mInstance = new InternalCodeCalculater();
		}
		return mInstance;
	}
	
	public char[] getGBKCode(String value) {
		byte[] utf8 = null;
		try {
			utf8 = value.getBytes("GB2312");
			Debug.d("", "--->utf8<---");
			for (int i = 0; i < utf8.length; i++) {
				Debug.d("", ""+Integer.toHexString(utf8[i]&0x0ff));
			}
			Debug.d("", "--->utf8<---");
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		return toGBCode(utf8);
	}
	
	/**
	 * 把编码转换成国标码
	 * 注：此处已经把内码转换成了国标码（区位码）
	 * 国标码 = 内码 - 0xA0
	 * @param code
	 * @return
	 */
	private char[] toGBCode(byte[] code) {
		if (code == null) {
			return null;
		}

		CharBuffer buffer = CharBuffer.allocate(code.length);
		for (int i = 0; i < code.length; i++) {
			if ((code[i]&0x0ff) > 0xA0 && (i+1) < code.length) {
				Debug.d("", "--->code:"+Integer.toHexString(code[i]<<8));
				char qu = (char) (((code[i]&0x00ff) - 0xA0)<<8 );
				char wei = (char) ((code[i+1] & 0x00ff) - 0xA0);
				char c = (char) (qu + wei);
				Debug.d("", "--->gbCode:"+Integer.toHexString(c));
				buffer.append(c);
				i++;
			} else if ((code[i]&0x0ff) < 0xA0) {
				Debug.d("", "--->not chinese");
				buffer.append((char) code[i]);
			}
		}
		int position = buffer.position();
		Debug.d("", "--->position: "+position);
		buffer.position(0);
		char[] dst = new char[position];
		buffer.get(dst, 0, position);
		return dst;
	}
}
