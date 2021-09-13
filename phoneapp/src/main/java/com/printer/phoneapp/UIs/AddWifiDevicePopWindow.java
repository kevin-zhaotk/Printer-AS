package com.printer.phoneapp.UIs;

import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.printer.phoneapp.Devices.ConnectDevice;
import com.printer.phoneapp.Devices.ConnectDeviceManager;
import com.printer.phoneapp.R;

import java.util.regex.Pattern;

/**
 * Created by hmwan on 2021/9/7.
 */

public class AddWifiDevicePopWindow {
    public static final String TAG = AddWifiDevicePopWindow.class.getSimpleName();

    private Context mContext = null;
    private PopupWindow mPopupWindow = null;
    private ConnectDeviceManager mConDevManager = null;

    public AddWifiDevicePopWindow(Context ctx) {
        mContext = ctx;
        mConDevManager = ConnectDeviceManager.getInstance(ctx);
    }

    public void show(View v, final ConnectDevice.OnAddDeviceListener l) {
        if(null == mContext || null == mConDevManager) {
            return;
        }

        View popupView = LayoutInflater.from(mContext).inflate(R.layout.phone_add_wifi_device, null);

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

        final EditText ipET = (EditText)popupView.findViewById(R.id.IP_address);
        final TextView errMsgTV = (TextView)popupView.findViewById(R.id.ErrorMsg);
        ipET.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                errMsgTV.setVisibility(View.GONE);
            }
        });

        TextView okTV = (TextView)popupView.findViewById(R.id.btn_ok);
        okTV.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!Pattern.matches("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$", ipET.getText())) {
                    errMsgTV.setText("Invalid IP Address!");
                    errMsgTV.setVisibility(View.VISIBLE);
                } else if(mConDevManager.hasAdded(ipET.getText().toString())) {
                    errMsgTV.setText("Address already added!");
                    errMsgTV.setVisibility(View.VISIBLE);
                } else {
                    if(null != l) {
                        l.onAdd(ipET.getText().toString(), ConnectDevice.DEVICE_TYPE_WIFI);
                    }
                    mPopupWindow.dismiss();
                }
            }
        });

        mPopupWindow.showAtLocation(v, Gravity.NO_GRAVITY, 0, 0);
    }
}
