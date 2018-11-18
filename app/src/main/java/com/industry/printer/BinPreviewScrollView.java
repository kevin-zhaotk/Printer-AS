package com.industry.printer;
import com.industry.printer.Utils.Debug;

import android.content.Context;
import android.view.View;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;


public class BinPreviewScrollView extends View {

	public static final String TAG="BinPreviewScrollView";
	public Paint mPaint=null;
	
	public static Bitmap	mPreBitmap;
	
	public BinPreviewScrollView(Context context) {
		super(context);
		mPaint = new Paint();
	}

	public BinPreviewScrollView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mPaint = new Paint();
		// TODO Auto-generated constructor stub
	}
	
	public BinPreviewScrollView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mPaint = new Paint();
		// TODO Auto-generated constructor stub
	}
	public void createBitmap(Bitmap bmp)
	{
		mPreBitmap = bmp;
	}
	
	@Override  
	 protected void onDraw(Canvas canvas) {
		Debug.d(TAG, "====>onDraw");
		if(mPreBitmap != null)			
			canvas.drawBitmap(mPreBitmap, 0, 0, mPaint);
		
	}
}
