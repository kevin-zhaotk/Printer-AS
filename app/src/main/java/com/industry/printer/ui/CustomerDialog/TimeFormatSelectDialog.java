package com.industry.printer.ui.CustomerDialog;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
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

import com.industry.printer.R;

/**
 * Created by hmwan on 2020/8/6.
 */

public class TimeFormatSelectDialog extends Dialog implements android.view.View.OnClickListener, AdapterView.OnItemClickListener {
    private static final String TAG = FontSelectDialog.class.getSimpleName();
    private Context mContext;
    private GridView mTimeFormatlist;
    private TimeFormatSelectAdapter mAdapter;

    private TextView mOk;
    private TextView mCancel;

    private Handler mHandler;

    private String mOriginalFormat = "";

    public TimeFormatSelectDialog(Context context, Handler handler) {
        super(context, R.style.Dialog_Fullscreen);
        mContext = context;
        mHandler = handler;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.time_format_select_layout);

        mOk = (TextView) findViewById(R.id.confirm);
        mCancel = (TextView) findViewById(R.id.cancel);

        mOk.setOnClickListener(this);
        mCancel.setOnClickListener(this);

        mTimeFormatlist = (GridView) findViewById(R.id.timeFormatList);
        mTimeFormatlist.setSelector(new ColorDrawable(Color.TRANSPARENT));
        mAdapter = new TimeFormatSelectDialog.TimeFormatSelectAdapter();

        mTimeFormatlist.setAdapter(mAdapter);
        mTimeFormatlist.setOnItemClickListener(this);
    }

    public void setCurrentFormat(String fmt) {
        mOriginalFormat = fmt;
    }

    public class TimeFormatSelectAdapter extends BaseAdapter {

        private int mSelectedPos = 0;
        private LayoutInflater mInflater;
        private String[] mTimeFormats;

        public TimeFormatSelectAdapter() {
            mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            init();
        }

        private void init() {
            mTimeFormats = mContext.getResources().getStringArray(R.array.strTimeFormat);
            for(int i=0; i<mTimeFormats.length; i++) {
                if(mOriginalFormat.equals(mTimeFormats[i])) {
                    setSelect(i);
                }
            }
        }

        public void setSelect(int pos) {
            mSelectedPos = pos;
            notifyDataSetChanged();
        }

        public String getSelectedItem() {
            return mTimeFormats[mSelectedPos];
        }

        @Override
        public int getCount() {
            return mTimeFormats.length;
        }

        @Override
        public Object getItem(int arg0) {
            return mTimeFormats[arg0];
        }

        @Override
        public long getItemId(int arg0) {
            return arg0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.time_format_item, null);
            }

            TextView tv = (TextView) convertView.findViewById(R.id.timeFormat);
            tv.setText(mTimeFormats[position]);
            if(position == mSelectedPos) {
                tv.setTextColor(Color.WHITE);
                tv.setBackgroundColor(Color.parseColor("#44000000"));
            } else {
                tv.setTextColor(Color.LTGRAY);
                tv.setBackgroundColor(Color.TRANSPARENT);
            }
            return convertView;
        }
    }

    @Override
    public void onClick(View arg0) {
        switch (arg0.getId()) {
            case R.id.confirm:
                Message msg = mHandler.obtainMessage(ObjectInfoDialog.MSG_SELECTED_TIME_FORMAT);
                Bundle bundle = new Bundle();
                bundle.putString("format", mAdapter.getSelectedItem());
                msg.setData(bundle);
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
