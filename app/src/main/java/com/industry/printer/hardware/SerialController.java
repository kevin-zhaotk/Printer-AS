package com.industry.printer.hardware;

import java.util.HashMap;
import java.util.Map;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.Rfid.RFIDData;

@Deprecated
public class SerialController {

	private final String TAG = SerialController.class.getSimpleName();
	public int fd = -1;
	public Map<String, String> mControlList; 
	
	
	public SerialController() {
		fd = RFIDDevice.open(PlatformInfo.getGraftDevice());
		mControlList = new HashMap<String, String>();
	}
	
	public void read() {
		byte[] msg = RFIDDevice.read(fd, 64);
		RFIDData data = new RFIDData(msg, true);
		parse(data);
	}
	
	public void parse(RFIDData data) {
		byte[] d = data.getData();
		mControlList.clear();
		for (int i = 0; i < d.length/2; i++) {
			Debug.d(TAG, "--->d[" + i + "]=" + d[i] + "");
			mControlList.put(String.valueOf(d[i]), "v" + String.valueOf(d[i+1]) + ".bin");
		}
		
	}
	public void close() {
		RFIDDevice.close(fd);
		fd = -1;
	}
}
