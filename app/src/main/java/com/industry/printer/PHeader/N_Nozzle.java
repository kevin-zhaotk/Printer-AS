package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/22.
 */

abstract public class N_Nozzle {
    protected int mIndex;                   // 索引
    protected int mType;                    // 种类
    protected int mHeads;                   // 头的数量
    protected boolean mEditZoomable;        // 编辑状态下是否支持缩放; HP头支持缩放，大字机不支持缩放

    protected int mDrawHeight;              // 作图范围的高度
    protected int mBufferHeight;            // 占有空间范围的高度
    protected int mFactorScale;             // 横向压缩或不压缩

    protected boolean mShiftable;           // 是否支持位移
    protected boolean mMirrorable;          // 是否支持镜像
    protected boolean mReversable;          // 是否支持反转
    protected boolean mRotatable;           // 是否支持旋转
    protected boolean mBuffer8Enable;       // buffer 8 times extension Switch

    public N_Nozzle(int index) {
        mIndex = index;

        mType = -1;
        mHeads = -1;
        mEditZoomable = false;
        mDrawHeight = 0;
        mBufferHeight = 0;
        mFactorScale = 2;
        mShiftable = false;
        mMirrorable = false;
        mReversable = false;
        mRotatable = false;
        mBuffer8Enable = false;
    }

    public int getIndex() {
        return mIndex;
    }
    public int getType() { return mType; }
    public int getHeadCount() { return mHeads; }
    public int getDrawHeight() { return mDrawHeight; }
    public int getBufferHeight() { return mBufferHeight; }
    public int getFactorScale() { return mFactorScale; }
    public boolean isZoomable() { return mEditZoomable; }
    public boolean isShiftable() { return mShiftable; }
    public boolean isMirrorable() { return mMirrorable; }
    public boolean isReversable() { return mReversable; }
    public boolean isRotatable() { return mRotatable; }
    public boolean is8Enable() { return mBuffer8Enable; }

    abstract public boolean isBigDotNozzle();

    // 打印头索引定义，该索引的值必须与R.arrays.strPrinterArray当中的项目顺序相对应
    public static class Index {
        public static final int NOZZLE_INDEX_127 	            = 0;    // 12.7
        public static final int NOZZLE_INDEX_127x2 	            = 1;    // 12.7X2
        public static final int NOZZLE_INDEX_127x3	            = 2;    // 12.7x3
        public static final int NOZZLE_INDEX_127x4 	            = 3;    // 12.7x4
        public static final int NOZZLE_INDEX_1_INCH             = 4;    // 1 inch
        public static final int NOZZLE_INDEX_1_INCH_DUAL        = 5;    // 1inch X2
        public static final int NOZZLE_INDEX_1_INCH_TRIPLE      = 6;    // 1inch X2
        public static final int NOZZLE_INDEX_1_INCH_FOUR        = 7;    // 1inch X2
        public static final int NOZZLE_INDEX_16_DOT             = 8;    // 16dot
        public static final int NOZZLE_INDEX_32_DOT             = 9;    // 32 dot
        public static final int NOZZLE_INDEX_64_DOT             = 10;   // 64 dot
        public static final int NOZZLE_INDEX_R6X48              = 11;   // R6X48
        public static final int NOZZLE_INDEX_R6X50              = 12;   // R6X50
        public static final int NOZZLE_INDEX_32DN               = 13;
        public static final int NOZZLE_INDEX_32SN               = 14;
        public static final int NOZZLE_INDEX_64SN               = 15;
        public static final int NOZZLE_INDEX_9MM                = 16;
        public static final int NOZZLE_INDEX_E6X48              = 17;   // E6X48
        public static final int NOZZLE_INDEX_E6X50              = 18;   // E6X50
        public static final int NOZZLE_INDEX_E6X1               = 19;
        public static final int NOZZLE_INDEX_96DN               = 20;
        public static final int NOZZLE_INDEX_E5X48              = 21;
        public static final int NOZZLE_INDEX_E5X50              = 22;
    }

    // 打印头类型定义，该值是代表打印头类型的唯一值，可以用作打印头类型的判断标准
    public static class Type {
        public static final int NOZZLE_TYPE_127                 = 0;
        public static final int NOZZLE_TYPE_127x2               = 2;
        public static final int NOZZLE_TYPE_127x3               = 5;
        public static final int NOZZLE_TYPE_127x4               = 6;
        public static final int NOZZLE_TYPE_32_DOT              = 7;
        public static final int NOZZLE_TYPE_16_DOT              = 8;
        public static final int NOZZLE_TYPE_1_INCH              = 10;
        public static final int NOZZLE_TYPE_1_INCH_DUAL         = 12;
        public static final int NOZZLE_TYPE_1_INCH_TRIPLE       = 18;
        public static final int NOZZLE_TYPE_1_INCH_FOUR         = 19;
        public static final int NOZZLE_TYPE_64_DOT              = 20;
        public static final int NOZZLE_TYPE_R6X48               = 30;
        public static final int NOZZLE_TYPE_R6X50               = 31;
        public static final int NOZZLE_TYPE_32DN                = 32;
        public static final int NOZZLE_TYPE_32SN                = 33;
        public static final int NOZZLE_TYPE_64SN                = 34;
        public static final int NOZZLE_TYPE_9MM                 = 35;
        public static final int NOZZLE_TYPE_E6X48               = 36;
        public static final int NOZZLE_TYPE_E6X50               = 37;
        public static final int NOZZLE_TYPE_E6X1                = 38;
        public static final int NOZZLE_TYPE_96DN                = 39;
        public static final int NOZZLE_TYPE_E5X48               = 40;
        public static final int NOZZLE_TYPE_E5X50               = 41;
    }
}
