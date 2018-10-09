package com.industry.printer.Utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class SystemFs {

	private static final String TAG = SystemFs.class.getSimpleName();
	
	/**
	 * 读取系统文件/sys/
	 * @param path
	 * @return
	 */
	public static String readSysfs(String path) {
        String str = null;
        StringBuilder value = new StringBuilder();

        /**
         * 看系统支不支持SystemWriterManager，如果支持直接调用
         */
        
        try {
            FileReader fr = new FileReader(path);
            BufferedReader br = new BufferedReader(fr);
            try {
                while ((str = br.readLine()) != null) {
                    if(str != null)
                        value.append(str + "/r/n");
                };
                fr.close();
                br.close();
                return value.toString();
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }
	
	/**
	 * 写入系统文件/sys/
	 * @param path 要写入的文件路径
	 * @param value 写入内容
	 * @return
	 */
	public static boolean writeSysfs(String path, String value) {
        
        if (!new File(path).exists()) {
            Debug.e(TAG, "File not found: " + path);
            return false;
        }

        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(path), 64);
            try {
                writer.write(value);
            } finally {
                writer.close();
            }
            return true;

        } catch (IOException e) {
            Debug.e(TAG, "IO Exception when write: " + path, e);
            return false;
        }
    }

}
