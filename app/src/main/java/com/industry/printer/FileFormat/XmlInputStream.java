package com.industry.printer.FileFormat;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.xml.sax.SAXException;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;


import com.industry.printer.Utils.Debug;

public class XmlInputStream {
	
	private static final String TAG = XmlInputStream.class.getSimpleName();
	
	private FileInputStream mInputStream;
	private boolean isReady = false;
	
	public XmlInputStream(String file) {
		try {
			Debug.d(TAG, "===>file:"+file);
			mInputStream = new FileInputStream(new File(file));
		} catch (Exception e) {
			Debug.e(TAG, "file not found:"+file);
		}
		isReady = true;
	}
	
	public XmlInputStream(FileInputStream inputStream) {
		
	}
	
	public List<XmlTag> read() {
		ArrayList<XmlTag> mPairs=null;
		if (mInputStream == null) {
			return null;
		}
		try {
			XmlPullParser parser = XmlPullParserFactory.newInstance().newPullParser();
			parser.setInput(mInputStream, "utf-8");
			parser.next();
			int event = parser.getEventType();
			mPairs = new ArrayList<XmlTag>();
			while (event != XmlPullParser.END_DOCUMENT) {
				switch (event) {
				case XmlPullParser.START_DOCUMENT:
					
					break;
				case XmlPullParser.START_TAG:
					Debug.d(TAG, "===>tag:"+parser.getName());
					String key = parser.getName();
					String value = parser.nextText();
					Debug.d(TAG, "===>tag key: "+key+", value: "+value);
					XmlTag tag = new XmlTag(key, value);
					mPairs.add(tag);
					break;
				case XmlPullParser.END_TAG:
				case XmlPullParser.END_DOCUMENT:
				default:
					break;
				}
				event = parser.next();
			}
			
		} catch (XmlPullParserException e) {
			Debug.d(TAG, "read error:"+e.getMessage());
			return null;
		} catch (IOException e) {
			Debug.d(TAG, "read error:"+e.getMessage());
			return null;
		}
		
		return mPairs;
	}
	
	public void close() {
		try {
			if (mInputStream == null) {
				return;
			}
			mInputStream.close();
			Debug.d(TAG, "--->close sysconfig.xml");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
