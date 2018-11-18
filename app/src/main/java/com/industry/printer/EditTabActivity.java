package com.industry.printer;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import java.util.logging.Logger;

import com.industry.printer.FileFormat.TlkFileParser;
import com.industry.printer.FileFormat.TlkFileWriter;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.hardware.HardwareJni;
import com.industry.printer.hardware.RFIDDevice;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.CounterObject;
import com.industry.printer.object.TLKFileParser;
import com.industry.printer.object.GraphicObject;
import com.industry.printer.object.JulianDayObject;
import com.industry.printer.object.MessageObject;
import com.industry.printer.object.ObjectsFromString;
import com.industry.printer.object.RealtimeObject;
import com.industry.printer.object.ShiftObject;
import com.industry.printer.object.TextObject;
import com.industry.printer.ui.ExtendMessageTitleFragment;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter.IOnItemClickListener;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase.OnPositiveListener;
import com.industry.printer.ui.CustomerDialog.FileBrowserDialog;
import com.industry.printer.ui.CustomerDialog.LoadingDialog;
import com.industry.printer.ui.CustomerDialog.MessageBrowserDialog;
import com.industry.printer.ui.CustomerDialog.MessageSaveDialog;
import com.industry.printer.ui.CustomerDialog.ObjectInfoDialog;
import com.industry.printer.ui.CustomerDialog.ObjectInsertDialog;
import com.industry.printer.ui.CustomerDialog.TextBrowserDialog;
import com.industry.printer.widget.PopWindowSpiner;
import com.industry.printer.widget.SpanableStringFormator;

import android.app.Fragment;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Spannable;
import android.text.SpannableString;
import android.view.Display;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnLongClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.HorizontalScrollView;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.Toast;

public class EditTabActivity extends Fragment implements OnClickListener, OnLongClickListener, IOnItemClickListener, OnTouchListener {
	public static final String TAG="EditTabActivity";
	
	public Context mContext;
	public EditScrollView mObjView;
	public HorizontalScrollView mHScroll;
	
	public String mObjName;
	
	public ExtendMessageTitleFragment mMsgTitle;
	/*************************
	 * file operation buttons
	 * ***********************/
	public RelativeLayout mBtnNew;
	public RelativeLayout mBtnSave;
	public RelativeLayout mBtnSaveas;
	public RelativeLayout mBtnOpen;
	public RelativeLayout mPrev;
	public RelativeLayout mNext;
	public PopWindowSpiner mSpBtnSave;
	/*************************
	 * object operation buttons
	 ************************/
	public ImageButton mBtnLeft;			//move left
	public ImageButton mBtnRight;			//move right 
	public ImageButton mBtnUp;				//move up
	public ImageButton mBtnDown;		//move down 
	public ImageButton mBtnZoomoutX;//zoom out
	public ImageButton mBtnZoominX;	//zoom in
	public ImageButton mBtnZoomoutY;//zoom out
	public ImageButton mBtnZoominY;	//zoom in
	public ImageButton mDel;
	public ImageButton mTrans;
	
	/************************
	 * 鏍戣帗娲�3-缂栬緫鐣岄潰鐩稿叧鎸夐挳
	 ***********************/
	public Button	mInsert;
	public Button	mTest;
	public Button 	mTest5;
	/************************
	 * create Object buttons
	 * **********************/
	public ImageButton 	mBtnText;
	public ImageButton 	mBtnCnt;
	public ImageButton 	mBtnBar;
	public ImageButton	mImage;
	public ImageButton 	mBtnDay;
	public ImageButton 	mBtnTime;
	public ImageButton 	mBtnLine;
	public ImageButton 	mBtnRect;
	public ImageButton 	mBtnEllipse;
	public ImageButton	mShift;
	public ImageButton	mScnd;
	/**********************
	 * Object Information Table
	 * **********************/
	public ScrollView mViewInfo;
	public Spinner mObjList;
	Button mShowInfo;
	
	/***********************
	 * object edit lines for smfy-super3
	 **********************/
	public ScrollView mScrollView1;
	public EditText mObjLine1;
	public SpannableString mSpanText;
	// public EditText mObjLine2;
	// public EditText mObjLine3;
	// public EditText mObjLine4;
	
	
	public MessageTask mMsgTask;
	public static ArrayList<BaseObject> mObjs;
	public ArrayAdapter<String> mNameAdapter;
	
