package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/23.
 */

public class N_Nozzle96DN extends N_Nozzle {

    public N_Nozzle96DN(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_96DN;
        mHeads              = 1;
        mEditZoomable       = false;
        mDrawHeight         = 96;
        mBufferHeight       = 96;
        mFactorScale        = 1;
        mShiftable          = true;
        mMirrorable         = false;
        mReversable         = false;
        mRotatable          = true;
        mBuffer8Enable      = true;

        mBigDot             = true;
    }
}
