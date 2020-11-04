package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.R;
import com.industry.printer.R.id;
import com.industry.printer.R.string;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.object.BarcodeObject;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.DynamicText;
import com.industry.printer.object.EllipseObject;
import com.industry.printer.object.HyperTextObject;
import com.industry.printer.object.JulianDayObject;
import com.industry.printer.object.LetterHourObject;
import com.industry.printer.object.LineObject;
import com.industry.printer.object.MessageObject;
import com.industry.printer.object.RealtimeObject;
import com.industry.printer.object.RealtimeSecond;
import com.industry.printer.object.RectObject;
import com.industry.printer.object.TextObject;
import com.industry.printer.object.GraphicObject;
import com.industry.printer.object.ShiftObject;
import com.industry.printer.object.WeekDayObject;
import com.industry.printer.object.WeekOfYearObject;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter.IOnItemClickListener;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnNagitiveListener;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnPositiveListener;
import com.industry.printer.ui.Items.PictureItem;
import com.industry.printer.widget.PopWindowSpiner;

import android.app.Dialog;
import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.inputmethod.InputMethodManager;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

public class ObjectInfoDialog extends Dialog implements android.view.View.OnClickListener, IOnItemClickListener, OnCheckedChangeListener
	, OnTouchListener, TextWatcher {
	
	public static final String TAG="ObjectInfoDialog";
	public OnPositiveBtnListener mPListener;
	public OnNagitiveBtnListener mNListener;
	public onDeleteListener mDelListener;
	
	public BaseObject mObject;
	
	public TextView mXCorView;
	public TextView mXCorUnit;
	public TextView mYCorView;
	public TextView mYCorUnit;
	public TextView mWidthView;
	public TextView mWidthUnit;
	public TextView mHighView;
	public TextView mHighUnit;
	public TextView mCntView;
	public TextView mFontView;
	public TextView mRtfmtView;
	public TextView mBitsView;
	public TextView mDirectView;
	public TextView mCodeView;
	public TextView mNumshowView;
	public TextView mLineView;
	public TextView mLinetypeView;
	
	
	public EditText mWidthEdit;
	public TextView mHighEdit;
	public EditText mXcorEdit;
	public EditText mYcorEdit;
	public EditText mContent;
	public TextView mContentView;
	public TextView mFont;
	public TextView mRtFormat;
	public EditText mDigits;
	public TextView mDir;
	public TextView mCode;
// H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
	public TextView mITF14FrameCaption;
	public CheckBox mITF14Frame;
// End of H.M.Wang 2020-2-25 追加ITF_14边框有无的设置

//	private EditText mHeight_O;
	private CheckBox mHeightType;
	public CheckBox mShow;
	public EditText mLineWidth;
	public TextView mPicture; // 圖片路徑
	public EditText mOffset;
// H.M.Wang 2020-2-4 添加Shift控件的位数项目
	public TextView mShiftBits;
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目

	public EditText mShift1;
	public EditText mShiftVal1;
	public EditText mShift2;
	public EditText mShiftVal2;
	public EditText mShift3;
	public EditText mShiftVal3;
	public EditText mShift4;
	public EditText mShiftVal4;
	private EditText mCounterStart;
	private EditText mCounterEnd;
	private EditText mCntIndex;
	private EditText mCntSteplen;
	public Button	mBtnOk;
	private Button  mPageup;
	private Button  mPagedown;
	private ScrollView mScroll;
	public TextView mLineType;
	private EditText mTextsize;
	private CheckBox mReverse;

	private EditText mIndex;

	public EditText mMsg;
	public CheckBox mMsgResolution;
	public TextView mPrinter;
	/*
	 * 
	 */
	public Button mOk;
	public Button mCancel;
	public Button mDelete;
	
	public Context mContext;
	
	private PopWindowSpiner  mSpiner;
	private PopWindowAdapter mFontAdapter;
// H.M.Wang 2020-2-4 添加Shift控件的位数项目
    private PopWindowAdapter mShiftBitsAdapter;
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目
// H.M.Wang 2020-8-6 时间格式选择改为对话窗
//	private PopWindowAdapter mFormatAdapter;
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗
	private PopWindowAdapter mTypeAdapter;
	private PopWindowAdapter mLineAdapter;
	private PopWindowAdapter mDirAdapter;
	private PopWindowAdapter mHeightAdapter;
	private PopWindowAdapter mBarFormatAdapter;

	public final static int MSG_SELECTED_FONT = 1;
	public final static int MSG_SELECTED_SIZE = 2;
// H.M.Wang 2020-8-6 时间格式选择改为对话窗
	public final static int MSG_SELECTED_TIME_FORMAT = 3;
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗

	public Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch(msg.what) {
			case MSG_SELECTED_FONT:
				Bundle data = msg.getData();
				String font = data.getString("font");
				mFont.setText(font);
				break;
			case MSG_SELECTED_SIZE:
				Bundle d = msg.getData();
				String size = d.getString("height");
				mHighEdit.setText(size);
// H.M.Wang 2020-4-15 追加"5x5"字体
				if (size.equalsIgnoreCase(MessageObject.mDotSizes[0])) {
					mFont.setText("5x5");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[1])) {
					mFont.setText("4");
					mFont.setClickable(false);
// H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[2])) {
					mFont.setText("10");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[3])) {
					mFont.setText("12");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[4])) {
					mFont.setText("14");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[5])) {
					mFont.setText("7");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[6])) {
					mFont.setText("7L");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[7])) {
					mFont.setText("7R");
					mFont.setClickable(false);
// End of H.M.Wang 2020-1-23 追加"10x8", "12x9", "14x10"字体
// End of H.M.Wang 2020-4-15 追加"5x5"字体
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
				} else if (size.equalsIgnoreCase(MessageObject.mDot_32_Size[8])) {
					mFont.setText("19");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDot_32_Size[9])) {
					mFont.setText("21");
					mFont.setClickable(false);
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
				} else {
					mFont.setClickable(true);
				}
				break;
