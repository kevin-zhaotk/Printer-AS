package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_NozzleE6X50 extends N_Nozzle {
    public static final int     E6_PRINT_COPY_NUM = 5;      // 没有单数行向后位移一个单位的操作，因此不像RX头一样，只复制5次
    public static final int     E6_HEAD_NUM = 6;
    public static final int     MAX_COL_NUM_EACH_UNIT = 50 * 6;

    public N_NozzleE6X50(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_E6X50;
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

    @Override
    public boolean isBigDotNozzle() {
        return false;
    }
}
