package com.industry.printer.ui.CustomerAdapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.industry.printer.MessageTask;
import com.industry.printer.R;
import com.industry.printer.object.BaseObject;

public class ObjectListAdapter extends BaseAdapter {

    private MessageTask mTask;
    private LayoutInflater mInflater;
    private int currentSelect;

    public ObjectListAdapter(Context ctx, MessageTask task, int selection) {
        mTask = task;
        currentSelect = selection;
        init(ctx);
    }

    private void init(Context ctx) {
        mInflater = (LayoutInflater) ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    public void setSelect(int position) {
        currentSelect = position;
        notifyDataSetChanged();
    }

    public int getSelect() {
        return currentSelect;
    }

    @Override
    public int getCount() {
        return mTask == null ? 0 : mTask.getObjects().size();
    }

    @Override
    public Object getItem(int position) {
        return mTask == null ? null : mTask.getObjects().get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Holder holder = null;
        if (convertView != null) {
            holder = (Holder) convertView.getTag();
        } else {
            convertView = mInflater.inflate(R.layout.object_item_layout, null);
            holder = new Holder();
            holder.mText = (TextView) convertView.findViewById(R.id.object_name);
            convertView.setTag(holder);
        }

        BaseObject object = mTask.getObjects().get(position);
        holder.mText.setText( (position + 1) + ". " + object.mName);

        if (position == currentSelect) {
            holder.mText.setSelected(true);
        } else {
            holder.mText.setSelected(false);
        }

        return convertView;
    }

    private class Holder {
        public TextView mText;
    }
}
