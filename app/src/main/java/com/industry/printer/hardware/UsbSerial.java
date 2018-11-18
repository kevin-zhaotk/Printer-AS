package com.industry.printer.hardware;

import java.io.ByteArrayInputStream;

import org.apache.http.util.ByteArrayBuffer;

import android.R.integer;
import android.R.string;
import android.content.SharedPreferences;
import android.util.Log;

import com.industry.printer.Usb.CRC16;
import com.industry.printer.Utils.Debug;

public class UsbSerial {
	
	public static final String TAG="UsbSerial"; 
	public static final int PACKAGE_MAX_LEN=132;
	
	/**
	 * return value for ttyACM device operation result
	 * ERR_NO_ERROR - operation success
	 * ERR_DEV_MOVED - ttyACM device not be inserted
	 * ERR_WRITE_FAILED - writing operation return failed
	 * ERR_READ_FAILED - reading operation return failed
	 * ERR_OPEN_FAILED - open ttyACM dev return failed
	 * ERR_PARAMETER_ILEGAL - input paramater ilegal
	 */
	public static final int ERR_NO_ERROR=0;
	public static final int ERR_WRITE_FAILED=-1;
	public static final int ERR_READ_FAILED	=-2;
	public static final int ERR_DEV_MOVED	=-3;
	public static final int ERR_OPEN_FAILED	=-4;
	public static final int ERR_RESPONSE_ERROR=-5;
	public static final int ERR_PARAMETER_ILEGAL=-6;
	
	static public int open(String dev) {
		return HardwareJni.open(dev);
	}
	
	static public int close(int fd) {
		return HardwareJni.close(fd);
	}
	static public int printStart(String dev)
	{
		short buf[] = { 0x80, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"====>printStart");
		short [] crcCmd = CRC16.crc(buf);
//		for(int i=0; i<crcCmd.length; i++)
//		{
//			Debug.d(TAG, "crcCmd["+i+"]="+(int) crcCmd[i]);
//		}

		int fd = HardwareJni.open(dev);
		if(fd<=0)
		{
			return ERR_OPEN_FAILED;
		}
			
		int ret = HardwareJni.write(fd, crcCmd, buf.length);
		if(ret != buf.length)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		Debug.d(TAG, "write ret="+ret);
		byte[] response = HardwareJni.read(fd, 10);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		if(response[4] != 0)
			ret = ERR_RESPONSE_ERROR;
		else 
			ret = ERR_NO_ERROR;
		HardwareJni.close(fd);
		Debug.d(TAG,"<====printStart");
		return ret;
	}
	
	
	static public int printStop(String dev)
	{
		short buf[]={0x80, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev==null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"====>printStop");
		short[] crcCmd = CRC16.crc(buf);
//		for(int i=0; i<crcCmd.length; i++)
//		{
//			Debug.d(TAG, "crcCmd["+i+"]="+(int) crcCmd[i]);
//		}
		//String buf = "FUCK serial write";
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		HardwareJni.setBaudrate(fd, 115200);
		HardwareJni.set_options(fd, 8, 1, 'n');
		int ret = HardwareJni.write(fd, crcCmd, buf.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		byte[] response = HardwareJni.read(fd, 10);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			Debug.d(TAG, "close fd");
			return ERR_READ_FAILED;
		}
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "buf["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		if(response[4] != 0)
		{
			ret = ERR_RESPONSE_ERROR;
			Debug.d(TAG, "this is response error");
		}
		else
			ret = ERR_NO_ERROR;
		Debug.d(TAG,"<====close");
		HardwareJni.close(fd);
		Debug.d(TAG,"<====printStop");
		return ret;
	}
	
	/*clean
	 *	80 00 02 04 00 00 00 00 00 00 04    
	 */
	static public int clean(String dev)
	{
		
		short buf[]={0x80, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		short[] crcCmd = CRC16.crc(buf);
		
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, buf.length);
		Debug.d(TAG, "clean return = "+ret);
		HardwareJni.close(fd);
		return ret>0?ERR_NO_ERROR:ERR_WRITE_FAILED;
	}
	
	
	static public int setAllParam(String dev, short param[])
	{
		//short buf[] = new short[132];
		//buf[0] = 0x81;
		
		short cmd[]={0x80, 0x01, 0xFF, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"====>setAllParam");
		short [] crcCmd = CRC16.crc(cmd);
		int fd = HardwareJni.open(dev);
		if(fd <= 0 )
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}

