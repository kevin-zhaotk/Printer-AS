package com.industry.printer.hardware;

import android.content.Context;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;

/**
 * factory for ink device manager: RFID, SmartCard
 */
public class InkManagerFactory {

    private static volatile IInkDevice manager = null;
    private static final String TAG = InkManagerFactory.class.getSimpleName();

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
        Debug.d(TAG, "--->Platform: " + inkDev);
        if (PlatformInfo.DEVICE_SMARTCARD.equals(inkDev)) {
              return new SmartCardManager(ctx);
//            return new RFIDManager(ctx);
        } else {
// H.M.Wang 2022-1-20 根据SmartCard是否连接来判断走SC还是RFID
            if(SmartCard.exist(PlatformInfo.getImgUniqueCode().startsWith("M") ? 1 : 0) == SmartCard.SC_SUCCESS) {
                return new SmartCardManager(ctx);
            }
// End of H.M.Wang 2022-1-20 根据SmartCard是否连接来判断走SC还是RFID
            return new RFIDManager(ctx);
        }
    }
}
