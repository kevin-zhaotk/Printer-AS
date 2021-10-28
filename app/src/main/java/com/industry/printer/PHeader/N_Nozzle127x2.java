package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_Nozzle127x2 extends N_Nozzle {

    public N_Nozzle127x2(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_127x2;
        mHeads              = 2;
        mEditZoomable       = true;
        mDrawHeight         = 152 * mHeads;
        mBufferHeight       = 160 * mHeads;
        mFactorScale        = 2;
        mShiftable          = true;
        mMirrorable         = true;
        mReversable         = false;
        mRotatable          = false;
        mBuffer8Enable      = false;
    }
}