package com.industry.printer.ui.CustomerDialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Window;

import com.industry.printer.PreviewScrollView;
import com.industry.printer.R;
import com.industry.printer.object.TlkObject;

import java.util.Vector;

public class PreviewDialog extends Dialog {

	public PreviewScrollView mView;
	public PreviewDialog(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.preview_dialog);
		mView = (PreviewScrollView) findViewById(R.id.sv_preview);
	}
	
	public void show(Vector<TlkObject> list)
	{	
		super.show();
		mView.setObjectList(list);
	}
}
