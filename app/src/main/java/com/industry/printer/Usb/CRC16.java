package com.industry.printer.Usb;

import android.util.Log;

public class CRC16 {
	public static String TAG="CRC16";
	public static int CRC16_BASE = 0xA001;
	
	public static short[] crc(short data[])
	{
		int i=0;
		int xdabit;
		short []crcData;
		int CRC_reg = 0xFFFF;
		for(i=1; i<data.length-3; i++)
		{
			CRC_reg ^= data[i];
			for(int j=0; j<8; j++)
			{
				xdabit =( int )(CRC_reg & 0x01) ;         
				CRC_reg >>= 1 ; 
				if( xdabit== 0x01)
				{
					CRC_reg ^= CRC16_BASE;
				}
				
			}
		}
		//Log.d(TAG, "CRC_reg = "+Integer.toHexString(CRC_reg)+",i="+i);
		data[i++] = (short) (CRC_reg& 0x00FF);
		data[i] =  (short) ((CRC_reg>>8) & 0x00FF);
		crcData = new short[data.length/2+1];
		for(i=0; i<data.length; i++)
		{
			if(data.length - i >= 2)		
			{
				//Log.d(TAG, "data["+i+"]="+Integer.toHexString(data[i]));
				//Log.d(TAG, "data["+(i+1)+"]="+Integer.toHexString(data[i+1]));
				crcData[i/2] = (short) (data[i] | data[i+1]<<8);
				//Log.d(TAG, "crcData["+i/2+"]="+Integer.toHexString(crcData[i/2]));
			}
			else
			{
				crcData[i/2] = data[i];
				//Log.d(TAG, "data["+i+"]="+Integer.toHexString(data[i]));
			}
			
			i++;
		}
		return crcData;
	}
	
	
	
	 public static int[] crc(int[] data){
		  int[] temdata=new int[data.length+2];
		       
		   int xda , xdapoly ;           
		   int i,j, xdabit ;            
		   xda = 0xFFFF ;  
		  xdapoly = 0xA001 ; // (X**16 + X**15 + X**2 + 1)   
		  for(i=0;i<data.length;i++)         
		  {         
		  xda ^= data[i] ; 
		  for(j=0;j<8;j++) {         
		   xdabit =( int )(xda & 0x01) ;         
		   xda >>= 1 ; 
		  if( xdabit==1 ) 
		   xda ^= xdapoly ; 
		   }        
		  }    
		  System.arraycopy(data, 0, temdata, 0, data.length);
		  temdata[temdata.length-2] = (int)(xda & 0xFF) ;           
		  temdata[temdata.length-1] = (int)(xda>>8) ;     
		  return temdata;
		  }
}
