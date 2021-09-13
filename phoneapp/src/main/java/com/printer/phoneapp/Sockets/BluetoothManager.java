package com.printer.phoneapp.Sockets;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothClass;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.ParcelUuid;
import android.os.Parcelable;
import android.util.Log;
import android.view.View;

import com.printer.phoneapp.Devices.ConnectDeviceManager;
import com.printer.phoneapp.PhoneMainActivity;
import com.printer.phoneapp.Utils.HTPermission;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import java.util.UUID;

/**
 * Created by hmwan on 2021/9/8.
 */

public class BluetoothManager {
    public static final String TAG = BluetoothManager.class.getSimpleName();

    public static final int REQUEST_ENBLE_BT = 18761;

    private Context mContext;
    private BluetoothAdapter mBluetoothAdapter = null;
    private ConnectDeviceManager mConnectDeviceManager = null;

    private ArrayList<BluetoothDevice> mFoundDevices = null;

    public interface OnDiscoveryListener {
        public void onDiscoveryStarted();
        public void onDiscoveryFinished();
        public void onDeviceFound(BluetoothDevice device);
    }
    private OnDiscoveryListener mOnDiscoveryListener = null;

    private BroadcastReceiver mStateBroadcaster = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            Log.d(TAG, "Action: " + action);

            switch (action) {
                case BluetoothAdapter.ACTION_STATE_CHANGED:
                    int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, -1);
                    int preState = intent.getIntExtra(BluetoothAdapter.EXTRA_PREVIOUS_STATE, -1);
                    int conState = intent.getIntExtra(BluetoothAdapter.EXTRA_PREVIOUS_CONNECTION_STATE, -1);
                    switch (state) {
                        case BluetoothAdapter.STATE_TURNING_ON:
                            Log.d(TAG, "Pre. State: " + preState);
                            Log.d(TAG, "State: STATE_TURNING_ON");
                            break;
                        case BluetoothAdapter.STATE_TURNING_OFF:
                            Log.d(TAG, "Pre. State: " + preState);
                            Log.d(TAG, "State: STATE_TURNING_OFF");
                            break;
                        case BluetoothAdapter.STATE_ON:
                            Log.d(TAG, "Pre. State: " + preState);
                            Log.d(TAG, "State: STATE_ON");
                            finishEnabling();
                            break;
                        case BluetoothAdapter.STATE_OFF:
                            Log.d(TAG, "Pre. State: " + preState);
                            Log.d(TAG, "State: STATE_OFF");
                            break;
                        case BluetoothAdapter.STATE_CONNECTING:
                            Log.d(TAG, "Pre. State: " + conState);
                            Log.d(TAG, "State: STATE_CONNECTING");
                            break;
                        case BluetoothAdapter.STATE_CONNECTED:
                            Log.d(TAG, "Pre. State: " + conState);
                            Log.d(TAG, "State: STATE_CONNECTED");
                            break;
                        case BluetoothAdapter.STATE_DISCONNECTING:
                            Log.d(TAG, "Pre. State: " + conState);
                            Log.d(TAG, "State: STATE_DISCONNECTING");
                            break;
                        case BluetoothAdapter.STATE_DISCONNECTED:
                            Log.d(TAG, "Pre. State: " + conState);
                            Log.d(TAG, "State: STATE_DISCONNECTED");
                            break;
                    }
                break;
            }
        }
    };

    private BroadcastReceiver mDiscoveryBroadcaster = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            Log.d(TAG, "Action: " + action);

            switch (action) {
                case BluetoothAdapter.ACTION_DISCOVERY_STARTED:
                    Log.d(TAG, "Discovery started.");
                    mFoundDevices.clear();
                    if(null != mOnDiscoveryListener) {
                        mOnDiscoveryListener.onDiscoveryStarted();
                    };
                    break;
                case BluetoothAdapter.ACTION_DISCOVERY_FINISHED:
                    Log.d(TAG, "Discovery finished.");
                    if(null != mOnDiscoveryListener) {
                        mOnDiscoveryListener.onDiscoveryFinished();
                    };
                    unregisterDiscoveryBroadcaster();
                    break;
                case BluetoothDevice.ACTION_FOUND:
                    Log.d(TAG, "Device found.");
                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    Log.d(TAG, "  Name = [" + device.getName() + "]");
                    Log.d(TAG, "  Address = [" + device.getAddress() + "]");
                    Log.d(TAG, "  Type = [" + device.getType() + "]");      // 可能无用
                    if(null != device.getUuids()) {
                        for(ParcelUuid uuid : device.getUuids()) {
                            Log.d(TAG, "  UUID = [" + uuid.getUuid() + "]");
                        }
                    } else {
                        Log.d(TAG, "  UUID = [null]");
                    }
                    Log.d(TAG, "  Bonded = [" + device.getBondState() + "]");
                    BluetoothClass bClass = intent.getParcelableExtra(BluetoothDevice.EXTRA_CLASS);
                    Log.d(TAG, "  Class = [" + bClass.getDeviceClass() + "]");
                    if(null != device.getName() &&
                        !device.getName().isEmpty() &&
                        !isAddressExists(device.getAddress()) &&
                        !mConnectDeviceManager.isAddressExists(device.getAddress())) {
                        mFoundDevices.add(device);
                        if(null != mOnDiscoveryListener) {
                            mOnDiscoveryListener.onDeviceFound(device);
                        };
                    }
                    break;
                case BluetoothDevice.ACTION_UUID:   // 这个只有是绑定了的设备才能够获得
                    BluetoothDevice dev = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    Log.d(TAG, "UUIDs of Device [" + (null == dev ? "null" : dev.getName()) + "]");
                    Parcelable[] uuids = intent.getParcelableArrayExtra(BluetoothDevice.EXTRA_UUID);
                    if(null != uuids) {
                        for(Parcelable uuid : uuids) {
                            Log.d(TAG, "  UUID = [" + uuid.toString() + "]");
                        }
                    } else {
                        Log.d(TAG, "  UUID = [null]");
                    }
                    break;
                case BluetoothDevice.ACTION_PAIRING_REQUEST:
                    dev = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    Log.d(TAG, "ACTION_PAIRING_REQUEST of Device [" + (null == dev ? "null" : dev.getName()) + "]");
                    break;
                case BluetoothDevice.ACTION_BOND_STATE_CHANGED:
                    dev = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    Log.d(TAG, "ACTION_BOND_STATE_CHANGED of Device [" + (null == dev ? "null" : dev.getName()) + "]");
                    Log.d(TAG, "Previous Bond state: " + intent.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE, BluetoothDevice.BOND_NONE));
                    Log.d(TAG, "Bond state: " + intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.BOND_NONE));
                    break;
            }
       }
    };

    private static BluetoothManager mBluetoothManager = null;

    public static BluetoothManager getInstance(Context ctx) {
        if(null == mBluetoothManager) {
            mBluetoothManager = new BluetoothManager(ctx);
        }
        return mBluetoothManager;
    }

    public static BluetoothManager getInstance() {
        return mBluetoothManager;
    }

    private BluetoothManager(Context ctx) {
        mContext = ctx;
        mFoundDevices = new ArrayList<BluetoothDevice>();
        mConnectDeviceManager = ConnectDeviceManager.getInstance(ctx);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    private boolean isAddressExists(String address) {
        for(BluetoothDevice dev : mFoundDevices) {
            if(dev.getAddress().equalsIgnoreCase(address)) {
                return true;
            }
        }
        return false;
    }

    private boolean mEnabling = false;

    public void enableBluetooth() {
        if(null != mBluetoothAdapter && !mBluetoothAdapter.isEnabled()) {
            mEnabling = true;
            registerStateBroadcaster();
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            ((Activity)mContext).startActivityForResult(enableIntent, REQUEST_ENBLE_BT);
//            mBluetoothAdapter.enable();
        }
    }

    public boolean isEnabling() {
        return mEnabling;
    }

    public void finishEnabling() {
        Log.d(TAG, "Enabling job finished");
        mEnabling = false;
        unregisterStateBroadcaster();
    }

    public void setEnablingResult(int result) {
        if(result == Activity.RESULT_CANCELED) {
            finishEnabling();
        }
    }

    public BluetoothAdapter getAdapter() {
        return mBluetoothAdapter;
    }

    public boolean isSupported() {
        return (null != BluetoothAdapter.getDefaultAdapter());
    }

    public boolean isEnabled() {
        return (null != mBluetoothAdapter && mBluetoothAdapter.isEnabled());
    }

    private boolean mStateReceiverRegisterred = false;

    private void registerStateBroadcaster() {
        try {
            if(null != mContext) {
                mContext.registerReceiver(mStateBroadcaster, new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED));
                mStateReceiverRegisterred = true;
            }
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }

    private void unregisterStateBroadcaster() {
        try {
            if(null != mContext && mStateReceiverRegisterred) {
                mStateReceiverRegisterred = false;
                mContext.unregisterReceiver(mStateBroadcaster);
            }
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }

    private boolean mPermissionReceiverRegisterred = false;

    private boolean mDiscoveryReceiverRegisterred = false;

    private void registerDiscoveryBroadcaster() {
        try {
            if(null != mContext) {
                mContext.registerReceiver(mDiscoveryBroadcaster, new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_STARTED));
                mContext.registerReceiver(mDiscoveryBroadcaster, new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED));
                mContext.registerReceiver(mDiscoveryBroadcaster, new IntentFilter(BluetoothDevice.ACTION_FOUND));
                mContext.registerReceiver(mDiscoveryBroadcaster, new IntentFilter(BluetoothDevice.ACTION_UUID));
                mContext.registerReceiver(mDiscoveryBroadcaster, new IntentFilter(BluetoothDevice.ACTION_PAIRING_REQUEST));
                mContext.registerReceiver(mDiscoveryBroadcaster, new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED));
                mDiscoveryReceiverRegisterred = true;
            }
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }

    private void unregisterDiscoveryBroadcaster() {
        try {
            if(null != mContext && mDiscoveryReceiverRegisterred) {
                mDiscoveryReceiverRegisterred = false;
                mContext.unregisterReceiver(mDiscoveryBroadcaster);
            }
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }

    public boolean startDiscovery(OnDiscoveryListener l) {
        if(!isSupported()) {
            Log.e(TAG, "Not supported!");
            return false;
        }

        if(!isEnabled()) {
            enableBluetooth();
            while(mEnabling) {try{Thread.sleep(100);}catch(Exception e){}};
        }

        if(!isEnabled()) {
            Log.e(TAG, "Not enabled!");
            return false;
        }

        if(mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
        }

        registerDiscoveryBroadcaster();
        mOnDiscoveryListener = l;
        return mBluetoothAdapter.startDiscovery();
    }

    public boolean cancelDiscovery() {
        if(!isEnabled()) {
            Log.e(TAG, "Not enabled!");
            return false;
        }

        mOnDiscoveryListener = null;
        unregisterDiscoveryBroadcaster();
        return mBluetoothAdapter.cancelDiscovery();
    }

    public void getBondedDevices() {
        if(isEnabled()) {
            Set<BluetoothDevice> mSet = mBluetoothAdapter.getBondedDevices();
            for(BluetoothDevice dev : mSet) {
                Log.d(TAG, "  --------------------------------  ");
                Log.d(TAG, "  Name = [" + dev.getName() + "]");
                Log.d(TAG, "  Address = [" + dev.getAddress() + "]");
                Log.d(TAG, "  Type = [" + dev.getType() + "]");      // 可能无用
                if(null != dev.getUuids()) {
                    for(ParcelUuid uuid : dev.getUuids()) {
                        Log.d(TAG, "  UUID = [" + uuid.getUuid() + "] (" + mUUIDTable.get(uuid.getUuid().toString().substring(4, 8)) + ")");
                        ;
                    }
                } else {
                    Log.d(TAG, "  UUID = [null]");
                }
                Log.d(TAG, "  Bonded = [" + dev.getBondState() + "]");
            }
        } else {
            Log.e(TAG, "Not enabled!");
        }
    }

    public void connectDevice() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if(!isSupported()) {
                        Log.e(TAG, "Not supported!");
                        return;
                    }

            //        if(!isEnabled()) {
            //            enableBluetooth();
            //            while(mEnabling) {try{Thread.sleep(100);}catch(Exception e){}};
            //        }

                    if(!isEnabled()) {
                        Log.e(TAG, "Not enabled!");
                        return;
                    }

                        Log.d(TAG, "Start-1004");
                        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice("F4:4E:FD:14:63:66");  // SANSUI 耳机
//                        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice("D0:9F:74:B1:ED:F9");    // 蓝牙鼠标
                        Log.d(TAG, "Device got " + device.getName());
                        Log.d(TAG, "Device Class " + device.getBluetoothClass().getDeviceClass());

                        // 这里调用配对，配对成功后，系统的蓝牙会自动连接这个蓝牙设备（因为是音乐播放器，已有的播放器会主动连接），因此导致我们后续的连接失败（因为蓝牙只支持一个连接）
//                        Method m1 = device.getClass().getMethod("createBond");
//                        Log.d(TAG, "Bond: " + m1.invoke(device));

                        // 直接获得socket(0000111e-0000-1000-8000-00805f9b34fb)成功，无论是否配对，但是其他的都会失败，无论配对与否，连接与否
                        // 如果有配对，Method的方法可以成功，如果没有配对，会提示配对，如果不配对会失败，配对会成功。（但是出过pin不匹配错误而失败）
                        // 如果已有其他设备连接，则都会失败；因为只支持一个连接
// 蓝牙鼠标
//
//                        BluetoothSocket socket = device.createRfcommSocketToServiceRecord(UUID.fromString("00001812-0000-1000-8000-00805f9b34fb"));
// 已配对未连接状态下，不成功                        BluetoothSocket socket = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("00001812-0000-1000-8000-00805f9b34fb"));
// 已配对未连接状态下，不成功                      Method m = device.getClass().getMethod("createRfcommSocket", new Class[] {int.class});
//                        BluetoothSocket socket = (BluetoothSocket)m.invoke(device, 1);

// SANSUI 耳机。
// 这个失败。出现Pin不匹配的错误。不配对也可以执行到这里                        BluetoothSocket socket = device.createRfcommSocketToServiceRecord(UUID.fromString("0000111e-0000-1000-8000-00805f9b34fb"));
// 这个成功。即使没有配对也可以链接成功，而不提示配对
                        BluetoothSocket socket = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("0000111e-0000-1000-8000-00805f9b34fb"));
