package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;
import com.industry.printer.object.LetterHourObject;

import android.animation.AnimatorSet.Builder;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Window;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;

public class ConfirmDialog extends Dialog implements View.OnClickListener {

	private Button mPositive;
	private Button mNagitive;
	private TextView mMsg;
	
	private String mMessage;
	
	private DialogListener mListener;
	
	public ConfirmDialog(Context context) {
		super(context);
	}
	
	public ConfirmDialog(Context context, int resId) {
		super(context);
		mMessage = context.getString(resId);
	}
	
	public ConfirmDialog(Context context, String msg) {
		super(context);
		mMessage = msg;
	}
	
	public void setListener(DialogListener listener) {
		mListener = listener;
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.confirm_dialog_layout);
		
		mPositive = (Button) findViewById(R.id.positive);
		mNagitive = (Button) findViewById(R.id.nagitive);
		mMsg = (TextView) findViewById(R.id.message);
		
		mPositive.setOnClickListener(this);
		mNagitive.setOnClickListener(this);
		if (mMessage != null) {
			mMsg.setText(mMessage);
		}
		
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.positive:	
			if (mListener != null) {
				mListener.onConfirm();
			}
			dismiss();
			break;
		case R.id.nagitive:
			if (mListener != null) {
				mListener.onCancel();
			}
			dismiss();
			break;
		default:
			break;
		}
		
	}
	
}
