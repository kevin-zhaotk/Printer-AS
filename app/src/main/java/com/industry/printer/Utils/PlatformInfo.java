package com.industry.printer.Utils;

import java.lang.reflect.Method;
import java.security.PublicKey;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;


//import android.os.SystemProperties;
/**
 * 系统属性调用SystemProperties是隐藏的{hide}无法直接调用
 * 因此通过类反射机制调用
 * 
 * 各个产品区分方式：
 * 1. 树莓32位系统： (mProduct = PRODUCT_SMFY_SUPER3) + fpga-sunxi-32.ko
 * 2. 树莓HP系统： (mProduct = PRODUCT_SMFY_SUPER3) + fpga-sunxi-hp.ko
 * 3. 4412系统： mProduct = 
 * @author zhaotongkai
 *
 */

public class PlatformInfo {

	private static final String TAG = PlatformInfo.class.getSimpleName();
	
	private static final String PROPERTY_PRODUCT = "ro.build.product";
	
	public static final String PRODUCT_SMFY_SUPER3 = "smfy_super3";
	public static final String PRODUCT_3INCH = "3inch";
	public static final String PRODUCT_7INCH = "7inch";
	public static final String PRODUCT_FRIENDLY_4412 = "tiny4412";
	
	/**
	 * The Serial Used for RFID device
	 * 4412平台: /dev/ttySAC2
	 * 树莓平台： /dev/ttyS3 
	 */
	private static final String RFID_SERIAL_4412 = "/dev/ttySAC3";
	private static final String RFID_SERIAL_SMFY = "/dev/ttyS3";
	
	private static final String GRAFT_SERIAL_4412 = "/dev/ttySAC2";
	private static final String GRAFT_SERIAL_SMFY = "/dev/ttyS2";
	
	/**
	 * usb mount paths
	 */
	// 4412
	public static final String USB_MOUNT_PATH_4412 = "/storage/usbdisk";
	// smfy
	public static final String USB_MOUNT_PATH_SMFY = "/mnt/usb";
	
	/* 大屏全編輯 */
	public static final int LARGE_SCREEN = 1;
	/* 小屏部分編輯 */
	public static final int SMALL_SCREEN_PART = 2;
	/* 小屏全編輯 - 按鈕合併 */
	public static final int SMALL_SCREEN_FULL = 3;
	
	/* 通過點陣字庫獲取buffer */
	public static int DotMatrixType = 0;
	
	/* 通过该常量来区分硬件平台 */
	private static String mProduct = PRODUCT_SMFY_SUPER3;
	
	public static void init() {
		// mProduct = getProduct();
	}
	
	/**
	 * 判断buffer获取方式，通过BMP图片提取或者点阵字库提取
	 * 目前支持点阵字库提取的设备为：树莓3
	 * 其他设备都是通过BMP提取
	 * @return
	 */
	public static final int isBufferFromDotMatrix() {	
		
		return DotMatrixType;
	}
	
	public static String getProduct() {
		// return SystemProperties.get(PROPERTY_PRODUCT);
		//String product = null;
		if(!StringUtil.isEmpty(mProduct)) {
			return mProduct;
		}
		try {
			Class<?> mClassType = Class.forName("android.os.SystemProperties");
			Method mGetMethod = mClassType.getDeclaredMethod("get", String.class);
			mProduct = (String) mGetMethod.invoke(mClassType, PROPERTY_PRODUCT);
		} catch (Exception e) {
			Debug.d(TAG, "Exception: " + e.getMessage());
		}
		Debug.d(TAG, "===>product: " + mProduct);
		return mProduct;
	}
	
	public static boolean isFriendlyProduct() {
		
		if (PRODUCT_FRIENDLY_4412.equalsIgnoreCase(mProduct)) {
			return true;
		}
		return false;
	}
	
	public static boolean isSmfyProduct() {
		
		if (PRODUCT_SMFY_SUPER3.equalsIgnoreCase(mProduct)) {
			return true;
		}
		return false;
	}
	
	/**
	 * RFID device connected Serial Port
	 */
	public static String getRfidDevice() {
		if (isFriendlyProduct()) {
			return RFID_SERIAL_4412;
		} else if (isSmfyProduct()) {
			return RFID_SERIAL_SMFY;
		} else {
			Debug.d(TAG, "unsupported platform right now");
		}
		return null;
	}
	
	/**
	 * Graft device connected Serial Port
	 */
	public static String getGraftDevice() {
		if (isFriendlyProduct()) {
			return GRAFT_SERIAL_4412;
		} else if (isSmfyProduct()) {
			return GRAFT_SERIAL_SMFY;
		} else {
			Debug.d(TAG, "unsupported platform right now");
		}
		return null;
	}
	
	/**
	 * usb storage device mounted path
	 * @return
	 */
	public static String getMntPath() {
		if (isFriendlyProduct()) {
			return USB_MOUNT_PATH_4412;
		} else if (isSmfyProduct()) {
			return USB_MOUNT_PATH_SMFY;
		} else {
			Debug.d(TAG, "unsupported platform right now");
			Debug.d(TAG, "use 4412 as default");
		}
		return USB_MOUNT_PATH_4412;
	}
	
	public static int getEditType() {
		return SMALL_SCREEN_FULL;
	}
	public static int  SetDotMatrixType(int Type) {
		DotMatrixType=Type;
		 return 0;
		 
	}		
	
	/** 
     * 获取版本名 
     *  
     * @param context 
     * @return 获取失败则返回null 
     */  
    public static String getVersionName(Context context) {  
        // 包管理者  
        PackageManager mg = context.getPackageManager();  
        try {  
            // getPackageInfo(packageName 包名, flags 标志位（表示要获取什么数据）);  
            // 0表示获取基本数据  
            PackageInfo info = mg.getPackageInfo(context.getPackageName(), 0);  
            return info.versionName;  
        } catch (NameNotFoundException e) {  
            e.printStackTrace();  
        }  
        return null;  
    }  
    /** 
     * 获取版本号 
     *  
     * @param context 
     * @return 获取失败则返回0 
     */  
    public static int getVersionCode(Context context) {  
        // 包管理者  
        PackageManager mg = context.getPackageManager();  
        try {  
            // getPackageInfo(packageName 包名, flags 标志位（表示要获取什么数据）);  
            // 0表示获取基本数据  
            PackageInfo info = mg.getPackageInfo(context.getPackageName(), 0);  
            return info.versionCode;  
        } catch (NameNotFoundException e) {  
            e.printStackTrace();  
        }  
        return 0;  
    }  
}
