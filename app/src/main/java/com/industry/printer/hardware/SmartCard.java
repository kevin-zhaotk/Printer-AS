package com.industry.printer.hardware;

import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2019/10/20.
 */

public class SmartCard {
    static {
		System.loadLibrary("smartcard");
    }

    public final static int SC_SUCCESS                      = 0;
    public final static int SC_INIT_HOST_CARD_NOT_PRESENT   = 100;
    public final static int SC_INIT_PRNT_CTRG_NOT_PRESENT   = 110;
    public final static int SC_INIT_BULK_CTRG_NOT_PRESENT   = 111;
    public final static int SC_INIT_PRNT_CTRG_INIT_FAILED   = 120;
    public final static int SC_INIT_BULK_CTRG_INIT_FAILED   = 121;
    public final static int SC_INIT_PRNT_CTRG_FID_NOT_MATCH = 130;
    public final static int SC_INIT_BULK_CTRG_FID_NOT_MATCH = 131;
    public final static int SC_PRINT_CARTRIDGE_ACCESS_FAILED = 200;
    public final static int SC_BULK_CARTRIDGE_ACCESS_FAILED = 201;
    public final static int SC_LEVEL_CENSOR_ACCESS_FAILED   = 202;
    public final static int SC_CONSISTENCY_FAILED           = 300;
    public final static int SC_OUT_OF_INK_ERROR             = 301;

    public static void loadLibrary() {
        Debug.d("SmartCard", "Loading smartcard library...");
    }

    /**
     * SmartCard JNI APIs
     **/
    static public native int init();

    static public native int close();

    static public native int chechConsistency(int card);

    static public native int chechOIB(int card);

    static public native int getLocalInk(int card);

    static public native int downLocal(int card);

    static public native int readLevel(int card);
}
