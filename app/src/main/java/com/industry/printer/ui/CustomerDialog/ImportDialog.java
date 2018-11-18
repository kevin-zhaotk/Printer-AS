package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.RelativeLayout;

public class ImportDialog extends Dialog implements android.view.View.OnClickListener{

	
	private ImageButton mImport;
	private ImageButton mExport;
	private ImageButton mFlush;
	private RelativeLayout mCancel;
	private Button mBtncl;
	
	private IListener mListener;
	
	public ImportDialog(Context context) {
		super(context);
		
	}

	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.import_dialog);
		
		mImport = (ImageButton) findViewById(R.id.ib_import);
		mExport = (ImageButton) findViewById(R.id.ib_export);
		mFlush = (ImageButton) findViewById(R.id.ib_flush);
		mCancel = (RelativeLayout) findViewById(R.id.cancel);
		mBtncl = (Button) findViewById(R.id.btn_cancel);

		mImport.setOnClickListener(this);
		mExport.setOnClickListener(this);
		mFlush.setOnClickListener(this);
		mCancel.setOnClickListener(this);
		mBtncl.setOnClickListener(this);
	}


	public void setListener(IListener listener) {
		mListener = listener;
	}
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case R.id.ib_import:
				dismiss();
				if (mListener != null) {
					mListener.onImport();
				}
				break;
			case R.id.ib_export:
				dismiss();
				if (mListener != null) {
					mListener.onExport();
				}
				break;
			case R.id.ib_flush:
				dismiss();
				if (mListener != null) {
					mListener.onFlush();
				}
				break;
			case R.id.cancel:
			case R.id.btn_cancel:
				dismiss();
				break;
			default:
				break;	
		}
	}
	
	public interface IListener {
		public void onImport();
		public void onExport();
		public void onFlush();
	}
}
