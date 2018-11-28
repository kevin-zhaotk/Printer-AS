package com.printer.corelib;

public class StringUtil {

	public static boolean isEmpty(String str) {
		if (str == null || str.isEmpty() || str.length() == 0) {
			return true;
		}
		return false;
	}
	
	public static int parseInt(String string) {
		if (string == null) {
			return 0;
		} else if (string.contains("-")) {  //處理負數
			String sub = string.substring(string.indexOf("-") + 1);
			int i = 0-Integer.parseInt(sub);
			return i;
		} else {
			return Integer.parseInt(string);
		}
	}
}