// H.M.Wang 2020-8-6 时间格式选择改为对话窗
			case MSG_SELECTED_TIME_FORMAT:
				String format = msg.getData().getString("format");

				RealtimeObject rtObj = new RealtimeObject(mContext, 0);
				rtObj.setFormat(format);
				mRtFormat.setText(format);
				mContent.setText(rtObj.getContent());
				break;
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗
			}
		}
	};
	
	public ObjectInfoDialog(Context context, BaseObject obj) {
		super(context, R.style.Dialog_Fullscreen);
		mContext = context;
		mObject = obj;
		initAdapter();
	}

	 @Override
	 protected void onCreate(Bundle savedInstanceState) {
		 super.onCreate(savedInstanceState);
	     this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		 // this.setTitle(R.string.str_title_infodialog);
	     if(mObject==null)
	     {
//	    	 Debug.d(TAG, "--->obj: " + mObject.mIndex);
	    	 this.setContentView(R.layout.obj_info_msg); 
	     }
	     else if(mObject instanceof TextObject)
	     {
	    	 this.setContentView(R.layout.obj_info_text); 	 
	     }
	     else if(mObject instanceof BarcodeObject)
	     {
	    	 this.setContentView(R.layout.obj_info_barcode);
	     }
	     else if(mObject instanceof CounterObject)
	     {
	    	 this.setContentView(R.layout.obj_info_counter);
	     }
	     else if(mObject instanceof GraphicObject)
	     {
	    	 this.setContentView(R.layout.obj_info_graphic);
	     }
	     else if(mObject instanceof RealtimeObject)
	     {
	    	 this.setContentView(R.layout.obj_info_realtime);
	     }
	     else if(mObject instanceof JulianDayObject ||
	    		 mObject instanceof RealtimeSecond)
	     {
	    	 this.setContentView(R.layout.obj_info_julian);
	     }
	     else if(mObject instanceof LineObject || mObject instanceof RectObject || mObject instanceof EllipseObject )
	     {
	    	 this.setContentView(R.layout.obj_info_shape);
	     }
	     else if(mObject instanceof ShiftObject)
	     {
	    	 this.setContentView(R.layout.obj_info_shift);
	     }
// H.M.Wang 2020-2-16 追加HyperText控件
		 else if(mObject instanceof HyperTextObject)
		 {
			 this.setContentView(R.layout.obj_info_hypertext);
		 }
// End of H.M.Wang 2020-2-16 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
		 else if(mObject instanceof DynamicText)
		 {
			 this.setContentView(R.layout.obj_info_dynamictext);
		 }
// End of H.M.Wang 2020-6-10 追加DynamicText控件
	     else if(mObject instanceof MessageObject)
	     {
	    	 this.setContentView(R.layout.msg_info);
	    	 mMsg = (EditText) findViewById(R.id.msgNameEdit);
	    	 mMsgResolution = (CheckBox) findViewById(R.id.resolution);
	    	 mPrinter = (TextView) findViewById(R.id.headTypeSpin);
	    	 mPrinter.setOnClickListener(this);
	    	 mMsgResolution.setOnCheckedChangeListener(this);
	     } else if (mObject instanceof LetterHourObject) {
	    	this.setContentView(R.layout.obj_info_julian); 
	     } else if (mObject instanceof WeekOfYearObject
	    		 || mObject instanceof WeekDayObject) {
		   	this.setContentView(R.layout.obj_info_text); 
		 }
	     else {
	    	 Debug.d(TAG, "--->obj: " + mObject.mIndex);
	    	 this.setContentView(R.layout.obj_info_text);
	     }
	     
	     mScroll = (ScrollView) findViewById(R.id.viewInfo);
	     mScroll.setOnTouchListener(this);
//	    mXCorView 	= (TextView) findViewById(R.id.xCorView);
	 	mXCorUnit 		= (TextView) findViewById(R.id.xCorUnit);
//	 	mYCorView	= (TextView) findViewById(R.id.yCorView);
	 	mYCorUnit 		= (TextView) findViewById(R.id.yCorUnit);
//	 	mWidthView 	= (TextView) findViewById(R.id.widthView);
//	 	mWidthUnit 	= (TextView) findViewById(R.id.widthUnitView);
//	 	mHighView 		= (TextView) findViewById(R.id.highView);
	 	mHighUnit 		= (TextView) findViewById(R.id.highUnitView);
//	 	mCntView 		= (TextView) findViewById(R.id.cntView);
	 	mFontView 		= (TextView) findViewById(R.id.fontView);
//	 	mRtfmtView 	= (TextView) findViewById(R.id.rtFmtView);
//	 	mBitsView 		= (TextView) findViewById(R.id.bitsView);
//	 	mDirectView 	= (TextView) findViewById(R.id.viewDirect);
//	 	mCodeView 	= (TextView) findViewById(R.id.viewCode);
//	 	mNumshowView = (TextView) findViewById(R.id.view_num_show);
//	 	mLineView 		= (TextView) findViewById(R.id.lineView);
//	 	mLinetypeView = (TextView) findViewById(R.id.view_line_type);
//	 	
	 	//Inflater inflater inflater= new Inflater();
	 	//View v1 = inflater.inflate(R.id.)
	 	if (! (mObject instanceof MessageObject)) {
		
		    mWidthEdit = (EditText)findViewById(R.id.widthEdit);
		    mHighEdit = (TextView)findViewById(R.id.highEdit);
//		    mHeight_O = (EditText) findViewById(R.id.highEdit_o);
		    mHighEdit.setOnClickListener(this);
			
		    mXcorEdit = (EditText)findViewById(R.id.xCorEdit);
		    mYcorEdit = (EditText)findViewById(R.id.yCorEdit);
		    mContent = (EditText)findViewById(R.id.cntEdit);
			mContentView = (TextView)findViewById(R.id.cntView);
		    mFont = (TextView) findViewById(R.id.fontSpin);
		    if (!mObject.fixedFont()) {
		    	mFont.setOnClickListener(this);
			}
		    mHeightType = (CheckBox) findViewById(R.id.height_type);
		    mHeightType.setOnCheckedChangeListener(this);
		    
		    mReverse = (CheckBox) findViewById(R.id.reverse_cb);
		    mReverse.setOnCheckedChangeListener(this);
		    
		    if (mObject instanceof RealtimeObject) {
		    	mRtFormat = (TextView) findViewById(R.id.rtFormat);
			    mRtFormat.setOnClickListener(this);
			    mOffset = (EditText) findViewById(R.id.et_offset);
			}
		    
		    if (mObject instanceof CounterObject) {
		    	mCntView = (TextView) findViewById(R.id.cntView);
		    	mDigits = (EditText) findViewById(R.id.cntBits);
		    	mDigits.addTextChangedListener(this);
			    // mDir = (TextView) findViewById(R.id.spinDirect);
			    // mDir.setOnClickListener(this);
				mCounterStart = (EditText) findViewById(R.id.et_start);
				mCounterEnd = (EditText) findViewById(R.id.et_end);
			    mCntIndex = (EditText) findViewById(R.id.et_cnt_index);
			    mCntSteplen = (EditText) findViewById(id.et_cnt_step);
			}
		    if (mObject instanceof BarcodeObject) {
			    mCode = (TextView) findViewById(R.id.spinCode);
			    mCode.setOnClickListener(this);
// H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
				mITF14FrameCaption = (TextView) findViewById(id.captionFrame);
				mITF14Frame = (CheckBox) findViewById(id.checkFrame);
// End of H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
			    mShow = (CheckBox) findViewById(R.id.check_Num_show);
		    	//mContent.setEnabled(false);
			    mTextsize = (EditText) findViewById(R.id.et_text_size);
			    if (mObject.mSource) {
					mContent.setEnabled(false);
				}

				// H.M.Wang 追加这段代码在添加条码的时候不显示字体，追加二维码的时候不显示内容和字体
				// H.M.Wang 2019-9-21 二维码有两种QRCode和DynamicQRCode，只有第二种需要隐藏内容编辑窗
//				if(((BarcodeObject)mObject).isQRCode()) {
				if(((BarcodeObject)mObject).isDynamicCode()) {
					mContentView.setVisibility(View.GONE);
					mContent.setVisibility(View.GONE);
				}
				mFontView.setVisibility(View.GONE);
				mFont.setVisibility(View.GONE);
			} else if (mObject instanceof LetterHourObject) {
				mContent.setEnabled(false);
			} else if (mObject instanceof WeekOfYearObject) {
				mContent.setEnabled(false);
			}
		    
		    mLineWidth = (EditText) findViewById(R.id.lineWidth);
		    
		    if (mObject instanceof LineObject 
		    		|| mObject instanceof RectObject
		    		|| mObject instanceof EllipseObject) {
		    	mLineType = (TextView) findViewById(R.id.spin_line_type);
			    mLineType.setOnClickListener(this);
			}
		    
		    if (mObject instanceof GraphicObject) {
			    mPicture = (TextView) findViewById(R.id.image);
			    mPicture.setOnClickListener(this);

				// H.M.Wang追加这段代码在插入图片时不显示内容和字体
				mFontView.setVisibility(View.GONE);
				mFont.setVisibility(View.GONE);
				mContentView.setVisibility(View.GONE);
				mContent.setVisibility(View.GONE);
			}

// H.M.Wang 2020-2-4 添加Shift控件的位数项目
			if (mObject instanceof ShiftObject) {
				mShiftBits = (TextView)findViewById(R.id.shiftBitSpin);

				mShift1 = (EditText) findViewById(R.id.edit_shift1);
				mShiftVal1 = (EditText) findViewById(R.id.edit_shiftValue1);
				mShift2 = (EditText) findViewById(R.id.edit_shift2);
				mShiftVal2 = (EditText) findViewById(R.id.edit_shiftValue2);
				mShift3 = (EditText) findViewById(R.id.edit_shift3);
				mShiftVal3 = (EditText) findViewById(R.id.edit_shiftValue3);
				mShift4 = (EditText) findViewById(R.id.edit_shift4);
				mShiftVal4 = (EditText) findViewById(R.id.edit_shiftValue4);
			}
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目
// H.M.Wang 2020-2-19 追加HyperText控件
			if (mObject instanceof HyperTextObject) {
				mCounterStart = (EditText) findViewById(R.id.et_start);
				mCounterEnd = (EditText) findViewById(R.id.et_end);
				mCntIndex = (EditText) findViewById(R.id.et_cnt_index);

				mShift1 = (EditText) findViewById(R.id.edit_shift1);
				mShiftVal1 = (EditText) findViewById(R.id.edit_shiftValue1);
				mShift2 = (EditText) findViewById(R.id.edit_shift2);
				mShiftVal2 = (EditText) findViewById(R.id.edit_shiftValue2);
				mShift3 = (EditText) findViewById(R.id.edit_shift3);
				mShiftVal3 = (EditText) findViewById(R.id.edit_shiftValue3);
				mShift4 = (EditText) findViewById(R.id.edit_shift4);
				mShiftVal4 = (EditText) findViewById(R.id.edit_shiftValue4);

				mOffset = (EditText) findViewById(R.id.et_offset);
			}
// End of H.M.Wang 2020-2-19 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
			if (mObject instanceof DynamicText) {
				mIndex = (EditText) findViewById(id.et_dt_index);
				mDigits = (EditText) findViewById(id.cntBits);
			}
// End of H.M.Wang 2020-6-10 追加DynamicText控件
	 	}

	     mOk = (Button) findViewById(R.id.btn_confirm);
	     mCancel = (Button) findViewById(R.id.btn_objinfo_cnl);
	     fillObjInfo();
	     selfInfoEnable();
//	     mOk.setClickable(false);
	     mOk.setOnClickListener(new View.OnClickListener(){
	    	 
				@Override
				public void onClick(View v) {
					if (mObject == null) {
						dismiss();
					}
					if (mContent != null && mContent.isEnabled() && mContent.getText().toString().isEmpty()) {
						ToastUtil.show(mContext, R.string.toast_content_empty);
						return;
					}
					try{
						
						if(mObject instanceof MessageObject)
						{
							mObject.setContent(mMsg.getText().toString());
							((MessageObject) mObject).setType(mPrinter.getText().toString());
							((MessageObject) mObject).setHighResolution(mMsgResolution.isChecked());
							dismiss();
							if(mPListener != null)
								mPListener.onClick();
							return;
						}

						if (mObject instanceof TextObject) {
							mObject.setContent(mContent.getText().toString());
							Debug.d(TAG, "--->redraw: " + mObject.isNeedDraw());
						}
						else if(mObject instanceof RealtimeObject) {
							((RealtimeObject) mObject).setFormat((String) mRtFormat.getText());
							((RealtimeObject) mObject).setOffset(Integer.parseInt(mOffset.getText().toString()));
							// ((RealtimeObject)mObject).setWidth(Float.parseFloat(mWidthEdit.getText().toString()));
						}
						else if(mObject instanceof CounterObject)
						{
							((CounterObject) mObject).setBits(Integer.parseInt(mDigits.getText().toString()));
							((CounterObject) mObject).setRange(StringUtil.parseInt(mCounterStart.getText().toString()),
									StringUtil.parseInt(mCounterEnd.getText().toString()));
// H.M.Wang 2020-7-3 CounterObject内容不参与编辑
//							((CounterObject) mObject).setContent(mContent.getText().toString());
// End of H.M.Wang 2020-7-3 CounterObject内容不参与编辑
							((CounterObject) mObject).setCounterIndex(Integer.valueOf(mCntIndex.getText().toString()));
							((CounterObject) mObject).setSteplen(Integer.valueOf(mCntSteplen.getText().toString()));
						}
						else if(mObject instanceof BarcodeObject)
						{
							if (!mObject.mSource) {
								mObject.setContent(mContent.getText().toString());
							}
							
							((BarcodeObject) mObject).setCode(mCode.getText().toString());
// H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
							((BarcodeObject) mObject).setWithFrame(mITF14Frame.isChecked());
// End of H.M.Wang 2020-2-25 追加ITF_14边框有无的设置

							((BarcodeObject) mObject).setShow(mShow.isChecked());
							((BarcodeObject) mObject).setTextsize(Integer.parseInt(mTextsize.getText().toString()));
						}
						else if(mObject instanceof RectObject)
						{
							mObject.setLineWidth(Integer.parseInt(mLineWidth.getText().toString()));
							((RectObject) mObject).setLineType(mLineType.getText().toString());
						}
						else if(mObject instanceof LineObject)
						{
							mObject.setLineWidth(Integer.parseInt(mLineWidth.getText().toString()));
							((LineObject) mObject).setLineType(mLineType.getText().toString());
						}
						else if(mObject instanceof EllipseObject)
						{
							mObject.setLineWidth(Integer.parseInt(mLineWidth.getText().toString()));
							((EllipseObject) mObject).setLineType(mLineType.getText().toString());
						}
						else if(mObject instanceof ShiftObject)
						{
// H.M.Wang 2020-2-4 添加Shift控件的位数项目
							String[] shiftBits = mContext.getResources().getStringArray(R.array.strShiftBitsArray);
							if(mShiftBits.getText().toString().equals(shiftBits[1])) {
								((ShiftObject) mObject).setBits(2);
							} else {
								((ShiftObject) mObject).setBits(1);
							}
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目
							((ShiftObject) mObject).setShift(0, mShift1.getText().toString());
							((ShiftObject) mObject).setValue(0, mShiftVal1.getText().toString());
							((ShiftObject) mObject).setShift(1, mShift2.getText().toString());
							((ShiftObject) mObject).setValue(1, mShiftVal2.getText().toString());
							((ShiftObject) mObject).setShift(2, mShift3.getText().toString());
							((ShiftObject) mObject).setValue(2, mShiftVal3.getText().toString());
							((ShiftObject) mObject).setShift(3, mShift4.getText().toString());
							((ShiftObject) mObject).setValue(3, mShiftVal4.getText().toString());
// H.M.Wang 2020-2-19 追加HyperText控件
						} else if (mObject instanceof HyperTextObject) {
							((HyperTextObject) mObject).setContent(mContent.getText().toString());
//							((HyperTextObject) mObject).setContent("A@YB@MC@DD@HE@mF@sG@SH@wI@WJ@2K@3CL@4CCM@5CCC --- @ --- A@YB@MC@DD@HE@mF@sG@SH@wI@WJ@2K@3CL@4CCM@5CCC");
							((HyperTextObject) mObject).setCounterIndex(mCntIndex.getText().toString());
							((HyperTextObject) mObject).setCounterStart(mCounterStart.getText().toString());
							((HyperTextObject) mObject).setCounterEnd(mCounterEnd.getText().toString());
							((HyperTextObject) mObject).setShiftTime(0, mShift1.getText().toString());
							((HyperTextObject) mObject).setShiftValue(0, mShiftVal1.getText().toString());
							((HyperTextObject) mObject).setShiftTime(1, mShift2.getText().toString());
							((HyperTextObject) mObject).setShiftValue(1, mShiftVal2.getText().toString());
							((HyperTextObject) mObject).setShiftTime(2, mShift3.getText().toString());
							((HyperTextObject) mObject).setShiftValue(2, mShiftVal3.getText().toString());
							((HyperTextObject) mObject).setShiftTime(3, mShift4.getText().toString());
							((HyperTextObject) mObject).setShiftValue(3, mShiftVal4.getText().toString());
							((HyperTextObject) mObject).setDateOffset(mOffset.getText().toString());
// End of H.M.Wang 2020-2-19 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
						} else if (mObject instanceof DynamicText) {
							try {
								((DynamicText) mObject).setDtIndex(Integer.parseInt(mIndex.getText().toString()));
							} catch (NumberFormatException e) {
								((DynamicText) mObject).setDtIndex(0);
							}
							try {
								((DynamicText) mObject).setBits(Integer.parseInt(mDigits.getText().toString()));
							} catch (NumberFormatException e) {
								((DynamicText) mObject).setBits(0);
							}
							((DynamicText) mObject).setContent(mContent.getText().toString());
// End of H.M.Wang 2020-6-10 追加DynamicText控件
						} else if (mObject instanceof GraphicObject) {

						}
						// mObject.setWidth(Float.parseFloat(mWidthEdit.getText().toString()));
						// mObject.setHeight(Float.parseFloat(mHighEdit.getText().toString()));

						Debug.d(TAG, "--->positive click");
						try {
////						String font = mFont.getText().toString();
							mObject.setFont(mFont.getText().toString());

//							if (mHeightType.isChecked()) {
//								mObject.setHeight(Integer.parseInt(mHeight_O.getText().toString()));
//							} else {
							// H.M.Wang 2019-9-25 属性内部设置高（已修改为字号）时，重新按自然比例调整
								if(!mObject.getDisplayHeight().equals(mHighEdit.getText().toString())) {
									mObject.setHeight(mHighEdit.getText().toString());
									mObject.resizeByHeight();
									mObject.setXRatio();
								} else {
									mObject.setHeight(mHighEdit.getText().toString());
								}
//							}
						} catch (Exception e) {
						}

						float ratio = mObject.getTask().getNozzle().getPhisicalRatio();
						if(ratio > 0.0f) {
							mObject.setX(Float.parseFloat(mXcorEdit.getText().toString())/ratio/2);
							mObject.setY(Float.parseFloat(mYcorEdit.getText().toString())/ratio/2);
						} else {
							mObject.setX(Float.parseFloat(mXcorEdit.getText().toString())/2);
							mObject.setY(Float.parseFloat(mYcorEdit.getText().toString())/2);
						}

//						mObject.setX(Float.parseFloat(mXcorEdit.getText().toString())/2);
//						mObject.setY(Float.parseFloat(mYcorEdit.getText().toString())/2);
						Debug.d(TAG, "content="+mContent.getText().toString());

////						Resources res = mContext.getResources();

						mObject.setReverse(mReverse.isChecked());
						Debug.d(TAG, "--->redraw: " + mObject.isNeedDraw());
						//mObjRefreshHandler.sendEmptyMessage(0);
					}catch(NumberFormatException e)
					{
						System.out.println("NumberFormatException 292");
					}
					dismiss();
					if(mPListener != null)
						mPListener.onClick();
				}
				
			});
	     
	     mCancel.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				dismiss();
			}
		});
	     
	     mDelete = (Button) findViewById(R.id.btn_delete);
	     mDelete.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				dismiss();
				if(mDelListener != null) {
					mDelListener.onClick(mObject);
				}
			}
		});
	    
	     /*mPageup = (Button) findViewById(R.id.btn_page_up);
	     mPageup.setOnClickListener(this);
	     mPagedown = (Button) findViewById(R.id.btn_page_down);
	     mPagedown.setOnClickListener(this);*/
	 }
	 
	 public void setObject(BaseObject obj)
	 {
		 mObject = obj;
	 }

	 public BaseObject getObject() {
		 return mObject;
	 }

	 private void fillObjInfo()
	 {
		 int i=0;
		 if(mObject == null)
			 return;
		 if(mObject instanceof MessageObject)
			{	
				mMsg.setText(mObject.getContent());
				mPrinter.setText(((MessageObject) mObject).getPrinterName());
			}
			else
			{
				Debug.d(TAG, "--->fillObjInfo");
				mWidthEdit.setText(String.valueOf((int)mObject.getWidth()) );
				mHighEdit.setText(mObject.getDisplayHeight());
//				mHeight_O.setText(String.valueOf(mObject.getHeight()));

				float ratio = mObject.getTask().getNozzle().getPhisicalRatio();
				mXcorEdit.setText(String.valueOf(1.0f /100 * Math.round(mObject.getX()*2*ratio*100)));
				mYcorEdit.setText(String.valueOf(1.0f /100 * Math.round(mObject.getY()*2*ratio*100)));
				if(mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_12_7 ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_25_4 ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_38_1 ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_50_8 ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
					mObject.getTask().getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR ) {
					mXCorUnit.setText(string.font_unit);
					mYCorUnit.setText(string.font_unit);
					mHighUnit.setText(string.font_unit);
				} else {
					mXCorUnit.setText(string.dot_unit);
					mYCorUnit.setText(string.dot_unit);
					mHighUnit.setText(string.dot_unit);
				}

//				mXcorEdit.setText(String.valueOf((int)mObject.getX()*2));
//				mYcorEdit.setText(String.valueOf((int)mObject.getY()*2));
				mContent.setText(String.valueOf(mObject.getContent()));
				mReverse.setChecked(mObject.getReverse());
				
				mFont.setText(mObject.getFont());
				if(mObject instanceof RealtimeObject)
				{
					mRtFormat.setText(((RealtimeObject) mObject).getFormat());
					mOffset.setText(String.valueOf(((RealtimeObject) mObject).getOffset()));
// H.M.Wang 2020-2-17 追加HyperText控件
				} else if(mObject instanceof HyperTextObject) {
					mCounterStart.setText(String.valueOf(((HyperTextObject) mObject).getCounterStart()));
					mCounterEnd.setText(String.valueOf(((HyperTextObject) mObject).getCounterEnd()));
					mCntIndex.setText(String.valueOf(((HyperTextObject)mObject).getCounterIndex()));
					mShift1.setText( String.valueOf(((HyperTextObject)mObject).getShiftTime(0)));
					mShiftVal1.setText(((HyperTextObject)mObject).getShiftValue(0));
					mShift2.setText( String.valueOf(((HyperTextObject)mObject).getShiftTime(1)));
					mShiftVal2.setText(((HyperTextObject)mObject).getShiftValue(1));
					mShift3.setText(String.valueOf(((HyperTextObject)mObject).getShiftTime(2)));
					mShiftVal3.setText(((HyperTextObject)mObject).getShiftValue(2));
					mShift4.setText(String.valueOf(((HyperTextObject)mObject).getShiftTime(3)));
					mShiftVal4.setText(((HyperTextObject)mObject).getShiftValue(3));
					mOffset.setText(String.valueOf(((HyperTextObject) mObject).getOffset()));
// End of H.M.Wang 2020-2-17 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
				} else if(mObject instanceof DynamicText) {
					mIndex.setText(String.valueOf(((DynamicText) mObject).getDtIndex()));
					mDigits.setText(String.valueOf( ((DynamicText) mObject).getBits()));
					mContent.setText(String.valueOf( ((DynamicText) mObject).getContent()));
// End of H.M.Wang 2020-6-10 追加DynamicText控件
				}
				else if(mObject instanceof CounterObject)
				{
					mDigits.setText(String.valueOf( ((CounterObject) mObject).getBits()));
					// mDir.setText( ((CounterObject) mObject).getDirection());
					mCounterStart.setText(String.valueOf(((CounterObject) mObject).getStart()));
					mCounterEnd.setText(String.valueOf(((CounterObject) mObject).getEnd()));
					mCntIndex.setText(String.valueOf(((CounterObject)mObject).getmCounterIndex()));
					mCntSteplen.setText(String.valueOf(((CounterObject)mObject).getSteplen()));
				}
				else if(mObject instanceof BarcodeObject)
				{
					mCode.setText(((BarcodeObject) mObject).getCode());
// H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
					mITF14Frame.setChecked(((BarcodeObject) mObject).getWithFrame());
					if("ITF_14".equals(((BarcodeObject) mObject).getCode())) {
						mITF14FrameCaption.setVisibility(View.VISIBLE);
						mITF14Frame.setVisibility(View.VISIBLE);
					} else {
						mITF14FrameCaption.setVisibility(View.GONE);
						mITF14Frame.setVisibility(View.GONE);
					}
// End of H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
					mShow.setChecked(((BarcodeObject) mObject).getShow());
					mTextsize.setText(String.valueOf(((BarcodeObject) mObject).getTextsize()));
				}
				else if(mObject instanceof ShiftObject)
				{
// H.M.Wang 2020-2-4 添加Shift控件的位数项目
					mShiftBits.setOnClickListener(this);
					String[] shiftBits = mContext.getResources().getStringArray(R.array.strShiftBitsArray);
					if(((ShiftObject)mObject).getBits() == 2) {
						mShiftBits.setText(shiftBits[1]);
					} else {
						mShiftBits.setText(shiftBits[0]);
					}
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目
					mShift1.setText( String.valueOf(((ShiftObject)mObject).getShift(0)));
					mShiftVal1.setText(((ShiftObject)mObject).getValue(0));
					mShift2.setText( String.valueOf(((ShiftObject)mObject).getShift(1)));
					mShiftVal2.setText(((ShiftObject)mObject).getValue(1));
					mShift3.setText(String.valueOf(((ShiftObject)mObject).getShift(2)));
					mShiftVal3.setText(((ShiftObject)mObject).getValue(2));
					mShift4.setText(String.valueOf(((ShiftObject)mObject).getShift(3)));
					mShiftVal4.setText(((ShiftObject)mObject).getValue(3));
				}
				else if(mObject instanceof RectObject){
					String lines[] = mContext.getResources().getStringArray(R.array.strLineArray);
					mLineWidth.setText(String.valueOf(((RectObject)mObject).getLineWidth()));
					mLineType.setText(lines[((RectObject)mObject).getLineType()]);
				}
				else if(mObject instanceof LineObject){
					String lines[] = mContext.getResources().getStringArray(R.array.strLineArray);
					mLineWidth.setText(String.valueOf(((LineObject)mObject).getLineWidth()));
					mLineType.setText(lines[((LineObject)mObject).getLineType()]);
				}
				else if(mObject instanceof EllipseObject){
					String lines[] = mContext.getResources().getStringArray(R.array.strLineArray);
					mLineWidth.setText(String.valueOf(((EllipseObject)mObject).getLineWidth()));
					mLineType.setText(lines[((EllipseObject)mObject).getLineType()]);
				} else if (mObject instanceof GraphicObject) {
					mPicture.setText(mObject.getContent());
				}
			}
	 }
	 
	public void selfInfoEnable()
	{
		if(mObject == null ||(mObject instanceof MessageObject))
			return ;
		
		if(mObject instanceof RealtimeObject ||
// H.M.Wang 2020-6-11 追加HyperText控件
//			mObject instanceof DynamicText ||
// End of H.M.Wang 2020-6-11 追加HyperText控件
// H.M.Wang 2020-7-3 CounterObject内容不参与编辑
				mObject instanceof CounterObject ||
// End of H.M.Wang 2020-7-3 CounterObject内容不参与编辑
			mObject instanceof GraphicObject ||
			mObject instanceof RealtimeSecond ||
			mObject instanceof ShiftObject ||
			mObject instanceof EllipseObject ||
			mObject instanceof RectObject ||
			mObject instanceof LineObject ||
			mObject instanceof WeekOfYearObject ||
			mObject instanceof WeekDayObject )
		{
			Debug.d(TAG, ">>>>>disable content");
			mContent.setEnabled(false);
		}
		
	}
	 
	 public void setOnPositiveBtnListener(OnPositiveBtnListener listener)
	 {
		mPListener = listener; 
	 }
	 
	 public void setOnNagitiveBtnListener(OnNagitiveBtnListener listener)
	 {
		 mNListener = listener;
	 }
	 
	 public void setOnDeleteListener(onDeleteListener listener) {
		 mDelListener = listener;
	 }
	 
	 public interface OnPositiveBtnListener
	 {
		 void onClick();
	 }
	 
	 public interface OnNagitiveBtnListener
	 {
		 void onClick();
	 }
	 
	 public interface onDeleteListener {
		 void onClick(BaseObject object);
	 }
	 
	 private void initAdapter() {
		 
		mSpiner = new PopWindowSpiner(mContext);
		mSpiner.setFocusable(true);
		mSpiner.setOnItemClickListener(this);
		
		mFontAdapter = new PopWindowAdapter(mContext, null);
// H.M.Wang 2020-2-4 添加Shift控件的位数项目
         mShiftBitsAdapter = new PopWindowAdapter(mContext, null);
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目
// H.M.Wang 2020-8-6 时间格式选择改为对话窗
//		mFormatAdapter = new PopWindowAdapter(mContext, null);
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗
		mTypeAdapter = new PopWindowAdapter(mContext, null);
		mLineAdapter = new PopWindowAdapter(mContext, null);
		// mDirAdapter = new PopWindowAdapter(mContext, null);
		mHeightAdapter = new PopWindowAdapter(mContext, null);
		mBarFormatAdapter = new PopWindowAdapter(mContext, null);
		
		// String[] heights = mContext.getResources().getStringArray(R.array.strarrayFontSize);
		if (mObject != null) {
			Debug.d(TAG, "--->initAdapter: " + mObject);
			MessageObject msg = mObject.getTask().getMsgObject();
			String[] heights = msg.getDisplayFSList();
			for (String height : heights) {
				Debug.d(TAG, "--->height: " + height);
				mHeightAdapter.addItem(height);
			}
			
		}

// H.M.Wang 2020-2-4 添加Shift控件的位数项目
         String[] shiftBits = mContext.getResources().getStringArray(R.array.strShiftBitsArray);
         for (String shiftBit : shiftBits) {
             mShiftBitsAdapter.addItem(shiftBit);
         }
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目

		String[] fonts = mContext.getResources().getStringArray(R.array.strFontArray);
		for (String font : fonts) {
			mFontAdapter.addItem(font);
		}

// H.M.Wang 2020-8-6 时间格式选择改为对话窗
//		String[] formats = mContext.getResources().getStringArray(R.array.strTimeFormat);
//		for (String format : formats) {
//			mFormatAdapter.addItem(format);
//		}
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗

		String[] types = mContext.getResources().getStringArray(R.array.strPrinterArray);
		for (String type : types) {
			mTypeAdapter.addItem(type);
		}
		
		String[] lines = mContext.getResources().getStringArray(R.array.strLineArray);
		for (String line : lines) {
			mLineAdapter.addItem(line);
		}
		
		String[] barFormats = mContext.getResources().getStringArray(R.array.strCodeArray);
		for (String format : barFormats) {
			mBarFormatAdapter.addItem(format);
		}
		/*
		String[] directions = mContext.getResources().getStringArray(R.array.strDirectArray);
		for (String direction : directions) {
			mDirAdapter.addItem(direction);
		}
		*/
	 }
	 
	 private void setHFullScreen() {
		 Window win = this.getWindow();
		 win.getDecorView().setPadding(0, 0, 0, 0);
		 WindowManager.LayoutParams lp = win.getAttributes();
		 // lp.width = WindowManager.LayoutParams.FILL_PARENT;
		 lp.height = WindowManager.LayoutParams.FILL_PARENT;
		 win.setAttributes(lp);
	 }

	@Override
	public void onClick(View v) {
		if (v == null) {
			return;
		}
		mSpiner.setAttachedView(v);
		mSpiner.setWidth(v.getWidth());
		
		switch (v.getId()) {
		case R.id.highEdit:
			// mSpiner.setAdapter(mHeightAdapter);
			// mSpiner.showAsDropUp(v);
			HeightSelectDialog d = new HeightSelectDialog(mContext, mHandler, mObject);
			d.show();
			break;
		case R.id.headTypeSpin:
			mSpiner.setAdapter(mTypeAdapter);
			mSpiner.showAsDropUp(v);
			break;
		case R.id.fontSpin:
			// mSpiner.setAdapter(mFontAdapter);
			// mSpiner.showAsDropUp(v);
			FontSelectDialog dialog1 = new FontSelectDialog(mContext, mHandler);
			dialog1.show();
			break;
// H.M.Wang 2020-2-4 添加Shift控件的位数项目
        case R.id.shiftBitSpin:
            mSpiner.setAdapter(mShiftBitsAdapter);
            mSpiner.showAsDropUp(v);
            break;
// End of H.M.Wang 2020-2-4 添加Shift控件的位数项目
		case R.id.rtFormat:
// H.M.Wang 2020-8-6 时间格式选择改为对话窗
//			mSpiner.setAdapter(mFormatAdapter);
//			mSpiner.showAsDropUp(v);
			TimeFormatSelectDialog tfDlg = new TimeFormatSelectDialog(mContext, mHandler);
			tfDlg.setCurrentFormat(mRtFormat.getText().toString());
			tfDlg.show();
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗
			break;
		case R.id.spin_line_type:
			mSpiner.setAdapter(mLineAdapter);
			mSpiner.showAsDropUp(v);
			break;
		case R.id.spinCode:
			mSpiner.setAdapter(mBarFormatAdapter);
			mSpiner.showAsDropUp(v);
			break;
		/*
		case R.id.spinDirect:
			mSpiner.setAdapter(mDirAdapter);
			mSpiner.showAsDropUp(v);
			break;*/
		case R.id.image:
			final PictureBrowseDialog dialog = new PictureBrowseDialog(mContext);
			dialog.setOnPositiveClickedListener(new OnPositiveListener() {
				
				@Override
				public void onClick(String content) {
					dialog.dismiss();
				}
				
				@Override
				public void onClick() {
					PictureItem item = dialog.getSelect();
					if (item == null) {
						dialog.dismiss();
						return;
					}
					((GraphicObject)mObject).setImage(item.getPath());
					mPicture.setText(mObject.getContent());
					dialog.dismiss();
				}
			});
			dialog.setOnNagitiveClickedListener(new OnNagitiveListener() {
				
				@Override
				public void onClick() {
					dialog.dismiss();
				}
			});
			dialog.show();
			break;
		case R.id.btn_page_up:
			mScroll.smoothScrollBy(0, -300);
			break;
		case R.id.btn_page_down:
			mScroll.smoothScrollBy(0, 300);
			break;
		default:
			break;
		}
		
		// mSpiner.showAsDropDown(v);
	}

	@Override
	public void onItemClick(int index) {
		TextView view = mSpiner.getAttachedView();
		if (view == mPrinter) {
			view.setText((String)mTypeAdapter.getItem(index));
		} else if (view == mFont) {
			view.setText((String)mFontAdapter.getItem(index));
        } else if (view == mShiftBits) {
            view.setText((String)mShiftBitsAdapter.getItem(index));
// H.M.Wang 2020-8-6 时间格式选择改为对话窗
/*
		} else if (view == mRtFormat) {

			// H.M.Wang追加下列5行，实时根据新的时间格式变更内容区域的显示内容
			RealtimeObject rtObj = new RealtimeObject(mContext, 0);
			String[] formats = mContext.getResources().getStringArray(R.array.strTimeFormat);
			rtObj.setFormat(formats[index]);
			Debug.d(TAG, String.valueOf(rtObj.getContent()));
			mContent.setText(String.valueOf(rtObj.getContent()));

			view.setText((String)mFormatAdapter.getItem(index));
*/
// End of H.M.Wang 2020-8-6 时间格式选择改为对话窗
		} else if (view == mLineType) {
			view.setText((String)mLineAdapter.getItem(index));
		} else if (view == mDir) {
			view.setText((String)mDirAdapter.getItem(index));
		} else if (view == mHighEdit) {
			String height = (String)mHeightAdapter.getItem(index);
			view.setText(height);
		} else if (view == mCode) {
			view.setText((String)mBarFormatAdapter.getItem(index));
// H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
			if("ITF_14".equals(view.getText())) {
				mITF14FrameCaption.setVisibility(View.VISIBLE);
				mITF14Frame.setVisibility(View.VISIBLE);
			} else {
				mITF14FrameCaption.setVisibility(View.GONE);
				mITF14Frame.setVisibility(View.GONE);
			}
// End of H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
		} else {
			Debug.d(TAG, "--->unknow view");
		}
	}

	@Override
	public void onCheckedChanged(CompoundButton view, boolean checked) {
		if (view == mHeightType) {
			if (checked) {
//				mHeight_O.setEnabled(true);
				mHighEdit.setEnabled(false);
			} else {
//				mHeight_O.setEnabled(false);
				mHighEdit.setEnabled(true);
			}
		} else if (view == mMsgResolution) {
			//if ()
		} else if (view == mReverse) {
		}
	}

	@Override
	public boolean onTouch(View arg0, MotionEvent arg1) {
		InputMethodManager im = (InputMethodManager) mContext.getSystemService(Context.INPUT_METHOD_SERVICE);
        im.hideSoftInputFromWindow(arg0.getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
		return false;
	}
	
	@Override
	public void afterTextChanged(Editable s) {
		if (s.toString().length() <= 0) {
			return;
		}
		mCounterStart.setText("0");
		try {
			int max = (int) Math.pow(10, Integer.parseInt(mDigits.getText().toString())) - 1;
			mCounterEnd.setText(String.valueOf(max));
		} catch (Exception e) {}
	}

	@Override
	public void beforeTextChanged(CharSequence s, int start, int count,
			int after) {
	}

	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count) {
		
	}

}
