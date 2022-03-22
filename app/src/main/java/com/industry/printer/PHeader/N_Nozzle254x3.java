package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_Nozzle254x3 extends N_Nozzle {

    public N_Nozzle254x3(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_1_INCH_TRIPLE;
        mHeads              = 3;
        mEditZoomable       = true;
        mDrawHeight         = 308 * mHeads;
        mBufferHeight       = 320 * mHeads;
        mFactorScale        = 2;
        mShiftable          = true;
        mMirrorable         = true;
        mReversable         = false;
        mRotatable          = false;
        mBuffer8Enable      = false;
    }
}