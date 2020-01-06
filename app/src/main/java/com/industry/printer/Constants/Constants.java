package com.industry.printer.Constants;

public class Constants {

    /************************ PC ********************************/
    // communication between PC and Android
    public static final String PC_CMD = "pcCommand";

    // result ok
    public static final String PC_RESULT_OK = "0000-ok: ";

    // result error
    public static final String PC_RESULT_ERR = "1111-error: ";


    public static String pcOk(String command) {
        return PC_RESULT_OK + command;
    }

    public static String pcErr(String command) {
        return PC_RESULT_ERR + command;
    }

    /************************ PC ********************************/


    public static final int LOG_ENABLE = 1234;

}
