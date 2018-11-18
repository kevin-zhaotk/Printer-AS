package com.industry.printer.PHeader;

import com.industry.printer.MessageTask;

public enum PrinterNozzle {


    MESSAGE_TYPE_12_7(0, 1, 1),         //12.7 单头，one segment
    MESSAGE_TYPE_12_7_S(1, 1, 1),       //12.7 单头，one segment
    MESSAGE_TYPE_25_4(2, 2, 2),         //25.4 double Nozzle,  double RFID，double segment
    MESSAGE_TYPE_16_3(3, 1, 1),         //16.3 single Nozzle, single RFID，double segment
    MESSAGE_TYPE_33(4, 2, 2),           //33   double RFID，double segment
    MESSAGE_TYPE_38_1(5, 3, 3),         //38.1 triple RFID，triple segment
    MESSAGE_TYPE_50_8(6, 4, 4),         //50.8 fourfold RFID，fourfold segment
    MESSAGE_TYPE_32_DOT(7, 1, 1),       //big data; single Nozzle, single RFID, 4 segments
    MESSAGE_TYPE_16_DOT(8, 1, 1),       // big data; single Nozzle, single RFID, 4 segments
    MESSAGE_TYPE_1_INCH(9, 1, 1),       // 1寸，单头，one segment
    MESSAGE_TYPE_1_INCH_FAST(10, 1, 1), // 1寸，单头，one segment

    MESSAGE_TYPE_1_INCH_DUAL(11, 2, 2), // 1寸双头； 2 RFID，2 segments
    MESSAGE_TYPE_1_INCH_DUAL_FAST(12, 2, 2),// 1寸双头； 2 RFID，2 segments
    MESSAGE_TYPE_9MM(13, 1, 1),         // 9MM； 1 RFID，1 segments
    MESSAGE_TYPE_NOVA(14, 6, 6);        // Nova; 6 RFID，6 segments

    public final int mType;
    public final int mHeads;
    public final int mSegments;
    public final boolean editZoomable; // 编辑状态下是否支持缩放

    public final boolean shiftEnable;   //是否支持位移
    public final boolean mirrorEnable;  // 是否支持镜像
    public final boolean reverseEnable; // 是否支持反转
    public final boolean rotateAble;
    public final boolean buffer8Enable; // buffer 8 times extension Switch
    /**
     *
     * @param type      喷头类型
     * @param heads     喷头数
     * @param segment   打印分段
     */
    private PrinterNozzle(int type, int heads, int segment) {

        this.mType = type;
        this.mHeads = heads;
        this.mSegments = segment;

        switch (mType) {
            case 8: //大字机
                reverseEnable = true;
                shiftEnable = true;
                mirrorEnable = true;
                rotateAble = true;
                break;
            /**
             * for 'Nova' header, shift & mirror is forbiden;
             */
            case 14:
                shiftEnable = false;
                mirrorEnable = false;
                reverseEnable = false;
                rotateAble = false;
                break;
            default:
                shiftEnable = true;
                mirrorEnable = true;
                reverseEnable = false;
                rotateAble = false;
                break;
        }

        // some features during editing
        switch (mType) {
            case 7:
            case 8:
                editZoomable = false;
                buffer8Enable = true;
                break;
            default:
                editZoomable = true;
                buffer8Enable = false;
                break;
        }

    }


    public static PrinterNozzle getInstance(int type) {
        switch (type) {
            case 0:
                return MESSAGE_TYPE_12_7;
            case 1:
                return MESSAGE_TYPE_12_7_S;
            case 2:
                return MESSAGE_TYPE_25_4;
            case 3:
                return MESSAGE_TYPE_16_3;
            case 4:
                return MESSAGE_TYPE_33;
            case 5:
                return MESSAGE_TYPE_38_1;
            case 6:
                return MESSAGE_TYPE_50_8;
            case 7:
                return MESSAGE_TYPE_32_DOT;
            case 8:
                return MESSAGE_TYPE_16_DOT;
            case 9:
                return MESSAGE_TYPE_1_INCH;
            case 10:
                return MESSAGE_TYPE_1_INCH_FAST;
            case 11:
                return MESSAGE_TYPE_1_INCH_DUAL;
            case 12:
                return MESSAGE_TYPE_1_INCH_DUAL_FAST;
            case 13:
                return MESSAGE_TYPE_9MM;
            case 14:
                return MESSAGE_TYPE_NOVA;
            default:

                return MESSAGE_TYPE_12_7;

        }
    }
}
