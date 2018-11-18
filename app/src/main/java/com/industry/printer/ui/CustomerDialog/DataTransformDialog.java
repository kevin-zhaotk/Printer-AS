package com.industry.printer.ui.CustomerDialog;


import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.R.drawable;
import com.industry.printer.R.id;
import com.industry.printer.R.layout;
import com.industry.printer.Utils.Debug;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SimpleAdapter;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.Spinner;

public class DataTransformDialog extends Dialog {

	public static final String TAG="DataTransformDialog";
	
	public Context mContext;
	
	private boolean mSdInserted;
	private boolean mUsbAInserted;
	private boolean mUsbBInserted;
	private boolean mUsbCInserted;
	
	public ListView mFilelist;
	public SimpleAdapter mFileAdapter;
	public LinkedList<Map<String, Object>> mContent;
	
	public Button mTransTo;
	public Spinner mDest;
	public ListView mDstlist;
	public LinkedList<Map<String, Object>> mDstContent;
	public SimpleAdapter mDstAdapter;
	
	public Button mOk;
	public Button mCancel;
	public EditText mDirEdit;
	
	private View mSrcView;
	private View mDstView;
	private int mSrcSelected;
	private int mDstSelected;
	
	public DataTransformDialog(Context context) {
		super(context);
		mContext = context;
		mSrcSelected = -1;
		mDstSelected = -1;
		mSrcView = null;
		mDstView = null;
		// TODO Auto-generated constructor stub
	}

