package com.industry.printer;

import com.industry.printer.Utils.Debug;

public class Global {

    private static StringBuffer gCode = new StringBuffer();
    private static long mLast = 0;


    public static void writeCode(char code) {
        long currency = System.currentTimeMillis();
        if (currency - mLast > 2000) {
            gCode.delete(0, gCode.length());
        }
        mLast = currency;
        gCode = gCode.append(code);
    }

    public static String readCode() {
        return gCode.toString();
    }
}
