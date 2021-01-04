package com.industry.printer.hardware;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;

import com.industry.printer.BuildConfig;
import com.industry.printer.ControlTabActivity;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

import java.util.Timer;
import java.util.TimerTask;

public class SmartCardManager implements IInkDevice {
    private static final String TAG = SmartCardManager.class.getSimpleName();

    public static boolean SMARTCARD_ACCESS = true;
    public static boolean CONSISTENCY_CHECK = false;
    public static boolean OIB_CHECK = false;
    public static boolean SUM_CHECK = false;

    private final static int PEN_VS_BAG_RATIO           = 3;
    private final static int MAX_BAG_INK_VOLUME         = 3150;
    private final static int MAX_PEN_INK_VOLUME         = MAX_BAG_INK_VOLUME * PEN_VS_BAG_RATIO;

    public final static int CARD_PEN1                   = 11;
    public final static int CARD_PEN2                   = 12;
    public final static int CARD_BULK1                  = 13;
    public final static int CARD_BULKX                  = 14;
    public final static int LEVEL1                      = 21;
    public final static int LEVEL2                      = 22;

    public final static int WORK_BULK                   = CARD_BULKX;
    private int mCurrentPen                             = CARD_PEN1;

    public static final int MSG_SMARTCARD_INIT_SUCCESS  = 11001;
    public static final int MSG_SMARTCARD_INIT_FAILED   = 11002;
    public static final int MSG_SMARTCARD_CHECK_SUCCESS = 11003;
    public static final int MSG_SMARTCARD_CHECK_FAILED  = 11004;

    private boolean mInitialied;
    private Context mContext;
    private Handler mCallback;
    private boolean mValid;
    private int     mBagInkLevel;
// M.M.Wang 2020-11-16 增加墨盒墨量显示
    private int     mPenInkLevel1;
    private int     mPenInkLevel2;
// End of M.M.Wang 2020-11-16 增加墨盒墨量显示

    private boolean mBlockAdding = false;

    private static int READ_LEVEL_TIMES                 = 1;
    private int[] mReadLevels;
    private int[] mCurrentLevels;
    private int mCurLvlNums;

    private AlertDialog mRecvedLevelPromptDlg = null;

    private static final int MSG_SHOW_CONSISTENCY       = 101;
    private static final int MSG_SHOW_LEVEL             = 102;
    private static final int MSG_SHOW_LOCAL_INK         = 103;
    private static final int MSG_SHOW_INIT_TIMES        = 104;
    private int mWrittenTimes = 0;

//    private final static int UPDATE_LEVEL_INTERVAL      = 1000 * 30;
    private final static int READ_LEVEL_INTERVAL        = 10;
    private final static int MSG_READ_CONSISTENCY_INTERVAL   = 1000 * 60;

    private Timer mTimer = null;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SHOW_CONSISTENCY:
                    if (null != mRecvedLevelPromptDlg) {
                        mRecvedLevelPromptDlg.setTitle("Legality" + " - " + MAX_BAG_INK_VOLUME);
                        mRecvedLevelPromptDlg.setMessage((String) msg.obj);
                        mRecvedLevelPromptDlg.show();
                        mRecvedLevelPromptDlg.show();
                    }
                    break;
                case MSG_SHOW_LEVEL:
                    if (null != mRecvedLevelPromptDlg) {
                        StringBuilder sb = new StringBuilder();
                        for (int i = 0; i < mCurLvlNums; i++) {
                            if (i != 0)
                                sb.append(",");
                            sb.append(mCurrentLevels[i] / 100000);
                        }

                        if (mAddInkFailed) {
                            sb.append("\n");
                            sb.append("!!! Adding Ink Failed !!!");
                        } else if (mBlockAdding) {
                            sb.append("\n");
                            sb.append("--- Adding Ink (" + mInkAddedTimes + ") ---");
                        }

                        mRecvedLevelPromptDlg.setTitle("Levels");
                        mRecvedLevelPromptDlg.setMessage(sb.toString());
                        mRecvedLevelPromptDlg.show();
                        mRecvedLevelPromptDlg.show();
                    }
                    break;
                case MSG_SHOW_LOCAL_INK:
//                    if(null != mRecvedLevelPromptDlg) {
//                        mRecvedLevelPromptDlg.setTitle("Local Ink");
//                        mRecvedLevelPromptDlg.setMessage("" + mInkLevel);
//                        mRecvedLevelPromptDlg.show();
//                        mRecvedLevelPromptDlg.show();
//                    }
                    break;
                case MSG_SHOW_INIT_TIMES:
                    if(null != mRecvedLevelPromptDlg) {
                        if(null != msg.obj) {
                            mRecvedLevelPromptDlg.setTitle("Written Times");
                            mRecvedLevelPromptDlg.setMessage("Times: " + mWrittenTimes);
                        } else {
                            mRecvedLevelPromptDlg.setTitle("Writing Failed");
                            mRecvedLevelPromptDlg.setMessage("After " + mWrittenTimes + " times");
                        }
                        mRecvedLevelPromptDlg.show();
                        mRecvedLevelPromptDlg.show();
                    }
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
        mBagInkLevel = -1;
        mPenInkLevel1 = -1;
        mPenInkLevel2 = -1;
        mCallback = null;

