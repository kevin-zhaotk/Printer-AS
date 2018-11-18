package com.industry.printer.ui.CustomerAdapter;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter.IOnItemClickListener;
import com.industry.printer.ui.CustomerDialog.HeaderSelectDialog;
import com.industry.printer.ui.CustomerDialog.NewMessageDialog;
import com.industry.printer.ui.CustomerDialog.ObjectInfoDialog;
import com.industry.printer.widget.PopWindowSpiner;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class SettingsListAdapter extends BaseAdapter implements OnClickListener, IOnItemClickListener {

	private final static String TAG = SettingsListAdapter.class.getSimpleName();
	
	private Context mContext;
	public SystemConfigFile mSysconfig;
	private ItemViewHolder mHolder;
	
	/**
	 * An inflater for inflate the view
	 */
	private LayoutInflater mInflater;
	
	private String[] mTitles;
	
	public PopWindowSpiner mSpiner;
	public PopWindowAdapter mEncoderAdapter;
	public PopWindowAdapter mTrigerMode;
	private PopWindowAdapter mDirection;
	private PopWindowAdapter mResolution;
	private PopWindowAdapter mPhotocell;
	private PopWindowAdapter mRepeat;
	private PopWindowAdapter mNozzle;
	private PopWindowAdapter mPen1Mirror;
	private PopWindowAdapter mPen2Mirror;
	private PopWindowAdapter mPen3Mirror;
	private PopWindowAdapter mPen4Mirror;

	private PopWindowAdapter mPen1Invert;
	private PopWindowAdapter mPen2Invert;
	private PopWindowAdapter mPen3Invert;
	private PopWindowAdapter mPen4Invert;
	
	private PopWindowAdapter mPens;
	private PopWindowAdapter mAutoVol;
	private PopWindowAdapter mAutoPulse;
	private PopWindowAdapter mDots;
	private PopWindowAdapter mHandle;
	private PopWindowAdapter mCntReset;
//	private PopWindowAdapter mQRsource;
	
	private ItemViewHolder mEncoderHolder;
	private HashMap<Integer, ItemViewHolder> mHoldMap;
	
	private ItemOneLine[] mSettingItems = new ItemOneLine[64];
	
	/**
	 * A customerized view holder for widgets 
	 * @author zhaotongkai
	 */
	private class ItemViewHolder{
		public TextView	mTitleL;		//message title
		public TextView	mValueLTv;
		public EditText	mValueLEt;
		public TextView	mUnitL;
		
		public TextView	mTitleR;		//message title
		public TextView	mValueRTv;
		public EditText	mValueREt;
		public TextView	mUnitR;
	}
	
	private class ItemType {
		public static final int TYPE_NONE = 0;
		public static final int TYPE_SWITCH = 1;
		public static final int TYPE_DIRECTION = 2;
		public static final int TYPE_VALUE = 3;
		public static final int TYPE_ARRAY = 4;
	}
	
	public Handler handler = new Handler(){
		@Override
		public void handleMessage(Message msg) {
			switch(msg.what) {
			case ObjectInfoDialog.MSG_SELECTED_HEADER:
				int index = msg.arg1;
				Debug.d(TAG, "--->index: " + index);
				mSettingItems[30].setValue(index);
				mSysconfig.setParam(30, mSettingItems[30].getValue());
				String value = mSettingItems[30].getDisplayValue();
				
				notifyDataSetChanged();
				break;
			}
		}
	};
	
	private class ItemOneLine {
		public int mParamId;
		public int mTitle;
		public String mValue;
		public int mEntry;
		public int mUnit;
		private int mType;
		
		public ItemOneLine(int param, int title, int unit) {
			mParamId = param;
			mTitle = title;
			mValue = String.valueOf(mSysconfig.getParam(param - 1));
			mUnit = unit;
			mType = ItemType.TYPE_NONE;
		}
		
		public ItemOneLine(int param, int title, int unit, int type) {
			this(param, title, unit);
			mType = type;
		}
		
		public ItemOneLine(int param, int title, int entry, int unit, int type) {
			mParamId = param;
			mTitle = title;
			mUnit = unit;
			mType = type;
			mEntry = entry;
			if (mType == ItemType.TYPE_SWITCH || mType == ItemType.TYPE_DIRECTION) {
				mValue = getEntry(entry, mSysconfig.getParam(param -1));
			} else if (mType == ItemType.TYPE_VALUE) {
				mValue = String.valueOf(mSysconfig.getParam(param-1));
			} else if (mType == ItemType.TYPE_ARRAY) {
				mValue = getEntry(entry, mSysconfig.getParam(param -1));
			}
			
		}
		
		public void setValue(int value) {
			switch (mType) {
			case ItemType.TYPE_NONE:
				mValue = String.valueOf(value);
				break;
			case ItemType.TYPE_SWITCH:
			case ItemType.TYPE_DIRECTION:
				mSysconfig.setParam(mParamId-1, value);
				mValue = getEntry(mEntry, value);
				break;
			case ItemType.TYPE_VALUE:
				mValue = getEntry(mEntry, value);
				try {
					mSysconfig.setParam(mParamId-1, Integer.parseInt(mValue));
				} catch (Exception e) {
					mSysconfig.setParam(mParamId-1, value);
				}
				
				break;
			case ItemType.TYPE_ARRAY:
				mValue = getEntry(mEntry, value);
				mSysconfig.setParam(mParamId-1, value);
				break;
			default:
				break;
			}
		}
		
		public void setValue(String value) {
			mValue = value;
		}
		
		public String getDisplayValue() {
			return mValue;
		}
		
		public int getValue() {
			return mSysconfig.getParam(mParamId - 1);
		}
	}
	
	public void setParam(int param, int value) {
		if (mSettingItems == null ||param >= mSettingItems.length) {
			return;
		}
		mSettingItems[param].setValue(value);
		mSysconfig.setParam(param, value);
		notifyDataSetChanged();
	}
	
	public SettingsListAdapter(Context context) {
		mContext = context;
		mSysconfig = SystemConfigFile.getInstance(mContext);
		mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		// mTitles = mContext.getResources().getStringArray(R.array.str_settings_params);
		loadSettings();
		mHoldMap = new HashMap<Integer, ItemViewHolder>();
		
		mEncoderAdapter = new PopWindowAdapter(mContext, null);
		mTrigerMode = new PopWindowAdapter(mContext, null);
		mDirection = new PopWindowAdapter(mContext, null);
		mResolution = new PopWindowAdapter(mContext, null);
		mPhotocell = new PopWindowAdapter(mContext, null);
		mRepeat = new PopWindowAdapter(mContext, null);
		mNozzle = new PopWindowAdapter(mContext, null);
		mPen1Mirror = new PopWindowAdapter(mContext, null);
		mPen2Mirror = new PopWindowAdapter(mContext, null);
		mPen3Mirror = new PopWindowAdapter(mContext, null);
		mPen4Mirror = new PopWindowAdapter(mContext, null);
		
		mPen1Invert = new PopWindowAdapter(mContext, null);
		mPen2Invert = new PopWindowAdapter(mContext, null);
		mPen3Invert = new PopWindowAdapter(mContext, null);
		mPen4Invert = new PopWindowAdapter(mContext, null);
		
		mPens = new PopWindowAdapter(mContext, null);
		mHandle = new PopWindowAdapter(mContext, null);
		
		mAutoVol = new PopWindowAdapter(mContext, null);
		mAutoPulse = new PopWindowAdapter(mContext, null);
		mDots = new PopWindowAdapter(mContext, null);
		mCntReset = new PopWindowAdapter(mContext, null);
		// mQRsource = new PopWindowAdapter(mContext, null);
		initAdapters();
	}
	
	@Override
	public int getCount() {
		Debug.d(TAG, "--->getCount=" + mSettingItems.length);
		return mSettingItems.length/2;
		//return Configs.gParams;
	}

	@Override
	public Object getItem(int arg0) {
		Debug.d(TAG, "--->getItem");
		return null;
	}

	@Override
	public long getItemId(int arg0) {
		return arg0;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		
		if(convertView != null) {
			mHolder = (ItemViewHolder) convertView.getTag();
		}
		else
		{
			//prepare a empty view 
			convertView = mInflater.inflate(R.layout.settings_frame_item, null);
			mHolder = new ItemViewHolder();
			//Left 
			mHolder.mTitleL = (TextView) convertView.findViewById(R.id.setting_title_left);
			mHolder.mValueLTv = (TextView) convertView.findViewById(R.id.setting_value_left_tv);
			mHolder.mValueLEt = (EditText) convertView.findViewById(R.id.setting_value_left_et);
			mHolder.mUnitL = (TextView) convertView.findViewById(R.id.setting_unit_left);
			
			//Right
			mHolder.mTitleR = (TextView) convertView.findViewById(R.id.setting_title_right);
			mHolder.mValueRTv = (TextView) convertView.findViewById(R.id.setting_value_right_tv);
			mHolder.mValueREt = (EditText) convertView.findViewById(R.id.setting_value_right_et);
			mHolder.mUnitR = (TextView) convertView.findViewById(R.id.setting_unit_right);
			
			
			convertView.setTag(mHolder);
			mHoldMap.put(position, mHolder);
			
		}
		mHolder.mValueLEt.setTag(2*position);
		mHolder.mValueLTv.setTag(2*position);
		mHolder.mValueREt.setTag(2*position+1);
		mHolder.mValueRTv.setTag(2*position+1);
		mHolder.mValueLEt.addTextChangedListener(new SelfTextWatcher(mHolder.mValueLEt));
		mHolder.mValueREt.addTextChangedListener(new SelfTextWatcher(mHolder.mValueREt));
		
		if (mSettingItems[2*position].mUnit > 0) {
			mHolder.mUnitL.setText(mSettingItems[2*position].mUnit);
		} else {
			mHolder.mUnitL.setText("");
		}
		if (mSettingItems[2*position+1].mUnit > 0) {
			mHolder.mUnitR.setText(mSettingItems[2*position+1].mUnit);
		} else {
			mHolder.mUnitR.setText("");
		}
		mHolder.mTitleL.setText(mContext.getString(mSettingItems[2*position].mTitle));
		mHolder.mTitleR.setText(mContext.getString(mSettingItems[2*position+1].mTitle));
		Debug.d(TAG, "===>getView position=" + position);
		
		if (mSettingItems[2*position].mType != ItemType.TYPE_NONE) {
			mHolder.mValueLTv.setVisibility(View.VISIBLE);
			mHolder.mValueLEt.setVisibility(View.GONE);
			mHolder.mValueLTv.setText(mSettingItems[2*position].getDisplayValue());
			mHolder.mValueLTv.setOnClickListener(this);
		} else {
			mHolder.mValueLTv.setVisibility(View.GONE);
			mHolder.mValueLEt.setVisibility(View.VISIBLE);
			Debug.d(TAG, "--->getView:left=" + mSettingItems[2*position].mValue + "---right=" + mSettingItems[2*position+1].mValue);
			mHolder.mValueLEt.setText(mSettingItems[2*position].getDisplayValue());
		}
		
		if (mSettingItems[2*position + 1].mType != ItemType.TYPE_NONE) {
			mHolder.mValueRTv.setVisibility(View.VISIBLE);
			mHolder.mValueREt.setVisibility(View.GONE);
			mHolder.mValueRTv.setText(mSettingItems[2*position+1].getDisplayValue());
			mHolder.mValueRTv.setOnClickListener(this);
		} else {
			mHolder.mValueRTv.setVisibility(View.GONE);
			mHolder.mValueREt.setVisibility(View.VISIBLE);
			mHolder.mValueREt.setText(mSettingItems[2*position+1].getDisplayValue());
		}
		
		return convertView;
	}
	
	public void loadSettings() {
		Debug.d(TAG, "--->loadSettings");
		
		mSettingItems[0] = new ItemOneLine(1, R.string.str_textview_param1, R.string.str_time_unit_mm_s);
		mSettingItems[1] = new ItemOneLine(2, R.string.str_textview_param2, R.array.direction_item_entries, 0, ItemType.TYPE_DIRECTION);
		mSettingItems[2] = new ItemOneLine(3, R.string.str_textview_param3, R.array.resolution_item_entries, R.string.strResunit, ItemType.TYPE_VALUE);
		mSettingItems[3] = new ItemOneLine(4, R.string.str_textview_param4, R.string.str_length_unit_mm);
		mSettingItems[4] = new ItemOneLine(5, R.string.str_textview_param5, R.array.photo_item_entries, 	0, ItemType.TYPE_SWITCH);
		mSettingItems[5] = new ItemOneLine(6, R.string.str_textview_param6, R.array.switch_item_entries, 	0, ItemType.TYPE_SWITCH);
		mSettingItems[6] = new ItemOneLine(7, R.string.str_textview_param7, R.string.str_length_unit_mm);
		mSettingItems[7] = new ItemOneLine(8, R.string.str_textview_param8, R.array.direction_item_entries, 0, ItemType.TYPE_DIRECTION);
		mSettingItems[8] = new ItemOneLine(9, R.string.str_textview_param9, R.string.str_length_unit_mm);
		mSettingItems[9] = new ItemOneLine(10,R.string.str_textview_param10,0);
		mSettingItems[10] = new ItemOneLine(11, R.string.str_textview_param11, R.string.str_length_unit_6_1mm);
		mSettingItems[11] = new ItemOneLine(12, R.string.str_textview_param12, R.string.str_length_unit_6_1mm);
		mSettingItems[12] = new ItemOneLine(13, R.string.str_textview_param13, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[13] = new ItemOneLine(14, R.string.str_textview_param14, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[14] = new ItemOneLine(15, R.string.str_textview_param15, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[15] = new ItemOneLine(16, R.string.str_textview_param16, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[16] = new ItemOneLine(17, R.string.str_textview_param17, 0);
		mSettingItems[17] = new ItemOneLine(18, R.string.str_textview_param18, 0);
		mSettingItems[18] = new ItemOneLine(19, R.string.str_textview_param19, R.string.str_length_unit_6_1mm);
		mSettingItems[19] = new ItemOneLine(20, R.string.str_textview_param20, R.string.str_length_unit_6_1mm);
		mSettingItems[20] = new ItemOneLine(21, R.string.str_textview_param21, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[21] = new ItemOneLine(22, R.string.str_textview_param22, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[22] = new ItemOneLine(23, R.string.str_textview_param23, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[23] = new ItemOneLine(24, R.string.str_textview_param24, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[24] = new ItemOneLine(25, R.string.str_textview_param25, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[25] = new ItemOneLine(26, R.string.str_textview_param26, R.string.str_time_unit_0_1v);
		mSettingItems[26] = new ItemOneLine(27, R.string.str_textview_param27, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[27] = new ItemOneLine(28, R.string.str_textview_param28, R.string.str_time_unit_0_1us);
		mSettingItems[28] = new ItemOneLine(29, R.string.str_textview_param29, R.string.str_time_unit_ms);
		mSettingItems[29] = new ItemOneLine(30, R.string.str_textview_param30, R.string.str_time_unit_ms);
		mSettingItems[30] = new ItemOneLine(31, R.string.str_textview_param31, R.array.strPrinterArray, 0, ItemType.TYPE_ARRAY);
		mSettingItems[31] = new ItemOneLine(32, R.string.str_textview_param32, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[32] = new ItemOneLine(33, R.string.str_textview_param33, R.string.str_time_unit_us);
		mSettingItems[33] = new ItemOneLine(34, R.string.str_textview_param34, R.string.str_time_unit_us);
		mSettingItems[34] = new ItemOneLine(35, R.string.str_textview_param35, R.string.str_time_unit_us);
		mSettingItems[35] = new ItemOneLine(36, R.string.str_textview_param36, R.string.str_time_unit_us);
		mSettingItems[36] = new ItemOneLine(37, R.string.str_textview_param37, R.string.str_time_unit_us);
		mSettingItems[37] = new ItemOneLine(38, R.string.str_textview_param38, 0);
		mSettingItems[38] = new ItemOneLine(39, R.string.str_textview_param39, 0);
		mSettingItems[39] = new ItemOneLine(40, R.string.str_textview_param40, R.array.message_dots,	0,	ItemType.TYPE_VALUE);
		mSettingItems[40] = new ItemOneLine(41, R.string.str_textview_param41, R.array.switch_item_entries, 0, ItemType.TYPE_SWITCH);
		mSettingItems[41] = new ItemOneLine(42, R.string.str_textview_param42, 0);
		mSettingItems[42] = new ItemOneLine(43, R.string.str_textview_param43, 0);
		mSettingItems[43] = new ItemOneLine(44, R.string.str_textview_param44, 0);
		mSettingItems[44] = new ItemOneLine(45, R.string.str_textview_param45, 0);
		mSettingItems[45] = new ItemOneLine(46, R.string.str_textview_param46, 0);
		mSettingItems[46] = new ItemOneLine(47, R.string.str_textview_param47, 0);
		mSettingItems[47] = new ItemOneLine(48, R.string.str_textview_param48, 0);
		mSettingItems[48] = new ItemOneLine(49, R.string.str_textview_param49, 0);
		mSettingItems[49] = new ItemOneLine(50, R.string.str_textview_param50, 0);
		mSettingItems[50] = new ItemOneLine(51, R.string.str_textview_param51, 0);
		mSettingItems[51] = new ItemOneLine(52, R.string.str_textview_param52, 0);
		mSettingItems[52] = new ItemOneLine(53, R.string.str_textview_param53, 0);
		mSettingItems[53] = new ItemOneLine(54, R.string.str_textview_param54, 0);
		mSettingItems[54] = new ItemOneLine(55, R.string.str_textview_param55, 0);
		mSettingItems[55] = new ItemOneLine(56, R.string.str_textview_param56, 0);
		mSettingItems[56] = new ItemOneLine(57, R.string.str_textview_param57, 0);
		mSettingItems[57] = new ItemOneLine(58, R.string.str_textview_param58, 0);
		mSettingItems[58] = new ItemOneLine(59, R.string.str_textview_param59, 0);
		mSettingItems[59] = new ItemOneLine(60, R.string.str_textview_param60, 0);
		mSettingItems[60] = new ItemOneLine(61, R.string.str_textview_param61, 0);
		mSettingItems[61] = new ItemOneLine(62, R.string.str_textview_param62, 0);
		mSettingItems[62] = new ItemOneLine(63, R.string.str_textview_param63, 0);
		mSettingItems[63] = new ItemOneLine(64, R.string.str_textview_param64, 0);
		Debug.d(TAG, "--->loadSettings");
	}
	
	private void initAdapters() {
		
		mSpiner = new PopWindowSpiner(mContext);
		mSpiner.setFocusable(true);
		mSpiner.setOnItemClickListener(this);
		
		String[] items = mContext.getResources().getStringArray(R.array.switch_item_entries); 
		// ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, R.layout.spinner_item, R.id.textView_id, items);
		for (int i = 0; i < items.length; i++) {
			mEncoderAdapter.addItem(items[i]);
		}
		
		items = mContext.getResources().getStringArray(R.array.array_triger_mode); 
		// ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, R.layout.spinner_item, R.id.textView_id, items);
		for (int i = 0; i < items.length; i++) {
			mTrigerMode.addItem(items[i]);
		}
		
		items = mContext.getResources().getStringArray(R.array.direction_item_entries);
		for (int i = 0; i < items.length; i++) {
			mDirection.addItem(items[i]);
		}
		
		items = mContext.getResources().getStringArray(R.array.resolution_item_entries);
		for (int i = 0; i < items.length; i++) {
			mResolution.addItem(items[i]);
		}

		items = mContext.getResources().getStringArray(R.array.photo_item_entries);
		for (int i = 0; i < items.length; i++) {
			mPhotocell.addItem(items[i]);
		}
		
		items = mContext.getResources().getStringArray(R.array.direction_item_entries);
		for (int i = 0; i < items.length; i++) {
			mRepeat.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mNozzle.addItem(items[i]);
		}
		
		items = mContext.getResources().getStringArray(R.array.switch_item_entries);
		for (int i = 0; i < items.length; i++) {
			mPen1Mirror.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen2Mirror.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen3Mirror.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen4Mirror.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen1Invert.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen2Invert.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen3Invert.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mPen4Invert.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mAutoVol.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mAutoPulse.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mCntReset.addItem(items[i]);
		}
		for (int i = 0; i < items.length; i++) {
			mHandle.addItem(items[i]);
		}
//		for (int i = 0; i < items.length; i++) {
//			mQRsource.addItem(items[i]);
//		}
		
		
		items = mContext.getResources().getStringArray(R.array.pens_item_entries);
		for (int i = 0; i < items.length; i++) {
			mPens.addItem(items[i]);
		}
		
		items = mContext.getResources().getStringArray(R.array.message_dots);
		for (int i = 0; i < items.length; i++) {
			mDots.addItem(items[i]);
		}
	}
	private String getEntry(int id,int index) {
		String entries[] = mContext.getResources().getStringArray(id);
		// Debug.d(TAG, "--->getEncoder:entries[" + index + "]=" + entries[index]);
		if (entries == null || entries.length <= 0) {
			return null;
		}
		if (index<0 || index >= entries.length) {
			return entries[0];
		}
		return entries[index];
	}
	
	private int getIndexByEntry(int id, String entry) {
		String entries[] = mContext.getResources().getStringArray(id);
		if (entry == null || entries == null || entries.length <= 0) {
			return 0;
		}
		for (int i = 0; i < entries.length; i++) {
			if (entry.equalsIgnoreCase(entries[i])) {
				return i;
			}
		}
		return 0;
	}
	
	

	@Override
	public void onClick(View view) {
		Debug.d(TAG, "===>onclick " + view);
		int position = 0;
		if (view != null) {
			position = (Integer)view.getTag();
		} else {
			return;
		}
		if (mSettingItems == null ||position >= mSettingItems.length) {
			return;
		}
		if (mSettingItems[position].mType == ItemType.TYPE_NONE) {
			return;
		}
		mSpiner.setAttachedView(view);
		if (position == 1) { //參數2
			mSpiner.setAdapter(mDirection);
		} else if (position == 2) { //參數3
			mSpiner.setAdapter(mResolution);
		} else if (position == 4) { //參數5
			mSpiner.setAdapter(mPhotocell);
		} else if (position == 5) { //參數6
			mSpiner.setAdapter(mEncoderAdapter);
		} else if (position == 6) { //參數7
			mSpiner.setAdapter(mRepeat);
		} else if (position == 7) { //參數8
			mSpiner.setAdapter(mNozzle);
		} else if (position == 12) { //參數13
			mSpiner.setAdapter(mPen1Mirror);
		} else if (position == 13) { //參數14
			mSpiner.setAdapter(mPen2Mirror);
		} else if (position == 14) { //參數15
			mSpiner.setAdapter(mPen1Invert);
		} else if (position == 15) { //參數16
			mSpiner.setAdapter(mPen2Invert);
		}/* else if (position == 16) { //參數17
			mSpiner.setAdapter(mQRsource);
		}*/ else if (position == 20) { //參數21
			mSpiner.setAdapter(mPen3Mirror);
		} else if (position == 21) { //參數22
			mSpiner.setAdapter(mPen4Mirror);
		} else if (position == 22) { //參數23
			mSpiner.setAdapter(mPen3Invert);
		} else if (position == 23) { //參數24
			mSpiner.setAdapter(mPen4Invert);
		} else if (position == 24) { //參數25
			mSpiner.setAdapter(mAutoVol);
		} else if (position == 26) { //參數27
			mSpiner.setAdapter(mAutoPulse);
		} else if (position == 30) { //參數31
			HeaderSelectDialog dialog = new HeaderSelectDialog(mContext, handler, mSysconfig.getParam(30));
			dialog.show();
			return;
		} else if (position == 31) {
			mSpiner.setAdapter(mCntReset);
		} else if (position == 39) { //參數40
			mSpiner.setAdapter(mDots);
		}  else if (position == 40) { //鍙冩暩40
			mSpiner.setAdapter(mHandle);
		}
		mSpiner.setWidth(view.getWidth());
		//mSpiner.showAsDropDown(view);
		mSpiner.showAsDropUp(view);
	}

	@Override
	public void onItemClick(int index) {
		TextView view = mSpiner.getAttachedView();
		int position = (Integer) view.getTag();
		String value = null;
		if (mSettingItems == null ||position >= mSettingItems.length) {
			return;
		}
		mSettingItems[position].setValue(index);
		mSysconfig.setParam(position, mSettingItems[position].getValue());
		value = mSettingItems[position].getDisplayValue();
		view.setText(value);
	}
	
	/**
	 * 检查已给定参数是否越界，如果越界就重置为默认值	
	 */
	public void checkParams() {
		Debug.d(TAG, "===>checkParams");
		int value = 0;
		
		for (int i = 1; i < mSysconfig.getParams().length; i++) {
			 value = mSysconfig.checkParam(i, mSysconfig.getParam(i-1));
			 if (value != mSysconfig.getParam(i-1)) {
				 mSysconfig.setParam(i-1, value);
				 mSettingItems[i-1].mValue = String.valueOf(value);
			 }
		}
		refresh();
	}
	
	public void refresh() {
		notifyDataSetChanged();
	}
	
	private class SelfTextWatcher implements TextWatcher {
		
		private EditText mEditText;
		
		public SelfTextWatcher(EditText e) {
			mEditText = e;
		}
		
		@Override
		public void afterTextChanged(Editable arg0) {
			int pos = (Integer) mEditText.getTag();
			
			Debug.d(TAG, "===>afterTextChanged, position=" + mEditText.getTag());
			if (mSettingItems == null ||pos >= mSettingItems.length) {
				return;
			}
			if (mSettingItems[pos].mType != ItemType.TYPE_NONE) {
				return;
			}
			mSettingItems[pos].mValue = arg0.toString();
//			Debug.d(TAG, "--->param=" + mSettingItems[pos].getDisplayValue());
			mSysconfig.setParam(pos, getValueFromEditText(arg0));
		}
		@Override
		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			// TODO Auto-generated method stub
			
		}
		@Override
		public void onTextChanged(CharSequence arg0, int arg1, int arg2, int arg3) {
			// TODO Auto-generated method stub
			
		}
	}

	private int getValueFromEditText(Editable s) {
		int iv = 0;
		String value = s.toString();
		try {
			iv = Integer.parseInt(value);
			Debug.d(TAG, "--->getValueFromEditText:" + iv);
		} catch (Exception e) {
			
		}
		// mHandler.removeMessages(PRINTER_SETTINGS_CHANGED);
		// mHandler.sendEmptyMessageDelayed(PRINTER_SETTINGS_CHANGED, 10000);
		return iv;
	}

	private int getSwitchvalue(int index) {
		return index;
	}
	
	private int getDirectionvalue(int index) {
		return index;
	}
	
	private int getValue(PopWindowAdapter adapter, int index) {
		try {
			int v = Integer.parseInt((String) adapter.getItem(index));
			return v;
		} catch (Exception e) {
			Debug.e(TAG, "--->" + e.getMessage());
			return 0;
		}
	}
	
}
