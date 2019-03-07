package com.industry.printer.PHeader;


public enum PrinterNozzle {


    MESSAGE_TYPE_12_7(MessageType.NOZZLE_INDEX_12_7, 0, 1, 1),         //12.7 单头（12.7），one segment，
    MESSAGE_TYPE_25_4(MessageType.NOZZLE_INDEX_25_4, 2, 2, 2),         //25.4 double Nozzle（12.7X2）,  double RFID，double segment
    MESSAGE_TYPE_38_1(MessageType.NOZZLE_INDEX_38_1, 5, 3, 3),         //38.1 triple RFID（12.7X3），triple segment
    MESSAGE_TYPE_50_8(MessageType.NOZZLE_INDEX_50_8, 6, 4, 4),         //50.8 fourfold RFID（12.7X4），fourfold segment
    MESSAGE_TYPE_32_DOT(MessageType.NOZZLE_INDEX_16_DOT, 7, 1, 1),       //big data; single Nozzle, single RFID, 4 segments
    MESSAGE_TYPE_16_DOT(MessageType.NOZZLE_INDEX_32_DOT, 8, 1, 1),       // big data; single Nozzle, single RFID, 4 segments
    MESSAGE_TYPE_1_INCH(MessageType.NOZZLE_INDEX_1_INCH, 10, 1, 1),       // 1寸（25.4），单头，one segment
    MESSAGE_TYPE_1_INCH_DUAL(MessageType.NOZZLE_INDEX_1_INCH_DUAL, 12, 2, 2),// 1寸双头（25.4X2）； 2 RFID，2 segments
    MESSAGE_TYPE_1_INCH_TRIPLE(MessageType.NOZZLE_INDEX_1_INCH_TRIPLE, 18, 3, 3),         //25.4 double Nozzle（12.7X2）,  double RFID，double segment
    MESSAGE_TYPE_1_INCH_FOUR(MessageType.NOZZLE_INDEX_1_INCH_FOUR, 19, 4, 4);

    public final int mIndex;
    public final int mType;
    public final int mHeads;
    public final int mSegments;
    public final boolean editZoomable; // 编辑状态下是否支持缩放

    private int mHeight;
    private float scaleW;
    private float scaleH;


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
    private PrinterNozzle(int index, int type, int heads, int segment) {

        this.mIndex = index;
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

        initScale();
        initHeight();
    }

    private void initHeight() {

        switch (mIndex) {
            case MessageType.NOZZLE_INDEX_12_7:
                mHeight = 152;
                break;
            case MessageType.NOZZLE_INDEX_25_4:
                mHeight = 152 * 2;
                break;
            case MessageType.NOZZLE_INDEX_38_1:
                mHeight = 152 * 3;
                break;
            case MessageType.NOZZLE_INDEX_50_8:
                mHeight = 152 * 4;
                break;
            case MessageType.NOZZLE_INDEX_16_DOT:
            case MessageType.NOZZLE_INDEX_32_DOT:
                mHeight = 32;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH:
                mHeight = 320;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH_DUAL:
                mHeight = 640;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH_TRIPLE:
                mHeight = 320 * 3;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH_FOUR:
                mHeight = 320 * 4;
                break;
        }
    }

    public int getHeight() {
        return mHeight;
    }

    private void initScale() {
        switch (mIndex) {
            case MessageType.NOZZLE_INDEX_12_7:
                scaleW = 1f;
                scaleH = 1f;
                break;
            case MessageType.NOZZLE_INDEX_25_4:
                scaleW = 2f;
                scaleH = 2f;
                break;
            case MessageType.NOZZLE_INDEX_38_1:
                scaleW = 3f;
                scaleH = 3f;
                break;
            case MessageType.NOZZLE_INDEX_50_8:
                scaleW = 4f;
                scaleH = 4f;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH:
                scaleW = 2f;
                scaleH = 2f;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH_DUAL:
                scaleW = 4f;
                scaleH = 4f;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH_TRIPLE:
                scaleW = 6f;
                scaleH = 6f;
                break;
            case MessageType.NOZZLE_INDEX_1_INCH_FOUR:
                scaleW = 8f;
                scaleH = 8f;
                break;
            case MessageType.NOZZLE_INDEX_16_DOT:
                scaleW = 16f/152;
                scaleH = 16f/152;
                break;
            case MessageType.NOZZLE_INDEX_32_DOT:
                scaleW = 32f/152;
                scaleH = 32f/152;
                break;
            default:
                scaleW = 1f;
                scaleH = 1f;
                break;
        }
    }

