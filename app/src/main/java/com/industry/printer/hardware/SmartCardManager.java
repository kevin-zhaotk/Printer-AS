package com.industry.printer.hardware;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.industry.printer.BuildConfig;
import com.industry.printer.ControlTabActivity;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PrinterApplication;
import com.industry.printer.R;
import com.industry.printer.ThreadPoolManager;
import com.industry.printer.Utils.Debug;

import java.util.Timer;
import java.util.TimerTask;

public class SmartCardManager implements IInkDevice {
    private static final String TAG = SmartCardManager.class.getSimpleName();

    public static boolean SKIP_SMARTCARD_ACCESS = false;
    public static boolean IGNORE_CONSISTENCY_CHECK = true;

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
    private int[] mCurrentLevels;
    private int mCurLvlNums;

    private AlertDialog mRecvedLevelPromptDlg = null;

    private static final int MSG_SHOW_CONSISTENCY       = 101;
    private static final int MSG_SHOW_LEVEL             = 102;
    private static final int MSG_SHOW_LOCAL_INK         = 103;

//    private final static int UPDATE_LEVEL_INTERVAL      = 1000 * 30;
    private final static int READ_LEVEL_INTERVAL        = 10;
    private final static int MSG_READ_CONSISTENCY_INTERVAL   = 1000 * 60;

