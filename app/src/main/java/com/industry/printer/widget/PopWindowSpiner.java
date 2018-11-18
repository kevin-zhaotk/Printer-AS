package com.industry.printer.widget;

import java.util.ArrayList;

import com.industry.printer.Utils.Debug;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter;
import com.industry.printer.ui.CustomerAdapter.PopWindowAdapter.IOnItemClickListener;
import com.industry.printer.R;

import android.app.ActionBar.LayoutParams;
import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

public class PopWindowSpiner extends PopupWindow implements OnItemClickListener, OnClickListener {

	private final String TAG = PopWindowSpiner.class.getSimpleName();
	public Context mContext;
	public ListView mListView;
	public PopWindowAdapter mAdapter;
	public IOnItemClickListener mItemClickListener;
	
	private TextView mView;
	private Button	mUp;
	private Button	mDown;
	
	public PopWindowSpiner(Context context) {
		super(context);
		mContext = context;
		initView();
	}
	
	private void initView() {
		View view = LayoutInflater.from(mContext).inflate(R.layout.popwindow_spiner_layout, null);
		setContentView(view);
		setWidth(LayoutParams.WRAP_CONTENT);
		setHeight(LayoutParams.WRAP_CONTENT);
		
		mListView = (ListView) view.findViewById(R.id.pop_spiner_listview);
		mListView.setOnItemClickListener(this);
		
		mDown = (Button) view.findViewById(R.id.down);
		mDown.setOnClickListener(this);

		mUp = (Button) view.findViewById(R.id.up);
		mUp.setOnClickListener(this);
	}
	
	

	public void setAdapter(PopWindowAdapter adapter) {
		mAdapter = adapter;
		mListView.setAdapter(mAdapter);
	}
	
	public void refreshData(ArrayList<String> list, int index) {
		if (mAdapter != null) {
			mAdapter.refreshData(list, index);
		}
	}
	
	public void setOnItemClickListener(IOnItemClickListener listener) {
		mItemClickListener = listener;
	}
	
	public void showAsDropUp(View v) {
		int[] location = new int[2];
		v.getLocationOnScreen(location);
		showAtLocation(v, Gravity.NO_GRAVITY, location[0], location[1] - getHeight());
	}
	
	
	
	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		Debug.d(TAG, "--->onItemclick");
		if (isShowing()) {
			dismiss();
		}
		if (mItemClickListener != null) {
			mItemClickListener.onItemClick(position);
		}
	}
	
	public void setAttachedView(View view) {
		mView = (TextView) view;
	}
	
	public TextView getAttachedView() {
		return mView;
	}

	@Override
	public void onClick(View v) {
		switch(v.getId()) {
			case R.id.up:
				mListView.smoothScrollByOffset(-2);
				break;
			case R.id.down:
				mListView.smoothScrollByOffset(2);
				break;
			default:
				break;
		}
		
	}

}
