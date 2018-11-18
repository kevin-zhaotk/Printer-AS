package com.industry.printer;



import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.LinearLayout;
import android.widget.TextView;

public class LoadingDialog_Activity extends Dialog {
	public TextView tv;
	//private int i = 0;  
	//private int TIME = 1000; 
	//private Handler handler = new Handler(); 
	public LoadingDialog_Activity(Context context) {
		super(context, R.style.loadingDialogStyle);
	}

	private LoadingDialog_Activity(Context context, int theme) {
		super(context, theme);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.dialog_loading_activity);
		tv = (TextView)this.findViewById(R.id.tv);
		 
		
		 tv.setTextSize(16);
		 tv.setText("处理中,请稍候..."); 
		// handler.postDelayed(runnable, TIME); 

		LinearLayout linearLayout = (LinearLayout)this.findViewById(R.id.LinearLayout);
		linearLayout.getBackground().setAlpha(210);
	}
	 
	 /* Runnable runnable = new Runnable() {  
	
	       @Override  
	        public void run() {  
	            // handler自带方法实现定时器  
	            try {  
	               handler.postDelayed(this, TIME); 
	              // tv.setText("处理中,请稍候...!");
	               tv.setText("处理中,请稍候 "+Integer.toString(i++)+" 秒");  
	               // System.out.println("do...");  
	            } catch (Exception e) {  
	                // TODO Auto-generated catch block  
	               e.printStackTrace();  
	                System.out.println("exception...");  
	            }  
	        }  
	    }; */ 
	

   // setOnKeyListener(keylistener);
    //setCancelable(false);
}
