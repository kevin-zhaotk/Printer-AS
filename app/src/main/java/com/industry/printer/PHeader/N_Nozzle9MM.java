package com.industry.printer.PHeader;

/**
 * Created by hmwan on 2021/9/22.
 */

public class N_Nozzle9MM extends N_Nozzle {

    public N_Nozzle9MM(int index) {
        super(index);

        mType               = Type.NOZZLE_TYPE_9MM;
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