        mReadLevels = new int[READ_LEVEL_TIMES];
        mCurrentLevels = new int[10];
        mCurLvlNums = 0;

        mCurrentPen = CARD_PEN1;

        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);

        mRecvedLevelPromptDlg = builder.setTitle("读取LEVEL值").setMessage("").setPositiveButton("关闭", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                mRecvedLevelPromptDlg.hide();
            }
        }).create();
    }

    private boolean isOpeningInit = false;
    private int reading = CARD_PEN1;

    @Override
    public void init(final Handler callback) {
        Debug.d(TAG, "---> enter init()");

        if(mInitialied) return;

        new Thread(new Runnable() {
            @Override
            public void run() {
                if(mCallback == null) {
                    isOpeningInit = true;
                } else {
                    isOpeningInit = false;
                }
                mCallback = callback;

                int ret = -1;
                if(SMARTCARD_ACCESS) {
                    synchronized (this) {
                        ret = SmartCard.init();
                        if(SmartCard.SC_SUCCESS == ret) ret = SmartCard.initComponent(CARD_PEN1);
                        if(SmartCard.SC_SUCCESS == ret) ret = SmartCard.initComponent(CARD_PEN2);
                        if(SmartCard.SC_SUCCESS == ret) ret = SmartCard.initComponent(WORK_BULK);
                        if(SmartCard.SC_SUCCESS == ret) ret = SmartCard.initComponent(LEVEL1);
                        if(SmartCard.SC_SUCCESS == ret) ret = SmartCard.initComponent(LEVEL2);
                    }
                } else {
                    ret = SmartCard.SC_SUCCESS;
                }

                if(SmartCard.SC_SUCCESS == ret) {
                    mInitialied = true;
                    checkConsistency();
                    checkOIB(mCurrentPen);
                    checkSum(mCurrentPen);
                    getLocalInk(0);
                    getLocalInk(1);

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
                                    readConsistency(reading);
                                    if(reading == WORK_BULK) reading = mCurrentPen;
                                }
                            }, (long)3000, (long)MSG_READ_CONSISTENCY_INTERVAL);
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
        }).start();
    }

    private void checkConsistency() {
        Debug.d(TAG, "---> enter checkConsistency()");

        if(!SMARTCARD_ACCESS || !CONSISTENCY_CHECK) return;

        synchronized (this) {
            if(mInitialied && mValid) {
                int ret = SmartCard.checkConsistency(mCurrentPen, WORK_BULK);
                if(SmartCard.SC_SUCCESS != ret) {
                    mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, ret, 0, null).sendToTarget();
                }
            }
        }
    }

    private void readConsistency(int card) {
        Debug.d(TAG, "---> enter readConsistency()");

        if(!SMARTCARD_ACCESS) {
            mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Skipping SC access!").sendToTarget();
            return;
        }

        synchronized (this) {
            if(mInitialied && mValid) {
                mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, SmartCard.readConsistency(card)).sendToTarget();
            } else {
                mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Not initialized or invalid!").sendToTarget();
            }
        }
    }

    private void checkOIB(int card) {
        Debug.d(TAG, "---> enter checkOIB(" + card + ")");

        if(!SMARTCARD_ACCESS || !OIB_CHECK) return;

        synchronized (this) {
            if(mInitialied && mValid) {
                int ret = SmartCard.checkOIB(card);
                if(0 != ret) {
                    mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_OUT_OF_INK_ERROR, 0, null).sendToTarget();
                }
            }
        }
    }

    private void checkSum(int cardType) {
        Debug.d(TAG, "---> enter checkSum(" + cardType + ")");

        if(Configs.SMARTCARDMANAGER || !SUM_CHECK) return;
        if(!SMARTCARD_ACCESS) return;

        synchronized (this) {
            if(mInitialied && mValid) {
                int ret = SmartCard.checkSum(cardType, Configs.CLIENT_UNIQUE_CODE);
                if(0 != ret) {
                    mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_CHECKSUM_FAILED, 0, null).sendToTarget();
                }
            }
        }
    }

    private void readLevelValue(int count) {
        if(!SMARTCARD_ACCESS) {
            mReadLevels[count] = 14000000;
        } else {
            synchronized (this) {
                if(mInitialied) {
                    int level = 0;
                    if(mCurrentPen == CARD_PEN1) {
                        level = SmartCard.readLevel(LEVEL1);
                    }
                    if(mCurrentPen == CARD_PEN2) {
                        level = SmartCard.readLevel(LEVEL2);
                    }
                    if ((level & 0xF0000000) == 0x00000000) {
                        Debug.d(TAG, "Read Level " + (count + 1) + " times = " + level);
                        mReadLevels[count] = level;
                    } else {
                        Debug.e(TAG, "Read Level Error: " + Integer.toHexString(level & 0xF0000000));
                        mReadLevels[count] = level;
                    }
                }
            }
        }
    }

    private void addInkOn() {
        Debug.d(TAG, "Add ink opened!");
        if(mCurrentPen == CARD_PEN1) {
            ExtGpio.rfidSwitch(ExtGpio.RFID_CARD6);
        }
        if(mCurrentPen == CARD_PEN2) {
            ExtGpio.rfidSwitch(ExtGpio.RFID_CARD7);
        }
        mBlockAdding = true;
    }

    private void addInOff() {
        Debug.d(TAG, "Add ink closed!");
        ExtGpio.rfidSwitch(ExtGpio.RFID_CARD1);
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
                if(mInkAddedTimes >= 10) {
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
// H.M.Wang 2020-11-13 当墨量<5%时，如果3次加墨失败则写OIB，本人认为这个操作不太好
// H.M.Wang 2020-11-27 修改<5%的数值BUG，getLocalInkPercentage函数返回的是0-100的值，不是0-1的值
                    if(getLocalInkPercentage(0) < 5.0f) {
                        synchronized (this) {
                            SmartCard.writeOIB(WORK_BULK);
                        }
                    }
                    if(getLocalInkPercentage(1) < 5.0f) {
                        synchronized (this) {
                            SmartCard.writeOIB(WORK_BULK);
                        }
                    }
// End of H.M.Wang 2020-11-13 当墨量<5%时，如果3次加墨失败则写OIB，本人认为这个操作不太好
// H.M.Wang 2020-11-24 追加加墨10次失败后停止打印
                    if(null != mCallback) mCallback.sendEmptyMessage(ControlTabActivity.MESSAGE_PRINT_STOP);
// End of H.M.Wang 2020-11-24 追加加墨10次失败后停止打印
                } else {
                    addInkOn();

                    try{Thread.sleep(200);addInOff();}catch(Exception e){
                        addInOff();
                    };

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
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(int i=0; i<READ_LEVEL_TIMES; i++) {
                    readLevelValue(i);
                    try{Thread.sleep(READ_LEVEL_INTERVAL);}catch(Exception e){};
                }
                levelValueUpdated();
            }
        }).start();
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
            if(SMARTCARD_ACCESS) {
                synchronized (this) {
                    SmartCard.shutdown();
                }
            }
        }
    }

    @Override
    public float getLocalInk(int head) {
        Debug.d(TAG, "---> enter getLocalInk(" + head + ")");

        if(mBagInkLevel == -1) {
            if(mInitialied && mValid) {
                if(!SMARTCARD_ACCESS) {
                    mBagInkLevel = MAX_BAG_INK_VOLUME / 2;           // 如果跳过Smartcard访问标识开启，则返回一个恰当值
                } else {
                    synchronized (this) {
                        mBagInkLevel = SmartCard.getLocalInk(WORK_BULK);
                        mBagInkLevel = (mBagInkLevel >= 0 ? MAX_BAG_INK_VOLUME - mBagInkLevel : mBagInkLevel);
                    }
                }
            }
        }

        Debug.d(TAG, "---> Ink Level(WORK_BULK) = " + mBagInkLevel);
        float localInk = mBagInkLevel;

        if(head == 0) {
            if(mPenInkLevel1 == -1) {
                if(mInitialied && mValid) {
                    if(!SMARTCARD_ACCESS) {
                        mPenInkLevel1 = MAX_PEN_INK_VOLUME / 2;
                    } else {
                        synchronized (this) {
                            mPenInkLevel1 = SmartCard.getLocalInk(CARD_PEN1);
                            mPenInkLevel1 = (mPenInkLevel1 >= 0 ? MAX_PEN_INK_VOLUME - mPenInkLevel1 : mPenInkLevel1);
                        }
                    }
                }
            }
            localInk = mPenInkLevel1;
            Debug.d(TAG, "---> Ink Level(CARD_PEN1) = " + localInk);
        }

        if(head == 1) {
            if(mPenInkLevel2 == -1) {
                if(mInitialied && mValid) {
                    if(!SMARTCARD_ACCESS) {
                        mPenInkLevel2 = MAX_PEN_INK_VOLUME / 2;
                    } else {
                        synchronized (this) {
                            mPenInkLevel2 = SmartCard.getLocalInk(CARD_PEN2);
                            mPenInkLevel2 = (mPenInkLevel2 >= 0 ? MAX_PEN_INK_VOLUME - mPenInkLevel2 : mPenInkLevel2);
                        }
                    }
                }
            }
            localInk = mPenInkLevel2;
            Debug.d(TAG, "---> Ink Level(CARD_PEN2) = " + localInk);
        }

        mHandler.sendEmptyMessage(MSG_SHOW_LOCAL_INK);
        return localInk;
    }

    @Override
    public float getLocalInkPercentage(int head) {
        Debug.d(TAG, "---> enter getLocalInkPercentage(" + head + ")");
// M.M.Wang 2020-11-16 增加墨盒墨量显示
        if(head == 0) {
            return (100.0f * mPenInkLevel1 / MAX_PEN_INK_VOLUME);
        } else if(head == 1) {
            return (100.0f * mPenInkLevel2 / MAX_PEN_INK_VOLUME);
        } else if(head == 2) {
            return (100.0f * mBagInkLevel / MAX_BAG_INK_VOLUME);
        }
// M.M.Wang 2020-11-16 增加墨盒墨量显示
        return 0f;
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
    public void downLocal(int head) {
        Debug.d(TAG, "---> enter downLocal()");

        mBagInkLevel--;

        if(!SMARTCARD_ACCESS) return;

        if(mInitialied && mValid) {
            synchronized (this) {
                if(head == 0) {
                    SmartCard.downLocal(CARD_PEN1);
                    mPenInkLevel1 = MAX_PEN_INK_VOLUME - SmartCard.getLocalInk(CARD_PEN1);
                    checkOIB(CARD_PEN1);
                }
                if(head == 1) {
                    SmartCard.downLocal(CARD_PEN2);
                    mPenInkLevel2 = MAX_PEN_INK_VOLUME - SmartCard.getLocalInk(CARD_PEN2);
                    checkOIB(CARD_PEN2);
                } else {
                    return;
                }
                SmartCard.downLocal(WORK_BULK);
                mBagInkLevel = MAX_BAG_INK_VOLUME - SmartCard.getLocalInk(WORK_BULK);
                checkOIB(WORK_BULK);
            }
        }
    }

    @Override
    public void switchRfid(int head) {
        Debug.d(TAG, "---> enter switchRfid -> " + head);
        if(0 == head) {
            mCurrentPen = CARD_PEN1;
        }
        if(1 == head) {
            mCurrentPen = CARD_PEN2;
        }
    }

    @Override
    public void defaultInkForIgnoreRfid() {
        Debug.d(TAG, "---> enter defaultInkForIgnoreRfid()");
        // No need to implement. In RFID, this function set ink level of every devices to 185.
    }
}
