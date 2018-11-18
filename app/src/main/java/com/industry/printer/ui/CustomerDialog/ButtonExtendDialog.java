package com.industry.printer.ui.CustomerDialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageButton;

import com.industry.printer.R;

public class ButtonExtendDialog extends Dialog implements View.OnClickListener{


	private Button mSave;
	private Button mSaveAndPrint;

	private IListener mListener;

	public ButtonExtendDialog(Context context) {
		super(context);
		
	}

	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.save_print_dialog);

		mSave = (Button) findViewById(R.id.btn_save);
		mSaveAndPrint = (Button) findViewById(R.id.btn_saveprint);

		mSave.setOnClickListener(this);
		mSaveAndPrint.setOnClickListener(this);
	}


	public void setListener(IListener listener) {
		mListener = listener;
	}
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case R.id.btn_save:
				dismiss();
				if (mListener != null) {
					mListener.onSave();
				}
				break;
			case R.id.btn_saveprint:
				dismiss();
				if (mListener != null) {
					mListener.onSaveAndPrint();
				}
				break;
			default:
				break;	
		}
	}
	
	public interface IListener {
		public void onSave();
		public void onSaveAndPrint();
	}
}
