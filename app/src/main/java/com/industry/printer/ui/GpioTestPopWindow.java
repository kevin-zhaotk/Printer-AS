package com.industry.printer.ui;

import android.app.ActionBar;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.R;
import com.industry.printer.Serial.SerialHandler;
import com.industry.printer.Serial.SerialPort;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.hardware.ExtGpio;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by hmwan on 2022/1/17.
 */

public class GpioTestPopWindow {
    public static final String TAG = GpioTestPopWindow.class.getSimpleName();

    String[] IN_PINS = new String[] {
            "PG0", "PI5", "PI6", "PE7", "PE8", "PE9", "PE10", "PE11"
    };

    String[] OUT_PINS = new String[] {
            "PI8", "PB11", "PG4", "PH26", "PH27", "PE4", "PE5", "Serial"
    };

    private final int PIN_ENABLE = 1;
    private final int PIN_DISABLE = 0;

    private final int FOR_OUT_PINS = 0;
    private final int FOR_IN_PINS = 1;

    private Context mContext = null;
    private PopupWindow mPopupWindow = null;
    private LinearLayout mOutPinLayout = null;
    private LinearLayout mInPinLayout = null;
    private TextView[] mOutPins = null;
    private TextView[] mInPins = null;
    private Timer mTimer = null;
    private int mCurrentOutPin = 0;
    private boolean mSerialWritting = false;

