package com.industry.printer.Rfid;

import android.content.Context;

import com.industry.printer.hardware.InkManagerFactory;

public class InkSchedulerFactory {

    private static volatile IInkScheduler scheduler = null;

    public static IInkScheduler getScheduler(Context ctx) {
        if (scheduler == null) {
            synchronized (InkManagerFactory.class) {
                if (scheduler == null) {
                    scheduler = create(ctx);
                }
            }
        }
        return scheduler;
    }

    private static IInkScheduler create(Context ctx) {
        if (true) {
            return new RfidScheduler(ctx);
        } else {
            return new SmardCardScheduler(ctx);
        }
    }
}
