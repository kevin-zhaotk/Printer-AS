package com.industry.printer.Socket_Server;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;
import java.util.TimeZone;

import android.content.Context;
import android.os.Environment;

/*
 * 写日志类
 */

public class WriteLog
{
private Context context;
private static final String SDCARD_ROOT = Environment.getExternalStorageDirectory().toString();
public WriteLog(Context context)
{
	this.context=context;

}
public static void recordLog(String LogDataStr) {  
	  


	boolean saveTypeStr=true;
	        try {  
	  
	           // String savePath = savaInfoToSD();  


	           
	            boolean saveType =saveTypeStr;  
	  
	           // 准备需要保存的文件  
	           /* File saveFilePath = new File(savaInfoToSD());  
	            if (!saveFilePath.exists()) {  
	                saveFilePath.mkdirs();  
	            }*/  


	           File saveFile = new File(savaInfoToSD());  
	            if (!saveType && saveFile.exists()) {  
	               saveFile.delete();  
	                saveFile.createNewFile();  
	                // 保存结果到文件  
	                OutputStreamWriter write = new OutputStreamWriter(new FileOutputStream(saveFile),"gbk");      
	                BufferedWriter writer=new BufferedWriter(write);          
	                writer.write(LogDataStr);      
	                writer.close();   
;
		            
	            } else if (saveType && saveFile.exists()) {  
	                //saveFile.createNewFile();  
	            	 OutputStreamWriter write = new OutputStreamWriter(new FileOutputStream(saveFile),"gbk");      
		                BufferedWriter writer=new BufferedWriter(write);          
		                writer.write(LogDataStr);      
		                writer.close();
	            }else if (saveType && !saveFile.exists()) {  
	                saveFile.createNewFile();  
	                OutputStreamWriter write = new OutputStreamWriter(new FileOutputStream(saveFile),"gbk");      
	                BufferedWriter writer=new BufferedWriter(write);          
	                writer.write(LogDataStr);      
	                writer.close();;
	            }  
	  
	  
	  
	        } catch (Exception e) {  
	            //recordLog(savePathStr, saveFileNameS, saveDataStr, saveTypeStr);  
	              
	            e.printStackTrace();  
	        }  
	  
	  
}
  
private static String savaInfoToSD(){
	String fileName = null;
	//StringBuffer sb = new StringBuffer();
	
	
	
	if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
		File dir = new File(SDCARD_ROOT + File.separator + "lc_assistant" + File.separator);
		if(! dir.exists()){
			dir.mkdir();
		}
		
		//try{
			fileName = dir.toString() + File.separator + "CurrWrite"+paserTime(System.currentTimeMillis()) + ".log";
		//	FileOutputStream fos = new FileOutputStream(fileName);
			//fos.write(sb.toString().getBytes());
			//fos.flush();
			//fos.close();
		//}catch(Exception e){
			//e.printStackTrace();
		//}
		
	}
	
	return fileName;
	
}
private static String paserTime(long milliseconds) {
	System.setProperty("user.timezone", "ry/private Ufpi");
	TimeZone tz = TimeZone.getTimeZone("ry/private Ufpi");
	TimeZone.setDefault(tz);
	SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
	String times = format.format(new Date(milliseconds));
	
	return times;
}
}