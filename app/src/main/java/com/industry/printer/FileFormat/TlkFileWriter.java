package com.industry.printer.FileFormat;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;

import com.industry.printer.MessageTask;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Debug;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.RealtimeObject;

import android.content.Context;

public class TlkFileWriter extends TlkFile{
	
	public static final String TAG = TlkFileWriter.class.getSimpleName();

	public MessageTask mTask;
	
	public TlkFileWriter(Context context, MessageTask task) {
		super(context, task.getName());
		mTask = task;
		makeDir(mFile);
	}


	public void write() {
		int i=1;
		BufferedWriter bw=null;
		Debug.d(TAG, "=====>saveObjFile path="+mPath);
		
		File tlk = new File(mPath);
		ArrayList<BaseObject> objs = mTask.getObjects();
		
		try {
			if(!tlk.exists() && !tlk.createNewFile()){
				Debug.d(TAG, "create error "+tlk.getPath());
				return;
			}
//			fw = new FileWriter(tlk);

			bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(tlk), "UTF-8"));

			for(BaseObject o : objs)
			{
				if(o instanceof RealtimeObject)
				{
					// System.out.println("******"+BaseObject.intToFormatString(i, 3)+"^"+o.toString());
					bw.write(BaseObject.intToFormatString(i, 3)+"^"+o.toString());
					bw.newLine();
					o.setIndex(i++);
					for(BaseObject so : ((RealtimeObject) o).getSubObjs())
					{
						// System.out.println("******"+BaseObject.intToFormatString(i, 3)+"^"+so.toString());
						bw.write(BaseObject.intToFormatString(i, 3)+"^"+so.toString());
						bw.newLine();

						so.setIndex(i++);
					}
				}
				else
				{
					// Debug.d(TAG, "filestr="+BaseObject.intToFormatString(i, 3) +"^"+o.toString());
					bw.write(BaseObject.intToFormatString(i, 3)+"^"+o.toString());
					bw.newLine();
					o.setIndex(i++);
				}
				
			}
			bw.flush();
			bw.close();
			//fw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 创建TLK文件目录
	 * @param name
	 */
	private void makeDir(String name) {
		File file = new File(ConfigPath.getTlkDir(name));
		if (!file.exists()) {
			file.mkdirs();
		}
	}
}
