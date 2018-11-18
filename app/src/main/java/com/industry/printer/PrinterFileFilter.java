package com.industry.printer;

import java.io.File;
import java.io.FilenameFilter;
import com.industry.printer.Utils.Debug;

public class PrinterFileFilter implements FilenameFilter {

	private final String TAG="PrinterFileFilter"; 
	public String mPrefix;
	
	PrinterFileFilter(String prefix)
	{
		mPrefix = prefix; 
	}
	@Override
	public boolean accept(File dir, String filename) {
		// TODO Auto-generated method stub
		if(mPrefix != null)
		{
			return filename.startsWith(mPrefix);
		}
		return false;
	}

}
