package com.industry.printer.Socket_Server;

import android.text.TextUtils;

public class PCCommand {

    /** 包头 */
    public String header;

    /** 设备号 Device number */
    public String deviceId;

    /** 命令字 Command number  */
    public String command;


    /** 字符串 content */
    public String content;

    /** 数据包大小 */
    public String size;

    /** 结束标志 */
    public String note;

    /**Note 1 */
    public String note2;

    /** 校验位 */
    public String check;

    /** 包尾 */
    public String end;

    public static PCCommand fromString(String message) {
        if (TextUtils.isEmpty(message) ) {
            return  null;
        }

        PCCommand cmd = new PCCommand();
        String[] f = message.split("\\|");
        if (f.length > 0) {
            cmd.header = f[0];
        }
        if (f.length > 1) {
            cmd.deviceId = f[1];
        }

        if (f.length > 2) {
            cmd.command = f[2];
        }
        if (f.length > 3) {
            cmd.content = f[3];
        }
        if (f.length > 4) {
            cmd.size = f[4];
        }

        if (f.length > 5) {
            cmd.note = f[5];
        }

        if (f.length > 6) {
            cmd.note2 = f[6];
        }

        if (f.length > 7) {
            cmd.check = f[7];
        }


        if (f.length > 8) {
            cmd.end = f[8];
        }
        return cmd;
    }
}
