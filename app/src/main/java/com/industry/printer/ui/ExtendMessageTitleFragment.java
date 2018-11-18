package com.industry.printer.ui;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class ExtendMessageTitleFragment extends Fragment {
	
	public Context mContext;
	public TextView mTitle;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		return inflater.inflate(R.layout.extend_msg_title_fragment, container, false);
	}
	
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		Debug.d("", "===>onActivityCreated");
		super.onActivityCreated(savedInstanceState);
		//requestWindowFeature(Window.FEATURE_NO_TITLE);
		//setContentView(R.layout.edit_frame);
		//this.setVisible(false);
		mContext = getActivity();
		mTitle = (TextView) getActivity().findViewById(R.id.msg_title);
	}
	
	public void setTitle(String title) {
		Debug.d("", "===>setTitle:"+title);
		mTitle.setText(title);
	}
}
