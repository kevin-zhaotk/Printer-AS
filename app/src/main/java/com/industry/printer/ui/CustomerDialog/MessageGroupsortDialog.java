package com.industry.printer.ui.CustomerDialog;

import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;

import com.industry.printer.MessageTask;
import com.industry.printer.R;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.ui.CustomerAdapter.MessageListAdater;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

/**
 * Created by kevin on 2018/3/8.
 */

public class MessageGroupsortDialog extends CustomerDialogBase implements View.OnClickListener, OnItemClickListener {

    private ImageButton mUp;
    private ImageButton mDown;
    private RelativeLayout mOk;
    private ListView    mListview;
    private MessageListAdater mFileAdapter;
    
    private int mSelected = -1;
    
    LinkedList<Map<String, Object>> tlkList = new LinkedList<Map<String, Object>>();

    public MessageGroupsortDialog(Context context, ArrayList<String> tlks) {
        super(context, R.style.Dialog_Fullscreen);

        if (tlks != null) {
            for (String t : tlks) {
                Map<String, Object> m = new HashMap<String, Object>();
                m.put("title", t);
                tlkList.add(m);
            }
            
        }
        mFileAdapter = new MessageListAdater(context,
                tlkList,
                R.layout.message_item_layout,
                new String[]{"title", "abstract", ""},
                // new int[]{R.id.tv_message_title, R.id.tv_message_abstract
                new int[]{R.id.tv_msg_title, R.id.ll_preview, R.id.image_selected});
    }

    @Override
    public void setOnPositiveClickedListener(OnPositiveListener listener) {
        super.setOnPositiveClickedListener(listener);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.group_sort_layout);

        mDown = (ImageButton) findViewById(R.id.move_down);
        mDown.setOnClickListener(this);
        mUp = (ImageButton) findViewById(R.id.move_up);
        mUp.setOnClickListener(this);
        mOk = (RelativeLayout) findViewById(R.id.ok);
        mOk.setOnClickListener(this);

        mListview = (ListView) findViewById(R.id.message_listview);
        mListview.setAdapter(mFileAdapter);
        mListview.setOnItemClickListener(this);
        // mListview.setOnItemSelectedListener();
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.move_up:
                Debug.d("XXX", "--->up mSelected: " + mSelected);
                if (mSelected > 0) {
                    Map<String, Object> m = tlkList.remove(mSelected);
                    mSelected = mSelected - 1;
                    tlkList.add(mSelected, m);
                    mFileAdapter.setSelected(mSelected);
                    mFileAdapter.notifyDataSetChanged();
                }
                
                break;

            case R.id.move_down:
            	Debug.d("XXX", "--->down mSelected: " + mSelected);
                if (mSelected >= 0 && mSelected < mListview.getCount() - 1) {
                    Map<String, Object> m = tlkList.remove(mSelected);
                    mSelected += 1;
                    tlkList.add(mSelected, m);
                    mFileAdapter.setSelected(mSelected);
                    mFileAdapter.notifyDataSetChanged();
                }
                
                break;

            case R.id.ok:
                StringBuilder result = new StringBuilder();
                for (int i = 0; i < tlkList.size(); i++) {
                    Map<String, Object> m = tlkList.get(i);
                    String name = (String) m.get("title");
                    result.append(name);
                    if (i != tlkList.size() - 1) result.append("^");
                }
                /* save group */
                String group = "Group-" + getSuffix();
                MessageTask.saveGroup(group, result.toString());
                if (pListener != null) {
                    pListener.onClick(group);
                }
                dismiss();
                break;

        }
    }
    
    @Override
	public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    	mFileAdapter.setSelected(position);
    	mFileAdapter.notifyDataSetChanged();
    	mSelected = position;
    }
    
    private int getSuffix() {
    	File dir = new File(ConfigPath.getTlkPath());
    	String[] files = dir.list(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String name) {
				if (name != null && name.startsWith("Group-")) {
					return true;
				} else {
					return false;
				}
			}
		});
    	if (files == null || files.length <= 0) {
			return 1;
		}
    	int max = 0;
    	for (String file : files) {
    		if (file.length()<= 6){
    			continue;
    		}
    		try {
    			int cur = Integer.valueOf(file.substring(6));
    			max = max <= cur ? cur: max;
    		} catch (Exception e) {
				// 
			}
			
 		}
    	return max+1;
    }
}
