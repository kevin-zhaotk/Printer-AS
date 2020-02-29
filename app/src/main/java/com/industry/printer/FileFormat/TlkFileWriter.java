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
import com.industry.printer.object.HyperTextObject;
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
// H.M.Wang 2019-9-24 修改换行符号，newLine很可能只返回/n，文件不会换行
//					bw.newLine();
					bw.write("\r\n");
					o.setIndex(i++);
					for(BaseObject so : ((RealtimeObject) o).getSubObjs())
					{
						// System.out.println("******"+BaseObject.intToFormatString(i, 3)+"^"+so.toString());
						bw.write(BaseObject.intToFormatString(i, 3)+"^"+so.toString());
// H.M.Wang 2019-9-24 修改换行符号，newLine很可能只返回/n，文件不会换行
//						bw.newLine();
						bw.write("\r\n");

						so.setIndex(i++);
					}
				}
// H.M.Wang 2020-2-16 追加HyperText控件
				else if(o instanceof HyperTextObject) {
					bw.write(BaseObject.intToFormatString(i, 3)+"^"+o.toString());
					bw.write("\r\n");
					o.setIndex(i++);
					for(BaseObject so : ((HyperTextObject) o).getSubObjs()) {
						bw.write(BaseObject.intToFormatString(i, 3)+"^"+so.toString());
						bw.write("\r\n");
						so.setIndex(i++);
					}
				}
// End of H.M.Wang 2020-2-16 追加HyperText控件
				else
				{
					// Debug.d(TAG, "filestr="+BaseObject.intToFormatString(i, 3) +"^"+o.toString());
					bw.write(BaseObject.intToFormatString(i, 3)+"^"+o.toString());
// H.M.Wang 2019-9-24 修改换行符号，newLine很可能只返回/n，文件不会换行
//					bw.newLine();
					bw.write("\r\n");
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
