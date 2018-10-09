package com.industry.printer;

public interface InkLevelListener {
	
	// 墨水值下降
	public void onInkLevelDown(int i);
	
	// 计数器更新
	public void onCountChanged();
	
	// 墨盒空
	public void onInkEmpty();

}
