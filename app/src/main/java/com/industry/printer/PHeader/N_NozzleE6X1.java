package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_NozzleE6X1 extends N_Nozzle {
    public static final int     E6_HEAD_NUM = 6;

    public N_NozzleE6X1(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_E6X1;
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
