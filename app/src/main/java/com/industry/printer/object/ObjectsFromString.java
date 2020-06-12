package com.industry.printer.object;

import java.util.ArrayList;
import java.util.List;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

import android.R.integer;
import android.content.Context;
import android.text.TextUtils;
import android.util.Log;


/**
 * 解析编辑的字符串，得到打印对象列表
 * @author kevin
 * 用户输入文本字符串规则如下：
 * 对象分隔符为 @#@ 
 * 文本对象：一串简单字符
 * 计数器对象： 以#N#作为起始标示，字符串的长度表示计数器的位数，比如 #N#000000‘表示一个6位计数器）
 * 实时时间对象：以#T#作为起始符，字符串表示实时时间变量的格式 如 #T#YYYY/MM/DD
 * 图形对象：	以单引号包围，以G为主键
 * 注：所有对象以 @#@ 作为分隔符
 */

public class ObjectsFromString {
	
	public static final String TAG = ObjectsFromString.class.getSimpleName();
	
	//各对象之间以；作为分隔符
	public static final String SPLITOR = "@#@";
	public static final String COUNTER_FLAG = "#N#";
	public static final String REALTIME_FLAG = "#T#";
	// H.M.Wang 2020-2-17 追加HyperText控件
	public static final String HYPERTEXT_FLAG = "#H#";
	// End of H.M.Wang 2020-2-17 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
	public static final String DYNAMICTEXT_FLAG = "#D#";
// End of H.M.Wang 2020-6-10 追加DynamicText控件
	public static final String IMAGE_FLAG = "#P#";
	
	public static ArrayList<BaseObject> makeObjs(Context context, String str) {
		int xcor=0;
		int index=1;
		if (str==null || str.isEmpty()) {
			return null;
		}
		ArrayList<BaseObject> objList=new ArrayList<BaseObject>();
		
		/**第一个对象为Messageobject，与其他项目保持一致**/
		MessageObject msgObject = new MessageObject(context, 0);
		msgObject.setIndex(index++);
		
		objList.add(msgObject);
		Debug.d(TAG, "===>str: "+str);
		String[] objStrings = str.split(SPLITOR);
		for (String s:objStrings) {
			if (TextUtils.isEmpty(s)) {
				continue;
			}
			if (s.startsWith(COUNTER_FLAG)) { //计数器对象
				Log.d(TAG, "===>counter: "+s);
				int count = s.length() - 3;
				CounterObject obj = new CounterObject(context, xcor);
				obj.setBits(count);
				obj.setIndex(index++);
				objList.add(obj);
				// xcor += obj.getXEnd() + 5;
				xcor += s.length() * 16;
			} else if (s.startsWith(IMAGE_FLAG)) {	//图形对象
				Debug.d(TAG, "makeObjs image object");
			} else if(s.startsWith(REALTIME_FLAG)) {
				RealtimeObject object = new RealtimeObject(context, xcor);
				String format = s.substring(3);
				object.setFormat(format);
				object.setIndex(index++);
				objList.add(object);
				xcor += format.length() * 16;
// H.M.Wang 2020-2-17 追加HyperText控件
			} else if(s.startsWith(HYPERTEXT_FLAG)) {
				HyperTextObject object = new HyperTextObject(context, xcor);
				String content = s.substring(3);
				object.setContent(content);
				object.setIndex(index++);
				objList.add(object);
				xcor += content.length() * 16;
// End of H.M.Wang 2020-2-17 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
			} else if(s.startsWith(DYNAMICTEXT_FLAG)) {
				DynamicText object = new DynamicText(context, xcor);
				String content = s.substring(3);
				object.setContent(content);
				object.setIndex(index++);
				objList.add(object);
				xcor += content.length() * 16;
// End of H.M.Wang 2020-6-10 追加DynamicText控件
			} else {	//文本对象
				if (s==null || s.isEmpty()) {
					continue;
				}
				Debug.d(TAG, "===>text: "+s + "-->len=" + s.length());
				TextObject obj = new TextObject(context, xcor);
				obj.setContent(s);
				obj.setIndex(index++);
				objList.add(obj);
				xcor += 16*s.length();
			}
		}
		return objList;
	}
}
