package com.industry.printer.FileFormat;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Vector;

import android.graphics.Bitmap.Config;

import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.object.TlkObject;

public class TlkFileParser {

	public static final String TAG="Tlk_Parser";
	public static int mColumns=0;
	
	public static boolean parse(String f, Vector<TlkObject> list)
	{
		try {
			BufferedReader reader = new BufferedReader(new FileReader(new File(f)));
			String s;
			while((s = reader.readLine()) != null)
			{
				
				String[] line = s.split("\\^");
				if (line == null) {
					break;
				}
				int index = Integer.parseInt(line[1]);
				if((index >= 1 && index <= 16)||(index >=21 && index <=24))	
				{
					TlkObject obj = new TlkObject();
					obj.setIndex(index);
					obj.setX(Integer.parseInt(line[2]));
					obj.setY(Integer.parseInt(line[3]));
					obj.setFont(line[19]);
					Debug.d(TAG,"index="+index+", x="+obj.x+", y="+obj.y+", font="+obj.font);
					
					list.add(obj);
				}
				else if(index == 31) //head
				{
					mColumns = Integer.parseInt(line[2]);
					Debug.d(TAG, "#############line[2]="+line[2]+", mColumns="+mColumns);
				}
			}
			reader.close();
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		catch(NumberFormatException e)
		{
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	public static void parseLine(String line)
	{
		
	}
	
}
