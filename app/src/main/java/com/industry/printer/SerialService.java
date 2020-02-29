package com.industry.printer;

import com.industry.printer.Utils.Debug;
import com.industry.printer.hardware.SerialController;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

@Deprecated
public class SerialService extends Service {

	private static final String TAG = SerialService.class.getSimpleName();
	
	private boolean mRunning = false;
	private SerialController mController;
	
	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override  
    public void onCreate() {  
        Debug.d(TAG, "ServiceDemo onCreate");  
        super.onCreate();  
    }  
      
    @Override  
    public void onStart(Intent intent, int startId) {  
        Debug.d(TAG, "ServiceDemo onStart");  
        super.onStart(intent, startId);  
    }  
      
    @Override  
    public int onStartCommand(Intent intent, int flags, int startId) {  
        Debug.d(TAG, "ServiceDemo onStartCommand");  
        return super.onStartCommand(intent, flags, startId);  
    }
    
    @Override
    public boolean onUnbind(Intent intent) {
    	Debug.d(TAG, "onUnbind");
    	mRunning = false;
    	if (mThread != null) {
    		Thread t = mThread;
    		mThread = null;
    		if (t != null) {
    			t.interrupt();
			}
    	}
    	
        return super.onUnbind(intent);
    }
    
    private Thread mThread = new Thread() {
    	
    	@Override
    	public void run() {
    		mController = new SerialController();
    		while(mRunning){
    			mController.read();
    		}
    	}
    };
    
    
}
