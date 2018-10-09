package com.industry.printer.data;

import android.content.Context;

public class BinFromDotMatrix extends BinCreater {

	public Context mContext;
	
	public BinFromDotMatrix(Context context) {
		mContext = context;
		mHeight = 32;
	}
	
	@Override
	public int extract(String text) {
		InternalCodeCalculater cal = InternalCodeCalculater.getInstance();
		char[] code = cal.getGBKCode(text);
		DotMatrixReader reader = DotMatrixReader.getInstance(mContext);
		mBinBits = reader.getDotMatrix(code);
		return reader.getDotCount(mBinBits);
	}
	
	
}
