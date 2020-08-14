package com.industry.printer.hardware;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.industry.printer.ControlTabActivity;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PrinterApplication;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

public class SmartCardManager implements IInkDevice {
    private static final String TAG = SmartCardManager.class.getSimpleName();

    public static boolean SKIP_SMARTCARD_ACCESS = false;
    public static boolean IGNORE_CONSISTENCY_CHECK = false;

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

    private boolean mBlockAdding = false;

    private static int READ_LEVEL_TIMES                 = 1;
    private int[] mReadLevels;
    private int mCurrentLevel;

    private AlertDialog mRecvedLevelPromptDlg = null;

    private static final int MSG_CHECK_USABILITY        = 100;
    private static final int MSG_PERIOD_CHECK_LEVEL     = 101;
    private static final int MSG_READ_LEVEL             = 102;
    private static final int MSG_LEVEL_UPDATED          = 103;
    private static final int MSG_ADD_INK_ON             = 104;
    private static final int MSG_ADD_INK_OFF            = 105;
    private static final int MSG_CLEAR_ADDING_BLOCK     = 106;
    private static final int MSG_DISP_LOCAL_INK         = 107;

//    private static final int MSG_CHECK_DOWNTEST       = 1000;
//    private static final int MSG_B11_LAMP_TEST        = 1001;
//    private boolean mLampOn = false;

