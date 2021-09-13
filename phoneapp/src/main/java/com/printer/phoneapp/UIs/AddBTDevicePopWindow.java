package com.printer.phoneapp.UIs;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.printer.phoneapp.Devices.ConnectDevice;
import com.printer.phoneapp.Devices.ConnectDeviceManager;
import com.printer.phoneapp.PhoneMainActivity;
import com.printer.phoneapp.R;
import com.printer.phoneapp.Sockets.BluetoothManager;

import java.util.ArrayList;
import java.util.regex.Pattern;

import static com.printer.phoneapp.UIs.BarcodeScanPopupWindow.TAG;

/**
 * Created by hmwan on 2021/9/10.
 */

public class AddBTDevicePopWindow {
    public static final String TAG = AddBTDevicePopWindow.class.getSimpleName();

    private Context mContext = null;
    private PopupWindow mPopupWindow = null;
    private LinearLayout mDevicesList = null;

    private ConnectDeviceManager mConDevManager = null;
    private BluetoothManager mBluetoothManager = null;

    public AddBTDevicePopWindow(Context ctx) {
        mContext = ctx;
        mConDevManager = ConnectDeviceManager.getInstance(ctx);
        mBluetoothManager = BluetoothManager.getInstance(ctx);
    }

    private void addDeviceView(final BluetoothDevice dev) {
        final LinearLayout linearLayout;

        linearLayout = (LinearLayout) LayoutInflater.from(mContext).inflate(R.layout.phone_add_bt_device_item, null);
        mDevicesList.addView(linearLayout);

        linearLayout.setTag(dev);

        final TextView deviceTV = (TextView) linearLayout.findViewById(R.id.idDevice);
        deviceTV.setText(dev.getName() + "(" + dev.getAddress() + ")");

        final ImageView selIV = (ImageView) linearLayout.findViewById(R.id.idSelected);
        selIV.setImageBitmap(null);
        linearLayout.setSelected(false);

        linearLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!linearLayout.isSelected()) {
                    selIV.setImageResource(R.drawable.check_mark);
                } else {
                    selIV.setImageBitmap(null);
                }
                linearLayout.setSelected(!selIV.isSelected());
            }
        });
    }

    public void show(View v, final ConnectDevice.OnAddDeviceListener l) {
        if(null == mContext || null == mConDevManager) {
            return;
        }

        View popupView = LayoutInflater.from(mContext).inflate(R.layout.phone_add_bt_device, null);
        mPopupWindow = new PopupWindow(popupView, ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT, true);
        mPopupWindow.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#CC000000")));
        mPopupWindow.setOutsideTouchable(true);
        mPopupWindow.setTouchable(true);
        mPopupWindow.update();

        ImageView closeIV = (ImageView)popupView.findViewById(R.id.Close);
        closeIV.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mPopupWindow.dismiss();
            }
        });

        mDevicesList = (LinearLayout) popupView.findViewById(R.id.idDevicesList);

        final TextView searchTV = (TextView)popupView.findViewById(R.id.idDiscovery);
        if(mBluetoothManager.isEnabled()) {
            searchTV.setEnabled(true);
        } else {
            searchTV.setEnabled(false);
        }

        final TextView addTV = (TextView)popupView.findViewById(R.id.idAdd);
        if(mDevicesList.getChildCount() > 0) {
            addTV.setEnabled(true);
        } else {
            addTV.setEnabled(false);
        }
        final ProgressBar progressBar = (ProgressBar)popupView.findViewById(R.id.idDiscovering);

        searchTV.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!mBluetoothManager.isEnabled()) {
                    Toast.makeText(mContext, "Bluetooth not enabled.", Toast.LENGTH_LONG).show();
                    return;
                }
                mBluetoothManager.startDiscovery(new BluetoothManager.OnDiscoveryListener() {
                    @Override
                    public void onDiscoveryStarted() {
                        Log.d(TAG, "Discovery started.");
                        searchTV.post(
                                new Runnable() {
                                    @Override
                                    public void run() {
                                        mDevicesList.removeAllViews();
                                        searchTV.setEnabled(false);
                                        addTV.setEnabled(false);
                                        progressBar.setVisibility(View.VISIBLE);
                                    }
                                }
                        );
                    }

                    @Override
                    public void onDiscoveryFinished() {
                        Log.d(TAG, "Discovery finished.");
                        searchTV.post(
                                new Runnable() {
                                    @Override
                                    public void run() {
                                        searchTV.setEnabled(true);
                                        addTV.setEnabled(true);
                                        progressBar.setVisibility(View.GONE);
                                    }
                                }
                        );
                    }

                    @Override
                    public void onDeviceFound(BluetoothDevice device) {
                        Log.d(TAG, "Device [" + device.getName() + "] found.");
                        addDeviceView(device);
                        addTV.setEnabled(true);
                    }
                });
            }
        });

        addTV.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != l) {
                    for(int i=mDevicesList.getChildCount()-1; i>=0; i--) {
                        View iv = mDevicesList.getChildAt(i);
                        Log.d(TAG, "iv.isSelected() = " + iv.isSelected());
                        if(iv.isSelected()) {
                            BluetoothDevice dev = (BluetoothDevice)iv.getTag();
Log.d(TAG, dev.getAddress() + " -> DEVICE_TYPE_BT");
                            l.onAdd(dev.getAddress(), ConnectDevice.DEVICE_TYPE_BT);
                        }
                    }
                }
                mPopupWindow.dismiss();
            }
        });

        mPopupWindow.showAtLocation(v, Gravity.NO_GRAVITY, 0, 0);
    }
}
