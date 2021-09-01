package com.industry.printer.Rfid;

import android.content.Context;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;

public class SmardCardScheduler implements IInkScheduler {

    public SmardCardScheduler(Context ctx) {

    }

    @Override
    public void init(int heads) {

    }

    @Override
    public int count() {
// H.M.Wang 2021-8-31 修改P2不减锁的问题，这里在有P2的时候应该返回2
        if(SystemConfigFile.getInstance().getPNozzle() == PrinterNozzle.MESSAGE_TYPE_25_4) {
            return 2;
        }
// End of H.M.Wang 2021-8-31 修改P2不减锁的问题，这里在有P2的时候应该返回2
        return 1;
    }

    @Override
    public void schedule() {

    }

    @Override
    public void doAfterPrint() {

    }
}
