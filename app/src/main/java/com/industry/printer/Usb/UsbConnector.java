package com.industry.printer.Usb;

import java.util.HashMap;
import java.util.Iterator;

import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.util.Log;

public class UsbConnector {
	
	public static final String TAG="UsbConnector";
	
	public UsbConnector(Context context)
	{
		UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
		HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
		Iterator<UsbDevice> deviceIterator = deviceList.values().iterator();
		Log.d(TAG, "===>UsbConnector");
		while(deviceIterator.hasNext()){
		    UsbDevice device = deviceIterator.next();
		    Log.d(TAG, "device="+device.getDeviceName()+","+device.getDeviceSubclass()+","+device.getDeviceProtocol());
		    Log.d(TAG, "ifs="+device.getInterfaceCount()+","+device.getInterface(0).toString());
		    Log.d(TAG, "end="+device.getInterface(0).getEndpoint(0).toString());
		    Log.d(TAG, "class="+device.getDeviceClass()+", pid="+device.getProductId()+",vid="+device.getVendorId());
		    //your code
		}
	}
}