// 这个不成功                        BluetoothSocket socket = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("0000110e-0000-1000-8000-00805f9b34fb"));
// 这个不成功                        BluetoothSocket socket = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("0000110b-0000-1000-8000-00805f9b34fb"));
// 这个不成功                        BluetoothSocket socket = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("00000000-0000-1000-8000-00805f9b34fb"));
// 会提示配对                       Method m = device.getClass().getMethod("createRfcommSocket", new Class[] {int.class});
//                        BluetoothSocket socket = (BluetoothSocket)m.invoke(device, 1);

                        Log.d(TAG, "Socket created " + socket.toString());
                        if(null != socket && !socket.isConnected()) {
                            Log.d(TAG, "Connecting...");
                            cancelDiscovery();
                            socket.connect();
                            Log.d(TAG, "Socket connected: " + socket.isConnected());
                            if(null != socket) {
                                BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                                String is = br.readLine();      // 首次从耳机收到[AT+BRSF=639]
                                Log.d(TAG, "Socket Received: [" + is + "]");

                                while(true) {
                                    OutputStream os = socket.getOutputStream();
                                    os.write(new byte[]{0x0D});
                                    os.write(new byte[]{0x0A});
                                    os.write("+BRSF:639".getBytes());
                                    os.write(new byte[]{0x0D});
                                    os.write(new byte[]{0x0A});
                                    os.flush();

                                    is = br.readLine();
                                    Log.d(TAG, "Socket Received: [" + is + "]");
                                }

//                                09-09 16:56:36.054 30112-30502/com.printer.phoneapp D/BluetoothManager: Socket Received: [AT+BRSF=639]
//                                发送 <CR><LF>+BRSF:639<CR><LF>
//                                09-09 16:56:42.760 30112-30502/com.printer.phoneapp D/BluetoothManager: Socket Received: [AT+CIND=?]
                            }

                        }
                } catch(Exception e) {
                    Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
                }
            }
        }).start();
    }

    private HashMap<String, String> mUUIDTable = new HashMap<String, String>() {
        {
            put("0001", "SDP");
            put("0003", "RFCOMM");
            put("0005", "TCS-BIN");
            put("0007", "ATT");
            put("0008", "OBEX");
            put("000f", "BNEP");
            put("0010", "UPNP");
            put("0011", "HIDP");
            put("0012", "Hardcopy Control Channel");
            put("0014", "Hardcopy Data Channel");
            put("0016", "Hardcopy Notification");
            put("0017", "AVCTP");
            put("0019", "AVDTP");
            put("001b", "CMTP");
            put("001e", "MCAP Control Channel");
            put("001f", "MCAP Data Channel");
            put("0100", "L2CAP");
            put("1000", "Service Discovery Server Service Class");
            put("1001", "Browse Group Descriptor Service Class");
            put("1002", "Public Browse Root");
            put("1101", "Serial Port");
            put("1102", "LAN Access Using PPP");
            put("1103", "Dialup Networking");
            put("1104", "IrMC Sync");
            put("1105", "OBEX Object Push");
            put("1106", "OBEX File Transfer");
            put("1107", "IrMC Sync Command");
            put("1108", "Headset");
            put("1109", "Cordless Telephony");
            put("110a", "Audio Source");
            put("110b", "Audio Sink");
            put("110c", "A/V Remote Control Target");
            put("110d", "Advanced Audio Distribution");
            put("110e", "A/V Remote Control");
            put("110f", "A/V Remote Control Controller");
            put("1110", "Intercom");
            put("1111", "Fax");
            put("1112", "Headset AG");
            put("1113", "WAP");
            put("1114", "WAP Client");
            put("1115", "PANU");
            put("1116", "NAP");
            put("1117", "GN");
            put("1118", "Direct Printing");
            put("1119", "Reference Printing");
            put("111a", "Basic Imaging Profile");
            put("111b", "Imaging Responder");
            put("111c", "Imaging Automatic Archive");
            put("111d", "Imaging Referenced Objects");
            put("111e", "Handsfree");
            put("111f", "Handsfree Audio Gateway");
            put("1120", "Direct Printing Refrence Objects Service");
            put("1121", "Reflected UI");
            put("1122", "Basic Printing");
            put("1123", "Printing Status");
            put("1124", "Human Interface Device Service");
            put("1125", "Hardcopy Cable Replacement");
            put("1126", "HCR Print");
            put("1127", "HCR Scan");
            put("1128", "Common ISDN Access");
            put("112d", "SIM Access");
            put("112e", "Phonebook Access Client");
            put("112f", "Phonebook Access Server");
            put("1130", "Phonebook Access");
            put("1131", "Headset HS");
            put("1132", "Message Access Server");
            put("1133", "Message Notification Server");
            put("1134", "Message Access Profile");
            put("1135", "GNSS");
            put("1136", "GNSS Server");
            put("1137", "3D Display");
            put("1138", "3D Glasses");
            put("1139", "3D Synchronization");
            put("113a", "MPS Profile");
            put("113b", "MPS Service");
            put("1200", "PnP Information");
            put("1201", "Generic Networking");
            put("1202", "Generic File Transfer");
            put("1203", "Generic Audio");
            put("1204", "Generic Telephony");
            put("1205", "UPNP Service");
            put("1206", "UPNP IP Service");
            put("1300", "UPNP IP PAN");
            put("1301", "UPNP IP LAP");
            put("1302", "UPNP IP L2CAP");
            put("1303", "Video Source");
            put("1304", "Video Sink");
            put("1305", "Video Distribution");
            put("1400", "HDP");
            put("1401", "HDP Source");
            put("1402", "HDP Sink");
            put("1800", "Generic Access Profile");
            put("1801", "Generic Attribute Profile");
            put("1802", "Immediate Alert");
            put("1803", "Link Loss");
            put("1804", "Tx Power");
            put("1805", "Current Time Service");
            put("1806", "Reference Time Update Service");
            put("1807", "Next DST Change Service");
            put("1808", "Glucose");
            put("1809", "Health Thermometer");
            put("180a", "Device Information");
            put("180d", "Heart Rate");
            put("180e", "Phone Alert Status Service");
            put("180f", "Battery Service");
            put("1810", "Blood Pressure");
            put("1811", "Alert Notification Service");
            put("1812", "Human Interface Device");
            put("1813", "Scan Parameters");
            put("1814", "Running Speed and Cadence");
            put("1815", "Automation IO");
            put("1816", "Cycling Speed and Cadence");
            put("1818", "Cycling Power");
            put("1819", "Location and Navigation");
            put("181a", "Environmental Sensing");
            put("181b", "Body Composition");
            put("181c", "User Data");
            put("181d", "Weight Scale");
            put("181e", "Bond Management");
            put("181f", "Continuous Glucose Monitoring");
            put("1820", "Internet Protocol Support");
            put("1821", "Indoor Positioning");
            put("1822", "Pulse Oximeter");
            put("1823", "HTTP Proxy");
            put("1824", "Transport Discovery");
            put("1825", "Object Transfer");
            put("2800", "Primary Service");
            put("2801", "Secondary Service");
            put("2802", "Include");
            put("2803", "Characteristic");
            put("2900", "Characteristic Extended Properties");
            put("2901", "Characteristic User Description");
            put("2902", "Client Characteristic Configuration");
            put("2903", "Server Characteristic Configuration");
            put("2904", "Characteristic Format");
            put("2905", "Characteristic Aggregate Formate");
            put("2906", "Valid Range");
            put("2907", "External Report Reference");
            put("2908", "Report Reference");
            put("2909", "Number of Digitals");
            put("290a", "Value Trigger Setting");
            put("290b", "Environmental Sensing Configuration");
            put("290c", "Environmental Sensing Measurement");
            put("290d", "Environmental Sensing Trigger Setting");
            put("290e", "Time Trigger Setting");
            put("2a00", "Device Name");
            put("2a01", "Appearance");
            put("2a02", "Peripheral Privacy Flag");
            put("2a03", "Reconnection Address");
            put("2a04", "Peripheral Preferred Connection Parameters");
            put("2a05", "Service Changed");
            put("2a06", "Alert Level");
            put("2a07", "Tx Power Level");
            put("2a08", "Date Time");
            put("2a09", "Day of Week");
            put("2a0a", "Day Date Time");
            put("2a0c", "Exact Time 256");
            put("2a0d", "DST Offset");
            put("2a0e", "Time Zone");
            put("2a0f", "Local Time Information");
            put("2a11", "Time with DST");
            put("2a12", "Time Accuracy");
            put("2a13", "Time Source");
            put("2a14", "Reference Time Information");
            put("2a16", "Time Update Control Point");
            put("2a17", "Time Update State");
            put("2a18", "Glucose Measurement");
            put("2a19", "Battery Level");
            put("2a1c", "Temperature Measurement");
            put("2a1d", "Temperature Type");
            put("2a1e", "Intermediate Temperature");
            put("2a21", "Measurement Interval");
            put("2a22", "Boot Keyboard Input Report");
            put("2a23", "System ID");
            put("2a24", "Model Number String");
            put("2a25", "Serial Number String");
            put("2a26", "Firmware Revision String");
            put("2a27", "Hardware Revision String");
            put("2a28", "Software Revision String");
            put("2a29", "Manufacturer Name String");
            put("2a2a", "IEEE 11073-20601 Regulatory Cert. Data List");
            put("2a2b", "Current Time");
            put("2a2c", "Magnetic Declination");
            put("2a31", "Scan Refresh");
            put("2a32", "Boot Keyboard Output Report");
            put("2a33", "Boot Mouse Input Report");
            put("2a34", "Glucose Measurement Context");
            put("2a35", "Blood Pressure Measurement");
            put("2a36", "Intermediate Cuff Pressure");
            put("2a37", "Heart Rate Measurement");
            put("2a38", "Body Sensor Location");
            put("2a39", "Heart Rate Control Point");
            put("2a3f", "Alert Status");
            put("2a40", "Ringer Control Point");
            put("2a41", "Ringer Setting");
            put("2a42", "Alert Category ID Bit Mask");
            put("2a43", "Alert Category ID");
            put("2a44", "Alert Notification Control Point");
            put("2a45", "Unread Alert Status");
            put("2a46", "New Alert");
            put("2a47", "Supported New Alert Category");
            put("2a48", "Supported Unread Alert Category");
            put("2a49", "Blood Pressure Feature");
            put("2a4a", "HID Information");
            put("2a4b", "Report Map");
            put("2a4c", "HID Control Point");
            put("2a4d", "Report");
            put("2a4e", "Protocol Mode");
            put("2a4f", "Scan Interval Window");
            put("2a50", "PnP ID");
            put("2a51", "Glucose Feature");
            put("2a52", "Record Access Control Point");
            put("2a53", "RSC Measurement");
            put("2a54", "RSC Feature");
            put("2a55", "SC Control Point");
            put("2a56", "Digital");
            put("2a58", "Analog");
            put("2a5a", "Aggregate");
            put("2a5b", "CSC Measurement");
            put("2a5c", "CSC Feature");
            put("2a5d", "Sensor Location");
            put("2a63", "Cycling Power Measurement");
            put("2a64", "Cycling Power Vector");
            put("2a65", "Cycling Power Feature");
            put("2a66", "Cycling Power Control Point");
            put("2a67", "Location and Speed");
            put("2a68", "Navigation");
            put("2a69", "Position Quality");
            put("2a6a", "LN Feature");
            put("2a6b", "LN Control Point");
            put("2a6c", "Elevation");
            put("2a6d", "Pressure");
            put("2a6e", "Temperature");
            put("2a6f", "Humidity");
            put("2a70", "True Wind Speed");
            put("2a71", "True Wind Direction");
            put("2a72", "Apparent Wind Speed");
            put("2a73", "Apparent Wind Direction");
            put("2a74", "Gust Factor");
            put("2a75", "Pollen Concentration");
            put("2a76", "UV Index");
            put("2a77", "Irradiance");
            put("2a78", "Rainfall");
            put("2a79", "Wind Chill");
            put("2a7a", "Heat Index");
            put("2a7b", "Dew Point");
            put("2a7c", "Trend");
            put("2a7d", "Descriptor Value Changed");
            put("2a7e", "Aerobic Heart Rate Lower Limit");
            put("2a7f", "Aerobic Threshold");
            put("2a80", "Age");
            put("2a81", "Anaerobic Heart Rate Lower Limit");
            put("2a82", "Anaerobic Heart Rate Upper Limit");
            put("2a83", "Anaerobic Threshold");
            put("2a84", "Aerobic Heart Rate Upper Limit");
            put("2a85", "Date of Birth");
            put("2a86", "Date of Threshold Assessment");
            put("2a87", "Email Address");
            put("2a88", "Fat Burn Heart Rate Lower Limit");
            put("2a89", "Fat Burn Heart Rate Upper Limit");
            put("2a8a", "First Name");
            put("2a8b", "Five Zone Heart Rate Limits");
            put("2a8c", "Gender");
            put("2a8d", "Heart Rate Max");
            put("2a8e", "Height");
            put("2a8f", "Hip Circumference");
            put("2a90", "Last Name");
            put("2a91", "Maximum Recommended Heart Rate");
            put("2a92", "Resting Heart Rate");
            put("2a93", "Sport Type for Aerobic/Anaerobic Thresholds");
            put("2a94", "Three Zone Heart Rate Limits");
            put("2a95", "Two Zone Heart Rate Limit");
            put("2a96", "VO2 Max");
            put("2a97", "Waist Circumference");
            put("2a98", "Weight");
            put("2a99", "Database Change Increment");
            put("2a9a", "User Index");
            put("2a9b", "Body Composition Feature");
            put("2a9c", "Body Composition Measurement");
            put("2a9d", "Weight Measurement");
            put("2a9e", "Weight Scale Feature");
            put("2a9f", "User Control Point");
            put("2aa0", "Magnetic Flux Density - 2D");
            put("2aa1", "Magnetic Flux Density - 3D");
            put("2aa2", "Language");
            put("2aa3", "Barometric Pressure Trend");
            put("2aa4", "Bond Management Control Point");
            put("2aa5", "Bond Management Feature");
            put("2aa6", "Central Address Resolution");
            put("2aa7", "CGM Measurement");
            put("2aa8", "CGM Feature");
            put("2aa9", "CGM Status");
            put("2aaa", "CGM Session Start Time");
            put("2aab", "CGM Session Run Time");
            put("2aac", "CGM Specific Ops Control Point");
            put("2aad", "Indoor Positioning Configuration");
            put("2aae", "Latitude");
            put("2aaf", "Longitude");
            put("2ab0", "Local North Coordinate");
            put("2ab1", "Local East Coordinate");
            put("2ab2", "Floor Number");
            put("2ab3", "Altitude");
            put("2ab4", "Uncertainty");
            put("2ab5", "Location Name");
            put("2ab6", "URI");
            put("2ab7", "HTTP Headers");
            put("2ab8", "HTTP Status Code");
            put("2ab9", "HTTP Entity Body");
            put("2aba", "HTTP Control Point");
            put("2abb", "HTTPS Security");
            put("2abc", "TDS Control Point");
            put("2abd", "OTS Feature");
            put("2abe", "Object Name");
            put("2abf", "Object Type");
            put("2ac0", "Object Size");
            put("2ac1", "Object First-Created");
            put("2ac2", "Object Last-Modified");
            put("2ac3", "Object ID");
            put("2ac4", "Object Properties");
            put("2ac5", "Object Action Control Point");
            put("2ac6", "Object List Control Point");
            put("2ac7", "Object List Filter");
            put("2ac8", "Object Changed");
            put("feff", "GN Netcom");
            put("fefe", "GN ReSound A/S");
            put("fefd", "Gimbal, Inc.");
            put("fefc", "Gimbal, Inc.");
            put("fefb", "Stollmann E&#43;V GmbH");
            put("fefa", "PayPal, Inc.");
            put("fef9", "PayPal, Inc.");
            put("fef8", "Aplix Corporation");
            put("fef7", "Aplix Corporation");
            put("fef6", "Wicentric, Inc.");
            put("fef5", "Dialog Semiconductor GmbH");
            put("fef4", "Google");
            put("fef3", "Google");
            put("fef2", "CSR");
            put("fef1", "CSR");
            put("fef0", "Intel");
            put("feef", "Polar Electro Oy");
            put("feee", "Polar Electro Oy");
            put("feed", "Tile, Inc.");
            put("feec", "Tile, Inc.");
            put("feeb", "Swirl Networks, Inc.");
            put("feea", "Swirl Networks, Inc.");
            put("fee9", "Quintic Corp.");
            put("fee8", "Quintic Corp.");
            put("fee7", "Tencent Holdings Limited");
            put("fee6", "Seed Labs, Inc.");
            put("fee5", "Nordic Semiconductor ASA");
            put("fee4", "Nordic Semiconductor ASA");
            put("fee3", "Anki, Inc.");
            put("fee2", "Anki, Inc.");
            put("fee1", "Anhui Huami Information Technology Co.");
            put("fee0", "Anhui Huami Information Technology Co.");
            put("fedf", "Design SHIFT");
            put("fede", "Coin, Inc.");
            put("fedd", "Jawbone");
            put("fedc", "Jawbone");
            put("fedb", "Perka, Inc.");
            put("feda", "ISSC Technologies Corporation");
            put("fed9", "Pebble Technology Corporation");
            put("fed8", "Google");
            put("fed7", "Broadcom Corporation");
            put("fed6", "Broadcom Corporation");
            put("fed5", "Plantronics Inc.");
            put("fed4", "Apple, Inc.");
            put("fed3", "Apple, Inc.");
            put("fed2", "Apple, Inc.");
            put("fed1", "Apple, Inc.");
            put("fed0", "Apple, Inc.");
            put("fecf", "Apple, Inc.");
            put("fece", "Apple, Inc.");
            put("fecd", "Apple, Inc.");
            put("fecc", "Apple, Inc.");
            put("fecb", "Apple, Inc.");
            put("feca", "Apple, Inc.");
            put("fec9", "Apple, Inc.");
            put("fec8", "Apple, Inc.");
            put("fec7", "Apple, Inc.");
            put("fec6", "Kocomojo, LLC");
            put("fec5", "Realtek Semiconductor Corp.");
            put("fec4", "PLUS Location Systems");
            put("fec3", "360fly, Inc.");
            put("fec2", "Blue Spark Technologies, Inc.");
            put("fec1", "KDDI Corporation");
            put("fec0", "KDDI Corporation");
            put("febf", "Nod, Inc.");
            put("febe", "Bose Corporation");
            put("febd", "Clover Network, Inc.");
            put("febc", "Dexcom, Inc.");
            put("febb", "adafruit industries");
            put("feba", "Tencent Holdings Limited");
            put("feb9", "LG Electronics");
            put("feb8", "Facebook, Inc.");
            put("feb7", "Facebook, Inc.");
            put("feb6", "Vencer Co, Ltd");
            put("feb5", "WiSilica Inc.");
            put("feb4", "WiSilica Inc.");
            put("feb3", "Taobao");
            put("feb2", "Microsoft Corporation");
            put("feb1", "Electronics Tomorrow Limited");
            put("feb0", "Nest Labs Inc.");
            put("feaf", "Nest Labs Inc.");
            put("feae", "Nokia Corporation");
            put("fead", "Nokia Corporation");
            put("feac", "Nokia Corporation");
            put("feab", "Nokia Corporation");
            put("feaa", "Google");
            put("fea9", "Savant Systems LLC");
            put("fea8", "Savant Systems LLC");
            put("fea7", "UTC Fire and Security");
            put("fea6", "GoPro, Inc.");
            put("fea5", "GoPro, Inc.");
            put("fea4", "Paxton Access Ltd");
            put("fea3", "ITT Industries");
            put("fea2", "Intrepid Control Systems, Inc.");
            put("fea1", "Intrepid Control Systems, Inc.");
            put("fea0", "Google");
            put("fe9f", "Google");
            put("fe9e", "Dialog Semiconductor B.V.");
            put("fe9d", "Mobiquity Networks Inc");
            put("fe9c", "GSI Laboratories, Inc.");
            put("fe9b", "Samsara Networks, Inc");
            put("fe9a", "Estimote");
            put("fe99", "Currant, Inc.");
            put("fe98", "Currant, Inc.");
            put("fe97", "Tesla Motor Inc.");
            put("fe96", "Tesla Motor Inc.");
            put("fe95", "Xiaomi Inc.");
            put("fe94", "OttoQ Inc.");
            put("fe93", "OttoQ Inc.");
            put("fe92", "Jarden Safety &amp; Security");
            put("fe91", "Shanghai Imilab Technology Co.,Ltd");
            put("fe90", "JUMA");
            put("fe8f", "CSR");
            put("fe8e", "ARM Ltd");
            put("fe8d", "Interaxon Inc.");
            put("fe8c", "TRON Forum");
            put("fe8b", "Apple, Inc.");
            put("fe8a", "Apple, Inc.");
            put("fe89", "B&amp;O Play A/S");
            put("fe88", "SALTO SYSTEMS S.L.");
            put("fe87", "Qingdao Yeelink Information Technology Co., Ltd. ( 青岛亿联客信息技术有限公司 )");
            put("fe86", "HUAWEI Technologies Co., Ltd. ( 华为技术有限公司 )");
            put("fe85", "RF Digital Corp");
            put("fe84", "RF Digital Corp");
            put("fe83", "Blue Bite");
            put("fe82", "Medtronic Inc.");
            put("fe81", "Medtronic Inc.");
            put("fe80", "Doppler Lab");
            put("fe7f", "Doppler Lab");
            put("fe7e", "Awear Solutions Ltd");
            put("fe7d", "Aterica Health Inc.");
            put("fe7c", "Stollmann E&#43;V GmbH");
            put("fe7b", "Orion Labs, Inc.");
            put("fffe", "Alliance for Wireless Power (A4WP)");
            put("fffd", "Fast IDentity Online Alliance (FIDO)");
        }
    };

    public ArrayList<BluetoothDevice> getFoundDevices() {
        return mFoundDevices;
    }
}

