package com.printer.phoneapp;

import android.Manifest;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.hardware.camera2.CameraManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.zxing.common.StringUtils;
import com.printer.phoneapp.Devices.ConnectDevice;
import com.printer.phoneapp.Devices.ConnectDeviceManager;
import com.printer.phoneapp.Sockets.BluetoothManager;
import com.printer.phoneapp.Sockets.SocketThread;
import com.printer.phoneapp.UIs.AddBTDevicePopWindow;
import com.printer.phoneapp.UIs.AddWifiDevicePopWindow;
import com.printer.phoneapp.UIs.BarcodeScanPopupWindow;
import com.printer.phoneapp.UIs.SendStringCmdPopWindow;
import com.printer.phoneapp.Utils.HTPermission;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Created by hmwan on 2021/9/7.
 */

public class PhoneMainActivity extends AppCompatActivity {
    public static final String TAG = PhoneMainActivity.class.getSimpleName();

    private HashMap<String, BroadcastReceiver> mPermissionResultReceivers = new HashMap<String, BroadcastReceiver>();
    public static final int PERMISSION_REQUEST_STORAGE      = 17101;
    public static final int PERMISSION_REQUEST_CAMERA       = 17102;
    public static final int PERMISSION_REQUEST_BLUETOOTH    = 17103;

    private LinearLayout mDeviceAddingArea = null;
    private TextView mAddWIFIDevice0 = null;
    private TextView mAddBTDevice0 = null;

    private LinearLayout mConnectStatusArea = null;
    private TextView mAddWIFIDevice = null;
    private TextView mAddBTDevice = null;
    private ScrollView mDeviceScrollView = null;
    private LinearLayout mDevicesList = null;

    private LinearLayout mCommandsArea = null;

    private TextView mSendStringCmd = null;
    private TextView mScanBarcodeCmd = null;

    private ConnectDeviceManager mConDevManager = null;
    private BluetoothManager mBluetoothManager = null;

    private static final int MSG_ADD_SAVED_DEVICES = 10001;

    public Handler mHandler = new Handler(){
        public void handleMessage(Message msg) {
            switch(msg.what) {
                case MSG_ADD_SAVED_DEVICES:
                    for(int i=mConDevManager.size()-1; i>=0; i--) {
                        addDeviceView(mConDevManager.getDevice(i));
                    }
                    adjustDeviceSVHeight();
                    break;
            }
        }
    };

    private void adjustDeviceSVHeight() {
        int orgSVHeight = mDeviceScrollView.getPaddingTop() + mDeviceScrollView.getPaddingBottom();
        int lineHeight = 0;
        LinearLayout linearLayout = (LinearLayout)mDevicesList.getChildAt(0);
        if(null != linearLayout) {
            lineHeight += (linearLayout.getPaddingTop() + linearLayout.getPaddingBottom());
            TextView textView = (TextView) linearLayout.findViewById(R.id.Text);
            lineHeight += Math.round(textView.getTextSize() * 1.35f);
        }

        LinearLayout.LayoutParams layoutParams =
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, orgSVHeight + lineHeight * Math.min(mConDevManager.size(), 5));
        mDeviceScrollView.setLayoutParams(layoutParams);
    }

    private void addDevice(String address, final int type, boolean selected) {
        addDeviceView(mConDevManager.addNewDevice(address, type, selected));
        adjustDeviceSVHeight();
    }

    private void addDeviceView(final ConnectDevice dev) {
        if(null == dev) return;

        final LinearLayout linearLayout;

        linearLayout = (LinearLayout) LayoutInflater.from(PhoneMainActivity.this).inflate(R.layout.phone_main_act_device_item, null);
        mDevicesList.addView(linearLayout, 0);
        dev.setLinearLayout(linearLayout);

        linearLayout.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                View popupView = LayoutInflater.from(PhoneMainActivity.this).inflate(R.layout.phone_connect_device_item_long_click_context_menu, null);
                final PopupWindow mPopupWindow = new PopupWindow(popupView, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, true);
                // 如果不设置PopupWindow的背景，无论是点击外部区域还是Back键都无法dismiss。只有设置了背景和外部点击事件才能够相应外部点击或者Back键返回
                mPopupWindow.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#ffDDDDDD")));
                // 设置PopupWindow是否能响应外部点击事件
                mPopupWindow.setOutsideTouchable(true);
                // 由于newPopupWindow的时候指定了focusable，因此这里不需要设置
                // 设置PopupWindow是否能响应点击事件
                mPopupWindow.setTouchable(true);
                mPopupWindow.update();

                TextView renameBtn = (TextView) popupView.findViewById(R.id.idRename);
                renameBtn.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
//                        AlertDialog.Builder;
                        mPopupWindow.dismiss();
                    }
                });
                TextView delBtn = (TextView) popupView.findViewById(R.id.idDelete);
                delBtn.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        delDevice(dev);
                        mPopupWindow.dismiss();
                    }
                });

