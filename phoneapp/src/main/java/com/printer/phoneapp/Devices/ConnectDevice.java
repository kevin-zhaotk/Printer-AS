package com.printer.phoneapp.Devices;

import android.widget.LinearLayout;

import com.printer.phoneapp.Sockets.BTSocketThread;
import com.printer.phoneapp.Sockets.SocketThread;
import com.printer.phoneapp.Sockets.WIFISocketThread;

/**
 * Created by hmwan on 2021/9/7.
 */

public class ConnectDevice {
    public static final int DEVICE_TYPE_UNKNOWN = 0;
    public static final int DEVICE_TYPE_WIFI = 1;
    public static final int DEVICE_TYPE_BT = 2;

    private String          mName;
    private String          mAddress;
    private int             mType;
    private SocketThread mSocketThread;
    private LinearLayout mLinearLayout;
    private String          mErrMsg = "";
    private boolean         mSelected = false;

    public ConnectDevice() {
        this("", "", DEVICE_TYPE_UNKNOWN, false);
    }

    public ConnectDevice(String name, String address, int type, boolean selected) {
        mName = name;
        mAddress = address;
        mType = type;
        if(mType == DEVICE_TYPE_WIFI) {
            mSocketThread = new WIFISocketThread(this);
        } else {
            mSocketThread = new BTSocketThread(this);
        }
        mLinearLayout = null;
        mErrMsg = "";
        mSelected = selected;
    }

    public String getName() {
        return mName;
    }

    public String getAddress() {
        return mAddress;
    }

    public int getType() {
        return mType;
    }

    public void setLinearLayout(LinearLayout l) {
        mLinearLayout = l;
    }

    public LinearLayout getLinearLayout() {
        return mLinearLayout;
    }

    public boolean isSeleted() {
        return mSelected;
    }

    public void setSeleted(boolean selected) {
        mSelected = selected;
    }

    public void setErrMsg(String errMsg) {
        mErrMsg = errMsg;
    }

    public String getErrMsg() {
        return mErrMsg;
    }

    public void sendString(String msg, SocketThread.OnSocketStringListener l) {
        if(null != mSocketThread) {
            mSocketThread.sendString(msg, l);
        }
    }

    public interface OnAddDeviceListener {
        public void onAdd(String address, int type);
    }

}
