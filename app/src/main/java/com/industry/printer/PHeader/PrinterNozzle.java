package com.industry.printer.PHeader;

import com.industry.printer.FileFormat.SystemConfigFile;

public enum PrinterNozzle {
    MESSAGE_TYPE_12_7(MessageType.NOZZLE_INDEX_12_7, NozzleType.NOZZLE_TYPE_12_7, 1, 1),         //12.7 单头（12.7），one segment，
    MESSAGE_TYPE_25_4(MessageType.NOZZLE_INDEX_25_4, NozzleType.NOZZLE_TYPE_25_4, 2, 2),         //25.4 double Nozzle（12.7X2）,  double RFID，double segment
    MESSAGE_TYPE_38_1(MessageType.NOZZLE_INDEX_38_1, NozzleType.NOZZLE_TYPE_38_1, 3, 3),         //38.1 triple RFID（12.7X3），triple segment
    MESSAGE_TYPE_50_8(MessageType.NOZZLE_INDEX_50_8, NozzleType.NOZZLE_TYPE_50_8, 4, 4),         //50.8 fourfold RFID（12.7X4），fourfold segment
    MESSAGE_TYPE_16_DOT(MessageType.NOZZLE_INDEX_16_DOT, NozzleType.NOZZLE_TYPE_16_DOT, 1, 1),       //big data; single Nozzle, single RFID, 4 segments
    MESSAGE_TYPE_32_DOT(MessageType.NOZZLE_INDEX_32_DOT, NozzleType.NOZZLE_TYPE_32_DOT, 1, 1),       // big data; single Nozzle, single RFID, 4 segments

    // H.M.Wang 追加下列一行
    MESSAGE_TYPE_64_DOT(MessageType.NOZZLE_INDEX_64_DOT, NozzleType.NOZZLE_TYPE_64_DOT, 1, 1),       // big data; single Nozzle, single RFID, 4 segments

    MESSAGE_TYPE_1_INCH(MessageType.NOZZLE_INDEX_1_INCH, NozzleType.NOZZLE_TYPE_1_INCH, 1, 1),       // 1寸（25.4），单头，one segment
    MESSAGE_TYPE_1_INCH_DUAL(MessageType.NOZZLE_INDEX_1_INCH_DUAL, NozzleType.NOZZLE_TYPE_1_INCH_DUAL, 2, 2),// 1寸双头（25.4X2）； 2 RFID，2 segments
    MESSAGE_TYPE_1_INCH_TRIPLE(MessageType.NOZZLE_INDEX_1_INCH_TRIPLE, NozzleType.NOZZLE_TYPE_1_INCH_TRIPLE, 3, 3),         //25.4 double Nozzle（12.7X2）,  double RFID，double segment
    MESSAGE_TYPE_1_INCH_FOUR(MessageType.NOZZLE_INDEX_1_INCH_FOUR, NozzleType.NOZZLE_TYPE_1_INCH_FOUR, 4, 4),

    // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//    MESSAGE_TYPE_12_7_R5(MessageType.NOZZLE_INDEX_12_7_R5, NozzleType.NOZZLE_TYPE_12_7_R5, 1, 1);
    MESSAGE_TYPE_R6X48(MessageType.NOZZLE_INDEX_R6X48, NozzleType.NOZZLE_TYPE_R6X48, 1, 1),
    MESSAGE_TYPE_R6X50(MessageType.NOZZLE_INDEX_R6X50, NozzleType.NOZZLE_TYPE_R6X50, 1, 1),
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
    // End of H.M.Wang 2020-4-17 追加12.7R5头类型

// H.M.Wang 2020-7-23 追加32DN打印头
    MESSAGE_TYPE_32DN(MessageType.NOZZLE_INDEX_32DN, NozzleType.NOZZLE_TYPE_32DN, 1, 1),
// End of H.M.Wang 2020-7-23 追加32DN打印头

// H.M.Wang 2020-8-14 追加32SN打印头
    MESSAGE_TYPE_32SN(MessageType.NOZZLE_INDEX_32SN, NozzleType.NOZZLE_TYPE_32SN, 1, 1),
// End of H.M.Wang 2020-8-14 追加32SN打印头

// H.M.Wang 2020-8-26 追加64SN打印头
    MESSAGE_TYPE_64SN(MessageType.NOZZLE_INDEX_64SN, NozzleType.NOZZLE_TYPE_64SN, 1, 1),
// End of H.M.Wang 2020-8-26 追加64SN打印头

// H.M.Wang 2021-1-19 追加9mm打印头
    MESSAGE_TYPE_9MM(MessageType.NOZZLE_INDEX_9MM, NozzleType.NOZZLE_TYPE_9MM, 1, 1),
// End of H.M.Wang 2021-1-19 追加9mm打印头

// H.M.Wang 2021-3-6 追加E6X48,E6X50头
    MESSAGE_TYPE_E6X48(MessageType.NOZZLE_INDEX_E6X48, NozzleType.NOZZLE_TYPE_E6X48, 1, 1),
    MESSAGE_TYPE_E6X50(MessageType.NOZZLE_INDEX_E6X50, NozzleType.NOZZLE_TYPE_E6X50, 1, 1),
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
    MESSAGE_TYPE_E6X1(MessageType.NOZZLE_INDEX_E6X1, NozzleType.NOZZLE_TYPE_E6X1, 1, 1),
// H.M.Wang 2021-8-16 追加96DN头
    MESSAGE_TYPE_96DN(MessageType.NOZZLE_INDEX_96DN, NozzleType.NOZZLE_TYPE_96DN, 1, 1),
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
    MESSAGE_TYPE_E5X48(MessageType.NOZZLE_INDEX_E5X48, NozzleType.NOZZLE_TYPE_E5X48, 1, 1),
    MESSAGE_TYPE_E5X50(MessageType.NOZZLE_INDEX_E5X50, NozzleType.NOZZLE_TYPE_E5X50, 1, 1),
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2022-4-29 追加25.4x10头类型
    MESSAGE_TYPE_254X10(MessageType.NOZZLE_INDEX_254X10, NozzleType.NOZZLE_TYPE_254X10, 1, 1);//10,10
// End of H.M.Wang 2022-4-29 追加25.4x10头类型

    public final int mIndex;
    public final int mType;
    public final int mHeads;
    public final int mSegments;         // 好像没有被用到 - H.M Wang
    public final boolean editZoomable; // 编辑状态下是否支持缩放

