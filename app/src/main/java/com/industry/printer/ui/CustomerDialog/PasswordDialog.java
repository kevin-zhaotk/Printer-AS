package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.Utils.StringUtil;
import com.industry.printer.Utils.ToastUtil;

import rx.internal.util.atomic.MpscLinkedAtomicQueue;
import android.R.string;
import android.content.Context;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;

import com.industry.printer.R;
import com.printer.corelib.Debug;

public class PasswordDialog extends CustomerDialogBase implements View.OnClickListener {

	public static final String PASSWORD = "ok";
	
	private Button mOk;
	private Button mCancel;
	private EditText mPasswd;
	
	public PasswordDialog(Context context) {
		super(context, R.style.Dialog_Fullscreen);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.password_dialog);
		initView();
	}

	private void initView() {
		mOk = (Button) findViewById(R.id.btn_confirm);
		mCancel = (Button) findViewById(R.id.btn_objinfo_cnl);
		mPasswd = (EditText) findViewById(R.id.password);
		
		mOk.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				Debug.d("XXX", "--->click confirm: " + mPasswd.getText().toString());
				if (StringUtil.equal(mPasswd.getText().toString(), PasswordDialog.PASSWORD)) {
					if (pListener != null) {
						pListener.onClick();
					}
					dismiss();
				} else {
					ToastUtil.show(getContext(), R.string.toast_passwd_error);
					mPasswd.setText("");
				}
			}
		});
		mCancel.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				Debug.d("XXX", "--->click cancel");
				if (nListener != null) {
					nListener.onClick();
				}
				dismiss();
			}
		});
	}
	
	@Override
	public void onClick(View view) {
		
	}
}
