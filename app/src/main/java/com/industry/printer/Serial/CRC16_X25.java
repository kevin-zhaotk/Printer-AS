package com.industry.printer.Serial;

import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2019/10/25.
 */

public class CRC16_X25 {
    private static String TAG = CRC16_X25.class.getSimpleName();

    private static int CRC16_BASE = 0x8408;

    public static short getCRCCode(byte data[]) {
        if (null == data || data.length <= 0) {
            return 0;
        }

        short crcCode = (short) 0xFFFF;

        int xBit;
        for (int i=0; i<data.length; i++) {
            crcCode ^= data[i];
            for (int j = 0; j < 8; j++) {
                xBit = (int) (crcCode & 0x01);
                crcCode >>= 1;
                crcCode &= 0x7FFF;
                if (xBit == 0x01) {
                    crcCode ^= CRC16_BASE;
                }
            }
        }

        crcCode ^= 0xFFFF;

        Debug.d(TAG, "crcCode = " + Integer.toHexString(crcCode & 0x0000FFFF));

        return (short)(crcCode & 0x0000FFFF);
    }
}
