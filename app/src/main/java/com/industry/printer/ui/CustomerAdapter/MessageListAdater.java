package com.industry.printer.ui.CustomerAdapter;

import java.io.File;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.industry.printer.BinInfo;
import com.industry.printer.MessageTask;
import com.industry.printer.R;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.DimenssionConvertion;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.data.DataTask;

import android.R.integer;
import android.app.ActionBar.LayoutParams;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Color;
import android.net.Uri;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.ImageView.ScaleType;


//addbylk
import android.R.color;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Paint.FontMetrics;
import android.graphics.Typeface;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;






import android.graphics.BitmapFactory;//addbylk70507

public class MessageListAdater extends BaseAdapter {
	
	private static final String TAG = MessageListAdater.class.getSimpleName();

	/**
	 * A customerized view holder for widgets 
	 * @author zhaotongkai
	 */
	private class ItemViewHolder{
		TextView	mTitle;		//message title
		TextView	mAbstract;	//message abstract
		// ImageView	mImage;
		LinearLayout mllPreview;
		ImageView	mMark;
		ImageView	mCheck;
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
	 * 
	 */
	private int mSelected;
	
	private int[] mSCroll;

	/**
	 * multi select mode
	 */
	private boolean mMultiMode = false;

	private Map<String, Boolean> mMultiSelected = new HashMap<String, Boolean>();
	
	private Map<String, Bitmap> mPreviews = new HashMap<String, Bitmap>();
	
	/**
	 * Construct
	 */
	public MessageListAdater(Context context, LinkedList<Map<String, Object>> list, int resource,
			String from[], int to[])
	{
		mSelected = -1;
		mCntList = list;
		mContext = context;
		mKeys = new String[from.length];
		Debug.d(TAG, "====key size="+mKeys.length);
		mViewIDs = new int[to.length];
		System.arraycopy(from, 0, mKeys, 0, from.length);
		System.arraycopy(to, 0, mViewIDs, 0, to.length);
		mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		Debug.d(TAG, "--->size = " + mCntList.size());
		mSCroll= new int [mCntList.size()];
		for(int x=0;x<mCntList.size();x++)
		{
			mSCroll[x]=0;
	
		}
		
	}
	
	public MessageListAdater getInstance()
	{
		return this;
	}
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		if (mSCroll.length != mCntList.size()) {
			mSCroll = new int[mCntList.size()];
		}
		return mCntList.size();
	}

	@Override
	public Object getItem(int arg0) {
		// TODO Auto-generated method stub
		return mCntList.get(arg0);
	}

	@Override
	public long getItemId(int arg0) {
		// TODO Auto-generated method stub
		return arg0;
	}

	public void setMode(boolean multi) {
		mMultiMode = multi;
	}

	public void setSelected(int position) {
		if (mMultiMode) {
			if (mMultiSelected.containsKey(String.valueOf(position))) {
				mMultiSelected.remove(String.valueOf(position));
			} else {
				mMultiSelected.put(String.valueOf(position), true);
			}
		} else {
			mSelected = position;
		}
	}

