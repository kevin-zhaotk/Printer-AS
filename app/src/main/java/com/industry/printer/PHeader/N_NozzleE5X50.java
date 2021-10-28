package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_NozzleE5X50 extends N_Nozzle {
    public static final int     E5_PRINT_COPY_NUM = 6;
    public static final int     E5_HEAD_NUM = 5;        // 减锁等等操作按着5个头来计算，但是生成打印缓冲区的时候生成相当于6个头的数据
    public static final int     MAX_COL_NUM_EACH_UNIT = 50 * 6;

    public N_NozzleE5X50(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_E5X50;
        mHeads              = 1;
        mEditZoomable       = true;
        mDrawHeight         = 104;
        mBufferHeight       = 112;
        mFactorScale        = 2;
        mShiftable          = true;
        mMirrorable         = true;
        mReversable         = false;
        mRotatable          = false;
        mBuffer8Enable      = false;
    }
}
