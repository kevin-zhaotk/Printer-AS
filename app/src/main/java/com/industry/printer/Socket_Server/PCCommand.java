package com.industry.printer.Socket_Server;

import android.text.TextUtils;

public class PCCommand {


    /** 打印 */
    public static final String CMD_PRINT = "100";

    /** 清洗 */
    public static final String CMD_CLEAN = "200";

    /** 发送文件 */
    public static final String CMD_SEND_FILE = "300";

    /** 取计数器 */
    public static final String CMD_READ_COUNTER = "400";

    /** 停止打印 */
    public static final String CMD_STOP_PRINT = "500";

    /** 设置计数器 */
    public static final String CMD_SET_REMOTE = "600";

    /** 生成tlk */
    public static final String CMD_MAKE_TLK = "700";

    /** 删除文件 */
    public static final String CMD_DEL_FILE = "800";

    /** 删除文件 */
    public static final String CMD_DEL_DIR = "900";

    /** PC 发送bin文件 */
    public static final String CMD_SEND_BIN = "1000";

    /** 删除指定index的bin */
    public static final String CMD_DEL_LAN_BIN = "1100";

    /** 打印归零 */
    public static final String CMD_RESET_INDEX = "1200";

    // H.M.Wang 2019-12-25 追加速度和清洗命令
    /** 设置速度 */
    public static final String CMD_SET_DOTSIZE = "dotsize";

    /** 打印归零 */
    public static final String CMD_SET_CLEAN = "Clean";
    // End of H.M.Wang 2019-12-25 追加速度和清洗命令

// H.M.Wang 2020-7-1 追加一个计数器设置数值命令
    public static final String CMD_SET_COUNTER = "Counter";
// End of H.M.Wang 2020-7-1 追加一个计数器设置数值命令

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