	public Map<String, Boolean> getSelected() {
		return mMultiSelected;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		// TODO Auto-generated method stub
		if(convertView!=null)
			mHolder = (ItemViewHolder) convertView.getTag();
		else
		{
			//prepare a empty view 
			convertView = mInflater.inflate(R.layout.message_item_layout, null);
			mHolder = new ItemViewHolder();
			// mHolder.mTitle = (TextView) convertView.findViewById(mViewIDs[0]);
			// mHolder.mAbstract = (TextView) convertView.findViewById(mViewIDs[1]);
			mHolder.mTitle = (TextView) convertView.findViewById(mViewIDs[0]);
			// mHolder.mImage = (ImageView) convertView.findViewById(mViewIDs[1]);
			mHolder.mllPreview = (LinearLayout) convertView.findViewById(mViewIDs[1]);
			mHolder.mMark = (ImageView) convertView.findViewById(mViewIDs[2]);
			mHolder.mCheck = (ImageView) convertView.findViewById(R.id.check_state);
			convertView.setTag(mHolder);
		}

		HashMap<String, Object> item = (HashMap<String, Object>) mCntList.get(position);
		
		String title = (String) item.get(mKeys[0]);
		// String abstrace = (String) item.get(mKeys[1]);
		//fill the elements into the empty view created early 
		mHolder.mTitle.setText(title);
		// mHolder.mAbstract.setText(abstrace);
		/*
		 * 最早的方案，使用編輯時生成的1.bmp
		 * 優點：效率高
		 * 缺點：非實時
		 */
		/*
		String path = ConfigPath.getTlkDir(title) + MessageTask.MSG_PREV_IMAGE;
		File img = new File(path);
		if (img.exists()) {
			mHolder.mImage.setImageURI(Uri.parse("file://" + path));
		} else {
			mHolder.mImage.setImageResource(R.drawable.preview_null);
		}*/
		/*
		 * 通過bin生成預覽圖
		 * 優點：實時
		 * 缺點：效率低
		 */
		Bitmap bmp_disk = mPreviews.get(title);
        Bitmap Bmp_bak   = mPreviews.get(title);
     /*   
		if (bmp_disk == null) {
			
			MessageTask task = new MessageTask(mContext, title);
			DataTask dTask = new DataTask(mContext, task);
			dTask.prepareBackgroudBuffer();
            /// 获得 浏览图像 
			bmp_disk = dTask.getPreview();///
			
			if (bmp_disk.getWidth() > 1500) {
				Bitmap b = Bitmap.createBitmap(bmp_disk, 0, 0, 1500, bmp_disk.getHeight());
				BinFromBitmap.recyleBitmap(bmp_disk);
				bmp_disk = b;
			}
			mPreviews.put(title, bmp_disk);		
		}
		*/
		 
		 if (bmp_disk == null) 
		 {
			try
			{  
				bmp_disk = Bitmap.createBitmap(1500, 100, Configs.BITMAP_CONFIG);
				String path = ConfigPath.getTlkDir(title) + MessageTask.MSG_PREV_IMAGE;
			   Debug.e(TAG, "===="+path);			
			    File file =new File(path);
				if( file.exists() )
				{
					bmp_disk=BitmapFactory.decodeFile(path);
					Debug.e(TAG, path);
				}
			}
			catch (Exception e)
			{
				Debug.d(TAG, "--->e:" + e.getMessage());
			}
			Bmp_bak = Bitmap.createBitmap(1500, 100, Configs.BITMAP_CONFIG);
			 Paint mPaint;
			 mPaint = new Paint();
			 
			 Canvas mCan;
			mCan = new Canvas(Bmp_bak);
			//mCan.drawBitmap(bmp_disk, 0, 0, mPaint);
			int iwidth = bmp_disk == null ? 0 : bmp_disk.getWidth();
			if(iwidth>1500)
			{
				iwidth=1500;
			} else if (iwidth <= 0) {
				return convertView;
			}
			
			 
			 mCan.drawBitmap(bmp_disk, new Rect(0, 0, iwidth, 100), new Rect(0, 0, iwidth, 100), null);
			   Debug.e(TAG, "mSCroll mSCroll==11111=============="+mSCroll);	
				
		//	bmp_disk = Bitmap.createScaledBitmap(bmp_disk);//,bmp_disk.getWidth(), bmp_disk.getHeight() , true);		
			
		//	bmp_disk = Bitmap.createScaledBitmap(bmp_disk,1500, 100, true);
//			mPreviews.put(title, Bmp_bak);
		}

		 
		// mHolder.mImage.setImageBitmap(bmp);
		//////////////////////////////////////////////

		
		/*	
	
		ImageView imgView = new ImageView(mContext);
		imgView.setScaleType(ScaleType.FIT_START);
//		if (density == 1) {
		//	imgView.setLayoutParams(new LayoutParams(1500,100));
//		} else {
//			imgView.setLayoutParams(new LayoutParams(cutWidth,LayoutParams.MATCH_PARENT));
//		}
		
		imgView.setBackgroundColor(Color.WHITE);
		imgView.setImageBitmap(Bmp_bak);
		mHolder.mllPreview.addView(imgView);
	*/
		

		Debug.d(TAG, "--->getview position= "+ position + "  -- selected=" + mSelected + "  MultiMode = " + mMultiMode);
		if (mMultiMode) {
			if (title.startsWith("Group-")) {
				mHolder.mCheck.setVisibility(View.GONE);
			} else {
				mHolder.mCheck.setVisibility(View.VISIBLE);
				if (mMultiSelected.containsKey(String.valueOf(position))) {
					mHolder.mCheck.setImageResource(R.drawable.checked);
				} else {
					mHolder.mCheck.setImageResource(R.drawable.check_nor);
				}
			}
		} else {
			if (position == mSelected) {
				try {
					bmp_disk = Bitmap.createBitmap(1500, 100, Configs.BITMAP_CONFIG);
					String path = ConfigPath.getTlkDir(title) + MessageTask.MSG_PREV_IMAGE;
					Debug.e(TAG, "====" + path);
					File file = new File(path);
					if (file.exists()) {
						bmp_disk = BitmapFactory.decodeFile(path);
						Debug.e(TAG, path);
					}
				} catch (Exception e) {

				}
				Bmp_bak = Bitmap.createBitmap(1500, 100, Configs.BITMAP_CONFIG);
				Paint mPaint;
				mPaint = new Paint();

				Canvas mCan;
				mCan = new Canvas(Bmp_bak);
				//mCan.drawBitmap(bmp_disk, 0, 0, mPaint);
				int iwidth = bmp_disk.getWidth();
				if (bmp_disk.getWidth() > 1500) {
					iwidth = 1500;
				}

				WindowManager wManager = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
				int width = wManager.getDefaultDisplay().getWidth();
				if (width >= bmp_disk.getWidth()) {
					mSCroll[position] = 0;
				} else if (mSCroll[position] + width > bmp_disk.getWidth()) {
					mSCroll[position] = bmp_disk.getWidth() - width;
				}
				mCan.drawBitmap(bmp_disk, new Rect(mSCroll[position], 0, iwidth + mSCroll[position], 100), new Rect(0, 0, iwidth, 100), null);
				Debug.e(TAG, "mSCroll mSCroll=2222==============" + mSCroll[position]);
//				mPreviews.put(title, Bmp_bak);


				Debug.d(TAG, "---blue");
				mHolder.mMark.setVisibility(View.VISIBLE);
			} else {
				//	mSCroll[position]=0;

				Debug.d(TAG, "---transparent");
				mHolder.mMark.setVisibility(View.GONE);
			}
			mHolder.mCheck.setVisibility(View.GONE);
		}
	 	dispPreview(Bmp_bak);	
		return convertView;
	}

	
	private void dispPreview(Bitmap bmp) {
		int x=0,y=0;
		int cutWidth = 0;
		if (bmp == null || bmp.getWidth() == 0 || bmp.getHeight() == 0) {
			return;
		}
		float scale = (float)DimenssionConvertion.dip2px(mContext, 100)/bmp.getHeight();
		mHolder.mllPreview.removeAllViews();
		Debug.e(TAG, "-===================-->width= " + bmp.getWidth() + "  scale============================= " + scale);
			for (int i = 0;x < bmp.getWidth(); i++) 
			{
				if (x + 1200 + 50 > bmp.getWidth()) 
				{
					cutWidth = bmp.getWidth() - x;
				} else {
					cutWidth =1200;
				}
				if (cutWidth <= 0) {
					break;
				}
				Bitmap child = Bitmap.createBitmap(bmp, x, 0, cutWidth, bmp.getHeight());
				Debug.d(TAG, "-->child: " + child.getWidth() + "  " + child.getHeight() + "   view h: " + mHolder.mllPreview.getHeight());
				
				Bitmap scaledChild = Bitmap.createScaledBitmap(child, (int) (cutWidth*scale), (int) (bmp.getHeight() * scale), true);
				child.recycle();
				x += cutWidth;
				ImageView imgView = new ImageView(mContext);
				imgView.setScaleType(ScaleType.FIT_START);
//				if (density == 1) {
					imgView.setLayoutParams(new LayoutParams(scaledChild.getWidth(),scaledChild.getHeight()));
//				} else {
//					imgView.setLayoutParams(new LayoutParams(cutWidth,LayoutParams.MATCH_PARENT));
//				}
				
				imgView.setBackgroundColor(Color.WHITE);
				imgView.setImageBitmap(scaledChild);
				mHolder.mllPreview.addView(imgView);
			}
	}
	
