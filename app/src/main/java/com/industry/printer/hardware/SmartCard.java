package com.industry.printer.hardware;

import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2019/10/20.
 */

public class SmartCard {
    static {
		System.loadLibrary("smartcard");
    }

    public final static int SC_FAILED                       = -1;
    public final static int SC_SUCCESS                      = 0;
    public final static int SC_INIT_HOST_CARD_NOT_PRESENT   = 100;
    public final static int SC_INIT_PRNT_CTRG_NOT_PRESENT   = 110;
    public final static int SC_INIT_BULK_CTRG_NOT_PRESENT   = 111;
    public final static int SC_INIT_PRNT_CTRG_INIT_FAILED   = 120;
    public final static int SC_INIT_BULK_CTRG_INIT_FAILED   = 121;
    public final static int SC_PRINT_CARTRIDGE_ACCESS_FAILED = 200;
    public final static int SC_BULK_CARTRIDGE_ACCESS_FAILED = 201;
    public final static int SC_LEVEL_CENSOR_ACCESS_FAILED   = 202;
    public final static int SC_CONSISTENCY_FAILED           = 300;
    public final static int SC_OUT_OF_INK_ERROR             = 301;
    public final static int SC_CHECKSUM_FAILED              = 400;

    public static void loadLibrary() {
        Debug.d("SmartCard", "Loading smartcard library...");
    }

    /**
     * SmartCard JNI APIs
     **/
    static public native int exist();

    static public native int init();

    static public native int initComponent(int card);

    static public native int writeCheckSum(int card, int clientUniqueCode);

    static public native int checkSum(int card, int clientUniqueCode);

    static public native int checkConsistency(int card, int supply);

    static public native int getMaxVolume(int card);

    static public native String readConsistency(int card);

    static public native int checkOIB(int card);

    static public native int getLocalInk(int card);

    static public native int downLocal(int card);

    static public native int writeOIB(int card);

    static public native int readLevel(int card);

    static public native int shutdown();

}
