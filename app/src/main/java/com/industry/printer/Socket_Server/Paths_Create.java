package com.industry.printer.Socket_Server;

import java.io.File;

import android.os.Environment;



public class Paths_Create {



	//鑽煎彇鏈満鐩綍骞跺垱寤簆rinter鐩綍
public String getSDPath() {
			File sdDir = null;
			boolean sdCardExist = Environment.getExternalStorageState().equals(
					android.os.Environment.MEDIA_MOUNTED); // 閸掋倖鏌�
			if (sdCardExist) {
				sdDir = Environment.getExternalStorageDirectory();// 閼惧嘲褰囩捄鐔烘窗
			}

			if (sdDir == null) {
				return "";
			}

			File destDir = new File(sdDir.toString() + "/Printer/");

			if (!destDir.exists()) {
				destDir.mkdirs();
			}

			return  "/mnt/usbhost0/MSG1/";//sdDir.toString() + "/Printer/";//mnt/usbhost0/MSG1/";// sdDir.toString() + "/Printer/";
			//"mnt/usbhost0/MSG1/";//
		}
	//鍒涘缓鐩綍	
		
	public String CreateDir(String Paths)
	{
		 File destDir = null;
		
		String Srcpaths=Paths.substring(Paths.indexOf("/")+1,Paths.lastIndexOf("/"));//=getSDPath();
		 String[] Apath = Srcpaths.split("\\/");
		 Srcpaths="/";// getSDPath();
		 for(int i=0;i<Apath.length;i++)
		 {
			 Srcpaths+=Apath[i]+"/";
			 destDir = new File(Srcpaths); 
			if (!destDir.exists()) {
				destDir.mkdirs();
			}
		}
		
		return destDir.toString()+"/";
	}
	//mnt/sdcard/Printer/
	//mnt/usbhost0/msg1/
	public int ListDirFiles(String dir)
	{
		 File destDir = null;
		 destDir = new File(dir); 
			if (!destDir.exists()) {
				return -1;
			}
		return 0;
	}
	
}
