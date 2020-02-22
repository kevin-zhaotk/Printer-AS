package com.industry.printer.hardware;

import android.content.Context;

import com.industry.printer.Utils.PlatformInfo;

/**
 * factory for ink device manager: RFID, SmartCard
 */
public class InkManagerFactory {

    private static volatile IInkDevice manager = null;

    public static IInkDevice inkManager(Context ctx) {

        if (manager == null) {
            synchronized (InkManagerFactory.class) {
                if (manager == null) {
                    manager = getManager(ctx);
                }
            }
        }
        return manager;
    }

    public static IInkDevice reInstance(Context ctx) {
        manager = null;
        return inkManager(ctx);
    }

    private static IInkDevice getManager(Context ctx) {

        String inkDev = PlatformInfo.getInkDevice();

        if (inkDev == PlatformInfo.DEVICE_SMARTCARD) {
            return new SmartCardManager(ctx);
        } else {
            return new RFIDManager(ctx);
        }
    }
}
