package com.industry.printer.FileFormat;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import android.R.xml;

import com.industry.printer.Utils.Debug;

public class XmlOutputStream {

	private static final String TAG = XmlOutputStream.class.getSimpleName();
	
	public FileOutputStream mOutputStream;
	
	public XmlOutputStream(String file) {
		try {
			mOutputStream = new FileOutputStream(new File(file));
			
		} catch (FileNotFoundException e) {
			Debug.e(TAG, "file not found:"+file);
		} 
	}
	
	public XmlOutputStream(FileOutputStream outputStream) {
		
	}
	
	public boolean write(List<XmlTag> pairs) {
		StringWriter writer = new StringWriter();
		try {
			XmlSerializer mSerializer = XmlPullParserFactory.newInstance().newSerializer();
			/*设置输出流对象*/
			mSerializer.setOutput(writer);
			/*开始标签*/
			mSerializer.startDocument("utf-8", true);
			mSerializer.text("\n");
			for (XmlTag pair : pairs) {
				mSerializer.startTag(null, pair.getKey());
				mSerializer.text(pair.getValue());
				mSerializer.endTag(null, pair.getKey());
				mSerializer.text("\n");
			}
			mSerializer.endDocument();
			
			if (mOutputStream != null) {
				mOutputStream.write(writer.toString().getBytes());
			}
			mOutputStream.flush();
			writer.close();
			mOutputStream.close();
			
		} catch (Exception e) {
			Debug.d(TAG, "--->error: " + e.toString());
			return false;
		}
		return true;
	}
	
	public void close() {
		try {
			mOutputStream.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
}
