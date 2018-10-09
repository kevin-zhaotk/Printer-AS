package com.industry.printer.FileFormat;

public class XmlTag {
	private String mKey;
	private String mValue;
	
	public XmlTag (String key, String value) {
		if (key == null || value == null) {
			return;
		}
		mKey = key;
		mValue = value;
	}
	
	public String getKey() {
		return mKey;
	}
	
	public String getValue() {
		return mValue;
	}
}