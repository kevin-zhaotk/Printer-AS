package com.industry.printer;

import com.industry.printer.ui.CustomerAdapter.SettingsListAdapter;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

@Deprecated
public class SettingsFragment extends Fragment {
	
	private static String TAG = SettingsFragment.class.getSimpleName();
	
	public Context mContext;
	public ListView mList;
	public SettingsListAdapter mAdapter;
	
	public SettingsFragment(Context context) {
		mContext = context;
	}
	@Override  
    public void onCreate(Bundle savedInstanceState)  
    {  
        super.onCreate(savedInstanceState);  
    }
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		Log.d(TAG, "===>onCreateView");
		
		return inflater.inflate(R.layout.settings_frame_list, container, false);
	}
	
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
		mList = (ListView) getView().findViewById(R.id.settings_list_view);
		mAdapter = new SettingsListAdapter(mContext);
		mList.setAdapter(mAdapter);
	}
	
	public void onConfigureChanged() {
		mAdapter.notifyDataSetChanged();
	}
	

}