    private final int MESSAGE_SET_BGCOLOR = 102;
    private final int MSG_AUTOTEST_NEXT = 103;
    private final int MSG_CHECK_PINS = 104;
    private final int MSG_TERMINATE_TEST = 105;
    private final int MSG_TEST_IN_PIN = 106;

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_SET_BGCOLOR:
                    TextView tv = (TextView)msg.obj;
                    if(msg.arg1 == PIN_ENABLE) {
                        tv.setBackgroundColor(Color.GREEN);
                    } else {
                        tv.setBackgroundColor(Color.GRAY);
                    }
                    break;
                case MSG_AUTOTEST_NEXT:
                    toggleOutPin(mCurrentOutPin);
/*
                    if(mCurrentOutPin != 7) {
                        mSerialWritting = false;
                        try {
                            ExtGpio.writeGpioTestPin(OUT_PINS[mCurrentOutPin].charAt(1), Integer.valueOf(OUT_PINS[mCurrentOutPin].substring(2)), PIN_ENABLE);
                        } catch (NumberFormatException e) {
                            Debug.e(TAG, e.getMessage());
                        }
                    } else {    // 串口
                        mSerialWritting = true;
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                SerialPort sp = SerialHandler.getInstance().getSerialPort();
                                if(null != sp) {
                                    while(mSerialWritting) {
                                        sp.writeSerial(new byte[]{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
                                    }
                                }
                            }
                        }).start();
                    }
                    sendEmptyMessageDelayed(MSG_CHECK_PINS, 100);
                    break;
                case MSG_CHECK_PINS:
                    int value;
                    if(mCurrentOutPin != 7) {
                        value = ExtGpio.readGpioTestPin(OUT_PINS[mCurrentOutPin].charAt(1), Integer.valueOf(OUT_PINS[mCurrentOutPin].substring(2)));
                        if(value == 0) {
                            mOutPins[mCurrentOutPin].setBackgroundColor(Color.RED);
                        } else {
                            mOutPins[mCurrentOutPin].setBackgroundColor(Color.GREEN);
                        }
                    } else {    // 串口
                        mOutPins[mCurrentOutPin].setBackgroundColor(Color.GREEN);
                    }

                    value = ExtGpio.readGpioTestPin(IN_PINS[mCurrentOutPin].charAt(1), Integer.valueOf(IN_PINS[mCurrentOutPin].substring(2)));
                    if(value != 0) {
                        mInPins[mCurrentOutPin].setBackgroundColor(Color.RED);
                    } else {
                        mInPins[mCurrentOutPin].setBackgroundColor(Color.GREEN);
                    }
*/
                    mCurrentOutPin++;
                    mCurrentOutPin %= 8;
                    if(mCurrentOutPin != 0) sendEmptyMessageDelayed(MSG_AUTOTEST_NEXT, 1000); else sendEmptyMessageDelayed(MSG_TERMINATE_TEST, 1000);
                    break;
                case MSG_TEST_IN_PIN:
                    testInPin(msg.arg1);
                    break;
                case MSG_TERMINATE_TEST:
                    mHandler.removeMessages(MSG_TEST_IN_PIN);
                    mSerialWritting = false;
//                    resetOutPins();
//                    resetInPins();
                    break;
            }
        }
    };

    private View.OnClickListener mOutPinBtnClickListener = new View.OnClickListener(){
        @Override
        public void onClick(View v) {
            Integer index = (Integer)v.getTag();
            Debug.d(TAG, "Out Index: " + index);

            toggleOutPin(index);
        }
    };

    private View.OnClickListener mInPinBtnClickListener = new View.OnClickListener(){
        @Override
        public void onClick(View v) {
            Integer index = (Integer)v.getTag();
            Debug.d(TAG, "In Index: " + index);

//            updateInPins();
        }
    };

    public GpioTestPopWindow(Context ctx) {
        mContext = ctx;
    }

    public void show(View v) {
        if(null == mContext) {
            return;
        }

        View popupView = LayoutInflater.from(mContext).inflate(R.layout.gpio_test_pop, null);

        mPopupWindow = new PopupWindow(popupView, ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT, true);
        mPopupWindow.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#CC000000")));
        mPopupWindow.setOutsideTouchable(true);
        mPopupWindow.setTouchable(true);
        mPopupWindow.update();

        TextView quitIV = (TextView)popupView.findViewById(R.id.btn_quit);
        quitIV.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                mTimer.cancel();
//                mTimer = null;
                mSerialWritting = false;
                mPopupWindow.dismiss();
            }
        });

        TextView autoTest = (TextView)popupView.findViewById(R.id.btn_autotest);
        autoTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resetOutPins();
                resetInPins();
                mSerialWritting = false;
                mCurrentOutPin = 0;
                mAutoTest = true;
                mHandler.sendEmptyMessage(MSG_AUTOTEST_NEXT);
            }
        });

        mOutPinLayout = (LinearLayout) popupView.findViewById(R.id.out_pin_area);
        mOutPins = new TextView[OUT_PINS.length];
        for(int i=0; i<OUT_PINS.length; i++) {
            TextView tv = new TextView(mContext);
            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            lp.setMargins(0,3,0,3);
            tv.setLayoutParams(lp);
            tv.setPadding(0,5,0,5);
            tv.setGravity(Gravity.CENTER);
            tv.setBackgroundColor(Color.GRAY);
            tv.setTextColor(Color.BLACK);
            tv.setTextSize(20);
            tv.setTag(i);
//            tv.setText(OUT_PINS[i]);
            tv.setText("Out-" + (i+1));
            tv.setOnClickListener(mOutPinBtnClickListener);
            mOutPinLayout.addView(tv);
            mOutPins[i] = tv;
        }

        mInPinLayout = (LinearLayout) popupView.findViewById(R.id.in_pin_area);
        mInPins = new TextView[IN_PINS.length];
        for(int i=0; i<IN_PINS.length; i++) {
            TextView tv = new TextView(mContext);
            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            lp.setMargins(0,3,0,3);
            tv.setLayoutParams(lp);
            tv.setPadding(0,5,0,5);
            tv.setGravity(Gravity.CENTER);
            tv.setBackgroundColor(Color.GRAY);
            tv.setTextColor(Color.BLACK);
            tv.setTextSize(20);
            tv.setTag(i);
//            tv.setText(IN_PINS[i]);
            tv.setText("In-" + (i+1));
            tv.setOnClickListener(mInPinBtnClickListener);
            mInPinLayout.addView(tv);
            mInPins[i] = tv;
        }

        mPopupWindow.showAtLocation(v, Gravity.NO_GRAVITY, 0, 0);

        resetOutPins();
        resetInPins();
