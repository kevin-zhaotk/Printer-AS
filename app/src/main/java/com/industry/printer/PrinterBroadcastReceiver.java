package com.industry.printer;

import java.util.ArrayList;
import java.util.Vector;

import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;

//import android.os.SystemProperties;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Handler;
import android.util.Log;

public class PrinterBroadcastReceiver extends BroadcastReceiver {
	public static final String TAG="PrinterBroadcastReceiver";
	public static final String BOOT_COMPLETED="android.intent.action.BOOT_COMPLETED";
	
	public static boolean mUsbAlive=false;
	public Handler mCallback;
	public static int mUsbAttached;
	
	public PrinterBroadcastReceiver(Handler callback) {
		mCallback = callback;
		mUsbAttached = 0;
	}
	
	@Override
	public void onReceive(Context context, final Intent intent) {
		// TODO Auto-generated method stub
		boolean isSerial=false;
		
		Debug.d(TAG, "--->action="+intent.getAction());
		if(intent.getAction().equals(BOOT_COMPLETED))
		{
			ArrayList<String> st = ConfigPath.updateMountedUsb();
			mUsbAttached = st.size();
			Debug.d(TAG, "--->boot usbStorage: " + mUsbAttached);
			return;
		}
		/*
		//else if(intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED))
		else if(intent.getAction().equals(Intent.ACTION_MEDIA_MOUNTED))
		{
			Debug.d(TAG, "new usb device attached");
			ArrayList<String> usbs = ConfigPath.updateMountedUsb();
			if (mUsbAttached == 0 && usbs.size() == 1) {
				mCallback.sendEmptyMessage(MainActivity.USB_STORAGE_ATTACHED);
			}
			mUsbAttached = usbs.size();
		}
		//else if(intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_DETACHED))
		else if(intent.getAction().equals(Intent.ACTION_MEDIA_UNMOUNTED))
		{
			Debug.d(TAG, "usb disconnected");
			ArrayList<String> usbs = ConfigPath.updateMountedUsb();
			mUsbAttached = usbs.size();
			Debug.d(TAG, "--->detach usbStorage: " + mUsbAttached);
		}
		*/
		if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
			Debug.d(TAG, "--->receive connectivity action");
			ConnectivityManager manager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo activeNetwork = manager.getActiveNetworkInfo();
			if (activeNetwork != null && activeNetwork.isConnected()) {
				// Ethernet connected	
				mCallback.sendEmptyMessage(MainActivity.NET_CONNECTED);
			} else {
				// Ethernet disconnected
				mCallback.sendEmptyMessage(MainActivity.NET_DISCONNECTED);
			}
			return;	
		}
		new Handler().postDelayed(new Runnable() {
			
			@Override
			public void run() {
				if(intent.getAction().equals(Intent.ACTION_MEDIA_MOUNTED))
				{
					Debug.d(TAG, "new usb device attached");
					ArrayList<String> usbs = ConfigPath.updateMountedUsb();
//					if (PlatformInfo.PRODUCT_FRIENDLY_4412.equals(PlatformInfo.getProduct())) {
//						mCallback.sendEmptyMessage(MainActivity.USB_STORAGE_ATTACHED);
//					} else if (PlatformInfo.PRODUCT_SMFY_SUPER3.equals(PlatformInfo.getProduct())) {
//						// if (mUsbAttached == 0 && usbs.size() == 1) {
//							mCallback.sendEmptyMessage(MainActivity.USB_STORAGE_ATTACHED);
//						// }
//					}
					mUsbAttached = usbs.size();
				}
				//else if(intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_DETACHED))
				else if(intent.getAction().equals(Intent.ACTION_MEDIA_UNMOUNTED))
				{
					Debug.d(TAG, "usb disconnected");
					ArrayList<String> usbs = ConfigPath.updateMountedUsb();
					mUsbAttached = usbs.size();
					Debug.d(TAG, "--->detach usbStorage: " + mUsbAttached);
				}
			}
		}, 3000);
	}

}
