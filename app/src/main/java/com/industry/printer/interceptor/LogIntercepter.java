/**
 *
 */

package com.industry.printer.interceptor;


import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.util.Log;

import com.industry.printer.Constants.Constants;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.MessageTask;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PreferenceConstants;
import com.industry.printer.data.DataTask;
import com.industry.printer.object.BaseObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * 每次打印时记录当前打印信息名称和1.tlk文件
 * 分两个文件保存，每个文件最多1000条记录，超出之后循环覆盖
 */
public class LogIntercepter implements IPrintIntercepter {

    private final static String TAG = LogIntercepter.class.getSimpleName();

    private static volatile LogIntercepter instance;

    // 记录当前1.txt的log条数
    FileOutputStream cWriter;
    int count;Context mCtx;

    // singleton
    public static LogIntercepter getInstance(Context ctx) {

        if (instance == null) {
            synchronized (LogIntercepter.class) {
                if (instance == null) {
                    instance = new LogIntercepter(ctx);
                }
            }
        }
        return instance;
    }

    private LogIntercepter(Context ctx) {

        mCtx = ctx;
        // save log count in sharedPreference
        SharedPreferences sp = ctx.getSharedPreferences(PreferenceConstants.SP_PRINT, Context.MODE_PRIVATE);
        count = sp.getInt(PreferenceConstants.LOG_COUNT, 0);
        if (count < 1000) {
            return;
        }
    }


    @Override
    public void execute(DataTask task) {
        Debug.d(TAG, "--->execute: " + task);
        if (task == null) return;

        int logEnable = SystemConfigFile.getInstance().getParam(SystemConfigFile.INDEX_LOG_ENABLE);
        // 要输入1234 才写log
        if (logEnable != Constants.LOG_ENABLE) return;

        RandomAccessFile randomFile = null;

        if (count >= 1000) {
            delete();
        }
//        "/mnt/sdcard/print.bin";
        File log = new File(Configs.LOG_1);
        try {

            if(!log.exists()) {
                log.createNewFile();
            }
            randomFile = new RandomAccessFile(log, "rw");
            // 文件长度，字节数
            long fileLength = randomFile.length();

            randomFile.seek(fileLength);
            List<String> tlks = readTlk(task.mTask);
            for (String line : tlks) {
                randomFile.writeUTF(line);
                randomFile.writeUTF("\r\n");
            }
            randomFile.close();
            count++;
            SharedPreferences sp = mCtx.getSharedPreferences(PreferenceConstants.SP_PRINT, Context.MODE_PRIVATE);
            SharedPreferences.Editor editor = sp.edit();
            editor.putInt(PreferenceConstants.LOG_COUNT, count);
            editor.commit();


        } catch (IOException e) {

        } finally {

            if (randomFile!= null) {
                try {
                    randomFile.close();
                } catch (Exception e) {}
            }
        }

    }

    private void delete() {

        File file2 = new File(Configs.LOG_2);
        if (file2.exists()) {
            file2.delete();
        }
        File file1 = new File(Configs.LOG_1);
        if (file1.exists()) {
            file1.renameTo(file2);
        }
        count = 0;
    }

    private List<String> readTlk(MessageTask message) {
        List<BaseObject> objects = message.getObjects();
        List<String> lines = new ArrayList<String>(objects.size() + 1);
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd:HH:mm:ss");
        Date date = new Date();
        lines.add(format.format(date));
        for (BaseObject obj : objects) {
            lines.add(obj.toString());
        }
        return lines;
    }

}
