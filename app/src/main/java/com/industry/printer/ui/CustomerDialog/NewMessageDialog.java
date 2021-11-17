package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;
import com.industry.printer.R.layout;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Window;

@Deprecated
public class NewMessageDialog extends Dialog {

	public NewMessageDialog(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.new_message_layout);
	}
}
