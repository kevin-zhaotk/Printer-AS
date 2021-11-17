package com.industry.printer.ui.CustomerDialog;

import android.R.menu;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Message;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.industry.printer.R;
import com.industry.printer.object.BaseObject;

public class ObjectInsertDialog extends Dialog implements android.view.View.OnClickListener {

	public static final String OBJECT_TYPE = "ObjType";
	public static final String OBJECT_FORMAT = "ObjFormat";
	public static final String OBJECT_SOURCE = "ObjSource";
	
	private Context 	mContext;
	public ImageView mText;
	public ImageView 	mRTime;
	public ImageView 	mCounter;
	public ImageView	mBarcode;
	public ImageView	mBarcodeV;
	public ImageView	mJulian;
	public ImageView	mGraphic;
	public ImageView	mLine;
	public ImageView	mRect;
	public ImageView	mEllipse;
	public ImageView 	mHour;
	public ImageView 	mWoy;
	public ImageView 	mWday;
// H.M.Wang 2020-1-21 追加shift控件
	public ImageView 	mShift;
// End of H.M.Wang 2020-1-21 追加shift控件

// H.M.Wang 2020-2-16 追加HyperText控件
	public ImageView 	mHyperText;
// End of H.M.Wang 2020-2-16 追加HyperText控件

// H.M.Wang 2020-6-10 追加DynamicText控件
	public ImageView 	mDynamicText;
// End of H.M.Wang 2020-6-10 追加DynamicText控件

	public Message		mDismissMsg;
	
	public ObjectInsertDialog(Context context) {
		super(context);
		mContext = context;
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.layout_objinsert_gridview);
		
		
		mText = (ImageView)findViewById(R.id.objinsert_text);
		mText.setOnClickListener(this);
		
		mRTime = (ImageView)findViewById(R.id.objinsert_time);
		mRTime.setOnClickListener(this);
		
		mCounter = (ImageView) findViewById(R.id.objinsert_counter);
		mCounter.setOnClickListener(this);
		
		mBarcode = (ImageView) findViewById(R.id.objinsert_barcode);
		mBarcode.setOnClickListener(this);
		
		mBarcodeV = (ImageView) findViewById(R.id.objinsert_barcode_var);
		mBarcodeV.setOnClickListener(this);
		
		
		mGraphic = (ImageView) findViewById(R.id.objinsert_graphic);
		mGraphic.setOnClickListener(this);
/*		
		mLine = (Button) findViewById(R.id.objinsert_line);
		mLine.setOnClickListener(this);
		
		mRect = (Button) findViewById(R.id.objinsert_rect);
		mRect.setOnClickListener(this);
		
		mEllipse = (Button) findViewById(R.id.objinsert_ellipse);
		mEllipse.setOnClickListener(this);
*/
		mJulian = (ImageView) findViewById(R.id.objinsert_julian);
		mJulian.setOnClickListener(this);
		
		mHour = (ImageView) findViewById(R.id.objinsert_hour);
		mHour.setOnClickListener(this);
		
		mWoy = (ImageView) findViewById(R.id.objinsert_week_year);
		mWoy.setOnClickListener(this);

		mWday = (ImageView) findViewById(R.id.objinsert_week_day);
		mWday.setOnClickListener(this);

// H.M.Wang 2020-1-21 追加shift控件
		mShift = (ImageView) findViewById(R.id.objinsert_shift);
		mShift.setOnClickListener(this);
// End of H.M.Wang 2020-1-21 追加shift控件

// H.M.Wang 2020-2-16 追加HyperText控件
		mHyperText = (ImageView) findViewById(R.id.objinsert_hypertext);
		mHyperText.setOnClickListener(this);
// End of H.M.Wang 2020-2-16 追加HyperText控件

// H.M.Wang 2020-6-10 追加DynamicText控件
		mDynamicText = (ImageView) findViewById(R.id.objinsert_dynamictext);
		mDynamicText.setOnClickListener(this);
// End of H.M.Wang 2020-6-10 追加DynamicText控件
	}

	@Override
	public void onClick(View arg0) {
		Bundle bundle = new Bundle();
		switch (arg0.getId()) {
			case R.id.objinsert_text:
				bundle.putString("object", "");
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_TEXT);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_time:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_RT);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_counter:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_CNT);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_barcode:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_BARCODE);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_barcode_var:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_BARCODE);
				bundle.putBoolean(OBJECT_SOURCE, true);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_julian:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_JULIAN);
				mDismissMsg.setData(bundle);
				break;

			case R.id.objinsert_hour:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_LETTERHOUR);
				mDismissMsg.setData(bundle);
				break;
				
			case R.id.objinsert_graphic:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_GRAPHIC);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_week_year:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_WEEKOFYEAR);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_week_day:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_WEEKDAY);
				mDismissMsg.setData(bundle);
				break;
// H.M.Wang 2020-1-21 追加shift控件
			case R.id.objinsert_shift:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_SHIFT);
				mDismissMsg.setData(bundle);
				break;
// End of H.M.Wang 2020-1-21 追加shift控件
// H.M.Wang 2020-2-16 追加HyperText控件
			case R.id.objinsert_hypertext:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_HYPERTEXT);
				mDismissMsg.setData(bundle);
				break;
// End of H.M.Wang 2020-2-16 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
			case R.id.objinsert_dynamictext:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_DYN_TEXT);
				mDismissMsg.setData(bundle);
				break;
// End of H.M.Wang 2020-6-10 追加DynamicText控件

/*
			case R.id.objinsert_line:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_LINE);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_rect:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_RECT);
				mDismissMsg.setData(bundle);
				break;
			case R.id.objinsert_ellipse:
				bundle.putString(OBJECT_TYPE, BaseObject.OBJECT_TYPE_ELLIPSE);
				mDismissMsg.setData(bundle);
				break;
				*/
			default:
				break;
		}
		dismiss();
	}
	
	@Override
	public void setDismissMessage(Message msg) {
		super.setDismissMessage(msg);
		mDismissMsg = msg;
	}
}