    private int mHeight;
    private float scaleW;
    private float scaleH;
    // 生成变量buffer bin时，宽度缩放因子, 只有16点和32点不需要缩放，其他喷头减半
    private int factorScale;

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
            case NozzleType.NOZZLE_TYPE_16_DOT:
            case NozzleType.NOZZLE_TYPE_32_DOT://大字机
// H.M.Wang 2020-3-2 修改64点头，不支持反转和镜像
                reverseEnable = true;
                shiftEnable = true;
                mirrorEnable = true;
                rotateAble = true;
                break;
// H.M.Wang 2020-7-23 追加32DN打印头
            case NozzleType.NOZZLE_TYPE_32DN:
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-18 追加32SN打印头
            case NozzleType.NOZZLE_TYPE_32SN:
// End of H.M.Wang 2020-8-18 追加32SN打印头
            // H.M.Wang 追加下列一行
            case NozzleType.NOZZLE_TYPE_64_DOT:
// H.M.Wang 2020-8-26 追加64SN打印头
            case NozzleType.NOZZLE_TYPE_64SN:
// End of H.M.Wang 2020-8-26 追加64SN打印头
// H.M.Wang 2021-8-16 追加96DN头
            case NozzleType.NOZZLE_TYPE_96DN:
// End of H.M.Wang 2021-8-16 追加96DN头
                reverseEnable = false;
//                reverseEnable = true;
                shiftEnable = true;
                mirrorEnable = false;
//                mirrorEnable = true;
                rotateAble = true;
                break;
// End of H.M.Wang 2020-3-2 修改64点头，不支持反转和镜像
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
            case NozzleType.NOZZLE_TYPE_16_DOT:
            case NozzleType.NOZZLE_TYPE_32_DOT:

// H.M.Wang 2020-7-23 追加32DN打印头
            case NozzleType.NOZZLE_TYPE_32DN:
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-18 追加32SN打印头
            case NozzleType.NOZZLE_TYPE_32SN:
// End of H.M.Wang 2020-8-18 追加32SN打印头
                // H.M.Wang 追加下列一行
            case NozzleType.NOZZLE_TYPE_64_DOT:
// H.M.Wang 2020-8-26 追加64SN打印头
            case NozzleType.NOZZLE_TYPE_64SN:
// End of H.M.Wang 2020-8-26 追加64SN打印头
// H.M.Wang 2021-8-16 追加96DN头
            case NozzleType.NOZZLE_TYPE_96DN:
// End of H.M.Wang 2021-8-16 追加96DN头
                editZoomable = false;
                buffer8Enable = true;
                factorScale = 1;
                break;
            default:
                factorScale = 2;
                editZoomable = true;
                buffer8Enable = false;
                break;
        }

        initScale();
        initHeight();
    }

    /*
        这里的高度是实际打印时的打印缓冲区的高度。但是12.7xN头，由于历史原因，在下发之前，在BinInfo里面会做19字节到20字节的补位
     */
    private void initHeight() {
        switch (mType) {
            case NozzleType.NOZZLE_TYPE_12_7:
            // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//            case NozzleType.NOZZLE_TYPE_12_7_R5:
            case NozzleType.NOZZLE_TYPE_R6X48:
            case NozzleType.NOZZLE_TYPE_R6X50:
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
            // End of H.M.Wang 2020-4-17 追加12.7R5头类型
                mHeight = 152;
                break;
            case NozzleType.NOZZLE_TYPE_25_4:
                mHeight = 152 * 2;
                break;
            case NozzleType.NOZZLE_TYPE_38_1:
                mHeight = 152 * 3;
                break;
            case NozzleType.NOZZLE_TYPE_50_8:
                mHeight = 152 * 4;
                break;
            case NozzleType.NOZZLE_TYPE_16_DOT:
            case NozzleType.NOZZLE_TYPE_32_DOT:
// H.M.Wang 2020-7-23 追加32DN打印头
            case NozzleType.NOZZLE_TYPE_32DN:
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-18 追加32SN打印头
            case NozzleType.NOZZLE_TYPE_32SN:
// End of H.M.Wang 2020-8-18 追加32SN打印头
                mHeight = 32;
                break;

            // H.M.Wang 追加下列三行
            case NozzleType.NOZZLE_TYPE_64_DOT:
// H.M.Wang 2020-8-26 追加64SN打印头
            case NozzleType.NOZZLE_TYPE_64SN:
// End of H.M.Wang 2020-8-26 追加64SN打印头
                mHeight = 64;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH:
                mHeight = 320;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_DUAL:
                mHeight = 640;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_TRIPLE:
                mHeight = 320 * 3;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_FOUR:
                mHeight = 320 * 4;
                break;
            case NozzleType.NOZZLE_TYPE_9MM:
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X48:
            case NozzleType.NOZZLE_TYPE_E6X50:
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X1:
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
            case NozzleType.NOZZLE_TYPE_E5X48:
            case NozzleType.NOZZLE_TYPE_E5X50:
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
                mHeight = 112;
                break;
// H.M.Wang 2021-8-16 追加96DN头
            case NozzleType.NOZZLE_TYPE_96DN:
                mHeight = 96;
                break;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2022-4-29 追加25.4x10头类型
            case NozzleType.NOZZLE_TYPE_254X10:
                mHeight = 3200;
                break;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
        }
    }

    public int getHeight() {
        return mHeight;
    }

    /*
        该函数取得的scaleX值是为了在标准内部坐标（152）到打印的绘制区域之间的转换。具体的如下：
        NOZZLE_TYPE_12_7
        NOZZLE_TYPE_R6X48
        NOZZLE_TYPE_R6X50
            基础打印头为12.7打印头
            ratioX = 1.0f。换算成pixels为152，即高度按152点，19个字节，在下发的时候，补齐一个字节，凑足20个字节（因为FPGA下发是两个字节一下发，所以每列必须有偶数个字节）
            生成bin的时候，高度为152个点，19个字节
        NOZZLE_TYPE_25_4
            基础打印头为12.7x2打印头
            ratioX = 2.0f。换算成pixels为304，即高度按304点，每个头19个字节，在下发的时候，补齐一个字节，凑足20个字节（因为FPGA下发是两个字节一下发，所以每列必须有偶数个字节）
            生成bin的时候，高度为304个点，19x2个字节
        NOZZLE_TYPE_38_1
            基础打印头为12.7x3打印头
            ratioX = 3.0f。换算成pixels为456，即高度按456点，每个头19个字节，在下发的时候，补齐一个字节，凑足20个字节（因为FPGA下发是两个字节一下发，所以每列必须有偶数个字节）
            生成bin的时候，高度为456个点，19x3个字节
        NOZZLE_TYPE_50_8
            基础打印头为12.7x4打印头
            ratioX = 4.0f。换算成pixels为608，即高度按608点，每个头19个字节，在下发的时候，补齐一个字节，凑足20个字节（因为FPGA下发是两个字节一下发，所以每列必须有偶数个字节）
            生成bin的时候，高度为608个点，19x4个字节

        以上由于历史原因，每个单头补齐的一个字节是在BinInfo类里面动态实现的。其实，在绘图的时候按着152xN进行绘制，然后在每个头之间插入一个字节的空挡，保存bin的时候直接保存每个单头的数据为20字节的话就更加方便了，
        后面的25.4xN头就是这么实现的

        NOZZLE_TYPE_1_INCH
            基础打印头为25.4x1打印头
            ratioX = 1.0f * 308 / 152。换算成pixels为308，即在绘制的时候的高度是308，但是下发之前会在后面插入空行，调整为320个pixels的高度
            生成bin的时候，高度为320个点，即40个字节，其中只有38.5个字节是实际数据，后面的1.5个字节是补空
        NOZZLE_TYPE_1_INCH_DUAL:
        NOZZLE_TYPE_1_INCH_TRIPLE:
        NOZZLE_TYPE_1_INCH_FOUR:
            同理，参照NOZZLE_TYPE_1_INCH。纵向放大相应倍数
        NOZZLE_TYPE_16_DOT
            基础打印头为16DOT打印头（大字机）
            ratioX = 1.0f * 16 / 152。就是16个点高，绘制在这16点内，但是生成bin时会生成32点高，下部16点为空，下发时也是下发每列32点（4个字节）
        NOZZLE_TYPE_32_DOT
        NOZZLE_TYPE_32DN
        NOZZLE_TYPE_32SN
            基础打印头为32DOT打印头（大字机）
            ratioX = 1.0f * 32 / 152。就是32个点高，绘制在这32点内,并且生成bin时会生成32点高，下发时也是下发每列32点（4个字节）
        NOZZLE_TYPE_64_DOT
        NOZZLE_TYPE_64SN
            基础打印头为64DOT打印头（大字机）
            ratioX = 1.0f * 64 / 152。就是64个点高，绘制在这64点内,并且生成bin时会生成64点高，下发时也是下发每列64点（8个字节）
        NOZZLE_TYPE_96DN
            基础打印头为96DOT打印头（大字机）
            ratioX = 1.0f * 96 / 152。就是96个点高，绘制在这96点内,并且生成bin时会生成96点高，下发时也是下发每列64点（12个字节）
            case NozzleType.NOZZLE_TYPE_9MM:
        NOZZLE_TYPE_9MM
        NOZZLE_TYPE_E6X48
        NOZZLE_TYPE_E6X50
        NOZZLE_TYPE_E6X1
        NOZZLE_TYPE_E5X48
        NOZZLE_TYPE_E5X50
            基础打印头为9MM打印头
            ratioX = 1.0f * 104 / 152。就是104个点高，绘制在这104点内,生成bin时会生成112点高，下发时也是下发每列112点
     */
    private void initScale() {
        switch (mType) {
            case NozzleType.NOZZLE_TYPE_12_7:
            // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//            case NozzleType.NOZZLE_TYPE_12_7_R5:
            case NozzleType.NOZZLE_TYPE_R6X48:
            case NozzleType.NOZZLE_TYPE_R6X50:
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
            // End of H.M.Wang 2020-4-17 追加12.7R5头类型
                scaleW = 1f;
                scaleH = 1f;
                break;
            case NozzleType.NOZZLE_TYPE_25_4:
                scaleW = 2f;
                scaleH = 2f;
                break;
            case NozzleType.NOZZLE_TYPE_38_1:
                scaleW = 3f;
                scaleH = 3f;
                break;
            case NozzleType.NOZZLE_TYPE_50_8:
                scaleW = 4f;
                scaleH = 4f;
                break;
            // H.M.Wang 修改下列16行。调整25.4xn头的放大比例
            case NozzleType.NOZZLE_TYPE_1_INCH:
                scaleW = 1.0f * 308 / 152;
                scaleH = 1.0f * 308 / 152;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_DUAL:
                scaleW = 2.0f * 308 / 152;
                scaleH = 2.0f * 308 / 152;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_TRIPLE:
                scaleW = 3.0f * 308 / 152;
                scaleH = 3.0f * 308 / 152;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_FOUR:
                scaleW = 4.0f * 308 / 152;
                scaleH = 4.0f * 308 / 152;
                break;
            case NozzleType.NOZZLE_TYPE_16_DOT:
                scaleW = 16f/152;
                scaleH = 16f/152;
                break;
            case NozzleType.NOZZLE_TYPE_32_DOT:
// H.M.Wang 2020-7-23 追加32DN打印头
            case NozzleType.NOZZLE_TYPE_32DN:
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-18 追加32SN打印头
            case NozzleType.NOZZLE_TYPE_32SN:
// End of H.M.Wang 2020-8-18 追加32SN打印头
                scaleW = 32f/152;
                scaleH = 32f/152;
                break;

            // H.M.Wang 追加下列四行
            case NozzleType.NOZZLE_TYPE_64_DOT:
// H.M.Wang 2020-8-26 追加64SN打印头
            case NozzleType.NOZZLE_TYPE_64SN:
// End of H.M.Wang 2020-8-26 追加64SN打印头
                scaleW = 64f/152;
                scaleH = 64f/152;
                break;
            case NozzleType.NOZZLE_TYPE_9MM:
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X48:
            case NozzleType.NOZZLE_TYPE_E6X50:
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
// H.M.Wang 2021-3-30 还应该是104
// H.M.Wang 2021-3-22 原来的104没有考虑双字节，导致与高度设置的112不一致，生成的图片纵向变形
            case NozzleType.NOZZLE_TYPE_E6X1:
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
            case NozzleType.NOZZLE_TYPE_E5X48:
            case NozzleType.NOZZLE_TYPE_E5X50:
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
                scaleW = 104f/152;
                scaleH = 104f/152;
// End of H.M.Wang 2021-3-22 原来的104没有考虑双字节，导致与高度设置的112不一致，生成的图片纵向变形
// End of H.M.Wang 2021-3-30 还应该是104
                break;
// H.M.Wang 2021-8-16 追加96DN头
            case NozzleType.NOZZLE_TYPE_96DN:
                scaleW = 96f/152;
                scaleH = 96f/152;
                break;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2022-4-29 追加25.4x10头类型
            case NozzleType.NOZZLE_TYPE_254X10:
                scaleW = 10.0f * 308 / 152;
                scaleH = 10.0f * 308 / 152;
                break;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
            default:
                scaleW = 1f;
                scaleH = 1f;
                break;
        }
    }

    // H.M.Wang 追加这个函数以便将pixel和mm中间进行转化
    public float getPhisicalRatio() {
        float ratio = 0.0f;

        switch (mType) {
            case NozzleType.NOZZLE_TYPE_12_7:
            // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//            case NozzleType.NOZZLE_TYPE_12_7_R5:
            case NozzleType.NOZZLE_TYPE_R6X48:
            case NozzleType.NOZZLE_TYPE_R6X50:
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
            // End of H.M.Wang 2020-4-17 追加12.7R5头类型
                ratio = 1.0f * 12.7f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_25_4:
                ratio = 2.0f * 12.7f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_38_1:
                ratio = 3.0f * 12.7f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_50_8:
                ratio = 4.0f * 12.7f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH:
                ratio = 1.0f * 25.4f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_DUAL:
                ratio = 2.0f * 25.4f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_TRIPLE:
                ratio = 3.0f * 25.4f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_1_INCH_FOUR:
                ratio = 4.0f * 25.4f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_16_DOT:
                ratio = 16f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_32_DOT:
// H.M.Wang 2020-7-23 追加32DN打印头
            case NozzleType.NOZZLE_TYPE_32DN:
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-18 追加32SN打印头
            case NozzleType.NOZZLE_TYPE_32SN:
// End of H.M.Wang 2020-8-18 追加32SN打印头
                ratio = 32f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_64_DOT:
// H.M.Wang 2020-8-26 追加64SN打印头
            case NozzleType.NOZZLE_TYPE_64SN:
// End of H.M.Wang 2020-8-26 追加64SN打印头
                ratio = 64f / 304;
                break;
            case NozzleType.NOZZLE_TYPE_9MM:
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X48:
            case NozzleType.NOZZLE_TYPE_E6X50:
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X1:
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
            case NozzleType.NOZZLE_TYPE_E5X48:
            case NozzleType.NOZZLE_TYPE_E5X50:
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2021-3-22 根据其他头类型的算式，这里应该是208，而不是104
                ratio = 1.0f * 9.0f / 208;
// End of H.M.Wang 2021-3-22 根据其他头类型的算式，这里应该是208，而不是104
                break;
// H.M.Wang 2021-8-16 追加96DN头
            case NozzleType.NOZZLE_TYPE_96DN:
                ratio = 96f / 304;
                break;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2022-4-29 追加25.4x10头类型
            case NozzleType.NOZZLE_TYPE_254X10:
                ratio = 10.0f * 25.4f / 304;
                break;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
            default:
                break;
        }
        return ratio;
    }

    public  float getScaleW() {
        return scaleW;
    }

    public  float getScaleH() {
        return scaleH;
    }

    public int getFactorScale() {
        if (factorScale != 0) {
            return factorScale;
        } else {
            return 2;
        }
    }
    /**
     * Nozzle height multiple of 152
     */
    public int factor() {
        if(this == MESSAGE_TYPE_9MM) return 1;
        return mHeight / 152;
    }

