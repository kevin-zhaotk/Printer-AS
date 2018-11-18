package com.industry.printer.widget;

import java.util.ArrayList;
import java.util.List;
import com.industry.printer.Utils.Debug;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.ObjectsFromString;
import com.industry.printer.object.RealtimeObject;
import com.industry.printer.object.TextObject;

import android.graphics.Color;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.style.BackgroundColorSpan;

public class SpanableStringFormator extends SpannableStringBuilder {

	private static final String TAG = SpanableStringFormator.class.getSimpleName();

	public SpanableStringFormator(ArrayList<BaseObject> objlist) {
		super();
		setText(objlist);
	}
	
	
	public SpanableStringFormator(String text) {
		super();
		setText(text);
	}
	
	public void setText(String text) {
		this.clear();
		this.clearSpans();
		boolean isFirst=true;
		List<ContentType> list = parseElements(text);
		for (ContentType contentType : list) {
			if (isFirst) {
				this.append(contentType.text);
				isFirst = false;
			} else {
				this.append(ObjectsFromString.SPLITOR + contentType.text);
			}
			
			if (contentType.isVar) {
				this.setSpan(new BackgroundColorSpan(Color.YELLOW), contentType.start, contentType.end, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
			}
		}
		
	}
	
	public void setText(ArrayList<BaseObject> objlist) {
		
		if (objlist == null) {
			Debug.d(TAG, "--->objlist is null");
			return;
		}
		this.clear();
		this.clearSpans();
		List<ContentType> list = parseElements(objlist);
		
		for (ContentType type : list) {
			Debug.d(TAG, "--->text=" + type.text);
			this.append(type.text);
			if (type.isVar) {
				this.setSpan(new BackgroundColorSpan(Color.YELLOW), type.start, type.end, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
			}
		}
	}
	
	/**
	 * 向SpanableStringFormator追加内容，需要根据添加的内容实事分析是否需要设置span
	 * 同理，delete函数也一样
	 * @param text
	 */
	/*public void append(String text) {
		//super.append(text);
		setText(toString());
	}*/
	
	/**
	 * 从SpanableStringFormator删除start开始的内容，需要根据添加的内容实事分析是否需要设置span
	 * 
	 * @param start TextView/EditText字符串的最后一个字符index
	 */
	/*public void delete(int start) {
		//super.delete(start, this.length()-1);
		setText(toString());
	}*/
	
	private List<ContentType> parseElements(String text) {
		int start=0;
		List<ContentType> mElements = new ArrayList<ContentType>();
		ContentType contentType = null;
		String[] elements = text.split(ObjectsFromString.SPLITOR);
		if (elements == null) {
			return mElements;
		}
		for (String element: elements) {
			//实时时钟
			if (element.startsWith(ObjectsFromString.REALTIME_FLAG) || element.startsWith(ObjectsFromString.COUNTER_FLAG)) {
				contentType = new ContentType(start, start + element.length(), element, true);
			} else {
				contentType = new ContentType(start, start + element.length(), element, false);
			}
			start += element.length() + ObjectsFromString.SPLITOR.length();
			mElements.add(contentType);
		}
		return mElements;
		
	}
	
	private List<ContentType> parseElements(List<BaseObject> objlist) {
		int start = 0;
		ContentType contentType = null;
		List<ContentType> mElements = new ArrayList<ContentType>();
		if (objlist == null) {
			return mElements;
		}
		
		for (BaseObject object : objlist) {
			if (object instanceof RealtimeObject || object instanceof CounterObject) {
				contentType = new ContentType(start, start + object.getContent().length(), object.getContent(), true);
			} else if (object instanceof TextObject) {
				contentType = new ContentType(start, start + object.getContent().length(), object.getContent(), false);
			} else {
				continue;
			}
			start += object.getContent().length();
			mElements.add(contentType);
		}
		return mElements;
	}
	
			
	public class ContentType {
		public boolean isVar=false;
		public int start = 0;
		public int end = 0;
		public String text="";
		
		public ContentType() {
			
		}
		
		public ContentType(int start, int end, String text, boolean var) {
			this.start = start;
			this.end = end;
			this.text = text;
			this.isVar = var;
		}
	}
}
