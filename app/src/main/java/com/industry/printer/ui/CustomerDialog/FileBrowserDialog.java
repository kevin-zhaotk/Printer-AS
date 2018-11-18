package com.industry.printer.ui.CustomerDialog;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.FileFormat.FilenameSuffixFilter;
import com.industry.printer.R.drawable;
import com.industry.printer.R.id;
import com.industry.printer.R.layout;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.data.BinCreater;
import com.industry.printer.ui.CustomerAdapter.ListViewButtonAdapter;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.DragEvent;
import android.view.View;
import android.view.View.OnDragListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;

public class FileBrowserDialog extends CustomerDialogBase {
	public static final String TAG="FileBrowserDialog";
	
	/**
	 * FLAG_OPEN_FILE
	 * 		the flag means open file from this dialog
	 */
	public static final int FLAG_OPEN_FILE = 1;
	/**
	 * FLAG_SAVE_FILE
	 * 		the flag means save file from this dialog
	 */
	public static final int FLAG_SAVE_FILE = 2;
	
	
	//public SimpleAdapter mFileAdapter;
	public ListViewButtonAdapter mFileAdapter;
	public LinkedList<Map<String, Object>> mContent;
	
	public ListView mFileList;
	//public TextView mPath;
	/**
	 * mLocal  radio button of local path
	 */
	public RadioButton mLocal;
	/**
	 * mUsbStorage radio button of usb storage
	 */
	
	public RadioButton mUsbStorage;
	public RadioButton mUsbStorage2;
	
	/**
	 * mSDCard radio button of SD-CARD storage
	 */
	public RadioButton mSDCard;
	
	/**
	 * mCurpath  current opened path
	 */
	public static String mCurPath;
	
	/**
	 * objDir
	 */
	public static String objDir;
	
	/**
	 * mSuffix  the file suffix for filter
	 */
	public String mSuffix;
	
	/**
	 * mSave save button
	 */
	public Button mSave;

	/**
	 * mCancel cancel button
	 */
	public Button mCancel;
	
	/**
	 * mName the tlk file name tobe save as
	 */
	public EditText mName;
	
	public View mVSelected=null;
	
	/**
	 * 
	 */
	public int mFlag;
	
	/**
	 * FileBrowserDialog - construct function
	 * @param context
	 */
	public FileBrowserDialog(Context context,int flag) {
		super(context);
		// TODO Auto-generated constructor stub
		
		mContent = new LinkedList<Map<String, Object>>();
		/*
		mFileAdapter = new SimpleAdapter(context, 
				mContent, 
				R.layout.file_browser, 
				new String[]{"icon", "name"}, 
				new int []{R.id.file_icon, R.id.file_name});
		*/
		mFileAdapter = new ListViewButtonAdapter(context, 
				mContent, 
				R.layout.file_browser, 
				new String[]{"icon", "name", "operation"}, 
				new int[]{R.id.file_icon, R.id.file_name, R.id.file_operation});
		//mCurPath="/storage/external_storage/sda1";
		mCurPath =Configs.LOCAL_ROOT_PATH;
		mSuffix = null;
		mFlag = flag;
	}
	
	/**
	 * FileBrowserDialog - construct function
	 * @param context
	 * @param path	path to be open
	 */
	public FileBrowserDialog(Context context, String path, int flag)
	{
		this(context,flag);
		mCurPath = path;
	}
	
	/**
	 * FileBrowserDialog - construct function with path & suffix
	 * @param context
	 * @param path - path to be open
	 * @param suffix - file suffix filter with
	 */
	public FileBrowserDialog(Context context, String path, String suffix, int flag)
	{
		this(context,flag);
		mCurPath = path;
		mSuffix = suffix;
	}
	 @Override
	 protected void onCreate(Bundle savedInstanceState) {
	        // TODO Auto-generated method stub
	        super.onCreate(savedInstanceState);
	        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
	        this.setContentView(R.layout.file_browser_dialog);
	        Debug.d(TAG, "===>onCreate");
	        mSave = (Button) findViewById(R.id.dialog_save);
	        mSave.setOnClickListener(new View.OnClickListener(){

				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					Debug.d(TAG,"*****TODO save clicked");
					if(mName.getText() == null)
						return;
					Debug.d(TAG, "*****file save as "+mName.getText().toString());
					objDir = mName.getText().toString();
					//cancel();
					dismiss();
					if(pListener != null)
						pListener.onClick();
				}
	        	
	        });
	        
