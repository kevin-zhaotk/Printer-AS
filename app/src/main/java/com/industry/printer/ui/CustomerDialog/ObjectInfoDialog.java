package com.industry.printer.ui.CustomerDialog;

import java.util.zip.Inflater;

import com.google.zxing.BarcodeFormat;
import com.google.zxing.ChecksumException;
import com.google.zxing.common.StringUtils;
import com.google.zxing.maxicode.MaxiCodeReader;
import com.industry.printer.R;
import com.industry.printer.MessageTask.MessageType;
import com.industry.printer.R.array;
import com.industry.printer.R.id;
import com.industry.printer.R.layout;
import com.industry.printer.R.string;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.object.BarcodeObject;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.EllipseObject;
import com.industry.printer.object.JulianDayObject;
import com.industry.printer.object.LetterHourObject;
import com.industry.printer.object.LineObject;
import com.industry.printer.object.MessageObject;
import com.industry.printer.object.RealtimeObject;
import com.industry.printer.object.RectObject;
import com.industry.printer.object.TextObject;
import com.industry.printer.object.GraphicObject;
import com.industry.printer.object.JulianDayObject;
import com.industry.printer.object.RTSecondObject;
import com.industry.printer.object.ShiftObject;
import com.industry.printer.object.WeekDayObject;
import com.industry.printer.object.WeekOfYearObject;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter.IOnItemClickListener;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnNagitiveListener;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnPositiveListener;
import com.industry.printer.ui.Items.PictureItem;
import com.industry.printer.widget.PopWindowSpiner;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.graphics.Color;
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
import android.webkit.MimeTypeMap;
import android.widget.Adapter;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.ScrollView;
import android.widget.Spinner;
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
	public TextView mFont;
	public TextView mRtFormat;
	public EditText mDigits;
	public TextView mDir;
	public TextView mCode;
	private EditText mHeight_O;
	private CheckBox mHeightType;
	public CheckBox mShow;
	public EditText mLineWidth;
	public TextView mPicture; // 圖片路徑
	public EditText mOffset;
	public EditText mShift1;
	public EditText mShiftVal1;
	public EditText mShift2;
	public EditText mShiftVal2;
	public EditText mShift3;
	public EditText mShiftVal3;
	public EditText mShift4;
	public EditText mShiftVal4;
	private EditText mMin;
	private EditText mMax;
	public Button	mBtnOk;
	private Button  mPageup;
	private Button  mPagedown;
	private ScrollView mScroll;
	public TextView mLineType;
	private EditText mTextsize;
	private CheckBox mReverse;

	
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
	private PopWindowAdapter mFormatAdapter;
	private PopWindowAdapter mTypeAdapter;
	private PopWindowAdapter mLineAdapter;
	private PopWindowAdapter mDirAdapter;
	private PopWindowAdapter mHeightAdapter;
	private PopWindowAdapter mBarFormatAdapter;

	public final static int MSG_SELECTED_FONT = 1;
	public final static int MSG_SELECTED_SIZE = 2;
	public final static int MSG_SELECTED_HEADER = 3;
	
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
				if (size.equalsIgnoreCase(MessageObject.mDotSizes[0])) {
					mFont.setText("4");
					mFont.setClickable(false);
				} else if (size.equalsIgnoreCase(MessageObject.mDotSizes[1])) {
					mFont.setText("7");
					mFont.setClickable(false);
				} else {
					mFont.setClickable(true);
				}
				break;
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
	    		 mObject instanceof RTSecondObject)
	     {
	    	 this.setContentView(R.layout.obj_info_julian);
	     }
	     else if(mObject instanceof LineObject || mObject instanceof RectObject || mObject instanceof EllipseObject )
	     {
	    	 this.setContentView(R.layout.obj_info_shape);
	     }
	     else if(mObject instanceof ShiftObject)
	     {
	    	 Debug.d(TAG, "ShiftObject");
	    	 this.setContentView(R.layout.obj_info_shift);
	     }
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
//	 	mXCorUnit 		= (TextView) findViewById(R.id.xCorUnit);
//	 	mYCorView	= (TextView) findViewById(R.id.yCorView);
//	 	mYCorUnit 		= (TextView) findViewById(R.id.yCorUnit);
//	 	mWidthView 	= (TextView) findViewById(R.id.widthView);
//	 	mWidthUnit 	= (TextView) findViewById(R.id.widthUnitView);
//	 	mHighView 		= (TextView) findViewById(R.id.highView);
//	 	mHighUnit 		= (TextView) findViewById(R.id.highUnitView);
//	 	mCntView 		= (TextView) findViewById(R.id.cntView);
//	 	mFontView 		= (TextView) findViewById(R.id.fontView);
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
		    mHeight_O = (EditText) findViewById(R.id.highEdit_o);
		    mHighEdit.setOnClickListener(this);
			
		    mXcorEdit = (EditText)findViewById(R.id.xCorEdit);
		    mYcorEdit = (EditText)findViewById(R.id.yCorEdit);
		    mContent = (EditText)findViewById(R.id.cntEdit);
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
			    mMin = (EditText) findViewById(R.id.et_start);
			    mMax = (EditText) findViewById(R.id.et_end);
			    
			}
		    if (mObject instanceof BarcodeObject) {
			    mCode = (TextView) findViewById(R.id.spinCode);
			    mCode.setOnClickListener(this);
			    mShow = (CheckBox) findViewById(R.id.check_Num_show);
		    	//mContent.setEnabled(false);
			    mTextsize = (EditText) findViewById(R.id.et_text_size);
			    if (mObject.mSource) {
					mContent.setEnabled(false);
				}
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
			}
	 	}
	     
	     mShift1 = (EditText) findViewById(R.id.edit_shift1);
	     mShiftVal1 = (EditText) findViewById(R.id.edit_shiftValue1);
	     mShift2 = (EditText) findViewById(R.id.edit_shift2);
	     mShiftVal2 = (EditText) findViewById(R.id.edit_shiftValue2);
	     mShift3 = (EditText) findViewById(R.id.edit_shift3);
	     mShiftVal3 = (EditText) findViewById(R.id.edit_shiftValue3);
	     mShift4 = (EditText) findViewById(R.id.edit_shift4);
	     mShiftVal4 = (EditText) findViewById(R.id.edit_shiftValue4);
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
						else if(mObject instanceof RealtimeObject)
						{
							((RealtimeObject) mObject).setFormat((String)mRtFormat.getText());
							((RealtimeObject) mObject).setOffset(Integer.parseInt(mOffset.getText().toString()));
							// ((RealtimeObject)mObject).setWidth(Float.parseFloat(mWidthEdit.getText().toString()));
						}
						else if(mObject instanceof CounterObject)
						{
							((CounterObject) mObject).setBits(Integer.parseInt(mDigits.getText().toString()));
							// ((CounterObject) mObject).setDirection("increase".equals(mDir.getText().toString())?true:false);
							((CounterObject) mObject).setRange(StringUtil.parseInt(mMin.getText().toString()), 
									StringUtil.parseInt(mMax.getText().toString()));
							((CounterObject) mObject).setContent(mContent.getText().toString());
							// ((CounterObject) mObject).setMin(StringUtil.parseInt(mMin.getText().toString()));
							// ((CounterObject) mObject).setMax(StringUtil.parseInt(mMax.getText().toString()));
						}
						else if(mObject instanceof BarcodeObject)
						{
							if (!mObject.mSource) {
								mObject.setContent(mContent.getText().toString());
							}
							
							((BarcodeObject) mObject).setCode(mCode.getText().toString());
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
							((ShiftObject) mObject).setShift(0, mShift1.getText().toString());
							((ShiftObject) mObject).setValue(0, mShiftVal1.getText().toString());
							((ShiftObject) mObject).setShift(1, mShift2.getText().toString());
							((ShiftObject) mObject).setValue(1, mShiftVal2.getText().toString());
							((ShiftObject) mObject).setShift(2, mShift3.getText().toString());
							((ShiftObject) mObject).setValue(2, mShiftVal3.getText().toString());
							((ShiftObject) mObject).setShift(3, mShift4.getText().toString());
							((ShiftObject) mObject).setValue(3, mShiftVal4.getText().toString());
						} else if (mObject instanceof GraphicObject) {
							
						}
						// mObject.setWidth(Float.parseFloat(mWidthEdit.getText().toString()));
						// mObject.setHeight(Float.parseFloat(mHighEdit.getText().toString()));
						Debug.d(TAG, "--->positive click");
						try {
							if (mHeightType.isChecked()) {
								mObject.setHeight(Integer.parseInt(mHeight_O.getText().toString()));
							} else {
								mObject.setHeight(mHighEdit.getText().toString());
							}
						} catch (Exception e) {
						}
						mObject.setX(Float.parseFloat(mXcorEdit.getText().toString())/2);
						mObject.setY(Float.parseFloat(mYcorEdit.getText().toString())/2);
						Debug.d(TAG, "content="+mContent.getText().toString());
						
						Resources res = mContext.getResources();
						
						String font = mFont.getText().toString();
						mObject.setFont(font);
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
				mHeight_O.setText(String.valueOf(mObject.getHeight()));
				mXcorEdit.setText(String.valueOf((int)mObject.getX()*2));
				mYcorEdit.setText(String.valueOf((int)mObject.getY()*2));
				mContent.setText(String.valueOf(mObject.getContent()));
				mReverse.setChecked(mObject.getReverse());
				
				mFont.setText(mObject.getFont());
				if(mObject instanceof RealtimeObject)
				{
					mRtFormat.setText(((RealtimeObject) mObject).getFormat());
					mOffset.setText(String.valueOf(((RealtimeObject) mObject).getOffset()));
				}
				else if(mObject instanceof CounterObject)
				{
					mDigits.setText(String.valueOf( ((CounterObject) mObject).getBits()));
					// mDir.setText( ((CounterObject) mObject).getDirection());
					mMin.setText(String.valueOf(((CounterObject) mObject).getMin()));
					mMax.setText(String.valueOf(((CounterObject) mObject).getMax()));
				}
				else if(mObject instanceof BarcodeObject)
				{
					mCode.setText(((BarcodeObject) mObject).getCode());
					mShow.setChecked(((BarcodeObject) mObject).getShow());
					mTextsize.setText(String.valueOf(((BarcodeObject) mObject).getTextsize()));
				}
				else if(mObject instanceof ShiftObject)
				{
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
				 mObject instanceof GraphicObject ||
				 mObject instanceof RTSecondObject ||
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
		mFormatAdapter = new PopWindowAdapter(mContext, null);
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
		
		String[] fonts = mContext.getResources().getStringArray(R.array.strFontArray);
		for (String font : fonts) {
			mFontAdapter.addItem(font);
		}
		
		String[] formats = mContext.getResources().getStringArray(R.array.strTimeFormat);
		for (String format : formats) {
			mFormatAdapter.addItem(format);
		}
		
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
		case R.id.rtFormat:
			mSpiner.setAdapter(mFormatAdapter);
			mSpiner.showAsDropUp(v);
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
		} else if (view == mRtFormat) {
			view.setText((String)mFormatAdapter.getItem(index));
		} else if (view == mLineType) {
			view.setText((String)mLineAdapter.getItem(index));
		} else if (view == mDir) {
			view.setText((String)mDirAdapter.getItem(index));
		} else if (view == mHighEdit) {
			String height = (String)mHeightAdapter.getItem(index);
			view.setText(height);
		} else if (view == mCode) {
			view.setText((String)mBarFormatAdapter.getItem(index));
		} else {
			Debug.d(TAG, "--->unknow view");
		}
	}

	@Override
	public void onCheckedChanged(CompoundButton view, boolean checked) {
		if (view == mHeightType) {
			if (checked) {
				mHeight_O.setEnabled(true);
				mHighEdit.setEnabled(false);
			} else {
				mHeight_O.setEnabled(false);
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
		mMin.setText("0");
		try {
			int max = (int) Math.pow(10, Integer.parseInt(mDigits.getText().toString())) - 1;
			mMax.setText(String.valueOf(max));
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
