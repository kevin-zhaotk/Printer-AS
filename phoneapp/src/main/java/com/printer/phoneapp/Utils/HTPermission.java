package com.printer.phoneapp.Utils;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.PermissionChecker;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by hmwan on 2021/9/8.
 */

public class HTPermission {
    public static final String TAG = HTPermission.class.getSimpleName();

    // 23之前，只要你在AndroidManifest中申请了权限，都会给你；在23之后，
    // 需要动态申请权限。权限申请分为两类：第一类是不涉及用户隐私的，
    // 只需要在Manifest中声明即可，比如网络、蓝牙、NFC等；
    // 第二类是涉及到用户隐私信息的，需要用户授权后才可使用，A比如SD卡读写、联系人、短信读写等。

    // https://www.jianshu.com/p/14ae4653787c

    // 检查权限是否获取（android6.0及以上系统可能默认关闭权限，且没提示）

    private Activity mActivity = null;
    private int mRequestCode = 0;
//    private boolean mReqesting = false;

    public HTPermission(Activity activity, int requestCode) {
        mActivity = activity;
        mRequestCode = requestCode;
    }

    public boolean isPermissionGuaranteed(String checkPermission) {
        if(null == mActivity) return false;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            return (ActivityCompat.checkSelfPermission(mActivity, checkPermission) == PackageManager.PERMISSION_GRANTED);
        } else {
            return (PermissionChecker.checkSelfPermission(mActivity, checkPermission) == PackageManager.PERMISSION_GRANTED);
        }
    }

    public int requestPermission(String needPermission) {
        return requestPermissions(new String[]{needPermission});
    }

    public int requestPermissions(String[] needPermissions) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            List<String> tmpPermissions = new ArrayList<String>();
            for (int i = 0; i < needPermissions.length; i++) {
                if (ActivityCompat.checkSelfPermission(mActivity, needPermissions[i]) != PackageManager.PERMISSION_GRANTED) {
                    tmpPermissions.add(needPermissions[i]);
                }
            }

            if (!tmpPermissions.isEmpty()) {
                String[] reqPermissions = tmpPermissions.toArray(new String[tmpPermissions.size()]);
                try {
//                    mReqesting = true;
                    ActivityCompat.requestPermissions(mActivity, reqPermissions, mRequestCode); // mRequestCode必须是1-65535
                    Log.d(TAG, "Request Count: " + reqPermissions.length);
                    return reqPermissions.length;
                } catch (Exception e) {
//                    mReqesting = false;
                    Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
                }
            }
        }
        Log.d(TAG, "Request Count: 0");
        return 0;
    }

/*
    public boolean isRequesting() {
        return mReqesting;
    }
    public void finishRequest() {
        mReqesting = false;
    }
*/
}
