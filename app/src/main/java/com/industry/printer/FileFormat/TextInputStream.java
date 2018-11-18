package com.industry.printer.FileFormat;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.nio.CharBuffer;

import com.industry.printer.Utils.Debug;

import android.nfc.Tag;

public class TextInputStream {

	private static final String TAG = TextInputStream.class.getSimpleName();
	
	private static TextInputStream mInstance;
	
	public static TextInputStream getInstance() {
		if (mInstance == null) {
			mInstance = new TextInputStream();
		}
		return mInstance;
	}
	
	public String getText(String file) {
		
		try {
			InputStreamReader in = new InputStreamReader(new FileInputStream(file), "GBK");
			BufferedReader reader = new BufferedReader(in);
			//Debug.d(TAG, "===>" + reader.readLine());
			return reader.readLine();
			
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return null;
	}
}
