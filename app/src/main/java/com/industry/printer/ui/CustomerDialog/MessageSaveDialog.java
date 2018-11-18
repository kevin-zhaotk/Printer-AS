package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;

public class MessageSaveDialog extends CustomerDialogBase implements android.view.View.OnClickListener {

	
	public Button mConfirm;
	public Button mCancel;
	public Button mSaveAndPrint;
	public EditText mTitleEdit;
	public static String mTitle;
	
	public MessageSaveDialog(Context context) {
		super(context);
	}
	
	@Override
	 protected void onCreate(Bundle savedInstanceState) {
		 super.onCreate(savedInstanceState);
		 this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		 this.setContentView(R.layout.message_save_layout);
		 
		 mConfirm = (Button) findViewById(R.id.btn_message_ok);
		 mConfirm.setOnClickListener(this);
		 
		 mCancel = (Button) findViewById(R.id.btn_message_cancel);
		 mCancel.setOnClickListener(this);

		 mSaveAndPrint = (Button) findViewById(R.id.btn_save_print);
		 mSaveAndPrint.setOnClickListener(this);

		 mTitleEdit = (EditText) findViewById(R.id.et_message_title);
	}

	@Override
	public void onClick(View arg0) {
		switch (arg0.getId()) {
			case R.id.btn_message_ok:
				mTitle = mTitleEdit.getEditableText().toString();
				if (mTitle == null || mTitle.isEmpty())
					break;
				dismiss();
				if (pListener != null) {
					pListener.onClick();
				}
				break;
			case R.id.btn_message_cancel:
				mTitle = null;
				dismiss();
				if (nListener != null) {
					nListener.onClick();
				}
				break;
			case R.id.btn_save_print:
				mTitle = mTitleEdit.getEditableText().toString();
				if (mTitle == null || mTitle.isEmpty())
					break;
				dismiss();
				if (eListener != null) {
					eListener.onClick();
				}
				break;
			default:
				break;
		}
	}
	
	public static String getTitle() {
		return mTitle;
	}
}
