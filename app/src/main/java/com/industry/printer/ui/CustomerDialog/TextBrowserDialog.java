package com.industry.printer.ui.CustomerDialog;

import java.io.File;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.industry.printer.R;
import com.industry.printer.FileFormat.FilenameSuffixFilter;
import com.industry.printer.FileFormat.TextInputStream;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.ui.CustomerAdapter.ListViewButtonAdapter;
import com.industry.printer.ui.CustomerAdapter.PreviewAdapter;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.Toast;

public class TextBrowserDialog extends CustomerDialogBase implements android.view.View.OnClickListener {
	
	public static final String TAG = TextBrowserDialog.class.getSimpleName();
	
	public PreviewAdapter mFileAdapter;
	public LinkedList<Map<String, Object>> mContent;
	
	public ListView mFileList;
	
	/**
	 * mSuffix  the file suffix for filter
	 */
	public String mSuffix;
	
	/**
	 * mSave save button
	 */
	public Button mEnter;

	/**
	 * mCancel cancel button
	 */
	public Button mCancel;
	
	/**
	 * mPagePrev cancel button
	 */
	public Button mPagePrev;

	/**
	 * mPageNext cancel button
	 */
	public Button mPageNext;
	
	/**
	 * mName the tlk file name tobe save as
	 */
	/**
	 * TextBrowserDialog - construct function
	 * @param context
	 */
	public TextBrowserDialog(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		
		mContent = new LinkedList<Map<String, Object>>();
		
//		mFileAdapter = new SimpleAdapter(context, 
//				mContent, 
//				R.layout.text_browser_item, 
//				new String[]{"content"}, 
//				new int []{R.id.text_browser_content});
		mFileAdapter = new PreviewAdapter(context, 
				mContent, 
				R.layout.text_browser_item, 
				new String[]{"content"}, 
				new int[] {R.id.text_browser_content});
		mSuffix = null;
	}
	
	/**
	 * FileBrowserDialog - construct function
	 * @param context
	 * @param path	path to be open
	 */
	public TextBrowserDialog(Context context, String suffix)
	{
		this(context);
		mSuffix = suffix;
	}
	
	 @Override
	 protected void onCreate(Bundle savedInstanceState) {
	        // TODO Auto-generated method stub
	        super.onCreate(savedInstanceState);
	        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
	        this.setContentView(R.layout.text_browser_dialog);
	        Debug.d(TAG, "===>onCreate");
	        mEnter = (Button) findViewById(R.id.dialog_save);
	        mEnter.setOnClickListener(this);
	        
	        mCancel = (Button) findViewById(R.id.dialog_cancel);
	        mCancel.setOnClickListener(this);
	        
	        mPagePrev = (Button) findViewById(R.id.dialog_page_prev);
	        mPagePrev.setOnClickListener(this);
	        
	        mPageNext = (Button) findViewById(R.id.dialog_page_next);
	        mPageNext.setOnClickListener(this);
	        
	        mFileList =(ListView) findViewById(R.id.file_list);
	        
	        mFileList.setOnItemClickListener(new OnItemClickListener(){

				@Override
				public void onItemClick(AdapterView<?> parent, View view,
						int position, long id) {
					Debug.d(TAG, "--->onitemclick " + position);
					mFileAdapter.setChecked(position);
					mFileAdapter.notifyDataSetChanged();
				}
	        	
	        });
	        
	        String path = ConfigPath.getTxtPath();
	        if (path != null) {
	        	fileOpen(new File(path));
			}

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
			 ToastUtil.show(getContext(), "Please plugin a USB device ");
			 mFileList.setAdapter(mFileAdapter);
			return;
		 }
		 
		 for(int i=0; files != null && i< files.length; i++)
		 {
			 Debug.d(TAG,"file name=" + files[i].toString());
			 Map<String, Object> map = new HashMap<String, Object>();
			 TextInputStream stream = TextInputStream.getInstance();
			 String txt = stream.getText(files[i].getAbsolutePath());
			 Debug.d(TAG, "--->txt: " +txt);
			 map.put("content", txt);
			 mContent.add(map);
		 }
		 mFileList.setAdapter(mFileAdapter);
	 }

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.dialog_save:
			int position = mFileAdapter.getChecked();
			Map<String, Object> item = (Map<String, Object>) mFileList.getItemAtPosition(position);
			
			if(pListener != null) {
				if (item == null) {
					pListener.onClick();
				} else {
					pListener.onClick((String) item.get("content"));
				}
			}
			dismiss();	
			break;
		case R.id.dialog_cancel:
			cancel();
			if(nListener != null)
				nListener.onClick();
			break;
		case R.id.dialog_page_prev:
			mFileList.smoothScrollBy(0, -300);
			break;
		case R.id.dialog_page_next:
			mFileList.smoothScrollBy(0, 300);
			break;
		
		default:
			break;
		}
	}
}
