package com.industry.printer.ui.CustomerAdapter;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.R.id;
import com.industry.printer.R.layout;
import com.industry.printer.Utils.Debug;

import android.R.integer;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.view.View.OnCreateContextMenuListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * ListViewButtonAdapter
 * 	this is a Adapter extends BaseAdapter which has a button widget
 * @author zhaotongkai
 *
 */
public class ListViewButtonAdapter extends BaseAdapter {

	public static final String TAG="ListViewButtonAdapter";
	
	/**
	 * A customerized view holder for widgets 
	 * @author zhaotongkai
	 */
	private class ItemViewHolder{
		ImageView 	mIcon;		//icon
		TextView	mName;		//file name
		ImageButton	mBtnOpt;	//operation button
	}
	
	/**
	 * The Content list
	 */
	private LinkedList<Map<String, Object>> mCntList;
	
	/**
	 * An inflater for inflate the view
	 */
	private LayoutInflater mInflater;
	
	/**
	 * context
	 */
	private Context mContext;
	
	/**
	 * The Keys in HashMap
	 */
	private String[] mKeys;
	
	/**
	 * The view id in HashMap 
	 */
	private int[]	mViewIDs;
	
	/**
	 * widget holder
	 */
	private ItemViewHolder mHolder;
	
	/**
	 * Construct
	 */
	public ListViewButtonAdapter(Context c, LinkedList<Map<String, Object>> list, int resource,
			String from[], int to[])
	{
		mCntList = list;
		mContext = c;
		mKeys = new String[from.length];
		Debug.d(TAG, "====key size="+mKeys.length);
		mViewIDs = new int[to.length];
		System.arraycopy(from, 0, mKeys, 0, from.length);
		System.arraycopy(to, 0, mViewIDs, 0, to.length);
		mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		
	}
	
	public ListViewButtonAdapter getInstance()
	{
		return this;
	}
	
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return mCntList.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return mCntList.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		// TODO Auto-generated method stub
		if(convertView!=null)
			mHolder = (ItemViewHolder) convertView.getTag();
		else
		{
			//prepare a empty view 
			convertView = mInflater.inflate(R.layout.file_browser, null);
			mHolder = new ItemViewHolder();
			mHolder.mIcon = (ImageView) convertView.findViewById(mViewIDs[0]);
			mHolder.mName = (TextView) convertView.findViewById(mViewIDs[1]);
			mHolder.mBtnOpt = (ImageButton) convertView.findViewById(mViewIDs[2]);
			convertView.setTag(mHolder);
		}
		
		HashMap<String, Object> item = (HashMap<String, Object>) mCntList.get(position);
		if(item!=null)
		{
			String name = (String) item.get(mKeys[1]);
			int iconId = (Integer) item.get(mKeys[0]);
			int btnId = (Integer) item.get(mKeys[2]);
			//fill the elements into the empty view created early 
			mHolder.mName.setText(name);
			mHolder.mIcon.setImageDrawable(mHolder.mIcon.getResources().getDrawable(iconId));
			mHolder.mBtnOpt.setImageDrawable(mHolder.mBtnOpt.getResources().getDrawable(btnId));
			mHolder.mBtnOpt.setOnClickListener(new ItemButtonListener(position));
		}
		
		
		return convertView;
	}

	/**
	 * ItemButtonListener
	 * @author zhaotongkai
	 *
	 */
	private class ItemButtonListener implements OnClickListener
	{
		/**
		 * mPosition - record the Button's position
		 */
		private int mPosition;
		
		ItemButtonListener(int pos)
		{
			mPosition = pos;
		}
		
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			FileDeleteDialog dialog = new FileDeleteDialog(mContext, mPosition);
			dialog.show();
		}
		
	}
	
	/**
	 * FileDeleteDialog
	 * @author zhaotongkai
	 * for further file operations
	 */
	private class FileDeleteDialog extends Dialog
	{
		/**
		 * delete button
		 */
		private Button mDelete;
		
		/**
		 * cancel button
		 */
		private Button mCancel;
		
		private int mPosition;
		public FileDeleteDialog(Context context,int position) {
			super(context);
			mPosition = position;
			// TODO Auto-generated constructor stub
		}
		
		@Override
		protected void onCreate(Bundle savedInstanceState) 
		{
			super.onCreate(savedInstanceState);
	        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
	        setContentView(R.layout.file_operation_dialog);
	        
	        mDelete = (Button) findViewById(R.id.btn_fileopt_del);
	        mDelete.setOnClickListener(new View.OnClickListener() {
				
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					Debug.d(TAG, "onclick delete");
					File f = new File((String)mCntList.get(mPosition).get("path"));
					delete(f);
					mCntList.remove(mPosition);
					getInstance().notifyDataSetChanged();
					dismiss();
				}

			});
	        
	        mCancel = (Button) findViewById(R.id.btn_fileopt_cancel);
	        mCancel.setOnClickListener(new View.OnClickListener() {
				
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					Debug.d(TAG, "onclick calcel");
					dismiss();
				}
			});
		}
		
	}
	
	public static void delete(File file)
	{
		if(file.isFile())
			file.delete();
		if(file.isDirectory())
		{
			File[] childs = file.listFiles();
			if(childs==null || childs.length==0) {
				file.delete();
				return;
			}
			for(int i=0;i<childs.length;i++)
			{
				delete(childs[i]);
			}
			file.delete();
		}
	}
}
