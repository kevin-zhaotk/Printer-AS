package com.industry.printer.Utils;


import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Environment;
import android.provider.ContactsContract.Directory;
import android.view.ViewDebug.FlagToString;

import com.industry.printer.R;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.data.SegmentBuffer;

public class Configs {
	
	public static final boolean DEBUG=true;

	/**
	 * 该版本打印是否需要忽略rfid
	 * if RFID is ignored, RFID scan  at start-time repeats upto 10 times if scan failed;
	 * set Rfid value to 370/2 if Rfid is missing;
	 * skip UID checking after 'print' command triggered before data transfer;
	 */
	public static final boolean IGNORE_RFID = false;

	
	/**
	 * 开关配置：大字机的特殊版本，buffer宽度x8
	 */
	public static final boolean BUFFER_8 = false;
	
	
	/** 每列的有效点阵数 **/
	public static int gDots;
	/** 每列的总字节数 **/
	public static int gDotsTotal;
	
	public static int gBytesPerColumn;
	public static int gCharsPerColumn;
	public static int gFixedRows;
	
	public static int gParams;
	
	public static boolean mTextEnable	=false;
	public static boolean mCounterEnable=false;
	public static boolean mRTYearEnable =false;
	public static boolean mRTMonThEnable=false;
	public static boolean mRTDateEnable=false;
	public static boolean mRTHourEnable=false;
	public static boolean mRTMinuteEnable=false;
	public static boolean mRTEnable=false;
	public static boolean mShiftEnable=false;
	public static boolean mDLYearEnable=false;
	public static boolean mDLMonthEnable=false;
	public static boolean mDLDateEnable=false;
	public static boolean mJulianEnable=false;
	public static boolean mGraphicEnable=false;
	public static boolean mBarcodeEnable=false;
	public static boolean mLineEnable=false;
	public static boolean mRectEnable=false;
	public static boolean mEllipseEnable=false;
	public static boolean mRTSecondEnable=false;
	public static boolean mQREnable=false;
	public static boolean mWeekDayEnable=false;
	public static boolean mWeeksEnable=false;
	
	
	public static final boolean gMakeBinFromBitmap = false;


	public static final Bitmap.Config BITMAP_CONFIG = Bitmap.Config.ARGB_4444;
	public static final Bitmap.Config BITMAP_PRE_CONFIG = Bitmap.Config.ARGB_4444;
	
	/**
	 * 一个单位的墨水量对应的打点数
	 */
	public static final int DOTS_PER_PRINT = 6018000;
	
	/**
	 * 墨盒总墨水量
	 */
	public static final int INK_LEVEL_MAX = 100000;

	/**
	 * PROC_MOUNT_FILE
	 * proc/mounts sys file
	 */
	public static final String PROC_MOUNT_FILE = "/proc/mounts";
	/**
	 * USB_ROOT_PATH
	 * 	usb mount root path on this platform
	 */
	public static final String USB_ROOT_PATH = "/mnt/usbhost0";
	
	/**
	 * LOCAL_ROOT_PATH
	 * 	the path local objects store at
	 */
	public static final String LOCAL_ROOT_PATH = "/data/TLK";
	
	/**
	 * LOCAL_ROOT_PATH
	 * 	the path local objects store at
	 */
	public static final String USB_ROOT_PATH2 = "/mnt/usbhost1";
	
	/**
	 * SDCARD_ROOT_PATH
	 * 	sd-card mount root path on this platform
	 */
	public static final String SDCARD_ROOT_PATH = "/storage/sd_external";
	
	/**
	 * System config
	 */
	public static  final String CONFIG_PATH_FLASH = "/mnt/sdcard";
	
	/**
	 * TLK_FILE_NAME
	 * tlk file
	 */
	public static final String TLK_FILE_NAME = "/1.TLK";

	/**
	 * BIN_FILE_NAME
	 * Bin file
	 */
	public static final String BIN_FILE_NAME = "/1.bin";
	
