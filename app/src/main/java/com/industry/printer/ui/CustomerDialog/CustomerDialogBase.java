package com.industry.printer.ui.CustomerDialog;


import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;

public class CustomerDialogBase extends Dialog {

	/**
	 * 按键处理回调函数
	 */
	public OnPositiveListener pListener;
	public OnNagitiveListener nListener;
	public OnExtraListener eListener;
	
	public CustomerDialogBase(Context context) {
		super(context);
	}

	public CustomerDialogBase(Context context, int theme) {
		super(context, theme);
	}

	/**
	  *Interface definition when positive button clicked 
	  **/
	 public interface OnPositiveListener{
		 void onClick();
		 void onClick(String content);
	 }

	 /**
	  *Interface definition when nagitive button clicked 
	  **/
	 public interface OnNagitiveListener{
		 void onClick();
	 }
	 
	public interface OnExtraListener {
	 	void onClick();
	}
	 /**
	  * setOnPositiveClickedListener - set the positive button listener for further deal with
	  * @param listener the listener callback
	  * @see OnPositiveListener
	  */
	 public void setOnPositiveClickedListener(OnPositiveListener listener)
	 {
		 pListener = listener;
	 }

	 public void setOnExtraClickedListener(OnExtraListener listener) {
	 	eListener = listener;
	 }

	 /**
	  * setOnNagitiveClickedListener - set the Nagitive button listener for further deal with
	  * @param listener  the listener callback
	  * @see OnNagitiveListener
	  */
	 public void setOnNagitiveClickedListener(OnNagitiveListener listener)
	 {
		 nListener = listener;
	 }
	 
	 
	 @Override
	 protected void onCreate(Bundle savedInstanceState) {
		 super.onCreate(savedInstanceState);
	 }
}
