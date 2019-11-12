/**
 *
 */

package com.industry.printer.interceptor;


import android.util.Log;

import com.industry.printer.MessageTask;
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
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class LogIntercepter implements IPrintIntercepter {

    private final static String TAG = LogIntercepter.class.getSimpleName();

    private static volatile LogIntercepter instance;

    // 记录当前1.txt的log条数
    File cFile;
    FileOutputStream cWriter;
    int current;

    // singleton
    public static LogIntercepter getInstance() {

        if (instance == null) {
            synchronized (LogIntercepter.class) {
                if (instance == null) {
                    instance = new LogIntercepter();
                }
            }
        }
        return instance;
    }

    private LogIntercepter() {

        cFile = new File("/mnt/sdcard/count.txt");

        try {
            if (!cFile.exists()) {
                cFile.createNewFile();
            }

            FileInputStream cReader = new FileInputStream(cFile);
            if (cReader.available() == 0) {
                current = 0;
            } else {
                byte[] content = new byte[cReader.available()];
                cReader.read(content);
                current = Integer.valueOf(new String(content));
            }
            cWriter = new FileOutputStream(cFile);

        } catch (Exception e) {
            Log.i(TAG, "--->failed to open count file: " + e.getMessage());
            current = 0;
        }

    }


    @Override
    public void execute(DataTask task) {
        if (task == null) return;
        MessageTask msg = task.mTask;
        BufferedWriter writer = null;
//        "/mnt/sdcard/print.bin";
        File log = new File("/mnt/sdcard/1.txt");
        try {
            if(!log.exists()) {
                log.createNewFile();
            }

            writer = new BufferedWriter(new FileWriter(log));

        } catch (IOException e) {

        } finally {

            if (writer!= null) {
                try {
                    writer.close();
                } catch (Exception e) {}
            }
        }

    }

    private void delete(File log) {
        if(log == null) return;

        File log2 = new File("/mnt/sdcard/2.txt");
        if (log2.exists()) {
            log2.delete();
        }

        log.renameTo(new File("/mnt/sdcard/2.txt"));
    }

    private List<String> readTlk(MessageTask message) {
        List<BaseObject> objects = message.getObjects();
        List<String> lines = new ArrayList<String>(objects.size() + 1);
        SimpleDateFormat format = new SimpleDateFormat("yyyy:MM:dd:HH:mm:ss");
        Date date = new Date();
        lines.add(format.format(date));
        for (BaseObject obj : objects) {
            lines.add(obj.toString());
        }
        return lines;
    }

}