    private Timer mTimer = null;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SHOW_CONSISTENCY:
                    if(null != mRecvedLevelPromptDlg) {
                        mRecvedLevelPromptDlg.setTitle("Legality");
                        mRecvedLevelPromptDlg.setMessage((String)msg.obj);
                        mRecvedLevelPromptDlg.show();
                        mRecvedLevelPromptDlg.show();
                    }
                    break;
                case MSG_SHOW_LEVEL:
                    if(null != mRecvedLevelPromptDlg) {
                        StringBuilder sb = new StringBuilder();
                        sb.append("Level: ");
                        for(int i=0; i<mCurLvlNums; i++) {
                            if(i!=0)
                                sb.append(",");
                            sb.append(mCurrentLevels[i]/100000);
                        }

                        if(mAddInkFailed) {
                            sb.append("\n");
                            sb.append("!!! Adding Ink Failed !!!");
                        } else if(mBlockAdding) {
                            sb.append("\n");
                            sb.append("--- Adding Ink (" + mInkAddedTimes + ") ---");
                        }

                        mRecvedLevelPromptDlg.setMessage(sb.toString());
                        mRecvedLevelPromptDlg.show();
                        mRecvedLevelPromptDlg.show();
                    }
                    break;
                case MSG_SHOW_LOCAL_INK:
//                    mRecvedLevelPromptDlg.setTitle("Ink Level");
//                    mRecvedLevelPromptDlg.setMessage("" + mInkLevel);
//                    mRecvedLevelPromptDlg.show();
//                    mRecvedLevelPromptDlg.show();
                    break;
            }
        }
    };

    public SmartCardManager(Context context) {
        Debug.d(TAG, "APK VERSION_CODE: " + BuildConfig.VERSION_CODE);
        Debug.d(TAG, "---> enter SmartCardManager()");
        mContext = context;
        mValid = true;
        mInitialied = false;
        mInkLevel = -1;
        mCallback = null;

        mReadLevels = new int[READ_LEVEL_TIMES];
        mCurrentLevels = new int[10];
        mCurLvlNums = 0;

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

    boolean isOpeningInit = false;

    @Override
    public void init(Handler callback) {
        Debug.d(TAG, "---> enter init()");

        if(mInitialied) return;

        if(mCallback == null) {
            isOpeningInit = true;
        } else {
            isOpeningInit = false;
        }
        mCallback = callback;

        ThreadPoolManager.mThreads.execute(new Runnable() {
            @Override
            public void run() {
                int ret = -1;
                if(!SKIP_SMARTCARD_ACCESS) {
                    ret = SmartCard.init();
                } else {
                    ret = SmartCard.SC_SUCCESS;
                }

                if(SmartCard.SC_SUCCESS == ret) {
                    mInitialied = true;
                    checkConsistency();
                    checkOIB(HP_PRINT_CARTRIDGE);
//            checkOIB(HP_BULK_CARTRIDGE);
                    mInkLevel = (int)getLocalInk(0);

                    if(isOpeningInit) {
                        shutdown();
                        if(mValid) mCallback.sendEmptyMessage(MSG_SMARTCARD_INIT_SUCCESS);
                    } else {
                        if(mValid) mCallback.sendEmptyMessage(MSG_SMARTCARD_CHECK_SUCCESS);
                        if(null == mTimer) {
                            mTimer = new Timer();
                            mTimer.scheduleAtFixedRate(new TimerTask() {
                                @Override
                                public void run() {
                                    checkConsistency();
                                    readConsistency();
                                }
                            }, (long)5000, (long)MSG_READ_CONSISTENCY_INTERVAL);
                        }
                    }
                } else {
                    mInitialied = false;
                    if(isOpeningInit) {
                        mCallback.obtainMessage(MSG_SMARTCARD_INIT_FAILED, ret, 0, null).sendToTarget();
                    } else {
                        mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, ret, 0, null).sendToTarget();
                    }
                }
            }
        });
    }

    private void checkConsistency() {
        Debug.d(TAG, "---> enter checkConsistency()");

        if(SKIP_SMARTCARD_ACCESS || IGNORE_CONSISTENCY_CHECK) return;

        if(mInitialied && mValid) {
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

        if(SKIP_SMARTCARD_ACCESS) {
            mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Skipping SC access!").sendToTarget();
            return;
        }

        if(mInitialied && mValid) {
            synchronized (this) {
                mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, SmartCard.readConsistency()).sendToTarget();
            }
        } else {
            mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Not initialized or invalid!").sendToTarget();
        }
    }

    private void checkOIB(int cardType) {
        Debug.d(TAG, "---> enter checkOIB(" + cardType + ")");

        if(SKIP_SMARTCARD_ACCESS) return;

        if(mInitialied && mValid) {
            synchronized (this) {
                int ret = SmartCard.chechOIB(cardType);
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

    private void addInkOn() {
        Debug.d(TAG, "Add ink opened!");
        ExtGpio.writeGpio('b', 11, 1);
        mBlockAdding = true;
    }

    private void addInOff() {
        Debug.d(TAG, "Add ink closed!");
        ExtGpio.writeGpio('b', 11, 0);
    }

    private int mInkAddedTimes = 0;
    private boolean mAddInkFailed = false;

    private void levelValueUpdated() {
        long sum = 0;
        for(int i=0; i<READ_LEVEL_TIMES; i++) {
            sum += mReadLevels[i];
        }

        int currentLevel = (int)(sum / READ_LEVEL_TIMES);
        Debug.d(TAG, "Current Level = " + currentLevel);

        if(currentLevel < 12000000 || currentLevel > 16000000) return;

        int avgLevel = 13500000;
        if(mCurLvlNums < 10) {
            mCurrentLevels[mCurLvlNums] = currentLevel;
            mCurLvlNums++;
        } else {
            long totalLevel = 0;
            for(int i=0; i<10; i++) {
                if(i == 9) {
                    mCurrentLevels[i] = currentLevel;
                } else {
                    mCurrentLevels[i] = mCurrentLevels[i+1];
                }
                totalLevel += mCurrentLevels[i];
            }
            avgLevel = (int)(totalLevel / 10);
        }

        if(avgLevel < 13500000) {
            if(!mBlockAdding) {
                if(mInkAddedTimes >= 3) {
                    mAddInkFailed = true;
                    new Thread() {
                        @Override
                        public void run() {
                            try{
                                ExtGpio.playClick();
                                sleep(50);
                                ExtGpio.playClick();
                                sleep(50);
                                ExtGpio.playClick();
                            } catch (Exception e) {
                                Debug.e(TAG, e.getMessage());
                            }
                        }
                    }.start();
                } else {
                    addInkOn();

                    try{Thread.sleep(1500);}catch(Exception e){
                        addInOff();
                    };
                    addInOff();

                    try{Thread.sleep(2 * 60 * 1000);}catch(Exception e){};
                    Debug.d(TAG, "Clear Adding Block!");
                    mBlockAdding = false;

                    mInkAddedTimes++;
                }
            }
        } else {
            mAddInkFailed = false;
            mInkAddedTimes = 0;
        }

        mHandler.sendEmptyMessage(MSG_SHOW_LEVEL);
    }

    public void updateLevel() {
        ThreadPoolManager.mThreads.execute(new Runnable() {
            @Override
            public void run() {
                for(int i=0; i<READ_LEVEL_TIMES; i++) {
                    readLevelValue(i);
                    try{Thread.sleep(READ_LEVEL_INTERVAL);}catch(Exception e){};
                }
                levelValueUpdated();
            }
        });
    }

    @Override
    public boolean checkUID(int heads) {
        Debug.d(TAG, "---> enter checkUID()");

        init(mCallback);

        return true;
    }

    public void shutdown() {
        if(mInitialied) {
            if(null != mTimer) {
                mTimer.cancel();
                mTimer = null;
            }

            mInitialied = false;
            synchronized (this) {
                SmartCard.shutdown();
            }
        }
    }

    @Override
    public float getLocalInk(int head) {
        Debug.d(TAG, "---> enter getLocalInk()");

        if(mInkLevel == -1) {
            if(mInitialied && mValid) {
                if(SKIP_SMARTCARD_ACCESS) {
                    mInkLevel = 2000;           // 如果跳过Smartcard访问标识开启，则返回一个恰当值
                } else {
                    synchronized (this) {
                        mInkLevel = SmartCard.getLocalInk(HP_PRINT_CARTRIDGE);
//                    mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
                    }
                }
            }
        }

        Debug.d(TAG, "---> Ink Level = " + mInkLevel);
        mHandler.sendEmptyMessage(MSG_SHOW_LOCAL_INK);
        return 1.0f * mInkLevel;
    }

    @Override
    public float getLocalInkPercentage(int head) {
        Debug.d(TAG, "---> enter getLocalInkPercentage()");
        return (100.0f * mInkLevel / MAX_INK_VOLUME);
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

        if(mInitialied && mValid) {
            ThreadPoolManager.mThreads.execute(new Runnable() {
                @Override
                public void run() {
                    synchronized (this) {
                        SmartCard.downLocal(HP_PRINT_CARTRIDGE);
//                SmartCard.downLocal(HP_BULK_CARTRIDGE);
                        mInkLevel = SmartCard.getLocalInk(HP_PRINT_CARTRIDGE);
//                mInkLevel = SmartCard.getLocalInk(HP_BULK_CARTRIDGE);
                    }
                    checkOIB(HP_PRINT_CARTRIDGE);
//            checkOIB(HP_BULK_CARTRIDGE);
                }
            });
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
