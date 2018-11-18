package com.industry.printer.ui.CustomerAdapter;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;
import com.industry.printer.ui.Items.PictureItem;


import android.content.Context;
import android.content.ClipData.Item;
import android.graphics.Color;
import android.net.Uri;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class PictureBrowseAdapter extends BaseAdapter {

	public static final String TAG="PreviewAdapter";
	public int mChecked;
	public int mlastChecked;
	public Context mContext;
	
	private LayoutInflater mInflater;
	private List<PictureItem> mItems = new ArrayList<PictureItem>();
	
	private ViewHolder	mHolder;
	
	private class ViewHolder {
		ImageView 	mImg;
		TextView	mTitle;
	}
	
	public PictureBrowseAdapter(Context context) {
		mChecked=-1;
		mlastChecked = -1;
		mContext = context;
		mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}

	@Override
	public View getView(final int position, View convertView, ViewGroup parent)
	{
		View view =null;
		if(convertView != null)
		{
			mHolder = (ViewHolder) convertView.getTag();
		}else
		{
			convertView = mInflater.inflate(R.layout.grid_item, null);
			mHolder = new ViewHolder();
			mHolder.mImg = (ImageView) convertView.findViewById(R.id.image);
			mHolder.mTitle = (TextView) convertView.findViewById(R.id.title);
			convertView.setTag(mHolder);
		}
		Debug.d(TAG, "--->checked=" + mChecked + ", pos=" + position);
		if(mChecked == position)
		{
			convertView.setBackgroundResource(R.drawable.grid_item_selected);
			convertView.setSelected(true);
		}
		else
		{
			convertView.setBackgroundResource(R.drawable.grid_item_unselected);
			convertView.setSelected(false);
		}
		mHolder.mImg.setImageURI(Uri.parse(mItems.get(position).getPath()));
		mHolder.mTitle.setText(mItems.get(position).getTitle());
		
		return convertView;
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

	
	public void setData(ArrayList<PictureItem> items) {
		mItems.clear();
		mItems.addAll(items);
		notifyDataSetChanged();
	}
	
	public void addItem(PictureItem item) {
		mItems.add(item);
	}

	@Override
	public int getCount() {
		return mItems.size();
	}

	@Override
	public Object getItem(int position) {
		return mItems.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}
}
