package com.industry.printer;

import android.content.Context;

import com.industry.printer.FileFormat.TlkFileParser;
import com.industry.printer.MessageTask;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.MessageObject;

import android.os.Handler;

public class MessageForPc {

	private Context mContext;
	private MessageTask mTask;
	
	/**
	 * 構造用於pc的消息創建
	 * @param ctx context
	 * @param name 打印信息名稱
	 */
	public MessageForPc(Context ctx, String name) {
		this(ctx);
		mTask.setName(name);
	}
	
	/**
	 * 構造用於pc的消息創建
	 * @param ctx
	 */
	public MessageForPc(Context ctx) {
		mContext = ctx;
		mTask = new MessageTask(mContext);
		MessageObject msg = new MessageObject(mContext, 0);
	}

	public MessageForPc(Context ctx, String tlk, String name) {
		mContext = ctx;
		mTask = new MessageTask(mContext, tlk, name);
	}
	
	/**
	 * 設置消息名稱，如果沒有名稱就無法正確保存
	 * @param name
	 */
	public void setName(String name) {
		mTask.setName(name);
	}
	
	public String getName() {
		return mTask.getName();
	}
	
	/**
	 * 插入一個打印對象
	 * @param object
	 */
	public void insert(BaseObject object) {
		mTask.addObject(object);
	}
	
	/**
	 * 刪除打印對象
	 * @param object
	 */
	public void delete(BaseObject object) {
		mTask.removeObject(object);
	}
	
	public void clear() {
		mTask.removeAll();
	}
	
	/**
	 * 保存打印信息，生成tlk和bin文件
	 */
//	public void save() {
//		mTask.save(null);
//	}
	
	public void reCreate(Context context, Handler handler, String message) {
		mTask.createTaskFolderIfNeed();
		mTask.save(handler, message);
	}
}