	/**
	 * 16*16的点阵字库，用于提取点阵图
	 */
	public static final String FONT_16_16 = "/16*16.zk";
	
	public static final String UPGRADE_APK_FILE = "/Printer.apk";
	/**
	 * SYSTEM_CONFIG_FILE
	 */
	public static final String SYSTEM_CONFIG_DIR = "/system";
	public static final String SYSTEM_CONFIG_FILE = SYSTEM_CONFIG_DIR + "/system_config.txt";
	public static final String SYSTEM_CONFIG_XML = SYSTEM_CONFIG_DIR + "/system_config.xml";
	public static final String LAST_MESSAGE_XML = SYSTEM_CONFIG_DIR + "/last_message.xml";
	public static final String FONT_METRIC_PATH = SYSTEM_CONFIG_DIR + "/ZK";
	
	/**
	 * 
	 */
	public static final String QR_DATA = CONFIG_PATH_FLASH + SYSTEM_CONFIG_DIR + "/QRdata/QR.txt";
	public static final String QR_CSV = CONFIG_PATH_FLASH + SYSTEM_CONFIG_DIR + "/QRdata/QR.csv";
	public static final String QR_LAST = CONFIG_PATH_FLASH + SYSTEM_CONFIG_DIR + "/QRdata/QRlast.txt";
	public static final String QR_DIR = "/QRdata";
	
	public static final String SYSTEM_CONFIG_MSG_PATH = "/MSG1";
	/**
	 * 用户pc端编辑的文本存放路径，编辑打印对象的内容时可以从这个目录加载，而不需要手动输入 
	 */
	public static final String TXT_FILES_PATH = SYSTEM_CONFIG_DIR + "/txt";
	
	/**
	 * TLK文件存放路径
	 */
	public static final String TLK_FILE_SUB_PATH = SYSTEM_CONFIG_MSG_PATH;
	
	/**
	 * picture path
	 */
	public static final String PICTURE_SUB_PATH = "/pictures";
	
	
	/**
	 * TLK path on flash
	 */
	public static String Devic_Number_Path; //device numb path;
	public static String Devic_File_Path; //device numb path;
	public static String Device_Printer_Path;//print File path;
	
	public static  final String TLK_PATH_FLASH = CONFIG_PATH_FLASH + "/MSG";
	
	public static final String FONT_DIR = CONFIG_PATH_FLASH + "/fonts";
	
	public static final String FONT_ZIP_FILE = "Well.Ftt";
	
	public static final String FONT_DIR_USB = "/ft";
	
	public static SystemConfigFile mSysconfig;
	/**
	 * initConfigs initiallize the system configs,such as dots and fixed rows 
	 * @param context
	 */
	public static void initConfigs(Context context)
	{
		gDots = context.getResources().getInteger(R.integer.dots_per_column);
		gDotsTotal = context.getResources().getInteger(R.integer.dots_per_column_total);
		gBytesPerColumn = context.getResources().getInteger(R.integer.bytes_per_column);
		gCharsPerColumn = context.getResources().getInteger(R.integer.chars_per_column);
		gFixedRows = context.getResources().getInteger(R.integer.fixed_rows);
		gParams = context.getResources().getInteger(R.integer.total_params);
		Debug.d("", "--->gdots=" + gDots);
		ConfigPath.updateMountedUsb();
		//如果需要，在u盘根目录创建系统所需的目录，当u盘插入是也需要调用
		ConfigPath.makeSysDirsIfNeed();
		/*从U盘中读取系统设置，解析*/
		mSysconfig = SystemConfigFile.getInstance(context);
		mSysconfig.init();
		/*读设备号*/
		PlatformInfo.init();
	}
	
	/**
	 * isRootpath - check whether the path specified by parameter path is root path
	 * @param path the path to check
	 * @return true if path is root path, false otherwise
	 */
	public static boolean isRootpath(String path)
	{
		if(LOCAL_ROOT_PATH.equals(path) || USB_ROOT_PATH.equals(path) || SDCARD_ROOT_PATH.equals(path))
			return true;
		else
			return false;
	}
	
	
	/**
	 * 鑾峰彇u鐩樻寕杞借矾寰�
	 * @return usb root path
	 */
	public static String getUsbPath() {
		return USB_ROOT_PATH;
	}
	