	public EditTabActivity() {
//		mMsgTitle = (ExtendMessageTitleFragment)fragment;
	}
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		return inflater.inflate(R.layout.edit_frame, container, false);
	}
	
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		
		super.onActivityCreated(savedInstanceState);
		//requestWindowFeature(Window.FEATURE_NO_TITLE);
		//setContentView(R.layout.edit_frame);
		//this.setVisible(false);
		mContext = getActivity();
		
		mObjs = new ArrayList<BaseObject>();
		mObjs.add(new MessageObject(mContext, 0));
		mHScroll = (HorizontalScrollView) getView().findViewById(R.id.scrollView1);
		
		mBtnNew = (RelativeLayout) getView().findViewById(R.id.btn_new);
		mBtnNew.setOnClickListener(this);
		mBtnNew.setOnTouchListener(this);
		
		mBtnOpen = (RelativeLayout) getView().findViewById(R.id.btn_open);
		mBtnOpen.setOnClickListener(this);
		mBtnOpen.setOnTouchListener(this);
		
		mBtnSaveas = (RelativeLayout) getView().findViewById(R.id.btn_saveas);
		mBtnSaveas.setOnClickListener(this);
		mBtnSaveas.setOnTouchListener(this);
		
		mBtnSave = (RelativeLayout) getView().findViewById(R.id.btn_save);
		mBtnSave.setOnClickListener(this);
		mBtnSave.setOnTouchListener(this);
		
		//mTest = (Button) getView().findViewById(R.id.btn_temp_4);
		//mTest.setOnClickListener(this);
		
		//mTest5 = (Button) getView().findViewById(R.id.btn_temp_5);
		//mTest5.setOnClickListener(this);
		
		mPrev = (RelativeLayout) getView().findViewById(R.id.btn_page_up);
		mPrev.setOnClickListener(this);
		mPrev.setOnTouchListener(this);
		
		mNext = (RelativeLayout) getView().findViewById(R.id.btn_page_down);
		mNext.setOnClickListener(this);
		mNext.setOnTouchListener(this);
		
		mScrollView1 = (ScrollView) getView().findViewById(R.id.edit_scrollview_line1);
		
		mObjLine1 = (EditText) getView().findViewById(R.id.edit_line1);
		mObjLine1.setText("");
		mObjLine1.setOnLongClickListener(this);
		mObjLine1.setOnEditorActionListener(new OnEditorActionListener() {
			
			@Override
			public boolean onEditorAction(TextView arg0, int arg1, KeyEvent arg2) {
				if (arg2 == null) {
					return false;
				}
				if (arg2.getKeyCode() == KeyEvent.KEYCODE_ENTER) {
					InputMethodManager imm = (InputMethodManager) getActivity().getSystemService(Context.INPUT_METHOD_SERVICE);  
					imm.toggleSoftInput(0, InputMethodManager.HIDE_NOT_ALWAYS);
					return true;
				} else {
					return false;
				}
			}
		});
		mSpanText = new SpannableString("");
		// 初始化下拉按钮界面
		mSpBtnSave = new PopWindowSpiner(getActivity());
		PopWindowAdapter adapter = new PopWindowAdapter(getActivity(), null);
		adapter.addItem(getResources().getString(R.string.str_btn_save));
		adapter.addItem(getResources().getString(R.string.str_btn_saveas));
		mSpBtnSave.setAdapter(adapter);
		mSpBtnSave.setFocusable(true);
		mSpBtnSave.setOnItemClickListener(this);
	}
	
