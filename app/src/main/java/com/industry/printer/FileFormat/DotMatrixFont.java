package com.industry.printer.FileFormat;


import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.industry.printer.Utils.Debug;


/*
 *dot matrix file format:
 *1st line: dot-matrix size width*height
 *
 */
public class DotMatrixFont {

	public final String TAG="DotMatrixFont";
	
	public static final String USB_PATH="/mnt/usb/";
	public static final String USB_SYS_PATH="/mnt/usb/system/";
	public static final String FONT_FILE_PATH="/mnt/usb/system/font/";
	public static final String LOGO_FILE_PATH="/mnt/usb/system/logo/";
	public static final String TLK_FILE_PATH="/mnt/usb/system/TLK/";
	
	
	public int mWidth;
	public int mHeight;
	BufferedReader mReader;
	File mfile;
	
	public DotMatrixFont(String file)
	{
		mfile = new File(file);
		
		try {
			mReader = new BufferedReader(new FileReader(mfile));
			mReader.mark(1);
			String s = mReader.readLine();
			String re = "//[0-9]*x[0-9]*";
			Pattern p = Pattern.compile(re);
			Matcher m = p.matcher(s);
			while(m.find())
				Debug.d(TAG, "********"+m.group(0));
			Debug.d(TAG, "*************");
			mReader.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	public void getDotbuf(String str, int[] buf)
	{
		int ascii;
		String s;
		if(str == null)
		{
			Debug.d(TAG, "******str="+str);
			return;
		}
		int columns = getColumns();
		//Debug.d(TAG, "#######columns="+columns);
		try {
			
			mReader = new BufferedReader(new FileReader(mfile));
			mReader.mark((int)mfile.length()+1);
			for(int i=0; i< str.length(); i++)
			{
				ascii = (int)str.charAt(i);
				mReader.reset();
				
				while((s=mReader.readLine())!=null)
				{
					if(s.startsWith("/*") && s.contains(String.valueOf(ascii)))
					break;
				}
				/*read P1 head*/
				for(;;)
				{
					s=mReader.readLine();
					if(s!=null && s.startsWith("P1"))
						break;
				}
				/*read P1 content*/
				s=mReader.readLine();
				if(s==null)
					break;
				String[] dot = s.split("  ");
				//Debug.d(TAG,"********P1*********");
				for(int k=0;k<8 && k<dot.length; k++)
				{
					if(columns < k+1 )
						break;
					dot[k]=dot[k].trim();
					try{
						buf[i*columns*2+2*k] = Integer.parseInt(dot[k],16);
					}catch(Exception e)
					{
						
					}
					//Debug.d(TAG, "k="+k+" ,buf["+(i*columns*2+2*k)+"]="+buf[i*columns*2+2*k]);
				}
				
				/*read P2 head*/
				for(;;)
				{
					s=mReader.readLine();
					if(s!=null && s.startsWith("P2"))
						break;
				}
				/*read P2 content*/
				s=mReader.readLine();
				if(s==null)
					break;
				//Debug.d(TAG,"********P2*********");
				dot = s.split("  ");
				for(int k=0;k<8 && k<dot.length; k++)
				{
					if(columns <= 8+k)
						break;
					dot[k]=dot[k].trim();
					try{
						buf[i*columns*2+2*(8+k)] = Integer.parseInt(dot[k],16);
					}catch(Exception e)
					{}
					//Debug.d(TAG, "k="+k+" ,buf["+(i*columns*2+2*(8+k))+"]="+buf[i*columns*2+2*(8+k)]);
				}
				
				/*read P3 head*/
				for(;;)
				{
					s=mReader.readLine();
					if(s!=null && s.startsWith("P3"))
						break;
				}
				/*read P3 content*/
				s=mReader.readLine();
				if(s==null)
					break;
				//Debug.d(TAG,"********P3*********");
				dot = s.split("  ");
				for(int k=0;k<8 && k<dot.length; k++)
				{
					if(columns < k+1 )
						break;
					dot[k]=dot[k].trim();
					try{
						buf[i*columns*2+2*k+1] = Integer.parseInt(dot[k], 16);
					}catch(Exception e)
					{}
					//Debug.d(TAG, "k="+k+" ,buf["+(i*columns*2+2*k+1)+"]="+buf[i*columns*2+2*k+1]);
				}
				
				/*read P4 head*/
				for(;;)
				{
					s=mReader.readLine();
					if(s!=null && s.startsWith("P4"))
						break;
				}
				/*read P4 content*/
				s=mReader.readLine();
				if(s==null)
					break;
				//Debug.d(TAG,"********P4*********");
				dot = s.split("  ");
				for(int k=0;k<8 && k<dot.length; k++)
				{
					if(columns <= 8+k)
						break;
					dot[k]=dot[k].trim();
					try{
						buf[i*columns*2+2*(8+k)+1] = Integer.parseInt(dot[k], 16);
					}catch(Exception e)
					{}
					//Debug.d(TAG, "k="+k+" ,buf["+(i*columns*2+2*(8+k)+1)+"]="+buf[i*columns*2+2*(8+k)+1]);
				}
			}
			mReader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Debug.d(TAG, "e="+e.getMessage());
		}
		
	}
	
	
	public void getDotbuf(int[] buf)
	{
		String s;
		String[] dot=null;
		try {
			
			mReader = new BufferedReader(new FileReader(mfile));
			mReader.mark((int)mfile.length()+1);
			/*read out the header*/
			s=mReader.readLine();
			s=mReader.readLine();
			
			{
				/*read P1 head*/
				s=mReader.readLine();
				if(s==null || !s.startsWith("P1"))
					return;
				/*read P1 content*/
				
				
				//Debug.d(TAG,"********P1*********");
				for(int i=0;i<32; i++)
				{
					s=mReader.readLine();
					if(s==null)
						return;
					dot = s.split("  ");
					for(int k=0;k<8 && k<dot.length; k++)
					{
						dot[k]=dot[k].trim();
						buf[i*8+k] = Integer.parseInt(dot[k],16);
						//Debug.d(TAG, "buf["+(i*8+k)+"]="+buf[(i*8+k)]);
					}
				}
				/*read space line*/
				s=mReader.readLine();
				/*read P2 head*/
				s=mReader.readLine();
				if(s==null || !s.startsWith("P2"))
					return;
				/*read P2 content*/
				for(int i=0;i<32;i++)
				{
					s=mReader.readLine();
					if(s==null)
						return;
					//Debug.d(TAG,"********P2*********");
					dot = s.split("  ");
					for(int k=0;k<8 && k<dot.length; k++)
					{
						dot[k]=dot[k].trim();
						buf[(i+32)*8+k] = Integer.parseInt(dot[k],16);
						//Debug.d(TAG, "buf["+((i+32)*8+k)+"]="+buf[(i+32)*8+k]);
					}
				}
				/*read space line*/
				s=mReader.readLine();
				/*read P3 head*/
				//Debug.d(TAG,"********P3*********");
				s=mReader.readLine();
				if(s==null || !s.startsWith("P3"))
					return;
				/*read P3 content*/
				for(int i=0;i<32;i++)
				{
					s=mReader.readLine();
					if(s==null)
						break;
					
					dot = s.split("  ");
					for(int k=0;k<8 && k<dot.length; k++)
					{
						dot[k]=dot[k].trim();
						buf[(i+64)*8+k] = Integer.parseInt(dot[k], 16);
						//Debug.d(TAG, "buf["+((i+64)*8+k)+"]="+buf[(i+64)*8+k]);
					}
				}
				
				/*read space line*/
				s=mReader.readLine();
				/*read P4 head*/
				s=mReader.readLine();
				if(s==null || !s.startsWith("P4"))
					return;
				/*read P4 content*/
				
				//Debug.d(TAG,"********P4*********");
				for(int i=0;i<32;i++)
				{
					s=mReader.readLine();
					if(s==null)
						return;
					dot = s.split("  ");
					for(int k=0;k<8 && k<dot.length; k++)
					{
						dot[k]=dot[k].trim();
						buf[(i+96)*8+k] = Integer.parseInt(dot[k], 16);
						//Debug.d(TAG, "buf["+((i+96)*8+k)+"]="+buf[(i+96)*8+k]);
					}
				}
			}
			mReader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Debug.d(TAG, "e="+e.getMessage());
		}
		
	}
	
	
	public int getColumns()
	{
		int columns=0;
		String s;
		try{
			mReader = new BufferedReader(new FileReader(mfile));
			s = mReader.readLine();
			s = s.trim().substring(2).trim();
			String head[] = s.split("x");
			if (head == null) {
				return 0;
			}
			columns = Integer.parseInt(head[0]);
			//Debug.d(TAG, "##################columns = "+columns);
			mReader.close();
		}catch(Exception e)
		{}
		return columns;
	}
}
