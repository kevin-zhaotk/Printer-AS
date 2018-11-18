package com.industry.printer.FileFormat;

import java.io.File;
import java.io.FilenameFilter;

import com.industry.printer.Utils.Debug;

public class FilenameSuffixFilter implements FilenameFilter {

	public static final String TLK_SUFFIX=".tlk";
	public static final String CSV_SUFFIX=".csv";
	public static final String BIN_SUFFIX=".bin";
	
	public String mSuffix;
	
	public FilenameSuffixFilter(String suffix)
	{
		mSuffix = suffix;
	}
	
	@Override
	public boolean accept(File dir, String filename) {
		// TODO Auto-generated method stub
		//Debug.d("FilenameSuffixFilter", "dir="+dir.toString());
		//Debug.d("FilenameSuffixFilter", new File(dir,filename).toString()+" is directory?"+new File(dir,filename).isDirectory());
		if(new File(dir,filename).isDirectory())
			return true;
		if(mSuffix==null || filename==null || !filename.toLowerCase().endsWith(mSuffix))
			return false;
		else 
			return true;
	}

}