	        mCancel = (Button) findViewById(R.id.dialog_cancel);
	        mCancel.setOnClickListener(new View.OnClickListener() {
				
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					//Debug.d(TAG,"*****TODO cancel clicked");
					cancel();
					if(nListener != null)
						nListener.onClick();
				}
			});
	        
	        mName = (EditText) findViewById(R.id.name_input);
	        //mName.setVisibility(View.INVISIBLE);
	        if(mFlag == FLAG_OPEN_FILE)
	        	mName.setVisibility(View.GONE);
	        mFileList =(ListView) findViewById(R.id.file_list);
	        
	        mFileList.setOnItemClickListener(new OnItemClickListener(){

				@Override
				public void onItemClick(AdapterView<?> parent, View view,
						int position, long id) {
					// TODO Auto-generated method stub
					/*modify the background color when item clicked*/
					if(mVSelected == null)
					{
						view.setBackgroundColor(Color.BLUE);
						mVSelected = view;
					}
					else
					{
						mVSelected.setBackgroundColor(Color.WHITE);
						view.setBackgroundColor(Color.BLUE);
						mVSelected = view;
					}
					Debug.d(TAG,"path="+(String)mContent.get(position).get("path")+", name="+(String)mContent.get(position).get("name"));
					mCurPath = (String)mContent.get(position).get("path");
					File item = new File(mCurPath);
					if(item.isDirectory())
					{
						Debug.d(TAG, "item clicked: "+mCurPath);
						fileOpen(item);
					}
				}
	        	
	        });
	        
	        ArrayList<String> paths = ConfigPath.getMountedUsb();
	        mUsbStorage = (RadioButton) findViewById(R.id.radio_usb1);
	        mUsbStorage.setOnCheckedChangeListener(new OnCheckedChangeListener() {
				
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					// TODO Auto-generated method stub
					Debug.d(TAG, "===>local path selected? "+isChecked);
					if(isChecked==true)
					{
						mCurPath = Configs.USB_ROOT_PATH;
						fileOpen(new File(mCurPath));
					}
					
				}
			});
	        
	        
	        mUsbStorage2 = (RadioButton) findViewById(R.id.radio_usb2);
	        mUsbStorage2.setOnCheckedChangeListener(new OnCheckedChangeListener() {
				
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					// TODO Auto-generated method stub
					Debug.d(TAG, "===>usb path selected? "+isChecked);
					if(isChecked==true)
					{
						mCurPath = Configs.USB_ROOT_PATH2;
						fileOpen(new File(mCurPath));
					}
				}
			});
	        if (paths != null && paths.size() > 0) {
	        	for (String p : paths) {
		        	if (p.contains(Configs.USB_ROOT_PATH)) {
		        		mUsbStorage.setEnabled(true);
		        		mUsbStorage.setTextColor(Color.BLACK);
		        	} else if (p.contains(Configs.USB_ROOT_PATH2)) {
		        		mUsbStorage2.setEnabled(true);
		        		mUsbStorage2.setTextColor(Color.BLACK);
		        	}
		        }
			}
	        
	        Debug.d(TAG, "===>onCreate 1");
	        fileOpen(new File(mCurPath));
//	        mSDCard = (RadioButton) findViewById(R.id.radio_sdcard);
//	        mSDCard.setOnCheckedChangeListener(new OnCheckedChangeListener() {
//				
//				@Override
//				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
//					// TODO Auto-generated method stub
//					Debug.d(TAG, "===>sdcard path selected? "+isChecked);
//					if(isChecked==true)
//					{
//						mCurPath = Configs.SDCARD_ROOT_PATH;
//						fileOpen(new File(mCurPath));
//					}
//				}
//			});
//	        
	 }
	 
	 /**
	  * fileOpen - open the given file path
	  * @param file
	  */
	 public void fileOpen(File file)
	 {
		 //mPath.setText(mCurPath);
		 mContent.clear();
		 Debug.d(TAG, ""+file.getPath()+", exists = "+file.exists());
		 File [] files;
		 if(mSuffix == null)
		 {
			 Debug.d(TAG, "suffix is null");
			 files = file.listFiles();
		 }
		 else
			 files = file.listFiles(new FilenameSuffixFilter(mSuffix));
		 if(files == null)
		 {
			 Debug.d(TAG, "Please plugin a USB device ");
			 ToastUtil.show(getContext(), R.string.toast_plug_usb);
			 mFileList.setAdapter(mFileAdapter);
			return;
		 }
		 //Debug.d(TAG, "files ="+files);
		 
		 if(!Configs.isRootpath(file.getPath()))
		 {
			 Map<String, Object> m = new HashMap<String, Object>();
			 m.put("icon", R.drawable.icon_directory);
			 m.put("name", "..");
			 m.put("path", file.getParent());
			 m.put("operation",R.drawable.file_delete);
			 mContent.add(m);
		 }
		 for(int i=0; files != null && i< files.length; i++)
		 {
			 Debug.d(TAG,"file name="+files[i].toString());
			 Map<String, Object> map = new HashMap<String, Object>();
			 if(files[i].isDirectory())
			 {				 
				 map.put("icon", R.drawable.icon_directory);
			 }
			 if(files[i].isFile())
			 {
				 map.put("icon", R.drawable.icon_file);
			 }
			 map.put("name", files[i].getName());
			 map.put("path", files[i].getPath());
			 map.put("operation", R.drawable.file_delete);
			 mContent.add(map);
		 }
		 mFileList.setAdapter(mFileAdapter);
	 }
	 
	 /**
	  * file - 
	  * @return current opened path
	  */
	 static public String file()
	 {
		 Debug.d(TAG, "file ="+mCurPath);
		 return mCurPath;
	 }
	 
	 /**
	  * getObjName - get the selected tlk
	  * @return tlk file 
	  */
	 static public String getObjName()
	 {
		 Debug.d(TAG, "file ="+objDir);
		 return objDir;
	 }
	 
}
