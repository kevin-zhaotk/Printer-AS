package com.industry.printer.ui.CustomerAdapter;

import java.util.List;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.R.id;
import com.industry.printer.Utils.Debug;


import android.content.Context;
import android.database.Cursor;
import android.graphics.Color;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.SimpleAdapter;
import android.widget.SimpleCursorAdapter;
import android.widget.TextView;

public class PreviewAdapter extends SimpleAdapter {

	public static final String TAG="PreviewAdapter";
	public int mChecked;
	public int mlastChecked;
	public Context mContext;
	public boolean mManual;
	
	public PreviewAdapter(Context context, List<? extends Map<String, ?>> data,
			int resource, String[] from, int[] to) {
		super(context, data, resource, from, to);
		mChecked=-1;
		mlastChecked = -1;
		mContext = context;
		mManual = false;
		// TODO Auto-generated constructor stub
	}

	@Override
	public View getView(final int position, View convertView, ViewGroup parent)
	{
		View view =null;
		if(convertView != null)
		{
			view = convertView;
		}else
		{
			view = super.getView(position, convertView, parent);
		}
		//int[] color = {Color.WHITE, Color.rgb(0x1c, 0x86, 0xee)};
		//view.setBackgroundColor(color[position%2]);
		//((TextView) view).setTextColor(Color.BLACK);
		Debug.d(TAG,"mChecked="+mChecked+", position="+position);
		//Debug.d(TAG, "position="+position);
		
		if(mChecked == position)
		{
			Debug.d(TAG, "set color to blue for position "+mChecked);
			view.setBackgroundColor(Color.GRAY);
		}
		else
		{
			view.setBackgroundColor(Color.WHITE);
		}
		
		if(mManual == true)
		{
			 
			EditText t1 = (EditText)view.findViewById(R.id.manual_text1);
			Debug.d(TAG, "===t1 content="+t1.getText().toString());
			t1.setOnEditorActionListener(new TextView.OnEditorActionListener() {
				
				@Override
				public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
					// TODO Auto-generated method stub
					Debug.d(TAG, "===v="+v.getEditableText().toString());
					return false;
				}
			});
		}
		return super.getView(position, convertView, parent);
	}
	
	public void setChecked(int position)
	{
		mlastChecked = mChecked;
		mChecked = position;
	}
	
	public int getChecked()
	{
		return mChecked;
	}
	
	public void setMode(boolean manual)
	{
		mManual = manual;
	}
	
}
