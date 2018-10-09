package com.industry.printer.ui.Items;

public class PictureItem {

	private String mPath;
	private String mTitle;
	
	public PictureItem(String path, String title) {
		mPath = path;
		mTitle = title;
	}
	
	public String getPath() {
		return mPath;
	}
	
	public String getTitle() {
		return mTitle;
	}

}