    private final static int UPDATE_LEVEL_INTERVAL      = 1000 * 30;
    private final static int READ_LEVEL_INTERVAL        = 10;
//    private final static int USABILITY_CHECK_INTERVAL   = 1000 * 60 * 3;
    private final static int USABILITY_CHECK_INTERVAL   = 1000 * 60;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_CHECK_USABILITY:
//                    checkConsistency();
//                    checkOIB();
                    readConsistency();
                    mHandler.sendEmptyMessageDelayed(MSG_CHECK_USABILITY, USABILITY_CHECK_INTERVAL);
                    break;
//                case MSG_CHECK_DOWNTEST:
//                    downLocal(HP_BULK_CARTRIDGE);
//                    mHandler.sendEmptyMessageDelayed(MSG_CHECK_DOWNTEST, 1);
//                    break;
                case MSG_PERIOD_CHECK_LEVEL:
                    mHandler.obtainMessage(MSG_READ_LEVEL, 0, 0).sendToTarget();
                    mHandler.sendEmptyMessageDelayed(MSG_PERIOD_CHECK_LEVEL, UPDATE_LEVEL_INTERVAL);
                    break;
                case MSG_READ_LEVEL:
                    readLevelValue(msg.arg1);
                    if(msg.arg1+1 < READ_LEVEL_TIMES) {
                        mHandler.sendMessageDelayed(obtainMessage(MSG_READ_LEVEL, msg.arg1+1,0), READ_LEVEL_INTERVAL);
                    } else {
                        levelValueUpdated();
                    }
                    break;
                case MSG_LEVEL_UPDATED:
//                    Toast.makeText(mContext, "Current Level: " + mCurrentLevel, Toast.LENGTH_LONG).show();
                    if(null != mRecvedLevelPromptDlg) {
//                        if(mCurrentLevel < 100000 || mCurrentLevel > 150000) {
//                            mRecvedLevelPromptDlg.setMessage("< Invalid Level Value >");
//                        } else {
                            mRecvedLevelPromptDlg.setMessage("Level: " + mCurrentLevel);
//                        }
                        mRecvedLevelPromptDlg.show();
                        mRecvedLevelPromptDlg.show();
                    }
                    break;
                case MSG_ADD_INK_ON:
                    Debug.d(TAG, "Add ink opened!");
                    ExtGpio.writeGpio('b', 11, 1);
                    mBlockAdding = true;
                    sendEmptyMessageDelayed(MSG_ADD_INK_OFF, 1500);
                    sendEmptyMessageDelayed(MSG_CLEAR_ADDING_BLOCK, 2 * 60 * 1000);
                    break;
                case MSG_ADD_INK_OFF:
                    Debug.d(TAG, "Add ink closed!");
                    ExtGpio.writeGpio('b', 11, 0);
                    break;
                case MSG_CLEAR_ADDING_BLOCK:
                    Debug.d(TAG, "Clear Adding Block!");
                    mBlockAdding = false;
                    break;
                case MSG_DISP_LOCAL_INK:
                    mRecvedLevelPromptDlg.setTitle("Ink Level");
                    mRecvedLevelPromptDlg.setMessage("" + mInkLevel);
                    mRecvedLevelPromptDlg.show();
                    mRecvedLevelPromptDlg.show();
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
                mRecvedLevelPromptDlg.hide();
//                mRecvedLevelPromptDlg.dismiss();
//                mRecvedLevelPromptDlg = null;
            }
        }).create();
    }

    @Override
    public void init(Handler callback) {
        Debug.d(TAG, "---> enter init()");

        if(mInitialied) return;

        mCallback = callback;

        int ret = -1;
        if(!SKIP_SMARTCARD_ACCESS) {
            ret = SmartCard.init();
        } else {
            ret = SmartCard.SC_SUCCESS;
        }

        if(SmartCard.SC_SUCCESS == ret) {
            mInitialied = true;
            mCallback.sendEmptyMessage(MSG_SMARTCARD_INIT_SUCCESS);
            checkConsistency();
            checkOIB();
            mHandler.sendEmptyMessage(MSG_CHECK_USABILITY);
//            mHandler.sendEmptyMessageDelayed(MSG_CHECK_DOWNTEST, 5000);
//            mHandler.sendEmptyMessageDelayed(MSG_B11_LAMP_TEST, 10000);
//            mHandler.sendEmptyMessageDelayed(MSG_PERIOD_CHECK_LEVEL, UPDATE_LEVEL_INTERVAL);
//            mValid = true;
        } else {
            mValid = false;
            mCallback.obtainMessage(MSG_SMARTCARD_INIT_FAILED, ret, 0, null).sendToTarget();
        }
    }

    private void checkConsistency() {
        Debug.d(TAG, "---> enter checkConsistency()");

        if(SKIP_SMARTCARD_ACCESS || IGNORE_CONSISTENCY_CHECK) return;

        if(mInitialied) {
            synchronized (this) {
                int ret = SmartCard.checkConsistency();
                if(SmartCard.SC_SUCCESS != ret) {
                    mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, ret, 0, null).sendToTarget();
                }
            }
        }
    }

    private void readConsistency() {
        Debug.d(TAG, "---> enter readConsistency()");

        if(SKIP_SMARTCARD_ACCESS || IGNORE_CONSISTENCY_CHECK) return;

        if(mInitialied) {
            synchronized (this) {
                String ret = SmartCard.readConsistency();
                if(null != mRecvedLevelPromptDlg) {
                    mRecvedLevelPromptDlg.setTitle("Legality");
                    mRecvedLevelPromptDlg.setMessage(ret);
                    mRecvedLevelPromptDlg.show();
                    mRecvedLevelPromptDlg.show();
                }
            }
        }
    }

    private void checkOIB() {
        Debug.d(TAG, "---> enter checkOIB()");

        if(SKIP_SMARTCARD_ACCESS) return;

        if(mInitialied) {
            synchronized (this) {
                int ret = SmartCard.chechOIB(HP_BULK_CARTRIDGE);
                if(0 != ret) {
                    mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_OUT_OF_INK_ERROR, 0, null).sendToTarget();
                }
            }
        }
    }

    private void readLevelValue(int count) {
        if(SKIP_SMARTCARD_ACCESS) {
            mReadLevels[count] = 14000000;
        } else if(mInitialied) {
            int level;
            synchronized (this) {
                level = SmartCard.readLevel(LEVEL_SENSOR);
            }
            if((level & 0xF0000000) == 0x00000000) {
                Debug.d(TAG, "Read Level " + (count + 1) + " times = " + level);
                mReadLevels[count] = level;
            } else {
                Debug.e(TAG, "Read Level Error: " + Integer.toHexString(level & 0xF0000000));
                mReadLevels[count] = level;
            }
        }
    }

    private void levelValueUpdated() {
        long sum = 0;
        for(int i=0; i<READ_LEVEL_TIMES; i++) {
            sum += mReadLevels[i];
        }
        mCurrentLevel = (int)(sum / READ_LEVEL_TIMES);
        Debug.d(TAG, "Current Level = " + mCurrentLevel);

        if(mCurrentLevel < 12000000 || mCurrentLevel > 16000000) return;

        mHandler.sendEmptyMessage(MSG_LEVEL_UPDATED);

        if(mCurrentLevel < 13500000 && !mBlockAdding) {
            mHandler.sendEmptyMessage(MSG_ADD_INK_ON);
        }
    }

    public void updateLevel() {
        mHandler.obtainMessage(MSG_READ_LEVEL, 0, 0).sendToTarget();
    }

    @Override
    public boolean checkUID(int heads) {
        Debug.d(TAG, "---> enter checkUID()");

/*        new Thread(new Runnable() {
            @Override
            public void run() {
                mInitialied = false;
                init(mCallback);
            }
        }).start();
*/
        if(SKIP_SMARTCARD_ACCESS) {
            mCallback.sendEmptyMessageDelayed(MSG_SMARTCARD_CHECK_SUCCESS, 100);
            return true;
        } else {
            if (mValid) {
                checkConsistency();
            } else {
                mCallback.sendEmptyMessageDelayed(MSG_SMARTCARD_CHECK_FAILED, 100);
            }

            if(mValid) {
                checkOIB();
            } else {
                // 如果mValid为false，在checkConsistency函数里面就已经发送异常了，无需再次发送
            }

            if(mValid) {
                mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
                mCallback.sendEmptyMessageDelayed(MSG_SMARTCARD_CHECK_SUCCESS, 100);
            } else {
                // 如果mValid为false，在checkOIB函数里面就已经发送异常了，无需再次发送
            }
        }

        return true;
    }

    @Override
    public float getLocalInk(int head) {
        Debug.d(TAG, "---> enter getLocalInk()");

        if(mInkLevel == 0) {
            if(SKIP_SMARTCARD_ACCESS) {
                mInkLevel = 2000;           // 如果跳过Smartcard访问标识开启，则返回一个恰当值
            } else if(mInitialied) {
                synchronized (this) {
                    mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
                }
            }
        }

        Debug.d(TAG, "---> Ink Level = " + mInkLevel);
        mHandler.sendEmptyMessage(MSG_DISP_LOCAL_INK);
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

        mInkLevel--;

        if(SKIP_SMARTCARD_ACCESS) return;

        if(mInitialied) {
            synchronized (this) {
                SmartCard.downLocal(HP_BULK_CARTRIDGE);
                mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
            }
            checkOIB();
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
