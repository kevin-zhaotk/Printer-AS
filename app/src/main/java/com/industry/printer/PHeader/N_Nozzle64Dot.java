package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_Nozzle64Dot extends N_Nozzle {

    public N_Nozzle64Dot(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_64_DOT;
        mHeads              = 1;
        mEditZoomable       = false;
        mDrawHeight         = 64;
        mBufferHeight       = 64;
        mFactorScale        = 1;
        mShiftable          = true;
        mMirrorable         = false;
        mReversable         = false;
        mRotatable          = true;
        mBuffer8Enable      = true;
    }

    @Override
    public boolean isBigDotNozzle() {
        return true;
    }
}
