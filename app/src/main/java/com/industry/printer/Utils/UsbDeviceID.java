package com.industry.printer.Utils;

import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Created by hmwan on 2022/5/13.
 */

public class UsbDeviceID {
    public static final String TAG = UsbDeviceID.class.getSimpleName();

    private static String mUsbDevice = "";

    public static void obtainUsbDeviceId() {
        Process pid = null;
        Debug.d(TAG, "LSUSB Start");
        try {
            pid = Runtime.getRuntime().exec("busybox lsusb");
            if(null != pid) {
                BufferedReader bReader = new BufferedReader(new InputStreamReader(pid.getInputStream()), 1024);
                String line;
                pid.waitFor();
                if(null != bReader && null != (line = bReader.readLine())) {
                    if(line.indexOf("Device ") > 0 && line.indexOf("Device 001") < 0) {
                        mUsbDevice = line.substring(line.indexOf("ID ") + 3);
                    } else {
                        mUsbDevice = "";
                    }
                } else {
                    mUsbDevice = "";
                }
                Debug.d(TAG, "UsbDeviceID = [" + mUsbDevice + "]");
            }
        } catch(Exception e) {

        }
        Debug.d(TAG, "LSUSB Done");
    }
}
