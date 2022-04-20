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
import com.industry.printer.Utils.ToastUtil;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class SmartCardManager implements IInkDevice {
    private static final String TAG = SmartCardManager.class.getSimpleName();

    public static boolean SMARTCARD_ACCESS = true;
    private static boolean CONSISTENCY_CHECK = false;
    private static boolean OIB_CHECK = true;
    private static boolean SUM_CHECK = false;

    private final static int PEN_VS_BAG_RATIO           = 3;
    private static int MAX_BAG_INK_VOLUME         = 3150;
    private static int MAX_PEN_INK_VOLUME         = MAX_BAG_INK_VOLUME * PEN_VS_BAG_RATIO;

    private final static int CARD_TYPE_PEN1             = 11;
    private final static int CARD_TYPE_PEN2             = 12;
    private final static int CARD_TYPE_BULK1            = 13;           // 真实墨袋
    private final static int CARD_TYPE_BULKX            = 14;           // 墨盒代替墨袋
    private final static int CARD_TYPE_LEVEL1           = 21;
    private final static int CARD_TYPE_LEVEL2           = 22;

    private class _device_status {
        private boolean mInitialized;
        private boolean mValid;
// H.M.Wang 2022-4-18 增加OIB属性，记录OIB状态
        private boolean mOIB;
// End of H.M.Wang 2022-4-18 增加OIB属性，记录OIB状态
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
            mOIB = false;

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
    private static final int MSG_SHOW_OIB_ERROR         = 103;

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
                case MSG_SHOW_OIB_ERROR:
                    ToastUtil.show(mContext, "Out of Ink!");
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
//                    new _device_status(CARD_TYPE_BULKX, 0, MAX_BAG_INK_VOLUME)
            };
        } else {
            mPenNum = 1;
            mCards = new _device_status[] {
                    new _device_status(CARD_TYPE_PEN1, CARD_TYPE_LEVEL1, MAX_PEN_INK_VOLUME),
                    new _device_status(CARD_TYPE_BULK1, 0, MAX_BAG_INK_VOLUME)
//                    new _device_status(CARD_TYPE_BULKX, 0, MAX_BAG_INK_VOLUME)
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
// H.M.Wang 2022-4-18 再次开放，失败尝试次数改为5次
// H.M.Wang 2022-4-14 另外一种对应国外用户的PEN经常会初始化失败，导致不能打印的问题，如果失败尝试3次初始化（暂时去功能）
    private int try_num = 0;
// End of H.M.Wang 2022-4-14 另外一种对应国外用户的PEN经常会初始化失败，导致不能打印的问题，如果失败尝试3次初始化
// End of H.M.Wang 2022-4-18 再次开放，失败尝试次数改为5次

// H.M.Wang 2022-1-24 将单卡的初始化流程提取出来成为一个独立的函数
    private void initComponent(int index) {
        if(index < 0 || index >= mPenNum+mBagNum) return;

        mCards[index].mRecentLevels.clear();
        if(index < mPenNum) {
            SmartCard.initComponent(mCards[index].mLevelType);
        }
        if(SmartCard.SC_SUCCESS == SmartCard.initComponent(mCards[index].mCardType)) {
            try_num = 0;
            mCards[index].mInitialized = true;
            mCards[index].mValid = true;
            MAX_BAG_INK_VOLUME = SmartCard.getMaxVolume(mCards[index].mCardType);
            if(mCards[index].mCardType == CARD_TYPE_PEN1 || mCards[index].mCardType == CARD_TYPE_PEN2 || mCards[index].mCardType == CARD_TYPE_BULKX) {
                mCards[index].mMaxVolume = MAX_BAG_INK_VOLUME * PEN_VS_BAG_RATIO;
            } else {
                mCards[index].mMaxVolume = MAX_BAG_INK_VOLUME;
            }
            checkOIB(index);
// H.M.Wang 2022-4-18 因为在初始化成功之前，getLocalInk函数可能就被主线程调用过，因为这时mInitialized和mValid都是false，因此mInkLevel会因为4-14日的修改，为了支持初始化失败也可以打印，所以mInkLevel被设上了一个虚假值（最大值的1/2)，这样初始化成功以后也不会再次去读卡中的值，因此这里设一个-1，强制其更新为卡中的值
            mCards[index].mInkLevel = -1;
// H.M.Wang 2022-4-18 因为在初始化成功之前，...
            getLocalInk(index);
        } else {
            try_num++;
            mCards[index].mInitialized = false;
            mCards[index].mValid = false;
            if(try_num < 5) initComponent(index);
            try_num = 0;
        }
    }
// End of H.M.Wang 2022-1-24 将单卡的初始化流程提取出来成为一个独立的函数

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
                    synchronized (SmartCardManager.this) {
                        SmartCard.init();
                        for(int i=0; i<mPenNum+mBagNum; i++) {
// H.M.Wang 2022-1-24 将单卡的初始化流程提取出来成为一个独立的函数
                            initComponent(i);
/*                            mCards[i].mRecentLevels.clear();
                            if(i<mPenNum) {
                                SmartCard.initComponent(mCards[i].mLevelType);
                            }
                            if(SmartCard.SC_SUCCESS == SmartCard.initComponent(mCards[i].mCardType)) {
                                mCards[i].mInitialized = true;
                                mCards[i].mValid = true;
                                MAX_BAG_INK_VOLUME = SmartCard.getMaxVolume(mCards[i].mCardType);
                                if(mCards[i].mCardType == CARD_TYPE_PEN1 || mCards[i].mCardType == CARD_TYPE_PEN2 || mCards[i].mCardType == CARD_TYPE_BULKX) {
                                    mCards[i].mMaxVolume = MAX_BAG_INK_VOLUME * PEN_VS_BAG_RATIO;
                                } else {
                                    mCards[i].mMaxVolume = MAX_BAG_INK_VOLUME;
                                }
                                checkOIB(i);
                                getLocalInk(i);
                            } else {
                                mCards[i].mInitialized = false;
                                mCards[i].mValid = false;
                            }*/
// End of H.M.Wang 2022-1-24 将单卡的初始化流程提取出来成为一个独立的函数
                        }
                    }
                }

                if(isOpeningInit) {
                    shutdown();
                    mCallback.sendEmptyMessage(MSG_SMARTCARD_INIT_SUCCESS);
                } else {
                    for(int i=0; i<mPenNum; i++) {
// H.M.Wang 2022-4-14 临时取消开始打印后检查valid状态，因为国外用户的PEN经常会初始化失败，导致不能打印（代码已经恢复原样）
//                        if(!mCards[i].mValid) {
//                            mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_FAILED, 0, null).sendToTarget();
//                            return;
//                        }
// End of H.M.Wang 2022-4-14 临时取消开始打印后检查valid状态，因为国外用户的PEN经常会初始化失败，导致不能打印
                    }
                    mCallback.sendEmptyMessage(MSG_SMARTCARD_CHECK_SUCCESS);
                    if(null == mTimer) {
                        mTimer = new Timer();
                        mTimer.scheduleAtFixedRate(new TimerTask() {
                            @Override
                            public void run() {
                                checkConsistency(mCurPenIdx, mCurBagIdx);
                                if(reading < mPenNum + mBagNum )readConsistency(reading++); // 仅最初显示一轮而已
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

        synchronized (SmartCardManager.this) {
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

        synchronized (SmartCardManager.this) {
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

        synchronized (SmartCardManager.this) {
            if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                int ret = SmartCard.checkOIB(mCards[cardIdx].mCardType);
                if(SmartCard.SC_SUCCESS != ret) {
                    mCards[cardIdx].mValid = false;
                    mCards[cardIdx].mOIB = true;
                    mCallback.obtainMessage(MSG_SMARTCARD_CHECK_FAILED, SmartCard.SC_OUT_OF_INK_ERROR, 0, null).sendToTarget();
                    mHandler.sendEmptyMessage(MSG_SHOW_OIB_ERROR);
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

        synchronized (SmartCardManager.this) {
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
                synchronized (SmartCardManager.this) {
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

//    private final int ADD_INK_THRESHOLD = 14000000;
//    private final int ADD_INK_THRESHOLD = 13900000;
    private final int ADD_INK_THRESHOLD = 13800000;
//    private final int ADD_INK_THRESHOLD = 13700000;
//    private final int ADD_INK_THRESHOLD = 13600000;
//    private final int ADD_INK_THRESHOLD = 13500000;

    private void levelValueUpdated(final int cardIdx) {
        Debug.d(TAG, "---> enter levelValueUpdated(" + cardIdx + ")");

        int avgLevel = ADD_INK_THRESHOLD;
        if(mCards[cardIdx].mRecentLevels.size() >= PROC_LEVEL_NUMS) {
            long totalLevel = 0;
            for(int i=0; i<PROC_LEVEL_NUMS; i++) {
                totalLevel += mCards[cardIdx].mRecentLevels.get(i);
            }
            avgLevel = (int)(totalLevel / PROC_LEVEL_NUMS);
        }
        Debug.d(TAG, "Average Level = " + avgLevel);

        if(avgLevel < ADD_INK_THRESHOLD) {
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
                        synchronized (SmartCardManager.this) {
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
            synchronized (SmartCardManager.this) {
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
                    synchronized (SmartCardManager.this) {
                        if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
                            mCards[cardIdx].mInkLevel = SmartCard.getLocalInk(mCards[cardIdx].mCardType);
                            mCards[cardIdx].mInkLevel = (mCards[cardIdx].mInkLevel >= 0 ? mCards[cardIdx].mMaxVolume - mCards[cardIdx].mInkLevel : mCards[cardIdx].mInkLevel);
// H.M.Wang 2022-4-14 临时取消开始打印后检查valid状态，因为国外用户的PEN经常会初始化失败，导致不能打印（代码已经恢复原样）
                        } else {
                            mCards[cardIdx].mInkLevel = mCards[cardIdx].mMaxVolume / 2;
// End of H.M.Wang 2022-4-14 临时取消开始打印后检查valid状态，因为国外用户的PEN经常会初始化失败，导致不能打印
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
    public boolean isValid(final int cardIdx) {
        Debug.d(TAG, "---> enter isValid(" + cardIdx + ")");
        boolean ret = false;

        if(cardIdx < mPenNum + mBagNum) {
            if(mCards[cardIdx].mOIB) return false;
// H.M.Wang 2022-1-24 追加在卡为无效或者未初始化的时候，尝试初始化
            if(!mCards[cardIdx].mValid || !mCards[cardIdx].mInitialized) {
                mCachedThreadPool.execute(new Runnable() {
                    @Override
                    public void run() {
                        initComponent(cardIdx);
                    }
                });
            }
// End of H.M.Wang 2022-1-24 追加在卡为无效或者未初始化的时候，尝试初始化
            ret = mCards[cardIdx].mValid;
        }

// H.M.Wang 2022-4-14 临时取消开始打印后检查valid状态，因为国外用户的PEN经常会初始化失败，导致不能打印
//        return ret;
        return (cardIdx < mPenNum ? true : ret);
// End of H.M.Wang 2022-4-14 临时取消开始打印后检查valid状态，因为国外用户的PEN经常会初始化失败，导致不能打印
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
    public void downLocal(final int cardIdx) {
        Debug.d(TAG, "---> enter downLocal(" + cardIdx + ")");

        if(cardIdx >= mPenNum) return;

        mCards[cardIdx].mInkLevel--;

// H.M.Wang 2021-10-26 取消重新读取卡中的值来修正本地值的操作后，bag的值没有修正
        mCards[mCurBagIdx].mInkLevel--;
// H.M.Wang 2021-10-26 取消重新读取卡中的值来修正本地值的操作后，bag的值没有修正

        if(!SMARTCARD_ACCESS) return;

        mCachedThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                synchronized (SmartCardManager.this) {
                    if(mCards[cardIdx].mInitialized && mCards[cardIdx].mValid) {
// H.M.Wang 2021-8-7 追加当写SC卡出现错误的时候报错的处理
                        int ret = SmartCard.downLocal(mCards[cardIdx].mCardType);
                        if(SmartCard.SC_SUCCESS != ret) {
                            mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Smartcard access error!").sendToTarget();
                        }
// End of H.M.Wang 2021-8-7 追加当写SC卡出现错误的时候报错的处理
// H.M.Wang 2021-8-23 取消重新读取卡中的值来修正本地值的操作
//                        mCards[cardIdx].mInkLevel = mCards[cardIdx].mMaxVolume - SmartCard.getLocalInk(mCards[cardIdx].mCardType);
// End of H.M.Wang 2021-8-23 取消重新读取卡中的值来修正本地值的操作
                        checkOIB(cardIdx);

                    }
// H.M.Wang 2022-4-16 将墨袋的减锁和墨盒的减锁分离
                    if(mCards[mCurBagIdx].mInitialized && mCards[mCurBagIdx].mValid) {
// H.M.Wang 2021-8-7 追加当写SC卡出现错误的时候报错的处理
                        int ret = SmartCard.downLocal(mCards[mCurBagIdx].mCardType);
                        if(SmartCard.SC_SUCCESS != ret) {
                            mHandler.obtainMessage(MSG_SHOW_CONSISTENCY, "Smartcard access error!").sendToTarget();
                        }
// End of H.M.Wang 2021-8-7 追加当写SC卡出现错误的时候报错的处理
// H.M.Wang 2021-8-23 取消重新读取卡中的值来修正本地值的操作
//                        mCards[mCurBagIdx].mInkLevel = mCards[mCurBagIdx].mMaxVolume - SmartCard.getLocalInk(mCards[mCurBagIdx].mCardType);
// End of H.M.Wang 2021-8-23 取消重新读取卡中的值来修正本地值的操作
                        checkOIB(mCurBagIdx);
                    }
// End of H.M.Wang 2022-4-16 将墨袋的减锁和墨盒的减锁分离
                }
            }
        });
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
