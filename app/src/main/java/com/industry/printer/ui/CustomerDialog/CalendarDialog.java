package com.industry.printer.ui.CustomerDialog;


import java.util.Calendar;
import java.util.logging.Logger;

import com.industry.printer.R;
import com.industry.printer.R.id;
import com.industry.printer.Utils.Debug;
import com.industry.printer.hardware.RTCDevice;

import android.R.string;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.TimePicker;

public class CalendarDialog extends Dialog {

	public static final String TAG="CalendarDialog"; 
	private int mLayout;
	public Button mPositive;
	public Button mNegative;
	public DatePicker mDPicker;
	public TimePicker mTPicker;
	public EditText mYear;
	public EditText mMonth;
	public EditText mDate;
	public EditText mHour;
	public EditText mMinute;
	
	
	protected CalendarDialog(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		
	}

	public  CalendarDialog(Context context, int resLayout)
	{
		super(context);
		mLayout = resLayout;
		Debug.d(TAG, "--->context="+context);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(mLayout);
		
		//mDPicker = (DatePicker) findViewById(R.id.picker_date);
		//mTPicker = (TimePicker) findViewById(R.id.picker_time);
		mYear = (EditText) findViewById(R.id.et_year);
		mMonth = (EditText) findViewById(R.id.et_month);
		mDate = (EditText) findViewById(R.id.et_date);
		mHour = (EditText) findViewById(R.id.et_hour);
		mMinute = (EditText) findViewById(R.id.et_minute);
		
		Calendar c = Calendar.getInstance();
		// mDPicker.init(c.get(Calendar.YEAR), c.get(Calendar.MONTH), c.get(Calendar.DAY_OF_MONTH), null);
		// mTPicker.setIs24HourView(true);
		// mTPicker.setCurrentHour(c.get(Calendar.HOUR_OF_DAY));
		// mTPicker.setCurrentMinute(c.get(Calendar.MINUTE));
		mYear.setText(String.valueOf(c.get(Calendar.YEAR)));
		mMonth.setText(String.valueOf(c.get(Calendar.MONTH) + 1));
		mDate.setText(String.valueOf(c.get(Calendar.DAY_OF_MONTH)));
		mHour.setText(String.valueOf(c.get(Calendar.HOUR_OF_DAY)));
		mMinute.setText(String.valueOf(c.get(Calendar.MINUTE)));
		
		mPositive = (Button) findViewById(R.id.btn_setTimeOk);
		mPositive.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Calendar c = Calendar.getInstance();
				check();
				try {
					c.set(Integer.parseInt(mYear.getText().toString()),
							Integer.parseInt(mMonth.getText().toString()) - 1,
							Integer.parseInt(mDate.getText().toString()),
							Integer.parseInt(mHour.getText().toString()),
							Integer.parseInt(mMinute.getText().toString()));
					long when = c.getTimeInMillis();

					Debug.d(TAG, "===>setDate");
					if (when / 1000 < Integer.MAX_VALUE) {
						SystemClock.setCurrentTimeMillis(when);
					}
					RTCDevice rtcDevice = RTCDevice.getInstance(getContext());
					rtcDevice.syncSystemTimeToRTC(getContext());
				} catch (Exception e) {
					Debug.e(TAG, "--->" + e.getMessage());
				}
				dismiss();
			}
		});
		
		mNegative =  (Button) findViewById(R.id.btn_setTimeCnl);
		mNegative.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				dismiss();
			}
		});
	}
	/**
	 * 验证时间是否有效
	 */
	
	private void check() {
		try {
			int year = Integer.parseInt(mYear.getText().toString());
			if (year > 2100 || year < 1970) {
				year = 1970;
				mYear.setText(String.valueOf(year));
			}
			int month = Integer.parseInt(mMonth.getText().toString());
			if (month > 12 || month < 1) {
				month = 1;
				mMonth.setText(String.valueOf(month - 1));
			}
			int date = Integer.parseInt(mDate.getText().toString());
			if (date > 31 || date < 1) {
				date = 1;
				mDate.setText(String.valueOf(date));
			}

			int hour = Integer.parseInt(mHour.getText().toString());
			if (hour < 0 || hour >= 24) {
				hour = 0;
				mHour.setText(String.valueOf(hour));
			}
			int minute = Integer.parseInt(mMinute.getText().toString());

			if (minute < 0 || minute >= 60) {
				minute = 0;
				mMinute.setText(String.valueOf(minute));
			}
			Calendar c = Calendar.getInstance();
			c.set(year, month, date, hour, minute);
		} catch (Exception e) {
			Debug.e(TAG, "--->" + e.getMessage());
		}
	}

	
}
