package com.industry.printer.Serial;

import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2019/10/25.
 */

public class CRC16_X25 {
    private static String TAG = CRC16_X25.class.getSimpleName();

    private static int CRC16_BASE = 0x00008408;

    public static short getCRCCode(byte data[]) {
        if (null == data || data.length <= 0) {
            return 0;
        }

        int crcCode = 0x0000FFFF;

        for (int i=0; i<data.length; i++) {
            crcCode ^= (int)(data[i] & 0x000000ff);

            for (int j = 0; j < 8; j++) {
                if ((crcCode & 0x00000001) == 0x00000001) {
                    crcCode >>= 1;
                    crcCode ^= CRC16_BASE;
                } else {
                    crcCode >>= 1;
                }
            }
        }

        crcCode ^= 0x0000FFFF;

        Debug.d(TAG, "crcCode = " + Integer.toHexString(crcCode & 0x0000FFFF));

        return (short)(crcCode & 0x0000FFFF);
    }
}