// H.M.Wang 2020-4-17 追加12.7R5头类型
    // 返回对于R5头，横向复制的份数
    public static final int     R6_PRINT_COPY_NUM = 6;
    public static final int     R6_HEAD_NUM = 6;
    public static final int     R6X48_MAX_COL_NUM_EACH_UNIT = 48 * 6;
    public static final int     R6X50_MAX_COL_NUM_EACH_UNIT = 50 * 6;

// H.M.Wang 2021-3-6 追加E6X48,E6X50头
// H.M.Wang 2021-3-18 取消奇数行(第一行为0)的向后位移一个单位的操作)
    public static final int     E6_PRINT_COPY_NUM = 5;      // 没有单数行向后位移一个单位的操作，因此不像RX头一样，只复制5次
// End of H.M.Wang 2021-3-18 取消奇数行(第一行为0)的向后位移一个单位的操作)
    public static final int     E6_HEAD_NUM = 6;
    public static final int     E6X48_MAX_COL_NUM_EACH_UNIT = 48 * 6;
    public static final int     E6X50_MAX_COL_NUM_EACH_UNIT = 50 * 6;
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头

// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
    public static final int     E5_PRINT_COPY_NUM = 6;
    public static final int     E5_HEAD_NUM = 5;        // 减锁等等操作按着5个头来计算，但是生成打印缓冲区的时候生成相当于6个头的数据
    public static final int     E5X48_MAX_COL_NUM_EACH_UNIT = 48 * 6;
    public static final int     E5X50_MAX_COL_NUM_EACH_UNIT = 50 * 6;
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
/*    public int getRTimes() {
        if(mIndex == MessageType.NOZZLE_INDEX_12_7_R5) {
            return 6;
        } else {
            return 0;
        }
    }

    // 返回对于R5头，纵向复制的份数（对应的头数）
    public int getRHeads() {
        if(mIndex == MessageType.NOZZLE_INDEX_12_7_R5) {
            return 6;
        } else {
            return 0;
        }
    }

//    public int getRMaxCols(int ratio) {
    public int getRMaxCols() {
        if(mIndex == MessageType.NOZZLE_INDEX_12_7_R5) {
//            return 48 * 6 * ratio;
            return 48 * 6;
        } else {
            return 0;
        }
    }
*/
    // End of H.M.Wang 2020-4-17 追加12.7R5头类型

    public static PrinterNozzle getInstance(int index) {
// H.M.Wang 2020-7-27 修改打印头的获取方式，不依据信息中的打印头信息获取，而是依据系统参数设置获取
        SystemConfigFile configFile = SystemConfigFile.getInstance();
        if(null != configFile) {
            return configFile.getPNozzle();
        }
// H.M.Wang 2020-7-27 修改打印头的获取方式，不依据信息中的打印头信息获取，而是依据系统参数设置获取

        switch (index) {
            case MessageType.NOZZLE_INDEX_12_7:
                return MESSAGE_TYPE_12_7;
            case MessageType.NOZZLE_INDEX_25_4:
                return MESSAGE_TYPE_25_4;
            case MessageType.NOZZLE_INDEX_38_1:
                return MESSAGE_TYPE_38_1;
            case MessageType.NOZZLE_INDEX_50_8:
                return MESSAGE_TYPE_50_8;
            case MessageType.NOZZLE_INDEX_16_DOT:
                return MESSAGE_TYPE_16_DOT;
            case MessageType.NOZZLE_INDEX_32_DOT:
                return MESSAGE_TYPE_32_DOT;
// H.M.Wang 2020-7-23 追加32DN打印头
            case MessageType.NOZZLE_INDEX_32DN:
                return MESSAGE_TYPE_32DN;
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-17 追加32SN打印头
            case MessageType.NOZZLE_INDEX_32SN:
                return MESSAGE_TYPE_32SN;
// End of H.M.Wang 2020-8-17 追加32SN打印头

            // H.M.Wang 追加下列两行
            case MessageType.NOZZLE_INDEX_64_DOT:
                return MESSAGE_TYPE_64_DOT;
// H.M.Wang 2020-8-26 追加64SN打印头
            case MessageType.NOZZLE_INDEX_64SN:
                return MESSAGE_TYPE_64SN;
// End of H.M.Wang 2020-8-26 追加64SN打印头

            case MessageType.NOZZLE_INDEX_1_INCH:
                return MESSAGE_TYPE_1_INCH;
            case MessageType.NOZZLE_INDEX_1_INCH_DUAL:
                return MESSAGE_TYPE_1_INCH_DUAL;
            case MessageType.NOZZLE_INDEX_1_INCH_TRIPLE:
                return MESSAGE_TYPE_1_INCH_TRIPLE;
            case MessageType.NOZZLE_INDEX_1_INCH_FOUR:
                return MESSAGE_TYPE_1_INCH_FOUR;

            // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//            case MessageType.NOZZLE_INDEX_12_7_R5:
//                return MESSAGE_TYPE_12_7_R5;
            case MessageType.NOZZLE_INDEX_R6X48:
                return MESSAGE_TYPE_R6X48;
            case MessageType.NOZZLE_INDEX_R6X50:
                return MESSAGE_TYPE_R6X50;
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
// End of H.M.Wang 2020-4-17 追加12.7R5头类型
            case MessageType.NOZZLE_INDEX_9MM:
                return MESSAGE_TYPE_9MM;
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case MessageType.NOZZLE_INDEX_E6X48:
                return MESSAGE_TYPE_E6X48;
            case MessageType.NOZZLE_INDEX_E6X50:
                return MESSAGE_TYPE_E6X50;
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case MessageType.NOZZLE_INDEX_E6X1:
                return MESSAGE_TYPE_E6X1;
// H.M.Wang 2021-8-16 追加96DN头
            case MessageType.NOZZLE_INDEX_96DN:
                return MESSAGE_TYPE_96DN;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
            case MessageType.NOZZLE_INDEX_E5X48:
                return MESSAGE_TYPE_E5X48;
            case MessageType.NOZZLE_INDEX_E5X50:
                return MESSAGE_TYPE_E5X50;
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2022-4-29 追加25.4x10头类型
            case NozzleType.NOZZLE_TYPE_254X10:
                return MESSAGE_TYPE_254X10;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
            default:
                return MESSAGE_TYPE_12_7;

        }
    }

    public static PrinterNozzle getByType(int type) {
        switch (type) {
            case NozzleType.NOZZLE_TYPE_12_7:
                return MESSAGE_TYPE_12_7;
            case NozzleType.NOZZLE_TYPE_25_4:
                return MESSAGE_TYPE_25_4;
            case NozzleType.NOZZLE_TYPE_38_1:
                return MESSAGE_TYPE_38_1;
            case NozzleType.NOZZLE_TYPE_50_8:
                return MESSAGE_TYPE_50_8;
            case NozzleType.NOZZLE_TYPE_16_DOT:
                return MESSAGE_TYPE_16_DOT;
            case NozzleType.NOZZLE_TYPE_32_DOT:
                return MESSAGE_TYPE_32_DOT;
// H.M.Wang 2020-7-23 追加32DN打印头
            case NozzleType.NOZZLE_TYPE_32DN:
                return MESSAGE_TYPE_32DN;
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-17 追加32SN打印头
            case NozzleType.NOZZLE_TYPE_32SN:
                return MESSAGE_TYPE_32SN;
// End of H.M.Wang 2020-8-17 追加32SN打印头
            // H.M.Wang 追加下列两行
            case NozzleType.NOZZLE_TYPE_64_DOT:
                return MESSAGE_TYPE_64_DOT;
// H.M.Wang 2020-8-26 追加64SN打印头
            case NozzleType.NOZZLE_TYPE_64SN:
                return MESSAGE_TYPE_64SN;
// End of H.M.Wang 2020-8-26 追加64SN打印头

            case NozzleType.NOZZLE_TYPE_1_INCH:
                return MESSAGE_TYPE_1_INCH;
            case NozzleType.NOZZLE_TYPE_1_INCH_DUAL:
                return MESSAGE_TYPE_1_INCH_DUAL;
            case NozzleType.NOZZLE_TYPE_1_INCH_TRIPLE:
                return MESSAGE_TYPE_1_INCH_TRIPLE;
            case NozzleType.NOZZLE_TYPE_1_INCH_FOUR:
                return MESSAGE_TYPE_1_INCH_FOUR;

            // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//            case NozzleType.NOZZLE_TYPE_12_7_R5:
//                return MESSAGE_TYPE_12_7_R5;
            case NozzleType.NOZZLE_TYPE_R6X48:
                return MESSAGE_TYPE_R6X48;
            case NozzleType.NOZZLE_TYPE_R6X50:
                return MESSAGE_TYPE_R6X50;
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
// End of H.M.Wang 2020-4-17 追加12.7R5头类型
            case NozzleType.NOZZLE_TYPE_9MM:
                return MESSAGE_TYPE_9MM;
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X48:
                return MESSAGE_TYPE_E6X48;
            case NozzleType.NOZZLE_TYPE_E6X50:
                return MESSAGE_TYPE_E6X50;
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
            case NozzleType.NOZZLE_TYPE_E6X1:
                return MESSAGE_TYPE_E6X1;
// H.M.Wang 2021-8-16 追加96DN头
            case NozzleType.NOZZLE_TYPE_96DN:
                return MESSAGE_TYPE_96DN;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
            case NozzleType.NOZZLE_TYPE_E5X48:
                return MESSAGE_TYPE_E5X48;
            case NozzleType.NOZZLE_TYPE_E5X50:
                return MESSAGE_TYPE_E5X50;
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2022-4-29 追加25.4x10头类型
            case NozzleType.NOZZLE_TYPE_254X10:
                return MESSAGE_TYPE_254X10;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
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

        // H.M.Wang 追加下列一行
        public static final int NOZZLE_INDEX_64_DOT  = 10;   // 64 dot

        // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//        public static final int NOZZLE_INDEX_12_7_R5 = 11;  // 12.7R5
        public static final int NOZZLE_INDEX_R6X48 = 11;        // R6X48
        public static final int NOZZLE_INDEX_R6X50 = 12;        // R6X50
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
        // End of H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-7-23 追加32DN打印头
        public static final int NOZZLE_INDEX_32DN = 13;
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-14 追加32SN打印头
        public static final int NOZZLE_INDEX_32SN = 14;
// End of H.M.Wang 2020-8-14 追加32SN打印头

// H.M.Wang 2020-8-26 追加64SN打印头
        public static final int NOZZLE_INDEX_64SN = 15;
// End of H.M.Wang 2020-8-26 追加64SN打印头

        public static final int NOZZLE_INDEX_9MM = 16;
// H.M.Wang 2021-3-6 追加E6X48,E6X50头
        public static final int NOZZLE_INDEX_E6X48 = 17;        // E6X48
        public static final int NOZZLE_INDEX_E6X50 = 18;        // E6X50
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
        public static final int NOZZLE_INDEX_E6X1 = 19;
// H.M.Wang 2021-8-16 追加96DN头
        public static final int NOZZLE_INDEX_96DN = 20;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
        public static final int NOZZLE_INDEX_E5X48 = 21;
        public static final int NOZZLE_INDEX_E5X50 = 22;
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2022-4-29 追加25.4x10头类型
        public static final int NOZZLE_INDEX_254X10 = 23;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
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

        // H.M.Wang 追加下列一行
        public static final int NOZZLE_TYPE_64_DOT = 20;

        // H.M.Wang 2020-4-17 追加12.7R5头类型
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//        public static final int NOZZLE_TYPE_12_7_R5 = 30;
        public static final int NOZZLE_TYPE_R6X48 = 30;
        public static final int NOZZLE_TYPE_R6X50 = 31;
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
        // End of H.M.Wang 2020-4-17 追加12.7R5头类型

// H.M.Wang 2020-7-23 追加32DN打印头
        public static final int NOZZLE_TYPE_32DN = 32;
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-14 追加32SN打印头
        public static final int NOZZLE_TYPE_32SN = 33;
// End of H.M.Wang 2020-8-14 追加32SN打印头

// H.M.Wang 2020-8-26 追加64SN打印头
        public static final int NOZZLE_TYPE_64SN = 34;
// End of H.M.Wang 2020-8-26 追加64SN打印头

        public static final int NOZZLE_TYPE_9MM = 35;

// H.M.Wang 2021-3-6 追加E6X48,E6X50头
        public static final int NOZZLE_TYPE_E6X48 = 36;
        public static final int NOZZLE_TYPE_E6X50 = 37;
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
        public static final int NOZZLE_TYPE_E6X1 = 38;
// H.M.Wang 2021-8-16 追加96DN头
        public static final int NOZZLE_TYPE_96DN = 39;
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
        public static final int NOZZLE_TYPE_E5X48 = 40;
        public static final int NOZZLE_TYPE_E5X50 = 41;
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2022-4-29 追加25.4x10头类型
        public static final int NOZZLE_TYPE_254X10 = 42;
// End of H.M.Wang 2022-4-29 追加25.4x10头类型
    }
}
