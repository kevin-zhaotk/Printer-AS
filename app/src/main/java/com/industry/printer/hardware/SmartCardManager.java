package com.industry.printer.hardware;

import android.content.Context;
import android.os.Handler;

public class SmartCardManager implements IInkDevice{


    public SmartCardManager(Context context) {
        // TO-DO
    }


    @Override
    public void init(Handler callback) {

    }

    @Override
    public boolean checkUID(int heads) {
        return false;
    }

    @Override
    public float getLocalInk(int head) {
        return 0;
    }

    @Override
    public boolean isValid(int dev) {
        return false;
    }

    @Override
    public int getFeature(int device, int index) {
        return 0;
    }

    @Override
    public void downLocal(int dev) {

    }

    @Override
    public void switchRfid(int i) {

    }

    @Override
    public void defaultInkForIgnoreRfid() {

    }
}
