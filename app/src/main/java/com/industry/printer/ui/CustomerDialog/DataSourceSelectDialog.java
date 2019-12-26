package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;
import com.industry.printer.ui.CustomerAdapter.SettingsListAdapter;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.TextView;

/**
 * Created by hmwan on 2019/12/19.
 */

public class DataSourceSelectDialog extends Dialog implements android.view.View.OnClickListener, AdapterView.OnItemClickListener {

    private Context                 mContext;

    private GridView                mDataSourceGV;
    private DataSourceGVAdapter     mAdapter;

    private TextView                mConfirm;
    private TextView                mCancel;

    private Handler                 mHandler;
    private int                     mSelected;

    public DataSourceSelectDialog(Context context, Handler handler, int current) {
        super(context, R.style.Dialog_Fullscreen);
        mContext = context;
        mHandler = handler;
        mSelected = current;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.data_source_select_layout);

        mConfirm = (TextView) findViewById(R.id.confirm);
        mCancel = (TextView) findViewById(R.id.cancel);

        mConfirm.setOnClickListener(this);
        mCancel.setOnClickListener(this);

        mDataSourceGV = (GridView) findViewById(R.id.data_source_grid);
        mAdapter = new DataSourceGVAdapter(mSelected);


        mDataSourceGV.setAdapter(mAdapter);
        mDataSourceGV.setOnItemClickListener(this);
    }


    public class DataSourceGVAdapter extends BaseAdapter {

        private int             position=0;
        private Holder          mHolder;
        private LayoutInflater  mInflater;
        private String[]        mDataSources;


        public DataSourceGVAdapter(int pos) {
            mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            position = pos;
            init();
        }

        private void init() {
            mDataSources = mContext.getResources().getStringArray(R.array.strDataSourceArray);
        }

        public void setSelect(int pos) {
            position = pos;
            notifyDataSetChanged();
        }

        public String getSelectedItem() {
            return (position >= mDataSources.length ? "" : mDataSources[position]);
        }

        public int getSelected() {
            return position;
        }

        @Override
        public int getCount() {
            return mDataSources.length;
        }

        @Override
        public Object getItem(int arg0) {
            return (arg0 >= mDataSources.length ? null :mDataSources[arg0]);
        }

        @Override
        public long getItemId(int arg0) {
            return arg0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView != null) {
                mHolder = (Holder) convertView.getTag();
            } else {
                convertView = mInflater.inflate(R.layout.header_item_layout, null);
                mHolder = new Holder();
                mHolder.mText = (TextView) convertView.findViewById(R.id.font);
            }

            mHolder.mText.setText(position >= mDataSources.length ? "" : mDataSources[position]);
            if (position == this.position) {
                mHolder.mText.setSelected(true);
            } else {
                mHolder.mText.setSelected(false);
            }

            convertView.setTag(mHolder);
            return convertView;
        }


    }

    public class Holder {
        public TextView mText;
    }

    @Override
    public void onClick(View arg0) {
        switch (arg0.getId()) {
            case R.id.confirm:
                Message msg = mHandler.obtainMessage(SettingsListAdapter.MSG_DATA_SOURCE_SELECTED);
                msg.arg1 = mAdapter.getSelected();
                mHandler.sendMessage(msg);
                dismiss();
                break;
            case R.id.cancel:
                dismiss();
            default:
                break;
        }
    }

    @Override
    public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {
        mAdapter.setSelect(position);
    }


}
