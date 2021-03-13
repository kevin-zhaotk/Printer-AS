package com.industry.printer.Serial;

import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2021/3/7.
 */

public class CRC16_MODBUS {
    private static String TAG = CRC16_MODBUS.class.getSimpleName();

    private static int CRC16_BASE = 0x0000A001; // 1000 0000 0000 0101(0x8005); 颠倒过来就是 1010 0000 0000 0001 = 0xA001

    public static short getCRCCode(byte[] data) {
        if (null == data || data.length <= 0) {
            return 0;
        }

        int crcCode = 0x0000FFFF;

        for(int i = 0; i < data.length; i++) {
            crcCode ^= ((int) data[i] & 0x000000FF);
            for(int j = 0; j < 8; j++) {
                if ((crcCode & 0x00000001) != 0) {
                    crcCode >>= 1;
                    crcCode ^= CRC16_BASE;
                } else {
                    crcCode >>= 1;
                }
            }
        }

        Debug.d(TAG, "crcCode = " + Integer.toHexString(crcCode & 0x0000FFFF));
        return (short)(crcCode & 0x0000FFFF);
    }
}
