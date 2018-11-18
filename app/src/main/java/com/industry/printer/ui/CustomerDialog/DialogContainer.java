package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Window;

public class DialogContainer extends Dialog {

	public DialogContainer(Context context) {
		super(context, R.style.Dialog_Fullscreen);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.password_dialog);
	}
}
