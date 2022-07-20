package com.industry.printer.ui;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
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
import android.widget.ScrollView;
import android.widget.TextView;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.R;
import com.industry.printer.Serial.SerialHandler;
import com.industry.printer.Serial.SerialPort;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.hardware.ExtGpio;
import com.industry.printer.hardware.IInkDevice;
import com.industry.printer.hardware.InkManagerFactory;
import com.industry.printer.hardware.SmartCardManager;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by hmwan on 2022/1/17.
 */

public class GpioTestPopWindow {
    public static final String TAG = GpioTestPopWindow.class.getSimpleName();

    private final int TEST_PHASE_NONE = 0;
    private final int TEST_PHASE_PINS = 1;
    private final int TEST_PHASE_ID = 2;
    private final int TEST_PHASE_SC = 3;
    private final int TEST_PHASE_LEVEL = 4;
// H.M.Wang 2022-7-20 增加Bag减1的操作内容
    private final int TEST_PHASE_REDUCE_BAG = 5;
// End of H.M.Wang 2022-7-20 增加Bag减1的操作内容

    private int mCurrentTestPhase = TEST_PHASE_NONE;

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

// H.M.Wang 2022-5-20 修改测试页面的布局，追加ID，SC及Level的测试功能。当前仅实现了ID，后续完善SC和Level。修改量很大，未一一标记
    private TextView mTestTitle = null;
    private ScrollView mTestArea = null;
    private TextView mTestResult = null;
// End of H.M.Wang 2022-5-20 修改测试页面的布局，追加ID，SC及Level的测试功能。当前仅实现了ID，后续完善SC和Level

    private LinearLayout mPinsTestArea = null;
    private LinearLayout mOutPinLayout = null;
    private LinearLayout mInPinLayout = null;
    private TextView[] mOutPins = null;
    private TextView[] mInPins = null;
    private Timer mTimer = null;
    private int mCurrentOutPin = 0;
    private boolean mSerialWritting = false;

    private final int MESSAGE_SET_BGCOLOR = 102;
    private final int MSG_PINSTEST_NEXT = 103;
    private final int MSG_CHECK_PINS = 104;
    private final int MSG_TERMINATE_TEST = 105;
    private final int MSG_TEST_IN_PIN = 106;
    private final int MSG_SHOW_TEST_RESULT = 107;
    private final int MSG_SHOW_BAG_CONFIRM_DLG = 108;

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
                case MSG_PINSTEST_NEXT:
                    toggleOutPin(mCurrentOutPin);
                    mCurrentOutPin++;
                    mCurrentOutPin %= 8;
                    if(mCurrentOutPin != 0) sendEmptyMessageDelayed(MSG_PINSTEST_NEXT, 1000); else sendEmptyMessageDelayed(MSG_TERMINATE_TEST, 1000);
                    break;
                case MSG_TEST_IN_PIN:
                    testInPin(msg.arg1);
                    break;
                case MSG_TERMINATE_TEST:
                    mHandler.removeMessages(MSG_TEST_IN_PIN);
                    mSerialWritting = false;
                    break;
                case MSG_SHOW_TEST_RESULT:
                    mTestResult.setTextColor(msg.arg1);
                    mTestResult.setText((CharSequence)msg.obj);
                    break;
                case MSG_SHOW_BAG_CONFIRM_DLG:
                    AlertDialog.Builder builder = new AlertDialog.Builder(mContext);

