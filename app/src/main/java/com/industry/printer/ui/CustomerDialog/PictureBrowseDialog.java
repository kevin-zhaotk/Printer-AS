package com.industry.printer.ui.CustomerDialog;

import java.io.File;

import com.industry.printer.R;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.ui.CustomerAdapter.PictureBrowseAdapter;
import com.industry.printer.ui.Items.PictureItem;

import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class PictureBrowseDialog extends CustomerDialogBase implements android.view.View.OnClickListener, OnItemClickListener {

	private static final String TAG = PictureBrowseDialog.class.getSimpleName();
	
	public RelativeLayout mConfirm;
	public RelativeLayout mCancel;
	public RelativeLayout mPagePrev;
	public RelativeLayout mPageNext;
	public EditText		  mSearch;
	public GridView		  mPicView;	
	private TextView	  mTips;
	
	public PictureBrowseAdapter mAdapter;
	private PictureItem   mItem;
	
	public PictureBrowseDialog(Context context) {
		super(context, R.style.Dialog_Fullscreen);
	}

	@Override
	 protected void onCreate(Bundle savedInstanceState) {
		Debug.d(TAG, "===>oncreate super");
		 super.onCreate(savedInstanceState);
		 Debug.d(TAG, "===>oncreate");
		 this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		 this.setContentView(R.layout.picture_preview);
		 
		 mConfirm = (RelativeLayout) findViewById(R.id.btn_ok_message_list);
		 mConfirm.setOnClickListener(this);
		 
		 mCancel = (RelativeLayout) findViewById(R.id.btn_cancel_message_list);
		 mCancel.setOnClickListener(this);
		 
		 mPagePrev = (RelativeLayout) findViewById(R.id.btn_page_prev);
		 mPagePrev.setOnClickListener(this);
		 
		 mPageNext = (RelativeLayout) findViewById(R.id.btn_page_next);
		 mPageNext.setOnClickListener(this);
		 
		 mSearch = (EditText) findViewById(R.id.et_search);
		 mSearch.setVisibility(View.GONE);
		 mPicView = (GridView) findViewById(R.id.picture_grid);
		 mPicView.setOnItemClickListener(this);
		 
		 mTips = (TextView) findViewById(R.id.tip_nothing);
		 
		 mAdapter = new PictureBrowseAdapter(getContext());
		 setupViews();
		 load();
		 mPicView.setAdapter(mAdapter);
	 }
	
	private void setupViews() {
		mPagePrev.setVisibility(View.GONE);
		mPageNext.setVisibility(View.GONE);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.btn_ok_message_list:
			
			dismiss();
			if (pListener != null) {
				pListener.onClick();
			}
			break;
		case R.id.btn_cancel_message_list:
			dismiss();
			if (nListener != null) {
				nListener.onClick();
			}
			break;
		case R.id.btn_page_prev:
			mPicView.smoothScrollBy(-200, 50);
			break;
		case R.id.btn_page_next:
			mPicView.smoothScrollBy(200, 50);
			break;
		
		}
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		Debug.d(TAG, "--->onitemclick=" + position);
		mItem = (PictureItem) mAdapter.getItem(position);
		mAdapter.setChecked(position);
		mAdapter.notifyDataSetChanged();
		
	}
	
	public void load() {
		String p = ConfigPath.getPictureDir();
		if (TextUtils.isEmpty(p)) {
			mTips.setVisibility(View.VISIBLE);
			mPicView.setVisibility(View.GONE);
			return;
		}
		File dir = new File(p);
		File[] list = dir.listFiles();
		if (list == null || list.length <= 0) {
			mTips.setVisibility(View.VISIBLE);
			mPicView.setVisibility(View.GONE);
			return;
		}
		for (File file : list) {
			PictureItem item = new PictureItem(file.getAbsolutePath(), file.getName());
			mAdapter.addItem(item);
		}
	}
	
	public PictureItem getSelect() {
		return mItem;
	}
}
