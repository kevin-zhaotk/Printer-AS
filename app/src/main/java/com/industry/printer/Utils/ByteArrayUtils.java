package com.industry.printer.Utils;

/**
 * Created by hmwan on 2019/10/26.
 */

public class ByteArrayUtils {
    public static String toHexString(byte[] buf) {
        if (buf == null) return "";
        final String HEX = "0123456789ABCDEF";
        StringBuffer result = new StringBuffer(2 * buf.length);
        for (int i = 0; i<buf.length; i++) {
            result.append(i==0?"":" ").append(HEX.charAt((buf[i] >> 4) & 0x0f)).append(HEX.charAt(buf[i] & 0x0f));
        }
        return result.toString();
    }

    public static byte[] pickPartial(byte[] msg, int start, int length) {
        byte[] retBytes = new byte[length];
        System.arraycopy(msg, start, retBytes, 0, length);
        return retBytes;
    }
}
