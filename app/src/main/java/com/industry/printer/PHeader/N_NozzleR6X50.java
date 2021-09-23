package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_NozzleR6X50 extends N_Nozzle {

    public static final int     R6_PRINT_COPY_NUM = 6;
    public static final int     R6_HEAD_NUM = 6;
    public static final int     MAX_COL_NUM_EACH_UNIT = 50 * 6;

    public N_NozzleR6X50(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_R6X50;
        mHeads              = 1;
        mEditZoomable       = true;
        mDrawHeight         = 152;
        mBufferHeight       = 160;
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