	 @Override
	 protected void onCreate(Bundle savedInstanceState) {
		 super.onCreate(savedInstanceState);
	     this.requestWindowFeature(Window.FEATURE_NO_TITLE);
	     this.setContentView( R.layout.file_trans_layout);
	     
	     mSdInserted=false;
	     mUsbAInserted=false;
	     mUsbBInserted=false;
	     mUsbCInserted=false;
	     
	     mOk = (Button) findViewById(R.id.btn_trans_ok);
	     mOk.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				
			}
		});
	     mCancel = (Button) findViewById(R.id.btn_trans_cnl);
	     mCancel.setOnClickListener(new View.OnClickListener() {
				
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					dismiss();
				}
			});
	     
	     mDirEdit = (EditText) findViewById(R.id.dirEdit);
	     mDirEdit.setText("MSG");
	     
	     mContent = new LinkedList<Map<String, Object>>();
	     mFileAdapter = new SimpleAdapter(mContext, 
					mContent, 
					R.layout.file_browser, 
					new String[]{"icon", "name"}, 
					new int []{R.id.file_icon, R.id.file_name});
	     
	     mFilelist = (ListView) findViewById(R.id.dirList);
	     mFilelist.setOnItemClickListener(new OnItemClickListener(){

			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				// TODO Auto-generated method stub
				
				mSrcSelected = position;
				Debug.d(TAG, "onItemClick, position="+position);
				if(mSrcView == null)
				{
					mSrcView = view;
				}
				else
				{
					mSrcView.setBackgroundColor(Color.WHITE);
					mSrcView = view;
				}
				view.setBackgroundColor(Color.BLUE);
			}
	    	 
	     });
	     filterObject();
	     
	     mTransTo = (Button) findViewById(R.id.btn_transTo);
	     mTransTo.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				copyToDst();
			}
		});
	     
	     mDest = (Spinner) findViewById(R.id.dirSpiner);
	     
	     mDstContent=new LinkedList<Map<String, Object>>();
	     mDstlist = (ListView) findViewById(R.id.dest_obj);
	     mDstAdapter = new SimpleAdapter(mContext, 
					mDstContent, 
					R.layout.list_item_color, 
					new String[]{"icon", "name"}, 
					new int []{R.id.file_img, R.id.file_tv});
	     
	     mDstlist.setOnItemClickListener(new OnItemClickListener(){

			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				// TODO Auto-generated method stub
				mDstSelected = position;
				
				//View v = parent.getChildAt(position);
				//v.setBackgroundColor(Color.BLUE);
				/*
				if(mDstView == null)
				{
					mDstView = view;
				}
				else
				{
					mDstView.setBackgroundColor(Color.WHITE);
					mDstView = view;
				}
				view.setBackgroundColor(Color.BLUE);
				*/
				//mDstlist.setSelection(position);
				//view.setSelected(true);
				parent.setSelection(position);
			}
	    	 
	     });
	     
	     mDstlist.setOnTouchListener(new OnTouchListener(){

			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				v.requestFocusFromTouch();
				return false;
			}
	    	 
	     });
	     fillDstlist();
	 }
	 
	 public void filterObject()
	 {
		 File dir = new File("/storage/sd_internal/MSG");
		File[] files = dir.listFiles(new MyFileFilter());
		mContent.clear();
		if(files==null || files.length<=0)
			return;
		for(File f : files)
		{
			Map<String, Object> map = new HashMap<String, Object>();
			map.put("name", f.getName());
			map.put("icon", R.drawable.tlk_file);
			map.put("path", f.getAbsolutePath());
			mContent.add(map);
		}
		mFilelist.setAdapter(mFileAdapter);
	 }
	 
	 private void fillDstlist()
	 {
		 mDstContent.clear();
		 if( mDest.getSelectedItemPosition() == 0)  //SD
		 {
			 
		 }
		 else if( mDest.getSelectedItemPosition() == 1)  //USB
		 {
			 
		 }
		 for(int i=100;i <200; i++)
		 {
			 Map<String, Object> map=new HashMap<String, Object>();
			 map.put("name", String.valueOf(i));
			 map.put("icon", R.drawable.tlk_file);
			 mDstContent.add(map);
		 }
		 mDstlist.setAdapter(mDstAdapter);
	 }
	 
	 public void copyToDst()
	 {
		 //int index = mFilelist.getSelectedItemPosition();
		 if(mSrcSelected < 0 || mDstSelected < 0)
			 return;
		 Debug.d(TAG, "index = "+mSrcSelected);
		String path = (String) mContent.get(mSrcSelected).get("path");
		String dest;	
		Debug.d(TAG, "copyToDst mDest.getSelectedItemPosition()="+mDest.getSelectedItemPosition());
		if( mDest.getSelectedItemPosition() == 0)  //SD
		{
			if(!new File("/storage/sd_external/MSG").exists())
				new File("/storage/sd_external/MSG").mkdir();
			Map<String, Object> m = (Map<String, Object>) mDstlist.getItemAtPosition(mDstSelected);
			
			dest = "/storage/sd_external/MSG/"+(String)m.get("name");
			Debug.d(TAG, "dest="+dest);
		}
		else if( mDest.getSelectedItemPosition() == 1)  //USB
		{
			if(!new File("/mnt/usb/MSG").exists())
				new File("/mnt/usb/MSG").mkdir();
			Map<String, Object> m = (Map<String, Object>) mDstlist.getItemAtPosition(mDstSelected);
			dest = "/mnt/usb/MSG/"+(String)m.get("name");
			Debug.d(TAG, "dest="+dest);
		}
		else
		{
			Debug.d(TAG, "Unsupported device");
			return;
		}
		copyFolder(path, dest);
	 }
	 
	 public boolean copyFolder(String oldPath, String newPath) {
		 boolean isok = true;
		 try {
			 (new File(newPath)).mkdirs(); 
			 File a=new File(oldPath);
			 String[] file=a.list();
			 File temp=null;
			 for (int i = 0; i < file.length; i++) {
				 if(oldPath.endsWith(File.separator)){
					  temp=new File(oldPath+file[i]);
				 }
				 else
				 {
					 temp=new File(oldPath+File.separator+file[i]);
				 }
				 if(temp.isFile()){
					 FileInputStream input = new FileInputStream(temp);
					 FileOutputStream output = new FileOutputStream(newPath + "/" +(temp.getName()).toString());
					 byte[] b = new byte[1024 * 5];
					 int len;
					 while ( (len = input.read(b)) != -1) {
						 output.write(b, 0, len);
					 }
					 output.flush();
					 output.close();
					 input.close();
				 }
				 if(temp.isDirectory()){
					 copyFolder(oldPath+"/"+file[i],newPath+"/"+file[i]);
				 }
			}
		 }
		 catch (Exception e) {
			 isok = false;
		 }
		 return isok;
	 }

	 
	 
	 public class MyFileFilter implements FileFilter{
		 @Override
		 public boolean accept (File pathname)
		 {
			 Debug.d(TAG, "isDirectory? "+pathname.isDirectory()+", isObject ? "+isObject(pathname.getAbsolutePath()));
			 if(pathname.isDirectory() && isObject(pathname.getAbsolutePath()))
				 return true;
			 else
				 return false;
		 }
		 
		 private boolean isObject(String path)
		 {
			 return (new File(path+"/1.TLK").exists());
		 }
	 }
}