/*
    启动配置蓝牙可见模式，
    Intent in = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
    in.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 200); // 200就表示200秒。
    startActivity(in);
 */
// https://blog.csdn.net/weixin_34183910/article/details/92445047
// https://www.jianshu.com/p/4ecc99679451
// https://blog.csdn.net/zqf_888/article/details/81060606
// https://www.pianshen.com/article/416249812/
// https://www.pianshen.com/article/72931302273/
// https://blog.csdn.net/shichaog/article/details/52180723
// https://blog.csdn.net/xsophiax/article/details/89396933
// https://www.cnblogs.com/Free-Thinker/p/6514529.html     （蓝牙耳机比较有用）
// https://blog.csdn.net/zrf1335348191/article/details/51103435 (比较全的蓝牙说明文档)
// https://www.cnblogs.com/loaderman/p/10075499.html
/*
    09-09 10:55:35.437    --------------------------------
    09-09 10:55:35.440    Name = [RK Bluetooth Keyboard]
    09-09 10:55:35.440    Address = [07:18:01:19:04:81]
    09-09 10:55:35.442    Type = [1]
    09-09 10:55:35.447    UUID = [00001124-0000-1000-8000-00805f9b34fb] (Human Interface Device Service)
    09-09 10:55:35.448    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.448    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.449    Bonded = [12]
    09-09 10:55:35.449    --------------------------------
    09-09 10:55:35.451    Name = [X1]
    09-09 10:55:35.451    Address = [48:77:48:B9:4E:E6]
    09-09 10:55:35.453    Type = [1]
    09-09 10:55:35.458    UUID = [00001101-0000-1000-8000-00805f9b34fb] (Serial Port)
    09-09 10:55:35.458    UUID = [0000111e-0000-1000-8000-00805f9b34fb] (Handsfree)
    09-09 10:55:35.458    UUID = [0000110b-0000-1000-8000-00805f9b34fb] (Audio Sink)
    09-09 10:55:35.459    UUID = [0000110e-0000-1000-8000-00805f9b34fb] (A/V Remote Control)
    09-09 10:55:35.459    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.459    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.460    Bonded = [12]
    09-09 10:55:35.460    --------------------------------
    09-09 10:55:35.462    Name = [BT4.0+2.4G Mouse]
    09-09 10:55:35.462    Address = [D0:9F:74:B1:ED:F9]
    09-09 10:55:35.464    Type = [2]
    09-09 10:55:35.469    UUID = [00001812-0000-1000-8000-00805f9b34fb] (Human Interface Device)
    09-09 10:55:35.470    Bonded = [12]
    09-09 10:55:35.470    --------------------------------
    09-09 10:55:35.472    Name = [Motorola S605]
    09-09 10:55:35.472    Address = [00:1C:EF:25:E5:D4]
    09-09 10:55:35.474    Type = [1]
    09-09 10:55:35.479    UUID = [0000111e-0000-1000-8000-00805f9b34fb] (Handsfree)
    09-09 10:55:35.480    UUID = [0000110b-0000-1000-8000-00805f9b34fb] (Audio Sink)
    09-09 10:55:35.480    UUID = [0000110e-0000-1000-8000-00805f9b34fb] (A/V Remote Control)
    09-09 10:55:35.480    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.480    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.481    Bonded = [12]
    09-09 10:55:35.482    --------------------------------
    09-09 10:55:35.484    Name = [CBB]
    09-09 10:55:35.484    Address = [00:1D:43:00:A8:09]
    09-09 10:55:35.486    Type = [1]
    09-09 10:55:35.492    UUID = [00001108-0000-1000-8000-00805f9b34fb] (Headset)
    09-09 10:55:35.492    UUID = [0000111e-0000-1000-8000-00805f9b34fb] (Handsfree)
    09-09 10:55:35.492    UUID = [00001105-0000-1000-8000-00805f9b34fb] (OBEX Object Push)
    09-09 10:55:35.493    UUID = [0000110b-0000-1000-8000-00805f9b34fb] (Audio Sink)
    09-09 10:55:35.493    UUID = [0000110e-0000-1000-8000-00805f9b34fb] (A/V Remote Control)
    09-09 10:55:35.493    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.493    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.495    Bonded = [12]
    09-09 10:55:35.495    --------------------------------
    09-09 10:55:35.497    Name = [WONEW-BR35]
    09-09 10:55:35.498    Address = [FF:FF:00:05:F3:BB]
    09-09 10:55:35.499    Type = [2]
    09-09 10:55:35.504    UUID = [00001812-0000-1000-8000-00805f9b34fb] (Human Interface Device)
    09-09 10:55:35.505    Bonded = [12]
    09-09 10:55:35.505    --------------------------------
    09-09 10:55:35.507    Name = [SANSUI]
    09-09 10:55:35.507    Address = [F4:4E:FD:14:63:66]
    09-09 10:55:35.509    Type = [1]
    09-09 10:55:35.514    UUID = [0000111e-0000-1000-8000-00805f9b34fb] (Handsfree)
    09-09 10:55:35.514    UUID = [0000110b-0000-1000-8000-00805f9b34fb] (Audio Sink)
    09-09 10:55:35.514    UUID = [0000110e-0000-1000-8000-00805f9b34fb] (A/V Remote Control)
    09-09 10:55:35.514    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.514    UUID = [00000000-0000-1000-8000-00805f9b34fb] (null)
    09-09 10:55:35.516    Bonded = [12]
*/