//                mPopupWindow.showAtLocation(v, Gravity.CENTER, 0, 0);
                mPopupWindow.showAsDropDown(v, -mPopupWindow.getWidth(), 0);
                return true;    // 后续不处理
            }
        });

        linearLayout.setTag(dev);
        ImageView iconIV = (ImageView) linearLayout.findViewById(R.id.Icon);
        if(dev.getType() == ConnectDevice.DEVICE_TYPE_WIFI) {
            iconIV.setImageResource(R.drawable.wifi);
        } else if(dev.getType() == ConnectDevice.DEVICE_TYPE_BT) {
            iconIV.setImageResource(R.drawable.bt);
        } else {
            iconIV.setImageBitmap(null);
        }
        final TextView textView = (TextView) linearLayout.findViewById(R.id.Text);
        textView.setText(dev.getName() + "(" + dev.getAddress() + ")");

        final ImageView selIV = (ImageView) linearLayout.findViewById(R.id.idSelected);

        if(dev.isSeleted()) {
            textView.setTextColor(getResources().getColor(R.color.black));
            selIV.setImageResource(R.drawable.check_mark);
        } else {
            textView.setTextColor(getResources().getColor(R.color.gray));
            selIV.setImageBitmap(null);
        }

        linearLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dev.setSeleted(!dev.isSeleted());
                if(dev.isSeleted()) {
                    textView.setTextColor(getResources().getColor(R.color.black));
                    selIV.setImageResource(R.drawable.check_mark);
                } else {
                    textView.setTextColor(getResources().getColor(R.color.gray));
                    selIV.setImageBitmap(null);
                }
                adjustCommandAreaEnability();
            }
        });

        adjustDeviceAreaVisibility();
        adjustCommandAreaEnability();
    }

    private void delDevice(ConnectDevice dev) {
        mConDevManager.deleteDevice(dev);

        delDeviceView(dev);
        adjustDeviceSVHeight();
    }

    private void delDeviceView(ConnectDevice dev) {
        mDevicesList.removeView(dev.getLinearLayout());

        adjustDeviceAreaVisibility();
        adjustCommandAreaEnability();
    }

    private void adjustCommandAreaEnability() {
        for(int i=0; i<mCommandsArea.getChildCount(); i++) {
            mCommandsArea.getChildAt(i).setEnabled(mConDevManager.getSelectededCount() > 0 ? true : false);
        }
    }

    private void adjustDeviceAreaVisibility() {
        if(mConDevManager.size() <= 0) {
            mDeviceAddingArea.setVisibility(View.VISIBLE);
            mConnectStatusArea.setVisibility(View.GONE);
        } else {
            mDeviceAddingArea.setVisibility(View.GONE);
            mConnectStatusArea.setVisibility(View.VISIBLE);
        }
    }

    private View.OnClickListener AddWIFIDeviceButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            AddWifiDevicePopWindow popWindow = new AddWifiDevicePopWindow(PhoneMainActivity.this);
            popWindow.show(mAddWIFIDevice, new ConnectDevice.OnAddDeviceListener() {
                @Override
                public void onAdd(String address, int type) {
                    addDevice(address, type, true);
                }
            });
        }
    };

    private View.OnClickListener AddBluetoothDeviceButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(checkBluetoothAccessPermission() == 0) {
                launchAddBluetoothDevices();
            }
        }
    };

    private void launchAddBluetoothDevices() {
        AddBTDevicePopWindow popWindow = new AddBTDevicePopWindow(PhoneMainActivity.this);
        popWindow.show(mAddBTDevice, new ConnectDevice.OnAddDeviceListener() {
            @Override
            public void onAdd(String address, int type) {
                addDevice(address, type, true);
            }
        });
    }

    private class SocketStringListener implements SocketThread.OnSocketStringListener {
        @Override
        public void onSent(final ConnectDevice dev, final String sent) {
            mCommandsArea.post(new Runnable() {
                @Override
                public void run() {
                Toast.makeText(PhoneMainActivity.this, dev.getName() + " sent: " + sent, Toast.LENGTH_LONG).show();
                }
            });
        }

        @Override
        public void onFailed(final ConnectDevice dev, final String errMsg) {
            mCommandsArea.post(new Runnable() {
                @Override
                public void run() {
                Toast.makeText(PhoneMainActivity.this, dev.getName() + " failed: " + errMsg , Toast.LENGTH_LONG).show();
                }
            });
        }

        @Override
        public void onReceived(final ConnectDevice dev, final String recv) {
            mCommandsArea.post(new Runnable() {
                @Override
                public void run() {
                Toast.makeText(PhoneMainActivity.this, dev.getName() + " received: " + recv, Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.phone_main_layout);

        mConDevManager = ConnectDeviceManager.getInstance(this);
        mBluetoothManager = BluetoothManager.getInstance(this);

        mDeviceAddingArea = (LinearLayout) findViewById(R.id.DeviceAddingArea);
        mAddWIFIDevice0 = (TextView) findViewById(R.id.CmdAddWIFIDevice0);
        mAddWIFIDevice0.setOnClickListener(AddWIFIDeviceButtonClickListener);
        mAddBTDevice0= (TextView) findViewById(R.id.CmdAddBTDevice0);
        mAddBTDevice0.setOnClickListener(AddBluetoothDeviceButtonClickListener);

        mConnectStatusArea = (LinearLayout) findViewById(R.id.ConnectStatusArea);
        mAddWIFIDevice = (TextView) findViewById(R.id.CmdAddWIFIDevice);
        mAddWIFIDevice.setOnClickListener(AddWIFIDeviceButtonClickListener);
        mAddBTDevice= (TextView) findViewById(R.id.CmdAddBTDevice);
        mAddBTDevice.setOnClickListener(AddBluetoothDeviceButtonClickListener);
        mDeviceScrollView = (ScrollView) findViewById(R.id.ConDevicesScrollView);
        mDevicesList = (LinearLayout) findViewById(R.id.ConDevicesList);

        mCommandsArea = (LinearLayout) findViewById(R.id.CommandsArea);

        mScanBarcodeCmd = (TextView) findViewById(R.id.CmdScanBarcode);
        mScanBarcodeCmd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                launchScanBarcodeProcess();
            }
        });

        mSendStringCmd = (TextView) findViewById(R.id.CmdSendString);
        mSendStringCmd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SendStringCmdPopWindow popWindow = new SendStringCmdPopWindow(PhoneMainActivity.this);
                popWindow.setConDevManager(mConDevManager);
                popWindow.setSocketListener(new SocketStringListener());
                popWindow.show(mSendStringCmd);
            }
        });

        adjustDeviceAreaVisibility();
        adjustCommandAreaEnability();

        mHandler.sendEmptyMessageDelayed(MSG_ADD_SAVED_DEVICES, 100);

        checkWriteExternalStoragePermission();
    }

    private void checkWriteExternalStoragePermission() {
        HTPermission mPermission = new HTPermission(PhoneMainActivity.this, PERMISSION_REQUEST_STORAGE);
        if(!mPermission.isPermissionGuaranteed(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            mPermission.requestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }
    }

    private void launchScanBarcodeProcess() {
        HTPermission mPermission = new HTPermission(PhoneMainActivity.this, PERMISSION_REQUEST_CAMERA);
        if(mPermission.requestPermission(Manifest.permission.CAMERA) == 0) {
            BarcodeScanPopupWindow popWindow = new BarcodeScanPopupWindow(PhoneMainActivity.this);
            popWindow.setSocketListener(new SocketStringListener());
            popWindow.show(mScanBarcodeCmd);
        }
    }

    public int checkBluetoothAccessPermission() {
        String[] needPermissions = new String[] {
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN,
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.ACCESS_COARSE_LOCATION
        };
        HTPermission mPermission = new HTPermission(PhoneMainActivity.this, PhoneMainActivity.PERMISSION_REQUEST_BLUETOOTH);
        return mPermission.requestPermissions(needPermissions);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        Log.d(TAG, "onRequestPermissionsResult requestCode = " + requestCode);
        int grantResult = PackageManager.PERMISSION_DENIED;
        for(int i=0; i<grantResults.length; i++) {
            Log.d(TAG, permissions[i] +": " + grantResults[i]);
            grantResult = grantResults[i];
            if(grantResult != PackageManager.PERMISSION_GRANTED) {
                break;
            }
        }

        if(grantResult == PackageManager.PERMISSION_GRANTED) {
            switch(requestCode) {
                case PERMISSION_REQUEST_STORAGE:
                    break;
                case PERMISSION_REQUEST_CAMERA:
                    launchScanBarcodeProcess();
                    break;
                case PERMISSION_REQUEST_BLUETOOTH:
                    launchAddBluetoothDevices();
                    break;
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // TODO Auto-generated method stub
        if(requestCode == BluetoothManager.REQUEST_ENBLE_BT){
            Log.d(TAG, "onActivityResult resultCode = " + resultCode);
            mBluetoothManager.setEnablingResult(resultCode);
        }
    }
}
