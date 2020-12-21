package com.industry.printer.FileFormat;

/**
 * Created by hmwan on 2020/6/7.
 */

import android.content.Context;

import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.HashMap;

/**
 * 从QR.txt文件当中逐行读取信息。信息格式为：
 *      012345abcdefghijk
 *   其中，
 *      012345：6位，代表包号
 *      abcdefghijk：11位，代表批号
 *   包号通过扫描获得，根据包号查找批号
 * @author H.M. Wang
 *
 */

public class PackageListReader {
    private static final String TAG = PackageListReader.class.getSimpleName();

    public Context mContext;

    private static PackageListReader mInstance;
    private HashMap<String, String> mPackageList;

    public static PackageListReader getInstance(Context ctx) {
        if (mInstance == null) {
            synchronized (PackageListReader.class) {
                if (mInstance == null) {
                    mInstance = new PackageListReader(ctx);
                }
            }
        }
        return mInstance;
    }

    private PackageListReader(Context ctx) {
        mContext = ctx;
        mPackageList = null;
        init();
    }

    private void init() {
        try {
// H.M.Wang 2020-12-17 包号批号对应表这个原来使用的是QR_DATA这个文件，与QR共用文件，这样容易混淆，现在分开，使用PP_DATA
            FileReader r = new FileReader(Configs.PP_DATA);
// End of H.M.Wang 2020-12-17 包号批号对应表这个原来使用的是QR_DATA这个文件，与QR共用文件，这样容易混淆，现在分开，使用PP_DATA
            BufferedReader bufferedReader = new BufferedReader(r);
            mPackageList = new HashMap<String, String>();

            String readLine = null;
            while(null != (readLine = bufferedReader.readLine())) {
                try {
                    mPackageList.put(readLine.substring(0,6), readLine.substring(6,17));
                    Debug.d(TAG, readLine.substring(0,6) + " => " + readLine.substring(6,17));
                } catch(IndexOutOfBoundsException e) {
                    Debug.e(TAG, e.getMessage());
                }
            }
        } catch(Exception e) {
            Debug.e(TAG, e.getMessage());
        }
    }

    public String getBatchCode(String packCode) {
        if(null != mPackageList) {
            Debug.d(TAG, "getCode(" + packCode + ") => " + mPackageList.get(packCode));
            return mPackageList.get(packCode);
        }
        return null;
    }
}
