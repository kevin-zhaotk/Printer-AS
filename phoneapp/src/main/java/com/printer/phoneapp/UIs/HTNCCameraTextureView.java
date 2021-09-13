package com.printer.phoneapp.UIs;

import android.content.Context;
import android.util.AttributeSet;
import android.view.TextureView;
import android.view.View;

/**
 * Created by hmwan on 2021/9/7.
 */

public class HTNCCameraTextureView extends TextureView {
    private int mRatioWidth = 0;
    private int mRatioHeight = 0;

    public HTNCCameraTextureView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    public void setAspectRatio(int width, int height) {
        mRatioWidth = width;
        mRatioHeight = height;
        requestLayout();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = View.MeasureSpec.getSize(widthMeasureSpec);
        int height = View.MeasureSpec.getSize(heightMeasureSpec);
        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height);
        } else {
// 这个算法是以大边为标准按比例展开，这样会得到不变形全屏显示的效果，但是会有越出屏幕而看不到的内容
            if(1.0f * width / height > 1.0f * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
            }
// 这个算法是以小边为标准按比例展开，这样会得到不变形全部显示的效果，屏幕上会出现空白区
/*            if(1.0f * width / height < 1.0f * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
            }
*/
        }
    }
}