	public static void objectsConfig(Context context) {
		String id="";
		int flag=0;
		try {
		InputStream stream = context.getResources().openRawResource(R.raw.objectconfig);
		XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
		XmlPullParser parser = factory.newPullParser();
		parser.setInput(stream, "UTF-8");
		
		int eventType = parser.getEventType();  
        while (eventType != XmlPullParser.END_DOCUMENT) {  
            switch (eventType) {  
            case XmlPullParser.START_DOCUMENT:  
                break;
            case XmlPullParser.START_TAG:
            	if (parser.getName().equals("object")) {
					parser.next();
					if (parser.getName().equals("id")) {
						id = parser.getText();
					} else if (parser.getName().equals("flag")) {
						flag = Integer.parseInt(parser.getText());
						setObjectFlag(id, flag);
					}
					parser.next();
				} else {
					parser.next();
				}
            	break;
            case XmlPullParser.END_TAG:
            	break;
            }
        }
		} catch (Exception e) {
		}
	}
	
	private static void setObjectFlag(String id, int flag) {
		boolean enable = false;
		if ( flag > 0) {
			enable = true;
		}
		if (id == null) {
			return;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_TEXT)) {
			mTextEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_CNT)) {
			mCounterEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RT_YEAR)) {
			mRTYearEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RT_MON)) {
			mRTMonThEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RT_DATE)) {
			mRTDateEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RT_HOUR)) {
			mRTHourEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RT_MIN)) {
			mRTMinuteEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_SHIFT)) {
			mRTSecondEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_DL_YEAR)) {
			mDLYearEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_DL_MON)) {
			mDLMonthEnable= enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_DL_DATE)) {
			mDLDateEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_JULIAN)) {
			mJulianEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_GRAPHIC)) {
			mGraphicEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_BARCODE)) {
			mBarcodeEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_LINE)) {
			mLineEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RECT)) {
			mRectEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_ELLIPSE)) {
			mEllipseEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_RT_SECOND)) {
			mRTSecondEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_QR)) {
			mQREnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_WEEKDAY)) {
			mWeekDayEnable = enable;
		} else if (id.equalsIgnoreCase(BaseObject.OBJECT_TYPE_WEEKS)) {
			mWeeksEnable = enable;
		}
	}
	
	
	public static int getMessageDir(int index) {
		int dir=0;
		switch (index) {
		case 0:
			dir = mSysconfig.getParam(12);
			break;
		case 1:
			dir = mSysconfig.getParam(13);
			break;
		case 2:
			dir = mSysconfig.getParam(20);
			break;
		case 3:
			dir = mSysconfig.getParam(21);
			break;
		default:
			break;
		}
		if (dir != SegmentBuffer.DIRECTION_NORMAL && dir != SegmentBuffer.DIRECTION_REVERS) {
			dir = SegmentBuffer.DIRECTION_NORMAL;
		}
		if (mSysconfig.getParam(1) == 1) {
			if (dir == SegmentBuffer.DIRECTION_NORMAL) {
				dir = SegmentBuffer.DIRECTION_REVERS;
			} else {
				dir = SegmentBuffer.DIRECTION_NORMAL;
			}
		}
		return dir;
	}
	
	public static int getMessageShift(int index) {
		int shift=0;
		switch (index) {
		case 0:
			shift = mSysconfig.getParam(10);
			break;
		case 1:
			shift = mSysconfig.getParam(11);
			break;
		case 2:
			shift = mSysconfig.getParam(18);
			break;
		case 3:
			shift = mSysconfig.getParam(19);
			break;
		default:
			break;
		}
		
		return shift;
	}
	
	/**
	 * 设置参数53表示32×Nbuffer的双数bit位移值
	 * @return
	 */
	public static int getEvenShift() {
		return mSysconfig.getParam(52);
	}
}
