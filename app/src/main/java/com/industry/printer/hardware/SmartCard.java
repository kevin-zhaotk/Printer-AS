package com.industry.printer.hardware;

import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2019/10/20.
 */

public class SmartCard {
    static {
		System.loadLibrary("smartcard");
    }

    public static void loadLibrary() {
        Debug.d("SmartCard", "Loading smartcard library...");
    }

    /**
     * SmartCard JNI APIs
     **/
    static public native int open();

    static public native int close();

    static public native int getSmartCardData(int card);

    static public native int getLevelData(int card);
}
