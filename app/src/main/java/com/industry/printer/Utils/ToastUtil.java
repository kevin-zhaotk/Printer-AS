package com.industry.printer.Utils;

import com.industry.printer.R;
import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

public class ToastUtil {
	
	private static Toast mToast;
	
	public static void show(Context ctx, int id) {
		LayoutInflater inflater = LayoutInflater.from(ctx);  
		View view = inflater.inflate(R.layout.toast_layout,  null);  
		TextView textView = (TextView) view.findViewById(R.id.text);
		if (mToast == null) {
			mToast = new Toast(ctx);
		}
		textView.setText(ctx.getString(id));  
		mToast.setDuration(Toast.LENGTH_LONG);  
		mToast.setView(view);  
		mToast.setGravity(Gravity.CENTER, 0, 0);  
		mToast.show();  
	}

	public static void show(Context ctx, String msg) {
		LayoutInflater inflater = LayoutInflater.from(ctx);  
		View view = inflater.inflate(R.layout.toast_layout,  null);  
		TextView textView = (TextView) view.findViewById(R.id.text);
		if (mToast == null) {
			mToast = new Toast(ctx);
		}
		textView.setText(msg);  
		mToast.setDuration(Toast.LENGTH_LONG);  
		mToast.setView(view);  
		mToast.setGravity(Gravity.CENTER, 0, 0);  
		mToast.show();  
	}
}