                    AlertDialog dlg;
                    dlg = builder.setTitle("Confirmation")
                            .setMessage("")
                            .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
//                                    Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
//                                    msg.obj = "Cancelled.\n";
//                                    msg.arg1 = Color.RED;
//                                    mHandler.sendMessage(msg);
                                    dialog.dismiss();
                                }
                            })
                            .setPositiveButton("Do", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    final IInkDevice scm = InkManagerFactory.inkManager(mContext);
                                    if(scm instanceof SmartCardManager) {
                                        ((SmartCardManager) scm).reduceBag(new SmartCardManager.SCTestListener() {
                                            @Override
                                            public void onError(String result) {
                                                Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                                                msg.obj = result;
                                                msg.arg1 = Color.RED;
                                                mHandler.sendMessage(msg);
                                            }

                                            @Override
                                            public void onResult(String result) {
                                                Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                                                msg.obj = result;
                                                msg.arg1 = Color.BLACK;
                                                mHandler.sendMessage(msg);
                                            }
                                        });
                                        dialog.dismiss();
                                    }
                                }
                            })
                            .create();
                    dlg.show();
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
        }
    };

    public GpioTestPopWindow(Context ctx) {
        mContext = ctx;
    }

    private void setTestPhase(int testPhase) {
//        if(testPhase == mCurrentTestPhase) return;

        final IInkDevice scm = InkManagerFactory.inkManager(mContext);
        switch(mCurrentTestPhase) {
            case TEST_PHASE_PINS:
                mHandler.removeMessages(MSG_TEST_IN_PIN);
                mHandler.removeMessages(MSG_PINSTEST_NEXT);
                mPinsTestArea.setVisibility(View.GONE);
                break;
            case TEST_PHASE_ID:
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).stopIDTest();
                }
                mTestArea.setVisibility(View.GONE);
                break;
            case TEST_PHASE_SC:
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).stopSCTest();
                }
                mTestArea.setVisibility(View.GONE);
                break;
            case TEST_PHASE_LEVEL:
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).stopLevelTest();
                }
                mTestArea.setVisibility(View.GONE);
                break;
            case TEST_PHASE_REDUCE_BAG:
                if (scm instanceof SmartCardManager) {
                    ((SmartCardManager) scm).stopBagReduce();
                }
                mTestArea.setVisibility(View.GONE);
                break;
        }

        mCurrentTestPhase = testPhase;

        switch(mCurrentTestPhase) {
            case TEST_PHASE_PINS:
                mTestTitle.setText("Pin Test");
                mPinsTestArea.setVisibility(View.VISIBLE);
                resetOutPins();
                resetInPins();
                mSerialWritting = false;
                mCurrentOutPin = 0;
                mAutoTest = true;
                mHandler.sendEmptyMessage(MSG_PINSTEST_NEXT);
                break;
            case TEST_PHASE_ID:
                mTestTitle.setText("ID Test");
                mTestArea.setVisibility(View.VISIBLE);
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).startIDTest(new SmartCardManager.SCTestListener() {
                        @Override
                        public void onError(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.RED;
                            mHandler.sendMessage(msg);
                        }

                        @Override
                        public void onResult(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.BLACK;
                            mHandler.sendMessage(msg);
                        }
                    });
                } else {
                    Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                    msg.obj = "Smart card not installed.\n";
                    msg.arg1 = Color.RED;
                    mHandler.sendMessage(msg);
                }
                break;
            case TEST_PHASE_SC:
                mTestTitle.setText("SC Test");
                mTestArea.setVisibility(View.VISIBLE);
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).startSCTest(new SmartCardManager.SCTestListener() {
                        @Override
                        public void onError(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.RED;
                            mHandler.sendMessage(msg);
                        }

                        @Override
                        public void onResult(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.BLACK;
                            mHandler.sendMessage(msg);
                        }
                    });
                } else {
                    Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                    msg.obj = "Smart card not installed.\n";
                    msg.arg1 = Color.RED;
                    mHandler.sendMessage(msg);
                }
                break;
            case TEST_PHASE_LEVEL:
                mTestTitle.setText("Level Test");
                mTestArea.setVisibility(View.VISIBLE);
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).startLevelTest(new SmartCardManager.SCTestListener() {
                        @Override
                        public void onError(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.RED;
                            mHandler.sendMessage(msg);
                        }

                        @Override
                        public void onResult(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.BLACK;
                            mHandler.sendMessage(msg);
                        }
                    });
                } else {
                    Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                    msg.obj = "Smart card not installed.\n";
                    msg.arg1 = Color.RED;
                    mHandler.sendMessage(msg);
                }
                break;
            case TEST_PHASE_REDUCE_BAG:
                mTestTitle.setText("Bag Reduction");
                mTestArea.setVisibility(View.VISIBLE);
                if(scm instanceof SmartCardManager) {
                    ((SmartCardManager)scm).startBagReduce(new SmartCardManager.SCTestListener() {
                        @Override
                        public void onError(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.RED;
                            mHandler.sendMessage(msg);
                        }

                        @Override
                        public void onResult(String result) {
                            Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                            msg.obj = result;
                            msg.arg1 = Color.BLACK;
                            mHandler.sendMessage(msg);

                            mHandler.sendEmptyMessageDelayed(MSG_SHOW_BAG_CONFIRM_DLG, 100);
                        }
                    });
                } else {
                    Message msg = mHandler.obtainMessage(MSG_SHOW_TEST_RESULT);
                    msg.obj = "Smart card not installed.\n";
                    msg.arg1 = Color.RED;
                    mHandler.sendMessage(msg);
                }
                break;
        }
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

        mTestTitle = (TextView)popupView.findViewById(R.id.test_title);
        mTestArea = (ScrollView) popupView.findViewById(R.id.id_test_area);
        TextView testID = (TextView)popupView.findViewById(R.id.btn_testID);
        testID.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setTestPhase(TEST_PHASE_ID);
            }
        });
        TextView testSC = (TextView)popupView.findViewById(R.id.btn_testSC);
        testSC.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setTestPhase(TEST_PHASE_SC);
            }
        });
        TextView testLVL = (TextView)popupView.findViewById(R.id.btn_testLVL);
        testLVL.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setTestPhase(TEST_PHASE_LEVEL);
            }
        });
// H.M.Wang 2022-7-20 增加Bag减1的操作内容
        TextView bagMinus1 = (TextView)popupView.findViewById(R.id.btn_bag_1);
        bagMinus1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setTestPhase(TEST_PHASE_REDUCE_BAG);
            }
        });
// End of H.M.Wang 2022-7-20 增加Bag减1的操作内容
        mTestResult = (TextView)popupView.findViewById(R.id.id_test_result);

        mPinsTestArea = (LinearLayout) popupView.findViewById(R.id.pins_test_area);
        TextView testpins = (TextView)popupView.findViewById(R.id.btn_testpins);
        testpins.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setTestPhase(TEST_PHASE_PINS);
            }
        });

        mOutPinLayout = (LinearLayout) popupView.findViewById(R.id.out_pin_area);
        mOutPins = new TextView[OUT_PINS.length];
        for(int i=0; i<OUT_PINS.length; i++) {
            TextView tv = new TextView(mContext);
            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            lp.setMargins(0,1,0,1);
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
            lp.setMargins(0,1,0,1);
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
