package com.industry.printer.Utils;


/**
 * 在byte[]和char[]之间互相转换时，因为java不支持unsigned类型导致通过位移进行类型转换时
 * 会
 * @author kevin
 *
 */
@Deprecated
public class TypeConverter {

	public static final String TAG = TypeConverter.class.getSimpleName();
	
	
	public static TypeConverter mInstance = null;
	
	public static TypeConverter getInstance() {
		if (mInstance == null) {
			mInstance = new TypeConverter();
		}
		return mInstance;
	}
	
	/**
	 * 把byte[]转换成char[]
	 * @param bytes	
	 * @return
	 */
	public char[] bytearray2chararray(byte[] bytes) {
		if (bytes == null || bytes.length <= 0) {
			return null;
		}
		int len = bytes.length%2==0? bytes.length/2 : (bytes.length/2 + 1);
		char[] chars = new char[len];
		for(int i = 0; i < chars.length; i++) {
    		chars[i] = bytetochar(bytes[2*i+1], bytes[2*i]);
    	}
		return chars;
	}
	
	/**
	 * 把char[]转换成byte[]
	 * @param chars
	 * @return
	 */
	public byte[] chararray2bytearray(char[] chars) {
		if (chars == null || chars.length <= 0) {
			return null;
		}
		int len = chars.length * 2;
		byte[] bytes = new byte[len];
		for(int i = 0; i < chars.length; i++) {
			bytes[2*i] =  (byte) (chars[i] & 0x0ff);
			bytes[2*i + 1] =  (byte) ((chars[i]>>8) & 0x0ff);
    	}
    	
		return bytes;
	}
	
	
	public char bytetochar(byte arg1, byte arg2) {
		return (char) (((char)(arg1 << 8) & 0x0ff00) | (arg2 & 0x0ff)); 
	}
	
	
}
