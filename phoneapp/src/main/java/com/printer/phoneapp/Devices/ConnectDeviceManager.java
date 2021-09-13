package com.printer.phoneapp.Devices;

import android.content.Context;
import android.content.SharedPreferences;
import android.widget.Toast;

import com.printer.phoneapp.Sockets.BluetoothManager;
import com.printer.phoneapp.Sockets.SocketThread;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

import static android.R.attr.value;

/**
 * Created by hmwan on 2021/9/7.
 */

public class ConnectDeviceManager {
    private ArrayList<ConnectDevice> mCandidateDevices = null;
    private Context mContext = null;

    public static ConnectDeviceManager mConnectDeviceManager = null;

    public static ConnectDeviceManager getInstance(Context ctx) {
        if(null == mConnectDeviceManager) {
            mConnectDeviceManager = new ConnectDeviceManager(ctx);
        }
        return mConnectDeviceManager;
    }

    public static ConnectDeviceManager getInstance() {
        return mConnectDeviceManager;
    }

    private ConnectDeviceManager(Context ctx) {
        if(null == mCandidateDevices) {
            mCandidateDevices = new ArrayList<ConnectDevice>();
        }
        mContext = ctx;
        readDevicesPreference();
    }

    public boolean hasAdded(String address) {
        for(ConnectDevice dev : mCandidateDevices) {
            if(address.equalsIgnoreCase(dev.getAddress())) return true;
        }
        return false;
    }

    private String createName(int index) {
        String name = "设备" + index;
        for(ConnectDevice dev : mCandidateDevices) {
            if(name.equalsIgnoreCase(dev.getName())) {
                return createName(index + 1);
            }
        }
        return name;
    }

    public boolean isAddressExists(String address) {
        for(ConnectDevice dev : mCandidateDevices) {
            if(dev.getAddress().equalsIgnoreCase(address)) {
                return true;
            }
        }
        return false;
    }

    public ConnectDevice addNewDevice(String address, final int type, boolean selected) {
        if(!isAddressExists(address)) {
            return addNewDevice(new ConnectDevice(createName(1), address, type, selected));
        }
        return null;
    }

    public ConnectDevice addNewDevice(ConnectDevice dev) {
        mCandidateDevices.add(0, dev);
        saveDevicesPreference();
        return dev;
    }

    public void deleteDevice(ConnectDevice dev) {
        mCandidateDevices.remove(dev);
        saveDevicesPreference();
    }

    public int size() {
        return mCandidateDevices.size();
    }

    public ArrayList<ConnectDevice> getDevices() {
        return mCandidateDevices;
    }

    public ConnectDevice getDevice(int index) {
        return (index >=0 && index < mCandidateDevices.size() ? mCandidateDevices.get(index) : null);
    }

    public int getSelectededCount() {
        int count = 0;
        for(ConnectDevice dev : mCandidateDevices) {
            if(dev.isSeleted()) count++;
        }

        return count;
    }

    public void sendString(String msg, SocketThread.OnSocketStringListener l) {
        for(ConnectDevice dev : mCandidateDevices) {
            if(dev.isSeleted()) {
                dev.sendString(msg, l);
            }
        }
    }

    private final String TAG_NAME           = "N";
    private final String TAG_ADDRESS        = "A";
    private final String TAG_TYPE           = "T";
    private final String TAG_SELECTED       = "S";

    private static final String PREFERENCE_KEY                              = "PrinterObjectPreference";
    private static final String PREF_DEVICES                                = "PrefDevices";

    private String createJSONArray() {
        JSONArray ja = new JSONArray();
        for(ConnectDevice dev : mCandidateDevices) {
            JSONObject jo = new JSONObject();
            try {
                jo.put(TAG_NAME, dev.getName());
                jo.put(TAG_ADDRESS, dev.getAddress());
                jo.put(TAG_TYPE, dev.getType());
                jo.put(TAG_SELECTED, dev.isSeleted());
                ja.put(jo);
            } catch (JSONException e) {
            }
        }
        return ja.toString();
    }

    public void saveDevicesPreference() {
        try {
            if(null != mContext) {
                SharedPreferences sp = mContext.getSharedPreferences(PREFERENCE_KEY, Context.MODE_PRIVATE);
                SharedPreferences.Editor prefEditor = sp.edit();
                prefEditor.putString(PREF_DEVICES, createJSONArray());
                prefEditor.apply();
            }
        } catch(Exception e) {
        }
    }

    private void createDeviceList(String list) {
        try {
            JSONArray ja = new JSONArray(list);
            for(int i=0; i<ja.length(); i++) {
                JSONObject jo = ja.getJSONObject(i);
                try {
                    ConnectDevice dev = new ConnectDevice(jo.getString(TAG_NAME), jo.getString(TAG_ADDRESS), jo.getInt(TAG_TYPE), jo.getBoolean(TAG_SELECTED));
                    mCandidateDevices.add(dev);
                } catch (JSONException e) {
                }
            }
        } catch (JSONException e) {
        }
    }

    public void readDevicesPreference() {
        if(null != mContext) {
            SharedPreferences sp = mContext.getSharedPreferences(PREFERENCE_KEY, Context.MODE_PRIVATE);
            createDeviceList(sp.getString(PREF_DEVICES, ""));
        }
    }
}
