package com.industry.printer.ui;

import android.R.integer;
import android.app.ActionBar;
import android.app.ActionBar.LayoutParams;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.opengl.Visibility;
import android.text.Layout;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsoluteLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.ImageView.ScaleType;

import com.industry.printer.MessageTask;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.MessageObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.zip.Inflater;

/**
 * Created by kevin on 2017/7/17.
 */
public class MessageDisplayManager implements View.OnTouchListener {

    private static final String TAG = MessageDisplayManager.class.getSimpleName();

    private Context mContext;
    private ViewGroup mContainer;
    private MessageTask mTask;
    private View mCursor;
    
    /** 
     * The select shadow, show up when object selected 
     */
    private ImageView mShadow;

    private HashMap<BaseObject, ViewGroup> mImageMap;

    public MessageDisplayManager(Context ctx, ViewGroup container, MessageTask task) {
        mContext = ctx;
        mContainer = container;
        mTask = task;
        mShadow = new ImageView(mContext);
        
        mImageMap = new HashMap<BaseObject, ViewGroup>();

        reset();
    }


    public void reset() {
        if (mContainer != null) {
            mContainer.removeAllViews();
        }
        mImageMap.clear();
        mShadow.setImageResource(R.drawable.msg_bg_selected);
        mShadow.setVisibility(View.GONE);
        if (mShadow != null) {
            mContainer.addView(mShadow, 0);
        }
        initCursor();
//        mShadow.setLayoutParams(new RelativeLayout.LayoutParams(150, 150));
    }

    public void fill(MessageTask task) {
        if (task == null) {
            return;
        }
        removeAll();
        mTask = task;
        for (BaseObject object : mTask.getObjects()) {
            add(object);
        }
    }
    
    public MessageTask getTask() {
    	return mTask;
    }

    public void add(BaseObject object) {

        if (object == null) {
            return;
        }
        if (object instanceof MessageObject) {
        	// if there is already a messageobject, do nothing
        	if (mTask.getMsgObject() != null) {
        		return;
        	}
        	initCursor();
        }
        if (mImageMap.containsKey(object)) {
            return;
        }
        draw(object);
//        mTask.addObject(object);
        setSelect(object);
    }

    public void remove(BaseObject object) {
        if (!mImageMap.containsKey(object)) {
            return;
        }
        ViewGroup view = mImageMap.get(object);
        mImageMap.remove(object);
        mContainer.removeView(view);
        mTask.removeObject(object);
        mShadow.setVisibility(View.GONE);
    }

    public void removeAll() {
        if (mImageMap != null) {
            mImageMap.clear();
        }
        if (mContainer != null) {
            mContainer.removeAllViews();
            mShadow.setVisibility(View.GONE);
            mContainer.addView(mShadow);
        }
        mTask.removeAll();
    }

    public void update(BaseObject object) {
        Debug.i(TAG, "--->update: " + object.mIsSelected);
        if (object instanceof MessageObject) {
            if (object.mIsSelected) {
                showCursor((int) object.getX(), (int) object.getY());
            } else {
                hideCursor();
            }
            return;
        }
        if (!mImageMap.containsKey(object)) {
            return;
        }
        ViewGroup view = mImageMap.get(object);
//        mImageMap.remove(object);
//        mContainer.removeView(view);
//
//        draw(object);
        RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) view.getLayoutParams();
        params.width = (int)object.getWidth();
        params.height = (int)object.getHeight();
        params.topMargin = (int) object.getY();
        params.leftMargin = (int) object.getX();
        view.setLayoutParams(params);

