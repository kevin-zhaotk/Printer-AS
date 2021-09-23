package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_Nozzle254 extends N_Nozzle {

    public N_Nozzle254(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_1_INCH;
        mHeads              = 1;
        mEditZoomable       = true;
        mDrawHeight         = 308;
        mBufferHeight       = 320;
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
