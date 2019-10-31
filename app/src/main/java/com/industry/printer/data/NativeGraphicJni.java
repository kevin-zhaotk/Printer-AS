package com.industry.printer.data;

import com.industry.printer.Serial.SerialPort;
import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2019/8/16.
 */

public class NativeGraphicJni {
    private static final String TAG = NativeGraphicJni.class.getSimpleName();

    static {
		System.loadLibrary("NativeGraphicJni");
    }

    public static void loadLibrary() {
        Debug.d(TAG, "Loading NativeGraphicJni library...");
    }

    public static native int[] ShiftImage(int[] src, int width, int height, int head, int orgLines, int tarLines);
    public static native byte[] Binarize(int[] src, int width, int height, int head, int value);
    public static native int[] GetDots();
    public static native char[] GetBgBuffer(byte[] src, int length, int bytesFeed, int bytesPerHFeed, int bytesPerH, int column, int type);
    public static native int[] GetPrintDots(char[] src, int length, int bytesPerHFeed, int heads);
}