        // H.M.Wang修改，以显示当前对象的选择框
//        showSelectRect(params.leftMargin, params.topMargin, params.width, params.height);
        if(object.mIsSelected) {
            showSelectRect((int)object.getX(), (int)object.getY(), (int)object.getWidth(), (int) object.getHeight());
        }
    }

    /**
     * initialize cross cursor
     */
    private void initCursor() {

        LayoutInflater inflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mCursor = inflater.inflate(R.layout.layout_cross_cursor, null);

        mCursor.setVisibility(View.VISIBLE);
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp.leftMargin = 0;
        lp.topMargin = 0;
        mContainer.addView(mCursor, lp);

    }

    /**
     * show cross cursor at the specified position
     * @param x
     * @param y
     */
    private void showCursor(int x, int y) {
        Debug.d(TAG, "--->showCursor: " + x + " ," + y);
        RelativeLayout.LayoutParams lp = (RelativeLayout.LayoutParams) mCursor.getLayoutParams();

        lp.topMargin = y - 15 > 0 ? y - 15 : 0;
        lp.leftMargin = x - 15 > 0 ? x - 15 : 0;
        mCursor.setLayoutParams(lp);
        mCursor.setVisibility(View.VISIBLE);
    }

    private void hideCursor() {
        mCursor.setVisibility(View.GONE);
    }

    
    public void updateDraw(BaseObject object) {
        if (object instanceof MessageObject) {
            return;
        }
        if (!mImageMap.containsKey(object)) {
            return;
        }
        ViewGroup view = mImageMap.get(object);
        mImageMap.remove(object);
        mContainer.removeView(view);
        draw(object);
    }

    private void draw(BaseObject object) {
//    	Debug.d(TAG, "--->draw: " + mContainer.getMeasuredHeight());
    	if (object instanceof MessageObject) {
            return;
        }
    	
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp.leftMargin = (int)object.getX();
        lp.topMargin = (int) object.getY();


//        ImageView image = new ImageView(mContext);
//
//        image.setScaleType(ImageView.ScaleType.FIT_XY);
        Bitmap bmp = object.getScaledBitmap(mContext);
//
//        image.setImageBitmap(bmp);

        ViewGroup vg = drawEach(object, bmp);
        /** width&height must be reseted after object bitmap drawed success */

        lp.height = (int)object.getHeight();
        mContainer.addView(vg, -1,lp);
        mImageMap.put(object, vg);
        vg.setTag(object);
        vg.setOnTouchListener(this);

        // H.M.Wang修改，以显示当前对象的选择框
//        showSelectRect(lp.leftMargin, lp.topMargin, lp.width, lp.height);
        if(object.mIsSelected) {
            showSelectRect((int)object.getX(), (int)object.getY(), (int)object.getWidth(), (int) object.getHeight());
        }
    }


    private ViewGroup drawEach(BaseObject object, Bitmap bmp) {
        LayoutInflater inflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        // LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.edit_image_layout, null);
        // ViewGroup.LayoutParams param = layout.getLayoutParams();
        LinearLayout layout = new LinearLayout(mContext);
        LinearLayout.LayoutParams param = new LinearLayout.LayoutParams((int)object.getWidth(), (int)object.getHeight());

        Debug.d(TAG, "--->w: " + param.width);
//        param.width = (int) object.getWidth();
//        param.height = (int) object.getHeight();
        layout.setOrientation(LinearLayout.HORIZONTAL); 
        layout.setLayoutParams(param);
        if (bmp == null) {
            return layout;
        }

        for (int w = 0; w < bmp.getWidth(); ) {
            float wd = 0f;
            if (w + 1500 < bmp.getWidth()) {
                wd = 1200;
            } else {
                wd = bmp.getWidth() - w;
            }
            ImageView image = new ImageView(mContext);
            Bitmap b = Bitmap.createBitmap(bmp, w, 0, (int) wd, bmp.getHeight());

            image.setScaleType(ScaleType.FIT_XY);
            image.setImageBitmap(b);
            LinearLayout.LayoutParams p = new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT);
            p.weight = wd/param.width;
            w = w + (int) wd;
            Debug.d(TAG, "--->p.weight= " + p.weight);
            layout.addView(image, -1, p);
        }
        return layout;
    }
    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        BaseObject object = (BaseObject)view.getTag();
        Log.d(TAG, "--->onTouch: " + object.mId);
        if (object.getSelected()) {
            return true;
        }

        setSelect(object);
        return true;
    }

    public void setSelect(int i) {
        ArrayList<BaseObject> objects = mTask.getObjects();
        if (i >= objects.size()) {
			return;
		}
        BaseObject object = objects.get(i);
        setSelect(object);
    }

    public void setSelect(BaseObject object) {


        ArrayList<BaseObject> objects = mTask.getObjects();

        for(BaseObject obj : objects)
        {
            if (obj.getSelected()) {
                // ViewGroup view = mImageMap.get(obj);
                obj.setSelected(false);
            }

        }
        if (object == null) {
            return;
        }
        object.setSelected(true);
        showSelectRect((int)object.getX(), (int)object.getY(), (int)object.getWidth(), (int) object.getHeight());
    }

    private void showSelectRect(int x, int y, int w, int h) {
    	// mContainer.removeView(mShadow);
    	RelativeLayout.LayoutParams lp = (RelativeLayout.LayoutParams) mShadow.getLayoutParams();
    	lp.width = w;
    	lp.height = h;
    	lp.leftMargin = x;
    	lp.topMargin = y;
    	// mContainer.addView(mShadow, 0, lp);
    	mShadow.setVisibility(View.VISIBLE);
    	mShadow.setLayoutParams(lp);
    	mShadow.bringToFront();
    }
    
}