	/**
	 * delete the selected message
	 */
	public void delete() {
		if (mMultiMode) {
			deleteMulti();
		} else {
			deleteSingle();
		}
	}
	
	public void deleteSingle() {
		if (mSelected < 0 || mSelected >= mCntList.size()) {
			return;
		}
		HashMap<String, Object> item = (HashMap<String, Object>)mCntList.get(mSelected);
		String title = (String) item.get(mKeys[0]);
		File file = new File(ConfigPath.getTlkDir(title));
		if (file.exists()) {
			File[] list = file.listFiles();
			for (int i = 0; i < list.length; i++) {
				File f = list[i];
				f.delete();
			} 
		}
		file.delete();
		mCntList.remove(mSelected);
		notifyDataSetChanged();
	}
	
	public void deleteMulti() {
		if (mMultiSelected == null) {
			return;
		}
		Iterator<String> keys = mMultiSelected.keySet().iterator();
		
		while (keys.hasNext()) {
			int index = Integer.parseInt(keys.next());
			HashMap<String, Object> item = (HashMap<String, Object>)mCntList.get(index);
			String title = (String) item.get(mKeys[0]);
			File file = new File(ConfigPath.getTlkDir(title));
			if (file.exists()) {
				File[] list = file.listFiles();
				for (int i = 0; i < list.length; i++) {
					File f = list[i];
					f.delete();
				} 
			}
			file.delete();
		}
		mCntList.clear();
		mMultiSelected.clear();
//		mMultiMode = false;
//		notifyDataSetChanged();
	}
	
	public void Scroll(int left_rigt) 
	{
		
		if(mSelected!=-1)
		{
			mSCroll[mSelected]+=left_rigt;
			if (mSCroll[mSelected] < 0) {
				mSCroll[mSelected] = 0;
			}
			/*
			if(left_rigt==1)
			{	
				mSCroll[mSelected]-=left_rigt;
				
				if(mSCroll[mSelected]<0)
				{
					mSCroll[mSelected]=0;;			 
				}				
			}
			else
			{
			*/
				
		
			// }
		}
		
		Debug.e(TAG, "------Scroll");	
		if (mSelected < 0 || mSelected >= mCntList.size()) {
			return;
		}


		Debug.e(TAG, "------Scroll2");		
		
	}


}
