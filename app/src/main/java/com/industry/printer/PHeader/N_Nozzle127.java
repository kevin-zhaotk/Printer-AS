package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/22.
 */

public class N_Nozzle127 extends N_Nozzle {

    public N_Nozzle127(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_127;
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
}