/*
        mTimer = new Timer();
        mTimer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                resetOutPins();
                resetInPins();

                toggleOutPin(mCurrentOutPin);

                updateOutPins();

                updateInPins();

                mCurrentOutPin++;
                mCurrentOutPin %= 8;
            }
        }, 1000L, 1000L);
*/
    }

    private void resetOutPins() {
        for (int i = 0; i < mOutPins.length; i++) {
            if(i != 7) {
                try {
                    ExtGpio.writeGpioTestPin(OUT_PINS[i].charAt(1), Integer.valueOf(OUT_PINS[i].substring(2)), PIN_DISABLE);
                } catch (NumberFormatException e) {
                    Debug.e(TAG, e.getMessage());
                }
            } else {
                // 串口
            }
            mOutPins[i].setBackgroundColor(Color.GRAY);
        }
    }

    private void resetInPins() {
        for (int i = 0; i < mInPins.length; i++) {
            mInPins[i].setBackgroundColor(Color.GRAY);
        }
    }

    private boolean mBeepOn = false;
    private boolean mAutoTest = false;

    private boolean toggleOutPin(int index) {
        boolean enable = false;

        if (index != 7) {
            try {
                if(index == 2 && !mAutoTest) {
                    Thread.sleep(1000);
                    mBeepOn = !mBeepOn;
                    enable = mBeepOn;
                } else {
                    enable = !(ExtGpio.readGpioTestPin(OUT_PINS[index].charAt(1), Integer.valueOf(OUT_PINS[index].substring(2))) == 0 ? false : true);
                }
//                Debug.d(TAG, "Value = " + enable);
                ExtGpio.writeGpioTestPin(OUT_PINS[index].charAt(1), Integer.valueOf(OUT_PINS[index].substring(2)), (enable ? 1 : 0));
                enable = ExtGpio.readGpioTestPin(OUT_PINS[index].charAt(1), Integer.valueOf(OUT_PINS[index].substring(2))) == 0 ? false : true;
            } catch (NumberFormatException e) {
                Debug.e(TAG, e.getMessage());
                return false;
            } catch (InterruptedException e) {
                Debug.e(TAG, e.getMessage());
                return false;
            }
        } else {
            // 写串口
            mSerialWritting = !mSerialWritting;
            enable = mSerialWritting;
            if(mSerialWritting) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        SerialPort sp = SerialHandler.getInstance().getSerialPort();
                        if(null != sp) {
                            while(mSerialWritting) {
                                sp.writeSerial(new byte[]{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
                            }
                        }
                    }
                }).start();
            }
        }

        mOutPins[index].setBackgroundColor(enable ? Color.GREEN : Color.GRAY);

        mHandler.removeMessages(MSG_TEST_IN_PIN);
        int value = ExtGpio.readGpioTestPin(IN_PINS[index].charAt(1), Integer.valueOf(IN_PINS[index].substring(2)));
        if(value != 0) {
            if(enable) {
                mInPins[index].setBackgroundColor(Color.RED);
                Message msg = mHandler.obtainMessage(MSG_TEST_IN_PIN);
                msg.arg1 = index;
                mHandler.sendMessageDelayed(msg, 10);
            } else {
                mInPins[index].setBackgroundColor(Color.GRAY);
            }
        } else {
            mInPins[index].setBackgroundColor(Color.GREEN);
        }

        return true;
    }

    private void testInPin(int index) {
        int value = ExtGpio.readGpioTestPin(IN_PINS[index].charAt(1), Integer.valueOf(IN_PINS[index].substring(2)));
        if(value != 0) {
            mInPins[index].setBackgroundColor(Color.RED);
            Message msg = mHandler.obtainMessage(MSG_TEST_IN_PIN);
            msg.arg1 = index;
            mHandler.sendMessageDelayed(msg, 10);
        } else {
            mInPins[index].setBackgroundColor(Color.GREEN);
        }
    }

    private void updateOutPins() {
        for (int i = 0; i < OUT_PINS.length; i++) {
            try {
                Message.obtain(mHandler, MESSAGE_SET_BGCOLOR,
                        ExtGpio.readGpioTestPin(OUT_PINS[i].charAt(1), Integer.valueOf(OUT_PINS[i].substring(2))),
                        i, mOutPins[i]).sendToTarget();
            } catch (NumberFormatException e) {
                Debug.e(TAG, e.getMessage());
            }
        }
    }

    private void updateInPins() {
        for (int i = 0; i < IN_PINS.length; i++) {
            try {
                Message.obtain(mHandler, MESSAGE_SET_BGCOLOR,
                        ExtGpio.readGpioTestPin(IN_PINS[i].charAt(1), Integer.valueOf(IN_PINS[i].substring(2))) == 0 ? PIN_ENABLE : PIN_DISABLE,
                        i, mInPins[i]).sendToTarget();
            } catch (NumberFormatException e) {
                Debug.e(TAG, e.getMessage());
            }
        }
    }
}