    public  float getScaleW() {
        return scaleW;
    }

    public  float getScaleH() {
        return scaleH;
    }

    public static PrinterNozzle getInstance(int type) {
        switch (type) {
            case MessageType.NOZZLE_INDEX_12_7:
                return MESSAGE_TYPE_12_7;
            case MessageType.NOZZLE_INDEX_25_4:
                return MESSAGE_TYPE_25_4;
            case MessageType.NOZZLE_INDEX_38_1:
                return MESSAGE_TYPE_38_1;
            case MessageType.NOZZLE_INDEX_50_8:
                return MESSAGE_TYPE_50_8;
            case MessageType.NOZZLE_INDEX_16_DOT:
                return MESSAGE_TYPE_32_DOT;
            case MessageType.NOZZLE_INDEX_32_DOT:
                return MESSAGE_TYPE_16_DOT;
            case MessageType.NOZZLE_INDEX_1_INCH:
                return MESSAGE_TYPE_1_INCH;
            case MessageType.NOZZLE_INDEX_1_INCH_DUAL:
                return MESSAGE_TYPE_1_INCH_DUAL;
            case MessageType.NOZZLE_INDEX_1_INCH_TRIPLE:
                return MESSAGE_TYPE_1_INCH_TRIPLE;
            case MessageType.NOZZLE_INDEX_1_INCH_FOUR:
                return MESSAGE_TYPE_1_INCH_FOUR;
            default:
                return MESSAGE_TYPE_12_7;

        }
    }

    public static PrinterNozzle getByType(int type) {
        switch (type) {
            case 0:
                return MESSAGE_TYPE_12_7;
            case 2:
                return MESSAGE_TYPE_25_4;
            case 5:
                return MESSAGE_TYPE_38_1;
            case 6:
                return MESSAGE_TYPE_50_8;
            case 7:
                return MESSAGE_TYPE_16_DOT;
            case 8:
                return MESSAGE_TYPE_32_DOT;
            case 9:
                return MESSAGE_TYPE_1_INCH;
            case 11:
                return MESSAGE_TYPE_1_INCH_DUAL;
            case 15:
                return MESSAGE_TYPE_1_INCH_TRIPLE;
            case 16:
                return MESSAGE_TYPE_1_INCH_FOUR;
            default:
                return MESSAGE_TYPE_12_7;
        }
    }

    /**
     * Nozzle index; this index must match with definition in array resource <string-array name="strPrinterArray"></string-array>
     */
    public static class MessageType {
        public static final int NOZZLE_INDEX_12_7 	= 0;    // 12.7
        public static final int NOZZLE_INDEX_25_4 	= 1;    // 12.7X2
        public static final int NOZZLE_INDEX_38_1  	= 2;    // 12.7x3
        public static final int NOZZLE_INDEX_50_8  	= 3;    // 12.7x4
        public static final int NOZZLE_INDEX_1_INCH = 4;    // 1 inch
        public static final int NOZZLE_INDEX_1_INCH_DUAL = 5; // 1inch X2
        public static final int NOZZLE_INDEX_1_INCH_TRIPLE = 6; // 1inch X2
        public static final int NOZZLE_INDEX_1_INCH_FOUR = 7; // 1inch X2
        public static final int NOZZLE_INDEX_16_DOT  = 8;   // 16dot
        public static final int NOZZLE_INDEX_32_DOT  = 9;   // 32 dot
    }

    public static class NozzleType {
        public static final int NOZZLE_TYPE_12_7 = 0;
        public static final int NOZZLE_TYPE_25_4 = 2;
        public static final int NOZZLE_TYPE_38_1 = 5;
        public static final int NOZZLE_TYPE_50_8 = 6;
        public static final int NOZZLE_TYPE_32_DOT = 7;
        public static final int NOZZLE_TYPE_16_DOT = 8;
        public static final int NOZZLE_TYPE_1_INCH = 10;
        public static final int NOZZLE_TYPE_1_INCH_DUAL = 12;
        public static final int NOZZLE_TYPE_1_INCH_TRIPLE = 18;
        public static final int NOZZLE_TYPE_1_INCH_FOUR = 19;

    }
}
