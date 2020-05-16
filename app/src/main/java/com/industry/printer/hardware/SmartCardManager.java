package com.industry.printer.hardware;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.industry.printer.ControlTabActivity;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

public class SmartCardManager implements IInkDevice {
    private static final String TAG = SmartCardManager.class.getSimpleName();

    public final static int MAX_INK_VOLUME              = 4700;

    public final static int HP_PRINT_CARTRIDGE          = 11;
    public final static int HP_BULK_CARTRIDGE           = 12;
    public final static int LEVEL_SENSOR                = 21;

    public static final int MSG_SMARTCARD_INIT_SUCCESS  = 11001;
    public static final int MSG_SMARTCARD_INIT_FAILED   = 11002;
    public static final int MSG_SMARTCARD_CHECK_SUCCESS = 11003;
    public static final int MSG_SMARTCARD_CHECK_FAILED  = 11004;

    private boolean mInitialied;
    private Context mContext;
    private Handler mCallback;
    private boolean mValid;
    private int     mInkLevel;

    private static int READ_LEVEL_TIMES                 = 10;
    private int[] mReadLevels;
    private int mCurrentLevel;


    private AlertDialog mRecvedLevelPromptDlg = null;

    private static final int MSG_CHECK_USABILITY        = 100;
    private static final int MSG_UPDATE_LEVEL           = 101;
    private static final int MSG_READ_LEVEL             = 102;
//    private static final int MSG_CHECK_DOWNTEST       = 1000;
//    private static final int MSG_B11_LAMP_TEST        = 1001;
//    private boolean mLampOn = false;

    private final static int UPDATE_LEVEL_INTERVAL      = 1000 * 30;
    private final static int READ_LEVEL_INTERVAL        = 100;
    private final static int USABILITY_CHECK_INTERVAL   = 1000 * 60 * 3;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_CHECK_USABILITY:
                    checkConsistency();
                    checkOIB();
                    mHandler.sendEmptyMessageDelayed(MSG_CHECK_USABILITY, USABILITY_CHECK_INTERVAL);
                    break;
//                case MSG_CHECK_DOWNTEST:
//                    downLocal(HP_BULK_CARTRIDGE);
//                    mHandler.sendEmptyMessageDelayed(MSG_CHECK_DOWNTEST, 1);
//                    break;
                case MSG_UPDATE_LEVEL:
                    mHandler.obtainMessage(MSG_READ_LEVEL, 0, 0).sendToTarget();
                    mHandler.sendEmptyMessageDelayed(MSG_UPDATE_LEVEL, UPDATE_LEVEL_INTERVAL);
                    break;
                case MSG_READ_LEVEL:
                    readLevelValue(msg.arg1);
                    if(msg.arg1+1 < READ_LEVEL_TIMES) {
                        mHandler.sendMessageDelayed(obtainMessage(MSG_READ_LEVEL, msg.arg1+1,0), READ_LEVEL_INTERVAL);
                    } else {
                        levelValueUpdated();
                    }

