package com.industry.printer.hardware;

import com.industry.printer.Utils.Debug;

import static java.lang.Thread.sleep;

/**
 * Created by hmwan on 2020/6/2.
 */

public class BarcodeScanParser {
    private static final String TAG = BarcodeScanParser.class.getSimpleName();

    private static StringBuffer mCodes = new StringBuffer();
    private static long mLast = 0;

    private static Thread mThread = null;

    public interface OnScanCodeListener {
        public void onCodeReceived(String code);
    }

    private static OnScanCodeListener mCodeListener = null;

    public static void append(char code) {
        synchronized(BarcodeScanParser.class) {
            long currency = System.currentTimeMillis();
//            if (currency - mLast > 200) {
//                mCodes.delete(0, mCodes.length());
//            }
            mLast = currency;
            mCodes = mCodes.append(code);
//            Debug.i("Global", "--->gCode: " + mCodes.toString());
        }

        if(null == mThread) {
            mThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    while(true) {
                        synchronized(BarcodeScanParser.class) {
                            if(System.currentTimeMillis() - mLast > 300) {
                                handleCode();
                                break;
                            }
                        }
                        try{sleep(10);} catch (Exception e){}
                    }
                    mThread = null;
                }
            });
            mThread.start();
        }
    }

    public static void setListener(OnScanCodeListener l) {
        mCodeListener = l;
    }

    private static void handleCode() {
        Debug.i(TAG, "Code: [" + mCodes.toString() + "](" + mCodes.length() + ")");
        String code = mCodes.toString();
        if(code.length() != 33) {                               // 扫描到的字符串长度为32+1
//            mCodes.delete(0, mCodes.length());
        } else if(code.charAt(1) != code.charAt(32)) {          // 最后一位与第二位的值需要一致
//            mCodes.delete(0, mCodes.length());
        } else {
            if(null != mCodeListener) {
//            Debug.i(TAG, "Handle Code: " + mCodes.toString());
                mCodes.setLength(32);
                mCodeListener.onCodeReceived(mCodes.toString());
            }
        }
        mCodes.delete(0, mCodes.length());
    }
}
