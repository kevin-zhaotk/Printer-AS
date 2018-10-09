package com.industry.printer.Utils;

public class StringUtil {

	public static boolean isEmpty(String str) {
		Debug.d("StringUtil", "--->str: " + str);
		if (str == null || str.isEmpty() || str.length() == 0) {
			return true;
		}
		return false;
	}
	
	public static int parseInt(String string) {
		try {
			if (string == null) {
				return 0;
			} else if (string.contains("-")) {  //處理負數
				String sub = string.substring(string.indexOf("-") + 1);
				int i = 0 - Integer.parseInt(sub);
				return i;
			} else {
				return Integer.parseInt(string);
			}
		} catch (Exception e) {
			return 0;
		}
	}
	
	public static boolean parseBool(String string) {
		try {
			if (string == null) {
				return false;
			} else {
				return Integer.parseInt(string) > 0;
			}
		} catch (Exception e) {
			return false;
		}
	}
	
	public static boolean equal(String v1, String v2) {
		if (v1 == null || v2 == null) {
			return false;
		}
		
		if (v1.equalsIgnoreCase(v2)) {
			return true;
		} else {
			return false;
		}
	}

	public static boolean equalCareCase(String v1, String v2) {
		if (v1 == null || v2 == null) {
			return false;
		}

		if (v1.equals(v2)) {
			return true;
		} else {
			return false;
		}
	}
	
	public static String formatTo(int i, int length) {
		StringBuilder builder = new StringBuilder(String.valueOf(i));
		if (builder.length() >= length) {
			return builder.toString();
		} else {
			int miss = length - builder.length();
			for (int j = 0; j < miss; j++) {
				builder.insert(0, 0);
			}
			
		}
		return builder.toString();
	}
}
