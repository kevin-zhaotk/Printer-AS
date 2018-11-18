package com.industry.printer.Utils;

import android.content.Context;
import android.os.FileObserver;
import android.text.TextUtils;

import com.industry.printer.FileFormat.SystemConfigFile;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by JLC-BJ0143-1708 on 2018/6/14.
 */

public class KZFileObserver extends FileObserver {

    private static final String TAG = KZFileObserver.class.getSimpleName();

    private Context mContext;

    private Map<String, KZFileObserverInterface> mCallbacks;

    public KZFileObserver(Context context, String path) {
        super(path);
        mCallbacks = new HashMap<String, KZFileObserverInterface>();
        Debug.d(TAG, "--->file: " + path);
    }

    @Override
    public void onEvent(int event, String path) {
        Debug.d(TAG, "--->event: " + event + "    path= " + path);
        switch(event) {
            case FileObserver.CLOSE_WRITE:
                Debug.d("Create", "path:"+ path);
                // reload config after system_config.xml modify
                KZFileObserverInterface callback = mCallbacks.get(path);
                Debug.d(TAG, "--->callback: " +callback);
                if (callback != null) {
                   callback.onChanged();
                }
                break;
        }
    }

    public void registerCallback(String path, KZFileObserver.KZFileObserverInterface callback) {
        if (TextUtils.isEmpty(path) || callback == null) {
            return;
        }
        mCallbacks.put(path, callback);
    }


    public interface KZFileObserverInterface {
        void onChanged();
    }
}