                    break;
//                case MSG_B11_LAMP_TEST:
//                    if(!mLampOn) {
//                        ExtGpio.writeGpio('b', 11, 1);
//                        mLampOn = true;
//                        mHandler.sendEmptyMessageDelayed(MSG_B11_LAMP, 1000);
//                    } else {
//                        ExtGpio.writeGpio('b', 11, 0);
//                        mLampOn = false;
//                        mHandler.sendEmptyMessageDelayed(MSG_B11_LAMP, 10000);
//                    }
//                    break;
            }
        }
    };

    public SmartCardManager(Context context) {
        Debug.d(TAG, "---> enter SmartCardManager()");
        mContext = context;
        mValid = true;
        mInitialied = false;
        mInkLevel = 0;

        mReadLevels = new int[READ_LEVEL_TIMES];

        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        mRecvedLevelPromptDlg = builder.setTitle("读取LEVEL值").setMessage("").setPositiveButton("关闭", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                mRecvedLevelPromptDlg.dismiss();
                mRecvedLevelPromptDlg = null;
            }
        }).create();
    }

    @Override
    public void init(Handler callback) {
        Debug.d(TAG, "---> enter init()");

        if(mInitialied) return;

        mCallback = callback;

        int ret = SmartCard.init();
        if(SmartCard.SC_SUCCESS == ret) {
            mInitialied = true;
            mCallback.sendEmptyMessage(MSG_SMARTCARD_INIT_SUCCESS);
            mHandler.sendEmptyMessage(MSG_CHECK_USABILITY);
//            mHandler.sendEmptyMessageDelayed(MSG_CHECK_DOWNTEST, 5000);
//            mHandler.sendEmptyMessageDelayed(MSG_B11_LAMP_TEST, 10000);
            mHandler.sendEmptyMessageDelayed(MSG_UPDATE_LEVEL, UPDATE_LEVEL_INTERVAL);
            mValid = true;
        } else {
            mValid = false;
            mCallback.obtainMessage(MSG_SMARTCARD_INIT_FAILED, ret, 0, null).sendToTarget();
        }
    }

    private void checkConsistency() {
        Debug.d(TAG, "---> enter checkConsistency()");
        if(mInitialied) {
            synchronized (this) {
                int ret = SmartCard.chechConsistency(HP_BULK_CARTRIDGE);
                if(SmartCard.SC_SUCCESS != ret) {
                    mValid = false;
                    mCallback.sendEmptyMessage(MSG_SMARTCARD_CHECK_FAILED);
                    return;
                }
            }
        }
    }

    private void checkOIB() {
        Debug.d(TAG, "---> enter checkOIB()");
        if(mInitialied) {
            synchronized (this) {
                int ret = SmartCard.chechOIB(HP_BULK_CARTRIDGE);
                if(0 != ret) {
                    mValid = false;
                    mCallback.sendEmptyMessage(ControlTabActivity.MESSAGE_RFID_ZERO);
                }
            }
        }
    }

    private void readLevelValue(int count) {
        if(mInitialied) {
            int level;
            synchronized (this) {
                level = SmartCard.readLevel(LEVEL_SENSOR);
            }
            Debug.d(TAG, "Read Level " + (count + 1) + " times = " + level);
            mReadLevels[count] = level;
        }
    }

    private void levelValueUpdated() {
        int sum = 0;
        for(int i=0; i<READ_LEVEL_TIMES; i++) {
            sum += mReadLevels[i];
        }
        mCurrentLevel = sum / READ_LEVEL_TIMES;
        Debug.d(TAG, "Current Level = " + mCurrentLevel);

//        Toast.makeText(mContext, "Current Level: " + mCurrentLevel, Toast.LENGTH_LONG).show();

        if(null != mRecvedLevelPromptDlg) {
            mRecvedLevelPromptDlg.setMessage("Level: " + mCurrentLevel);
            mRecvedLevelPromptDlg.show();
            mRecvedLevelPromptDlg.show();
        }
    }

    @Override
    public boolean checkUID(int heads) {
        Debug.d(TAG, "---> enter checkUID()");
        if (mValid) {
            mCallback.sendEmptyMessageDelayed(MSG_SMARTCARD_CHECK_SUCCESS, 100);
        } else {
            mCallback.sendEmptyMessageDelayed(MSG_SMARTCARD_CHECK_FAILED, 100);
        }
        return true;
    }

    @Override
    public float getLocalInk(int head) {
        Debug.d(TAG, "---> enter getLocalInk()");
        if(mInitialied) {
            synchronized (this) {
                mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
            }
        }
        Debug.d(TAG, "---> Ink Level = " + mInkLevel);
        return mInkLevel;
    }

    @Override
    public float getLocalInkPercentage(int head) {
        Debug.d(TAG, "---> enter getLocalInkPercentage()");
        return (100.0f * getLocalInk(1) / MAX_INK_VOLUME);
    }

    @Override
    public boolean isValid(int dev) {
        Debug.d(TAG, "---> enter isValid()");
        return mValid;
    }

    @Override
    public int getFeature(int device, int index) {
//        Debug.d(TAG, "---> enter getFeature()");
        // Just return from parameters.
        if(index == 4) {
            // Voltage
            return SystemConfigFile.getInstance(mContext).getParam(25);
        } else if(index == 5) {
            // Pulse
            return SystemConfigFile.getInstance(mContext).getParam(27);
        }
        return 0;
    }

    @Override
    public void downLocal(int dev) {
        Debug.d(TAG, "---> enter downLocal()");
        if(mInitialied) {
            synchronized (this) {
                SmartCard.downLocal(HP_BULK_CARTRIDGE);
                mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
            }
            checkOIB();
        } else {
            mInkLevel--;
        }
    }

    @Override
    public void switchRfid(int i) {
        Debug.d(TAG, "---> enter switchRfid()");
        // No need to rolling.
    }

    @Override
    public void defaultInkForIgnoreRfid() {
        Debug.d(TAG, "---> enter defaultInkForIgnoreRfid()");
        // No need to implement. In RFID, this function set ink level of every devices to 185.
    }
}
