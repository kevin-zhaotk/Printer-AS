package com.industry.printer.Socket_Server;

import java.math.BigDecimal;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.json.JSONException;
import org.json.JSONObject;

import com.industry.printer.R;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;





public class Adapter_Net_Scan extends BaseAdapter{
	
	Activity mConext;
	private final LayoutInflater layoutFlater;
	private List<HashMap<String, Object>> list;
	
	public Adapter_Net_Scan(Activity context,List<HashMap<String, Object>> mList){
		mConext = context;
		layoutFlater = (LayoutInflater) mConext
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

		list = mList;
	}
	
	@Override
	public int getCount() {
		return list.size();
	}

	@Override
	public Object getItem(int position) {
		return null;
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(final int position, View convertView, ViewGroup arg2) {
		ViewHolder holder = null;
		if(convertView == null){
			convertView = layoutFlater.inflate(R.layout.adapter_net_scan, null);
			holder = new ViewHolder();
			holder.Device_name = (TextView)convertView.findViewById(R.id.Device_name);
			holder.Ip_add = (TextView)convertView.findViewById(R.id.Ip_add);
			holder.OnLine = (TextView)convertView.findViewById(R.id.OnLine);
			holder.checkbox = (CheckBox)convertView.findViewById(R.id.checkbox);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		
		holder.Device_name.setText((String)list.get(position).get("name"));
		//Double scale = (Double)list.get(position).get("scale");
		//BigDecimal value = new BigDecimal(scale).multiply(new BigDecimal(100));
		holder.Ip_add.setText((String)list.get(position).get("name"));
		holder.OnLine.setText((String)list.get(position).get("name"));
		//DecimalFormat format = new DecimalFormat(value.toString());
		
		return convertView;
	}
	
	public class ViewHolder{
		TextView Device_name;
		TextView Ip_add;
		TextView OnLine;
		CheckBox checkbox;
	} 
	public static String formatDecimal(String str) {
		String result = "";
		try {
			if (isNotBlank(str)) {
				Double d = Double.parseDouble(str);
				BigDecimal amount = new BigDecimal(d);
				DecimalFormat df = new DecimalFormat("##0.00");
				return df.format(amount);
			}
		} catch (NumberFormatException e) {
			e.printStackTrace();
		}
		return result;
	}
	public static boolean isNotBlank(String str) {
		return !isBlank(str);
	}
	public static boolean isBlank(String str) {
		return (str == null || "".equals(str.trim()) || "null".equals(str));
	}
}
