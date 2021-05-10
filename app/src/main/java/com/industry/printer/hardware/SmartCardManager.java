package com.industry.printer.hardware;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;

import com.industry.printer.BuildConfig;
import com.industry.printer.ControlTabActivity;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class SmartCardManager implements IInkDevice {
    private static final String TAG = SmartCardManager.class.getSimpleName();

    public static boolean SMARTCARD_ACCESS = true;
    private static boolean CONSISTENCY_CHECK = false;
    private static boolean OIB_CHECK = false;
    private static boolean SUM_CHECK = false;

    private final static int PEN_VS_BAG_RATIO           = 3;
    private final static int MAX_BAG_INK_VOLUME         = 3150;
    private final static int MAX_PEN_INK_VOLUME         = MAX_BAG_INK_VOLUME * PEN_VS_BAG_RATIO;

    private final static int CARD_TYPE_PEN1             = 11;
    private final static int CARD_TYPE_PEN2             = 12;
    private final static int CARD_TYPE_BULK1            = 13;
    private final static int CARD_TYPE_BULKX            = 14;
    private final static int CARD_TYPE_LEVEL1           = 21;
    private final static int CARD_TYPE_LEVEL2           = 22;

    private class _device_status {
        private boolean mInitialized;
        private boolean mValid;
        private int     mCardType;
        private int     mLevelType;
        private int     mInkLevel;
        private int     mMaxVolume;
        private ArrayList<Integer> mRecentLevels;
        private volatile boolean mInkAdding;
        private int mInkAddedTimes;
        private boolean mAddInkFailed;


        public _device_status(int cardType, int levelType, int maxVolume) {
            mInitialized = false;
            mValid = true;

            mCardType = cardType;
            mLevelType = levelType;

            mInkLevel = -1;
            mMaxVolume = maxVolume;

            mRecentLevels = new ArrayList<Integer>();

            mInkAdding = false;
            mInkAddedTimes = 0;
            mAddInkFailed = false;
        }
    };

    private int mBagNum                                 = 0;
    private _device_status[] mCards                      = null;
    private int mCurBagIdx                              = 0;
    private int mPenNum                                 = 0;
    private int mCurPenIdx                              = 0;

    public static final int MSG_SMARTCARD_INIT_SUCCESS  = 11001;
    public static final int MSG_SMARTCARD_INIT_FAILED   = 11002;
    public static final int MSG_SMARTCARD_CHECK_SUCCESS = 11003;
    public static final int MSG_SMARTCARD_CHECK_FAILED  = 11004;

    private Context mContext;
    private Handler mCallback;
    private boolean mLibInited;

    private static int READ_LEVEL_TIMES                 = 1;        // 每次读取LEVEL值时尝试的最大次数，然后从成功的次数当中获取平均值，作为本次的读取值。如设置10次，则从下层读取10次，如成功5次，则使用成功的5次获取平均值作为本次读取的最终值
    private static int PROC_LEVEL_NUMS                  = 10;       // 对读取数据进行处理的最小次数，当达到这个数字的时候，处理是否加墨的处理
    private static int ADD_INK_TRY_LIMITS               = 10;       // 寂寞的尝试次数

    private AlertDialog mRecvedLevelPromptDlg = null;

    private static final int MSG_SHOW_CONSISTENCY       = 101;
    private static final int MSG_SHOW_LEVEL             = 102;

    private final static int READ_LEVEL_INTERVAL        = 10;
    private final static int MSG_READ_CONSISTENCY_INTERVAL   = 1000 * 60;

    private Timer mTimer = null;

    ExecutorService mCachedThreadPool = null;

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
                        for(int i=0; i<mPenNum; i++) {
                            sb.append("Level - " + (i + 1) + "\n");
                            for(int j=0; j<mCards[i].mRecentLevels.size(); j++) {
                                if (j != 0) sb.append(",");
                                sb.append(mCards[i].mRecentLevels.get(j) / 100000);
                            }
                            sb.append("\n");
                            if (mCards[i].mAddInkFailed) {
                                sb.append("!!! Adding Ink Failed !!!\n");
                            } else if(mCards[i].mInkAddedTimes > 0) {
                                sb.append("--- Adding Ink (" + mCards[i].mInkAddedTimes + ") ---\n");
                            }
                        }

                        mRecvedLevelPromptDlg.setTitle("Levels");
                        mRecvedLevelPromptDlg.setMessage(sb.toString());
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

        mCachedThreadPool = Executors.newCachedThreadPool();

        mBagNum = 1;        // 暂时墨袋只有一个
        if(SystemConfigFile.getInstance().getPNozzle() == PrinterNozzle.MESSAGE_TYPE_25_4) {
            mPenNum = 2;
            mCards = new _device_status[] {
                    new _device_status(CARD_TYPE_PEN1, CARD_TYPE_LEVEL1, MAX_PEN_INK_VOLUME),
                    new _device_status(CARD_TYPE_PEN2, CARD_TYPE_LEVEL2, MAX_PEN_INK_VOLUME),
                    new _device_status(CARD_TYPE_BULK1, 0, MAX_BAG_INK_VOLUME)
            };
        } else {
            mPenNum = 1;
            mCards = new _device_status[] {
                    new _device_status(CARD_TYPE_PEN1, CARD_TYPE_LEVEL1, MAX_PEN_INK_VOLUME),
                    new _device_status(CARD_TYPE_BULK1, 0, MAX_BAG_INK_VOLUME)
            };
        }
        mCurPenIdx = 0;
        mCurBagIdx = mPenNum;

        mCallback = null;

        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);

        mRecvedLevelPromptDlg = builder.setTitle("读取LEVEL值").setMessage("").setPositiveButton("关闭", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                mRecvedLevelPromptDlg.hide();
            }
        }).create();
    }

    private boolean isOpeningInit = false;
    private int reading = 0;

    @Override
    public void init(final Handler callback) {
        Debug.d(TAG, "---> enter init()");

        if(mLibInited) return;
        mLibInited = true;

        mCachedThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                if(mCallback == null) {
                    isOpeningInit = true;
                } else {
                    isOpeningInit = false;
                }
                mCallback = callback;

                if(SMARTCARD_ACCESS) {
                    synchronized (this) {
                        SmartCard.init();
                        for(int i=0; i<mPenNum+mBagNum; i++) {
                            mCards[i].mRecentLevels.clear();
                            if(i<mPenNum) {
                                SmartCard.initComponent(mCards[i].mLevelType);
                            }
                            if(SmartCard.SC_SUCCESS == SmartCard.initComponent(mCards[i].mCardType)) {
                                mCards[i].mInitialized = true;
                                mCards[i].mValid = true;
                                checkOIB(i);
                                getLocalInk(i);
                            } else {
                                mCards[i].mInitialized = false;
                                mCards[i].mValid = false;
                            }
                        }
                    }
                }

                if(isOpeningInit) {
                    shutdown();
                    mCallback.sendEmptyMessage(MSG_SMARTCARD_INIT_SUCCESS);
                } else {
                    for(int i=0; i<mPenNum; i++) {
                        if(!mCards[i].mValid) {
                            mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_FAILED, 0, null).sendToTarget();
                            return;
                        }
                    }
                    mCallback.sendEmptyMessage(MSG_SMARTCARD_CHECK_SUCCESS);
                    if(null == mTimer) {
                        mTimer = new Timer();
                        mTimer.scheduleAtFixedRate(new TimerTask() {
                            @Override
                            public void run() {
                                checkConsistency(mCurPenIdx, mCurBagIdx);
                                if(reading < mPenNum + mBagNum )readConsistency(reading++); // 进最初显示一轮而已
                            }
                        }, (long)3000, (long)MSG_READ_CONSISTENCY_INTERVAL);
                    }
                }
            }
        });
    }

    private void checkConsistency(int penIdx, int bagIdx) {
        Debug.d(TAG, "---> enter checkConsistency(" + penIdx + ", " + bagIdx + ")");

        if(!SMARTCARD_ACCESS || !CONSISTENCY_CHECK) return;

        synchronized (this) {
            if(mCards[penIdx].mInitialized && mCards[penIdx].mValid) {
                int ret = SmartCard.checkConsistency(mCards[penIdx].mCardType, mCards[bagIdx].mCardType);
                if(SmartCard.SC_SUCCESS != ret) {
                    mCards[penIdx].mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, ret, 0, null).sendToTarget();
                }
            }
        }
    }

    private void readConsistency(int cardIdx) {
        Debug.d(TAG, "---> enter readConsistency(" + cardIdx + ")");

        if(!SMARTCARD_ACCESS) {
            mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Skipping SC access!").sendToTarget();
            return;
        }

        synchronized (this) {
            if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, SmartCard.readConsistency(mCards[cardIdx].mCardType)).sendToTarget();
            } else {
                mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Not initialized or invalid!").sendToTarget();
            }
        }
    }

    private boolean checkOIB(int cardIdx) {
        Debug.d(TAG, "---> enter checkOIB(" + cardIdx + ")");

        if(!SMARTCARD_ACCESS || !OIB_CHECK) return true;

        synchronized (this) {
            if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                int ret = SmartCard.checkOIB(mCards[cardIdx].mCardType);
                if(SmartCard.SC_SUCCESS != ret) {
                    mCards[cardIdx].mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_OUT_OF_INK_ERROR, 0, null).sendToTarget();
                    return false;
                }
            }
        }
        return true;
    }

    private void checkSum(int cardIdx) {
        Debug.d(TAG, "---> enter checkSum(" + cardIdx + ")");

        if(Configs.SMARTCARDMANAGER || !SUM_CHECK) return;
        if(!SMARTCARD_ACCESS) return;

        synchronized (this) {
            if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                int ret = SmartCard.checkSum(mCards[cardIdx].mCardType, Configs.CLIENT_UNIQUE_CODE);
                if(0 != ret) {
                    mCards[cardIdx].mValid = false;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_CHECKSUM_FAILED, 0, null).sendToTarget();
                }
            }
        }
    }

    private void readLevelValue(int cardIdx) {
        Debug.d(TAG, "---> enter readLevelValue(" + cardIdx + ")");

        long readLevels = 0L;
        int readCount = 0;

        for(int i=0; i<READ_LEVEL_TIMES; i++) {
            if(!SMARTCARD_ACCESS) {
                readLevels += 14000000;
                readCount++;
            } else {
                synchronized (this) {
                    if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                        int level = SmartCard.readLevel(mCards[cardIdx].mLevelType);
                        if ((level & 0xF0000000) == 0x00000000) {
                            Debug.d(TAG, "Read Level " + (readCount + 1) + " times = " + level);
                            readLevels += level;
                            readCount++;
                        } else {
                            Debug.e(TAG, "Read Level Error: " + Integer.toHexString(level & 0xF0000000));
                        }
                    }
                }
            }
            try{Thread.sleep(READ_LEVEL_INTERVAL);}catch(Exception e){};
        }

        int avgLevel = (readCount == 0 ? 0 : (int)(readLevels / readCount));
        Debug.d(TAG, "Read Level = " + avgLevel);

        if(avgLevel >= 12000000 && avgLevel <= 16000000) {
            mCards[cardIdx].mRecentLevels.add(avgLevel);
            if(mCards[cardIdx].mRecentLevels.size() > PROC_LEVEL_NUMS) {
                mCards[cardIdx].mRecentLevels.remove(0);
            }
        }
    }

    private void addInkOn(int cardIdx) {
        Debug.d(TAG, "---> enter addInkOn(" + cardIdx + ")");

        if(mCards[cardIdx].mCardType == CARD_TYPE_PEN1) {
            ExtGpio.rfidSwitch(ExtGpio.RFID_CARD6);
        }
        if(mCards[cardIdx].mCardType == CARD_TYPE_PEN2) {
            ExtGpio.rfidSwitch(ExtGpio.RFID_CARD7);
        }
    }

    private void addInkOff(int cardIdx) {
        Debug.d(TAG, "---> enter addInkOff(" + cardIdx + ")");

        ExtGpio.rfidSwitch(ExtGpio.RFID_CARD1);
    }

    private void levelValueUpdated(final int cardIdx) {
        Debug.d(TAG, "---> enter levelValueUpdated(" + cardIdx + ")");

        int avgLevel = 13500000;
        if(mCards[cardIdx].mRecentLevels.size() >= PROC_LEVEL_NUMS) {
            long totalLevel = 0;
            for(int i=0; i<PROC_LEVEL_NUMS; i++) {
                totalLevel += mCards[cardIdx].mRecentLevels.get(i);
            }
            avgLevel = (int)(totalLevel / PROC_LEVEL_NUMS);
        }
        Debug.d(TAG, "Average Level = " + avgLevel);

        if(avgLevel < 13500000) {
            if(!mCards[cardIdx].mInkAdding) {
                if(mCards[cardIdx].mInkAddedTimes >= ADD_INK_TRY_LIMITS) {
                    mCards[cardIdx].mAddInkFailed = true;
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try{
                                ExtGpio.playClick();
                                Thread.sleep(50);
                                ExtGpio.playClick();
                                Thread.sleep(50);
                                ExtGpio.playClick();
                            } catch (Exception e) {
                                Debug.e(TAG, e.getMessage());
                            }
                        }
                    }).start();
// H.M.Wang 2020-11-13 当墨量<5%时，如果3次加墨失败则写OIB，本人认为这个操作不太好
// H.M.Wang 2020-11-27 修改<5%的数值BUG，getLocalInkPercentage函数返回的是0-100的值，不是0-1的值
                    if(mCards[mCurBagIdx].mInkLevel / mCards[mCurBagIdx].mMaxVolume < 0.05f) {
                        synchronized (this) {
                            SmartCard.writeOIB(mCards[mCurBagIdx].mCardType);
                        }
                    }
// End of H.M.Wang 2020-11-13 当墨量<5%时，如果3次加墨失败则写OIB，本人认为这个操作不太好
// H.M.Wang 2020-11-24 追加加墨10次失败后停止打印
                    if(null != mCallback) mCallback.sendEmptyMessage(ControlTabActivity.MESSAGE_PRINT_STOP);
// End of H.M.Wang 2020-11-24 追加加墨10次失败后停止打印
                } else {
                    if(!mCards[cardIdx].mInkAdding) {
                        mCachedThreadPool.execute(new Runnable() {
                            @Override
                            public void run() {
                                mCards[cardIdx].mInkAdding = true;
                                addInkOn(cardIdx);

                                try{Thread.sleep(100);addInkOff(cardIdx);}catch(Exception e){
                                    addInkOff(cardIdx);
                                };

                                long startTiem = System.currentTimeMillis();
                                while(true) {
                                    try{Thread.sleep(100);}catch(Exception e){};
                                    if(System.currentTimeMillis() - startTiem >= 11500) break;
                                }

                                Debug.d(TAG, "Clear Adding Block!");
                                mCards[cardIdx].mInkAdding = false;

                                mCards[cardIdx].mInkAddedTimes++;
                            }
                        });
                    }
                }
            }
        } else {
            mCards[cardIdx].mAddInkFailed = false;
            mCards[cardIdx].mInkAddedTimes = 0;
        }
    }


    public void updateLevel() {
        Debug.d(TAG, "---> enter updateLevel()");
        mCachedThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                for(int i=0; i<mPenNum; i++) {
                    readLevelValue(i);
                    levelValueUpdated(i);
                }
                mHandler.sendEmptyMessage(MSG_SHOW_LEVEL);
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
        if(null != mTimer) {
            mTimer.cancel();
            mTimer = null;
        }

        mLibInited = false;

        if(SMARTCARD_ACCESS) {
            synchronized (this) {
                SmartCard.shutdown();
            }
        }

        for(int i=0; i<mPenNum+mBagNum; i++) {
            mCards[i].mInitialized = false;
        }
    }

    @Override
    public float getLocalInk(int cardIdx) {
        Debug.d(TAG, "---> enter getLocalInk(" + cardIdx + ")");

        if(cardIdx < mPenNum + mBagNum) {
            if(mCards[cardIdx].mInkLevel == -1) {
                if(!SMARTCARD_ACCESS) {
                    mCards[cardIdx].mInkLevel = mCards[cardIdx].mMaxVolume / 2;
                } else {
                    synchronized (this) {
                        if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                            mCards[cardIdx].mInkLevel = SmartCard.getLocalInk(mCards[cardIdx].mCardType);
                            mCards[cardIdx].mInkLevel = (mCards[cardIdx].mInkLevel >= 0 ? mCards[cardIdx].mMaxVolume - mCards[cardIdx].mInkLevel : mCards[cardIdx].mInkLevel);
                        }
                    }
                }
            }
            Debug.d(TAG, "---> Ink Level(" + mCards[cardIdx].mCardType + ") = " + mCards[cardIdx].mInkLevel);
            return mCards[cardIdx].mInkLevel;
        }

        return -1;
    }

    @Override
    public float getLocalInkPercentage(int cardIdx) {
        Debug.d(TAG, "---> enter getLocalInkPercentage(" + cardIdx + ")");
        float percentage = 0f;

        if(cardIdx < mPenNum + mBagNum) {
            if(mCards[cardIdx].mInkLevel != -1) {
                percentage = (100.0f * mCards[cardIdx].mInkLevel / mCards[cardIdx].mMaxVolume);
            } else {
                percentage = 100f;     // 在还没有初始化的情况下，返回100以防止报警
            }
        }

        return percentage;
    }

    @Override
    public boolean isValid(int cardIdx) {
        Debug.d(TAG, "---> enter isValid(" + cardIdx + ")");
        boolean ret = false;

        if(cardIdx < mPenNum + mBagNum) {
            ret = mCards[cardIdx].mValid;
        }

        return ret;
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
    public void downLocal(int cardIdx) {
        Debug.d(TAG, "---> enter downLocal(" + cardIdx + ")");

        if(cardIdx >= mPenNum) return;

        mCards[cardIdx].mInkLevel--;

        if(!SMARTCARD_ACCESS) return;

        synchronized (this) {
            if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                SmartCard.downLocal(mCards[cardIdx].mCardType);
                mCards[cardIdx].mInkLevel = mCards[cardIdx].mMaxVolume - SmartCard.getLocalInk(mCards[cardIdx].mCardType);
                checkOIB(cardIdx);

                SmartCard.downLocal(mCards[mCurBagIdx].mCardType);
                mCards[mCurBagIdx].mInkLevel = mCards[mCurBagIdx].mMaxVolume - SmartCard.getLocalInk(mCards[mCurBagIdx].mCardType);
                checkOIB(mCurBagIdx);
            }
        }
    }

    @Override
    public void switchRfid(int head) {
        Debug.d(TAG, "---> enter switchRfid -> " + head);

        if(head < mPenNum) {
            mCurPenIdx = head;
        }
    }

    @Override
    public void defaultInkForIgnoreRfid() {
        Debug.d(TAG, "---> enter defaultInkForIgnoreRfid()");
        // No need to implement. In RFID, this function set ink level of every devices to 185.
    }
}
