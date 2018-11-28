package com.printer.corelib;

public class Paramter {

	private static final String TAG = Paramter.class.getSimpleName();
	
	public static Paramter mInstance;
	
	public int mFPGAParam[] = new int[24];
	
	public static Paramter getInstance() {
		if (mInstance == null) {
			mInstance = new Paramter();
		}
		return mInstance;
	}
	
	public void paramTrans(int[] param, int feature4, int feature5, int heads) {

		if(param[0] <= 0) {
			param[0] = 1;
		}
		if (param[2] <= 0) {
			param[2] = 150;
		}
		if (param[8] <= 0) {
			param[8] = 1;
		}
		// S16
		mFPGAParam[15] = param[2]/150;
				
		
		// S5
		mFPGAParam[4] = 170000/(param[0]*mFPGAParam[15]);
		if (mFPGAParam[4] > 65535) {
			mFPGAParam[4] = 65535;
		} else if (mFPGAParam[4] < 10) {
			mFPGAParam[4] = 10;
		}
		
		// S7
		mFPGAParam[6] = (int) (param[9]*25.4/(param[8] *3.14)/param[2] + 0.5);
		if (mFPGAParam[6] < 1 || mFPGAParam[6] > 200) {
			mFPGAParam[6] = 1;
		}
		// S4
		mFPGAParam[3] = param[3] * mFPGAParam[15] * 6 * mFPGAParam[4]/1000;
		if (mFPGAParam[3] <= 2) {
			mFPGAParam[3] = 3;
		} else if (mFPGAParam[3] >= 65535) {
			mFPGAParam[3] = 65534;
		}
		
		// S9
		// mFPGAParam[8] = (int) (param[3]*param[9]/(param[8]*3.14) * mFPGAParam[15]);
		mFPGAParam[8] = (int) (param[3]*param[9]/(param[8]*3.14));
		if (mFPGAParam[ 8] <= 10) {
			mFPGAParam[8] = 11;
		} else if (mFPGAParam[8] >= 65535) {
			mFPGAParam[8] = 65534;
		}
		// S1
		if (param[4] == 0 || param[4] == 1) {
			mFPGAParam[0] = 0;
		} else if (param[4] == 2) {
			mFPGAParam[0] = 1;
		}
		
		// S2
		if (param[4] == 0 && param[5] == 0) {
			mFPGAParam[1] = 4;
		} else if (param[4] == 0 && param[5] == 1) {
			mFPGAParam[1] = 3;
		} else if (param[4] != 0 && param[5] == 0) {
			mFPGAParam[1] = 2;
		} else if (param[4] != 0 && param[5] == 1) {
			mFPGAParam[1] = 1;
		}
		// S6
		mFPGAParam[5] = param[6] * mFPGAParam[15] * 6 * mFPGAParam[4]/1000;
		if (mFPGAParam[5] < 3) {
			mFPGAParam[5] = 3;
		} else if (mFPGAParam[5] > 65534) {
			mFPGAParam[5] = 65534;
		}
		
		// S8
		mFPGAParam[7] = (int) (param[6]*param[9]/(param[8]*3.14));
		if (mFPGAParam[7] < 11) {
			mFPGAParam[7] = 11;
		} else if (mFPGAParam[7] > 65534) {
			mFPGAParam[7] = 65534;
		}
		
		// S18
		if (param[7] == 0) {
			mFPGAParam[17] = mFPGAParam[17] | 0x10;
		} else if (param[7] == 1) {
			mFPGAParam[17] = mFPGAParam[17] & 0xef;
		}
		
		if (param[14] == 0) {
			mFPGAParam[17] = mFPGAParam[17] & 0xfe;
		} else if (param[14] == 1) {
			mFPGAParam[17] = mFPGAParam[17] | 0x01;
		}

		if (param[15] == 0) {
			mFPGAParam[17] = mFPGAParam[17] & 0xfd;
		} else if (param[15] == 1) {
			mFPGAParam[17] = mFPGAParam[17] | 0x02;
		}

		// S17
		Debug.d(TAG, "--->heads=" + heads + ", " + (mFPGAParam[16] & 0xe7f));
		if (heads == 1) {
			mFPGAParam[16] = mFPGAParam[16] & 0xe7f;
		} else if (heads == 2) {
			mFPGAParam[16] = mFPGAParam[16] & 0xe7f;
			mFPGAParam[16] = mFPGAParam[16] | 0x080;
		} else if (heads == 3) {
			mFPGAParam[16] = mFPGAParam[16] & 0xe7f;
			mFPGAParam[16] = mFPGAParam[16] | 0x100;
		} else if (heads == 4) {
			mFPGAParam[16] = mFPGAParam[16] & 0xe7f;
			mFPGAParam[16] = mFPGAParam[16] | 0x180;
		}
		Debug.d(TAG, "--->param[16]=" + mFPGAParam[16]);
		// S23
		if (param[22] == 0) {
			mFPGAParam[17] = mFPGAParam[17] & 0xfb;
		} else if (param[22] == 1) {
			mFPGAParam[17] = mFPGAParam[17] | 0x04;
		}
		// S24
	    if (param[23] == 0) {
			mFPGAParam[17] = mFPGAParam[17] & 0xf7;
		} else if (param[23] == 1) {
			mFPGAParam[17] = mFPGAParam[17] | 0x08;
		}
	    // S25
	    int feature = 0;
	    if (param[24] == 0) {
			feature = param[25];
		} else if (param[24] == 1) {
//			RFIDManager manager = RFIDManager.getInstance(mContext);
//			RFIDDevice device = manager.getDevice(0);
//			if (device == null || !device.isReady()) {
//				feature = 50;	//如果參數不合法就按默認值
//			} else {
			feature = feature4;
		}
	    if (feature < 0 || feature > 118) {
			feature = 118;
		}
	    mFPGAParam[18] = feature;
	    // 参数27
	    
	    //RFID特征值6
	    int info = 17;
	    // 参数28
	    if (param[26] == 0) {
	    	info = param[27];
//	    	mFPGAParam[16] = mFPGAParam[16] & 0xff8f;
//			mFPGAParam[16] = mFPGAParam[16] | ((param[27]-17) << 4);
//			Debug.d(TAG, "--->param=" + ((param[27]-17) << 4));
//			Debug.d(TAG, "--->fpgaparam=" + mFPGAParam[16]);
		} else if (param[26] == 1) {
//			RFIDManager manager = RFIDManager.getInstance(mContext);
//			RFIDDevice device = manager.getDevice(0);
//			if (device == null || !device.isReady()) {
//				info = 17;	//如果參數不合法就按默認值
//			} else {
			info = feature5;
				
		}
	    if (info > 24 || info < 17) {
			info = 17;
		}
	    mFPGAParam[16] = mFPGAParam[16] & 0xff8f;
	    mFPGAParam[16] = mFPGAParam[16] | ((info-17) << 4);
	    
	    // S20
	    mFPGAParam[19] = param[28];
	    // 参数30
	    mFPGAParam[2] = param[29];
	    for (int i = 0; i < mFPGAParam.length; i++) {
			Debug.e(TAG, "--->mFPGAParam[" + i + "]=" + mFPGAParam[i]);
		}
	    mFPGAParam[13] = (int) ((param[9] * 25.4 * 128)/(param[8] * 3.14)/param[2]);
	    mFPGAParam[14] = param[34];
	    mFPGAParam[20] = param[36];
	    mFPGAParam[21] = param[33];
	    mFPGAParam[22] = param[32];
	    mFPGAParam[23] = param[35];
	}
	
	public int getFPGAParam(int index) {
		if (index >= mFPGAParam.length) {
			return 0;
		}
		return mFPGAParam[index];
	}
}