//	@Override
//	public boolean onTouchEvent(MotionEvent event)
//	{
//		Debug.d(TAG, "event:"+event.toString());
//		InputMethodManager manager = (InputMethodManager)getSystemService(Service.INPUT_METHOD_SERVICE);
//		Debug.d(TAG, "ime is active? "+manager.isActive());
//		manager.hideSoftInputFromWindow(EditTabActivity.this.getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
////			manager.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_NOT_ALWAYS);
//		return true;
//	}
	
	/**
	 * REFRESH_OBJECT_CHANGED
	 *   some object changes, need to resave the tlk&bin files
	 */
	public static final int REFRESH_OBJECT_CHANGED=0;
	/**
	 * REFRESH_OBJECT_PROPERTIES
	 *   the object properties changed
	 */
	public static final int REFRESH_OBJECT_PROPERTIES=1;
	/**
	 * REFRESH_OBJECT_JUST
	 *   just refresh the object list, no need to resave tlk or bin files
	 */
	public static final int REFRESH_OBJECT_JUST=2;
	
	/**
	 * REFRESH_OBJECT_CHANGED
	 *   some object changes, need to resave the tlk&bin files
	 */
	public static final int REFRESH_TEXT_CHANGED=3;
	
	public Handler mObjRefreshHandler = new Handler(){
		@Override
		public void  handleMessage (Message msg)
		{
			
			switch (msg.what) {
			
			case REFRESH_OBJECT_CHANGED:	
				String content = "";
				String obj = "";
				Debug.d(TAG, "===========refresh object============");
				for (BaseObject object : mObjs) {
					if(object instanceof MessageObject) {
						continue;
					} else if (object instanceof TextObject) {
						obj = object.getContent();
					} else if (object instanceof RealtimeObject) {
						// obj = "#T#" + ((RealtimeObject)object).getContent();
						obj = ObjectsFromString.REALTIME_FLAG + ((RealtimeObject)object).getFormat();
					} else if (object instanceof CounterObject) {
						obj = ObjectsFromString.COUNTER_FLAG + ((CounterObject)object).getContent();
					} else {
						continue;
					}
					
					if (content.isEmpty()) {
						content += obj;
					} else {
						content += ObjectsFromString.SPLITOR + obj;
					}
					
				}
				Debug.d(TAG, "===========refresh object============");
				mObjLine1.setText(new SpanableStringFormator(content));
				break;
			case REFRESH_TEXT_CHANGED:
				Bundle bundle = msg.getData();
				String text = (String)bundle.getString("key_text");
				mObjLine1.setText(new SpanableStringFormator(text));
				break;
			case REFRESH_OBJECT_PROPERTIES:
				OnPropertyChanged(true);
			case REFRESH_OBJECT_JUST:
				//mNameAdapter.notifyDataSetChanged();
				break;
			default:
				break;
			}
		}
	};
	
	
	public static BaseObject getCurObj()
	{
		if (mObjs == null) {
			return null;
		}
		for(BaseObject obj : mObjs)
		{
			if(obj.getSelected())
				return obj;
		}
		if(mObjs!=null&& mObjs.size()>0)
		{
			BaseObject object=mObjs.get(0);
			object.setSelected(true);
			return object;
		}
		return null;
	}
	
	public static void setCurObj(int i)
	{
		if(i >= mObjs.size())
			return;
		BaseObject obj=mObjs.get(i);
		obj.setSelected(true);
	}
	
	public static float getNextXcor()
	{
		float x=0;
		for(BaseObject obj : mObjs)
		{
			if(obj instanceof MessageObject)
				continue;
			x = obj.getXEnd()>x ? obj.getXEnd() : x;
		}
		return x;
	}

	/**
	 * HANDLER_MESSAGE_NEW
	 *  Handler message for new a tlk file
	 */
	public static final int HANDLER_MESSAGE_NEW=0;
	
	/**
	 * HANDLER_MESSAGE_OPEN
	 *  Handler message for open tlk file
	 */
	public static final int HANDLER_MESSAGE_OPEN=1;
	
	/**
	 * HANDLER_MESSAGE_SAVE
	 *   Handler message for save event happens
	 */
	public static final int HANDLER_MESSAGE_SAVE=2;
	
	/**
	 * HANDLER_MESSAGE_SAVEAS
	 *   Handler message for saveas event happens
	 */
	public static final int HANDLER_MESSAGE_SAVEAS=3;
	
	/**
	 * HANDLER_MESSAGE_IMAGESELECT
	 *   Handler message for image object selected
	 */
	public static final int HANDLER_MESSAGE_IMAGESELECT=4;

	/**
	 * HANDLER_MESSAGE_DISMISSDIALOG
	 *   Handler message for dismiss loading dialog
	 */
	public static final int HANDLER_MESSAGE_DISMISSDIALOG=5;
	
	/**
	 * HANDLER_MESSAGE_INSERT_MSG
	 * Handler message for insert a new object
	 */
	public static final int HANDLER_MESSAGE_INSERT_OBJECT = 6;
	
	
	Handler mHandler = new Handler(){
		public void handleMessage(Message msg) {  
			//	String f;
			String title = getResources().getString(R.string.str_file_title);;
			boolean createfile=false;
            switch (msg.what) {

        		case HANDLER_MESSAGE_NEW:
        			mObjName = null;
        			mObjLine1.setText("");
        			((MainActivity) getActivity()).mEditTitle.setText(getResources().getString(R.string.str_filename_no));
        			break;
            	case HANDLER_MESSAGE_OPEN:		//open
            		Debug.d(TAG, "open file="+MessageBrowserDialog.getSelected());
					List<String> objects = MessageBrowserDialog.getSelected();
					mObjName = objects.get(0);
            		if (mObjName == null || mObjName.isEmpty()) {
						break;
					}
            		mMsgTask = new MessageTask(mContext, mObjName);
            		mObjs = mMsgTask.getObjects();
    				setCurObj(0);
    				mObjRefreshHandler.sendEmptyMessage(REFRESH_OBJECT_CHANGED);
            		((MainActivity) getActivity()).mEditTitle.setText(title + mObjName);
            		//mMsgTitle.setTitle(mObjName);
            		break;
            		
            	case HANDLER_MESSAGE_SAVEAS:		//saveas
            		Debug.d(TAG, "save as file="+MessageSaveDialog.getTitle());
            		mObjName = MessageSaveDialog.getTitle();
            		createfile=true;
            		
            	case HANDLER_MESSAGE_SAVE:    //save
            		progressDialog();
            		if (mObjName == null) {
						break;
					}
            		mMsgTask = new MessageTask(mContext, mObjName, mObjLine1.getText().toString());
            		mMsgTask.createTaskFolderIfNeed();
            		
            		//保存1.bin文件
            		mMsgTask.saveBin();
            		//保存1.TLK文件
           			mMsgTask.saveTlk(mContext);
           			//保存vx.bin文件
           			mMsgTask.saveVarBin();
           			
            		dismissProgressDialog();
            		((MainActivity) getActivity()).mEditTitle.setText(title + mObjName);

            		break;
            		
            	case HANDLER_MESSAGE_IMAGESELECT:		//select image
            		File file = new File(FileBrowserDialog.file());
            		if(!file.exists() || !GraphicObject.isPic(FileBrowserDialog.file()))
            		{
            			Debug.d(TAG, "Image file error");
            			return;
            		}
            		setCurObj(0);
            		GraphicObject o = new GraphicObject(mContext, getNextXcor());
            		o.setImage(FileBrowserDialog.file());
    				mObjs.add(o);    	
    				mObjRefreshHandler.sendEmptyMessage(REFRESH_OBJECT_JUST);
            		break;
            		
            	case HANDLER_MESSAGE_DISMISSDIALOG:
            		mProgressDialog.dismiss();
            		break;
            	case HANDLER_MESSAGE_INSERT_OBJECT:
            		Bundle bundle = msg.getData();
            		if (bundle == null) {
						break;
					}
            		String text = bundle.getString("object", null);
            		if (text != null) {
            			String content = mObjLine1.getText().toString();
            			Message message = mObjRefreshHandler.obtainMessage(REFRESH_TEXT_CHANGED);
        				Bundle bundle2 = new Bundle();
        				
            			if (content.isEmpty() || content.endsWith(ObjectsFromString.SPLITOR)) {
            				bundle2.putString("key_text", content + text);
            				// mObjLine1.append(text);
						} else {
							bundle2.putString("key_text",  content + ObjectsFromString.SPLITOR  + text);
							// mObjLine1.append(ObjectsFromString.SPLITOR + text);
						}
            			message.setData(bundle2);
            			mObjRefreshHandler.removeMessages(REFRESH_TEXT_CHANGED);
        				mObjRefreshHandler.sendMessage(message);
        				
					}
            		
            		break;
            }   
            super.handleMessage(msg);   
       } 
	};
	
	
	int getTouchedObj(float x, float y)
	{
		int i=1;
		BaseObject o;
		for(i=1; mObjs!= null &&i< mObjs.size(); i++)
		{
			o = mObjs.get(i);
			if(x>= o.getX() && x<= o.getXEnd() && y >=o.getY() && y <= o.getYEnd())
			{
				Debug.d(TAG, "Touched obj = "+i);
				return i;
			}
		}
		Debug.d(TAG, "no object Touched");
		return -1;
	}
	
	/**
	 * 使用系统字库，生成bitmap，然后通过灰度化和二值化之后提取点阵生成buffer
	 * @param f
	 */