		byte[] response = HardwareJni.read(fd, PACKAGE_MAX_LEN);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		/*
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		*/
		Debug.d(TAG, "write ALL parameters");
		short buf[] = new short[132];
		buf[0] = 0x81;
		buf[131]=0x04;
		crcCmd = CRC16.crc(buf);
		/*
		for(int i=0; i<buf.length; i++)
		{
			Debug.d(TAG, "buf["+i+"]="+(int)(buf[i] & 0x0FF));
		}
		*/
		ret = HardwareJni.write(fd, crcCmd, buf.length);
		Debug.d(TAG, "write param ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}

		response = HardwareJni.read(fd, PACKAGE_MAX_LEN);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		Debug.d(TAG,"<====setAllParam");
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	/**
	 * 
	 * 0x80 	0x01		0xFE	0x04		0x00		0x00		0x00		0x00		CRC16L		CRC16H		0x04
	 **/
	static public int readAllParam(String dev)
	{
		//short buf[] = new short[132];
		//buf[0] = 0x81;
		
		short cmd[]={0x80, 0x01, 0xFE, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		short [] crcCmd = CRC16.crc(cmd);
		/*
		for(int i=0; i<crcCmd.length; i++)
		{
			Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
		}
		*/
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}

		byte[] response = HardwareJni.read(fd, PACKAGE_MAX_LEN);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		/*
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		*/
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	/**
	 * set one param
	 * 0x80 	0x01			0x04		0x00		0x00		0x00		0x02		CRC16L		CRC16H		0x04
	 **/
	static public int setParam(String dev, int pIndex)
	{
		//short buf[] = new short[132];
		//buf[0] = 0x81;
		
		short cmd[]={0x80, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		cmd[2] = (short)pIndex;
		short [] crcCmd = CRC16.crc(cmd);
		/*
		for(int i=0; i<crcCmd.length; i++)
		{
			Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
		}
		*/
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}

		Debug.d(TAG, "write ret="+ret);
		byte[] response = HardwareJni.read(fd, PACKAGE_MAX_LEN);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		/*
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		*/
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	/**
	 * read param
	 * 0x80 	0x01		0x80 	0x04		0x00		0x00		0x00		0x00		CRC16L		CRC16H		0x04
	 **/
	static public int readParam(String dev, int pIndex)
	{
		//short buf[] = new short[132];
		//buf[0] = 0x81;
		
		short cmd[]={0x80, 0x01, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		cmd[2] += pIndex;
		short [] crcCmd = CRC16.crc(cmd);
		/*
		for(int i=0; i<crcCmd.length; i++)
		{
			Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
		}
		*/
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}

		byte[] response = HardwareJni.read(fd, PACKAGE_MAX_LEN);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		/*
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		*/
		HardwareJni.close(fd);		
		return ERR_NO_ERROR;
	}
	
	
	static public int sendDataCtrl(String dev, int len)
	{
		short cmd[]={0x80, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"===>sendDataCtrl");
		
		cmd[4] = (short) (len >> 24 & 0x0ff);
		cmd[5] = (short) (len >> 16 & 0x0ff);
		cmd[6] = (short) (len >> 8 & 0x0ff);
		cmd[7] = (short) (len & 0x0ff);
		
		short [] crcCmd = CRC16.crc(cmd);
//		for(int i=0; i<crcCmd.length; i++)
//		{
//			Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
//		}
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		byte[] response = HardwareJni.read(fd, 10);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		/*for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}*/
		Debug.d(TAG,"<===sendDataCtrl");
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	/*
	 * Printing Data 
	 * sendData firstly, then printData,
	 * 
	 */
	static public int printData(String dev, byte[]data)
	{
		short cmd[];
		//short cmd[]={0x80, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		
		Debug.d(TAG,"====>printData, data len="+data.length);
		//disable CRC
		cmd = new short[data.length+4];
		cmd[0]=0x81&0x0ff;
		for(int i=0; i <data.length; i++)
		{
			cmd[i+1] = (short) (data[i]&0x0ff);
		}
		cmd[cmd.length-3] = 0x00;
		cmd[cmd.length-2] = 0x00;
		cmd[cmd.length-1] = 0x04;
		
		short [] crcCmd = CRC16.crc(cmd);
		/*
		 * the slave can only receive 2Mbytes data once
		 * so we need split the data and send several times 
		 * when the data length > 2Mbytes
		 */
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "=============printData write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		byte[] response = HardwareJni.read(fd, 10);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		Debug.d(TAG,"<====printData");
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	/*
	* Command - Send setting (0006)      
	*Send this command to inform the print head receive setting data. 
	*After send this command and get right return, start setting Data (0007)
	*/
	public static int sendSetting(String dev)
	{
		short cmd[]={0x80, 0x01, 0xff, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"====>sendSetting");
		short [] crcCmd = CRC16.crc(cmd);
		/*
		for(int i=0; i<crcCmd.length; i++)
		{
			Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
		}
		*/
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		byte[] response = HardwareJni.read(fd, 10);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		/*
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}*/
		Debug.d(TAG,"<====sendSetting");
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	
	public static int sendSettingData(String dev, byte data[])
	{
		short[] cmd = new short[132];
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"====>sendSettingData");
		cmd[0] = 0x81;
		for(int i=0; i<data.length && i < 128; i++)
		{
			cmd[i+1] = (short) (data[i]&0x0ff);
		}
		cmd[129] = 0;
		cmd[130] = 0;
		cmd[131] = 4;
		/*for(int i=0; i<cmd.length; i++)
		{
			Debug.d(TAG, "cmd["+i+"]="+Integer.toHexString(cmd[i]));
		}*/
		short [] crcCmd = CRC16.crc(cmd);
		/*for(int i=0; i<crcCmd.length; i++)
		{
			//Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
		}*/
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_OPEN_FAILED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		byte[] response = HardwareJni.read(fd, 10);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		Debug.d(TAG,"<====sendSettingData");
		HardwareJni.close(fd);
		return ERR_NO_ERROR;
	}
	
	
	static public int getInfo(String dev, byte[] info)
	{
		short cmd[]={0x80, 0x01, 0xfd, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
		if(dev == null)
		{
			Debug.d(TAG, "ttyACM0 device node not opened");
			return ERR_DEV_MOVED;
		}
		Debug.d(TAG,"====>getInfo");
		short [] crcCmd = CRC16.crc(cmd);
//		for(int i=0; i<crcCmd.length; i++)
//		{
//			Debug.d(TAG, "crcCmd["+i+"]="+Integer.toHexString(crcCmd[i]));
//		}
		int fd = HardwareJni.open(dev);
		if(fd <= 0)
			return ERR_DEV_MOVED;
		int ret = HardwareJni.write(fd, crcCmd, cmd.length);
		Debug.d(TAG, "write ret="+ret);
		if(ret<=0)
		{
			HardwareJni.close(fd);
			return ERR_WRITE_FAILED;
		}
		byte[] response = HardwareJni.read(fd, 23);
		if(response == null)
		{
			Debug.d(TAG, "read return null");
			HardwareJni.close(fd);
			return ERR_READ_FAILED;
		}
		for(int i=0; i<response.length; i++)
		{
			Debug.d(TAG, "response["+i+"]="+Integer.toHexString(response[i] & 0x0FF));
		}
		if(info == null)
		{
			HardwareJni.close(fd);
			return ERR_PARAMETER_ILEGAL;
		}
			
		ByteArrayInputStream s = new ByteArrayInputStream(response);
			
		//Debug.d(TAG, "===avaliable size="+s.available());
		if(s.read(info, 0, 23)==-1)
			ret = ERR_NO_ERROR;
		Debug.d(TAG,"<====getInfo");
		HardwareJni.close(fd);
		return ret;
	}
	
}