//	public void saveObjectBin(String f)
//	{
//		int width=0;
//		Paint p=new Paint();
//		if(mObjs==null || mObjs.size() <= 0)
//			return ;
//		for(BaseObject o:mObjs)
//		{
//			width = (int)(width > o.getXEnd() ? width : o.getXEnd());
//		}
//		
//		Bitmap bmp = Bitmap.createBitmap(width , Configs.gDotsTotal, Configs.BITMAP_CONFIG);
//		Debug.d(TAG, "drawAllBmp width="+width+", height="+Configs.gDotsTotal);
//		Canvas can = new Canvas(bmp);
//		can.drawColor(Color.WHITE);
//		for(BaseObject o:mObjs)
//		{
//			if((o instanceof MessageObject)	)
//				continue;
//			
//			if(o instanceof CounterObject)
//			{
//				o.drawVarBitmap(f);
//			}
//			else if(o instanceof RealtimeObject)
//			{
//				Bitmap t = ((RealtimeObject)o).getBgBitmap(mContext,f);
//				can.drawBitmap(t, o.getX(), o.getY(), p);
//				BinFromBitmap.recyleBitmap(t);
//			}
//			else if(o instanceof JulianDayObject)
//			{
//				o.drawVarBitmap(f);
//			}
//			else if(o instanceof ShiftObject)
//			{
//				o.drawVarBitmap(f);
//			}
//			else
//			{
//				Bitmap t = o.getScaledBitmap(mContext);
//				can.drawBitmap(t, o.getX(), o.getY(), p);
//				BinFromBitmap.recyleBitmap(t);
//			}
//		//can.drawText(mContent, 0, height-30, mPaint);
//		}
//		// 生成bin文件
//		BinFileMaker maker = new BinFileMaker(mContext);
//		maker.extract(bmp);
//		// 保存bin文件
//		maker.save(f + "/1.bin");
//		
//		return ;
//	}
//	
//	/**
//	 * 从16*16的点阵字库中提取点阵，生成打印buffer
//	 * @param f
//	 */
//	public void saveBinDotMatrix(String f) {
//		if(mObjs==null || mObjs.size() <= 0)
//			return ;
//		
//		String content="";
//		for(BaseObject o:mObjs)
//		{
//			if((o instanceof MessageObject)	)
//				continue;
//			
//			if(o instanceof CounterObject)
//			{
//				content += o.getContent();
//			}
//			else if(o instanceof RealtimeObject)
//			{
//				content += o.getContent();
//				Debug.d(TAG, "--->realtime: " + content);
//			}
//			else if(o instanceof JulianDayObject)
//			{
//				content += o.getContent();
//			}
//			else if(o instanceof ShiftObject)
//			{
//				content += o.getContent();
//			}
//			else
//			{
//				content += o.getContent();
//			}
//		//can.drawText(mContent, 0, height-30, mPaint);
//		}
//		// 生成bin文件
//		BinFileMaker maker = new BinFileMaker(mContext);
//		int dotCount = maker.extract(content);
//		// 保存bin文件
//		maker.save(f + "/1.bin");
//		for(BaseObject o:mObjs)
//		{
//			if((o instanceof MessageObject)	) {
//				((MessageObject) o).setDotCount(dotCount);
//				break;
//			}
//		}
//		
//		return ;
//	}
	
	public LoadingDialog mProgressDialog;
	public Thread mProgressThread;
	public boolean mProgressShowing;
	public void progressDialog()
	{
		mProgressDialog = LoadingDialog.show(mContext, R.string.strSaving);
		mProgressShowing = true;
		
		mProgressThread = new Thread(){
			
			@Override
			public void run(){
				
				try{
					for(;mProgressShowing==true;)
					{
						Thread.sleep(2000);
					}
					mHandler.sendEmptyMessage(HANDLER_MESSAGE_DISMISSDIALOG);
				}catch(Exception e)
				{
					
				}
			}
		};
		mProgressThread.start();
	}
	
	public void dismissProgressDialog()
	{
		mProgressShowing=false;
		//Thread thread = mProgressThread;
		//thread.interrupt();
		
	}
	
	public boolean mPropertyChanged=false;
	private void OnPropertyChanged(boolean state)
	{
		mPropertyChanged=state;
	}
	
	public boolean isPropertyChanged()
	{
		return mPropertyChanged;
	}

	@Override
	public void onClick(View arg0) {
		CustomerDialogBase dialog;
		switch (arg0.getId()) {
			case R.id.btn_new:
				mHandler.sendEmptyMessage(HANDLER_MESSAGE_NEW);
				break;
			case R.id.btn_open:	//test fpga-gpio write
				
				dialog = new MessageBrowserDialog(mContext);
				dialog.setOnPositiveClickedListener(new OnPositiveListener() {
					
					@Override
					public void onClick() {
						mHandler.sendEmptyMessage(HANDLER_MESSAGE_OPEN);
					}

					@Override
					public void onClick(String content) {
						// TODO Auto-generated method stub
						
					}
				});
				dialog.show();
				
				break;
			case R.id.btn_save:
				mSpBtnSave.setWidth(mBtnSave.getWidth());
				mSpBtnSave.showAsDropDown(mBtnSave);
				
				
				break;
			case R.id.btn_saveas:
				/*
				if (mObjLine1.getText().toString().isEmpty()) {
					break;
				}
				getObjectList();
				dialog = new MessageSaveDialog(mContext);
				dialog.setOnPositiveClickedListener(new OnPositiveListener() {
					
					@Override
					public void onClick() {
						mHandler.sendEmptyMessage(HANDLER_MESSAGE_SAVEAS);
					}

					@Override
					public void onClick(String content) {
						// TODO Auto-generated method stub
						
					}
				});
				dialog.show();
				*/
				ObjectInsertDialog dialog1 = new ObjectInsertDialog(getActivity());
				dialog1.show();
				Message msg = mHandler.obtainMessage();
				msg.what = HANDLER_MESSAGE_INSERT_OBJECT;
				dialog1.setDismissMessage(msg);
				break;
			/*
			case R.id.btn_temp_4:
				//制卡
				device = RFIDDevice.getInstance();
				device.makeCard();
				break;
			case R.id.btn_temp_5:
				
				//writer.connect();
				//writer.setType();
				//寻卡
				device = RFIDDevice.getInstance();
				device.init();
				break;
			*/
			case R.id.btn_page_up:
				float y = mObjLine1.getTranslationY();
				Debug.d(TAG, "===>y="+y);
				//mObjLine1.scrollBy(0, -100);
				mScrollView1.smoothScrollBy(0, -50);
				break;
			case R.id.btn_page_down:

				float ny = mObjLine1.getY();
				Debug.d(TAG, "===>y="+ny);
				ny = mObjLine1.getScaleY();
				Debug.d(TAG, "===>sy="+ny);
				mScrollView1.smoothScrollBy(0, 50);
				//mObjLine1.scrollBy(0, 100);
				break;
			
			default:
				
				break;
		}
	}
	
	private void onSaveClicked(boolean saveas) {
		CustomerDialogBase dialog;
		if (mObjLine1.getText().toString().isEmpty()) {
			return;
		}
		
		if (!saveas && mObjName != null) {
			mHandler.sendEmptyMessage(HANDLER_MESSAGE_SAVE);
		} else {
			dialog = new MessageSaveDialog(mContext);
			dialog.setOnPositiveClickedListener(new OnPositiveListener() {
				
				@Override
				public void onClick() {
					mHandler.sendEmptyMessage(HANDLER_MESSAGE_SAVEAS);
				}

				@Override
				public void onClick(String content) {
					
				}
			});
			dialog.show();
		}
	}
	
	
	@Override
	public boolean onLongClick(View arg0) {
		TextBrowserDialog dialog = new TextBrowserDialog(mContext, "txt");
		dialog.setOnPositiveClickedListener(new OnPositiveListener() {
			
			@Override
			public void onClick() {
				
			}
			@Override
			public void onClick(String content) {
				Debug.d(TAG, "--->content: " +content);
				mObjLine1.setText(content);
			}
		});
		dialog.show();
		return false;
	}
	@Override
	public void onItemClick(int index) {
		Debug.d(TAG, "--->onPopWinowdItemClick: " + index);
		if (index == 0) {
			onSaveClicked(false);
		} else {
			onSaveClicked(true);
		}
	}
	
	@Override
	public boolean onTouch(View view, MotionEvent event) {
		switch(view.getId()) {
			case R.id.StartPrint:
			case R.id.StopPrint:
			case R.id.btnFlush:
			case R.id.btnBinfile:
			case R.id.btn_page_forward:
			case R.id.btn_page_backward:
				if (event.getAction() == MotionEvent.ACTION_DOWN) {
//					PWMAudio.Play();
				}
			default:
				break;
		}
		return false;
	}
	
}
