package com.industry.printer.object;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

import com.industry.printer.PHeader.N_NozzleManager;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by hmwan on 2021/10/12.
 */

public class N_MessageTask {
    private static final String TAG = N_MessageTask.class.getSimpleName();

    public enum Mode {
        EDIT, PRINT
    }

    protected Mode mMode = Mode.EDIT;

    private ArrayList<N_BaseObject> mObjects = null;
    private Rect mStandardFrame;            // 点数
//    private Rect mLastPreviewFrame;
    private Rect mPreviewFrame;             // 点数
//    private Rect mLastPrintFrame;
    private Rect mPrintFrame;               // 横向：列数；纵向：字节数

    private Bitmap mPreviewBitmap;

    private ArrayList<N_BaseObject> mMovedObjs = new ArrayList<N_BaseObject>();
    public final static int STANDARD_HEIGHT = 152;
    public final static int EDIT_PREVIEW_HEIGHT = STANDARD_HEIGHT;
    public final static int PRINT_PREVIEW_HEIGHT = 100;

    private float mPreviewHeightRatio = 1.0f;
    private float mPrintHeightRatio = 1.0f;

    public float getPreviewHeightRatio() {
        return mPreviewHeightRatio;
    }

    public float getPrintHeightRatio() {
        return mPrintHeightRatio;
    }

    public int pixel2Bytes(int pixels) {
        return (int)Math.ceil(1.0f * pixels / 8);
    }

    public N_MessageTask(Mode mode) {
        mMode = mode;
        mObjects = new ArrayList<N_BaseObject>();
//        mLastPreviewFrame = new Rect(0, 0, 0, PREVIEW_HEIGHT);
        mStandardFrame = new Rect(0, 0, 0, STANDARD_HEIGHT);
        mPreviewFrame = new Rect(0, 0, 0, mMode == Mode.EDIT ? EDIT_PREVIEW_HEIGHT : PRINT_PREVIEW_HEIGHT);
        mPreviewHeightRatio = 1.0f * mPreviewFrame.height() / mStandardFrame.height();
        mPrintHeightRatio = N_NozzleManager.getSystemNozzle().getDrawHeight() / mStandardFrame.height();
//        mLastPrintFrame = new Rect(0, 0, 0, N_NozzleManager.getSystemNozzle().getDrawHeight());
        mPrintFrame = new Rect(0, 0, 0, pixel2Bytes(N_NozzleManager.getSystemNozzle().getBufferHeight()));
    }

    public int getStandardHeight() {
        return STANDARD_HEIGHT;
    }

    public boolean hasBlockObj(List<N_BaseObject> excepts, Rect affectecRect) {
        boolean has = false;

        for(N_BaseObject aObj : mObjects) {
            if(excepts.contains(aObj)) continue;
            if(aObj.isPositionFixed()) continue;	// 当前变量为固定变量，不在考虑之列
            if(aObj.isForwardBlock(affectecRect)) {
                has = true;
                break;
            }
        }
//		Debug.d(TAG, "TEST - hasBlockObj =  " + has);
        return has;
    }

    public int getTargetLeft(List<N_BaseObject> excepts, Rect affectRect) {
        for(N_BaseObject aObj : mObjects) {
            if(excepts.contains(aObj)) continue;
//			if(aObj.isPositionFixed()) continue;	// 当前变量为固定变量，不在考虑之列
            affectRect.left = aObj.getBackwardLine(affectRect);
//			Debug.d(TAG, "TEST - affectRect.left " + affectRect.left);
        }
        return affectRect.left;
    }

    /*
        对于一个发生了位置变化，或者宽度变化的变量，总而言之是变量（称原变量）的右边沿发生了位移，调整所有受此影响而必须发生位移的变量（称目标变量），并且实施位移。目标变量必须满足以下条件：
        1. 目标变量的定位类型不能为固定位置，否则不需要位移
        2. 目标变量不能完全在原变量的上部或者下部，否则不会受到原变量右边位移的影响
        原变量拓宽的情况下，
        3. 如果目标变量的左边线在原变量位移前的右边线的左侧，则不会受到原变量位移的影响
        4. 如果目标变量的左边线在原变量位移后的右边线的右侧，则不会受到原变量位移的影响
        5. 如果在原变量位移前的右边线到目标变量的左边线之间没有阻挡变量（即不固定位置，且左边线和右边线完全嵌入其中间，且高度有交叉的变量），则将目标变量位移至原变量的位移后右边线位置
        原变量变窄的情况下，
        6. 如果目标变量不在原变量的右边线的右侧，则不会受到原变量位移的影响
        7. 位移到与原变量右边线之间最近的一个阻碍变量的右边线处（这个阻碍变量可以是固定位置的）
     */
    private void adjustPosition(N_BaseObject orgObj) {
//		Debug.d(TAG, "TEST - ORG(" + orgObj.mID + ") mLastRect " + orgObj.mLastRect.toString() + " - mCurRect " + orgObj.mCurRect.toString());

        for(N_BaseObject tarObj : mObjects) {        // 目标变量
//			Debug.d(TAG, "TEST - TAR(" + tarObj.mID + ") mLastRect " + tarObj.mLastRect.toString() + " - mCurRect " + tarObj.mCurRect.toString());
            if(tarObj.adjustPosition(orgObj)) {
                mMovedObjs.remove(tarObj);
                mMovedObjs.add(tarObj);
                Debug.d(TAG, "TEST - moved (" + tarObj.mId + ") to " + tarObj.mCurRect.toString() + " by (" + orgObj.mId + ")");
            }
        }
    }

    private void finishMove() {
        for(N_BaseObject obj : mObjects) {
            obj.finishMove();
        }
    }

    private void updatePreviewFrame() {
        mPreviewFrame.right = 0;
        for(N_BaseObject obj : mObjects) {
            mPreviewFrame.right = Math.max(mPreviewFrame.right, obj.getCurrentRect().right);
        }
    }
    /*
        对于一个宽度发生了变化的变量(orgObj)，其右边边缘（right）发生变化，由于这个变化，会有其他的变量直接或间接的受到影响而产生位移，这个函数的目的就是完成这个位移的操作和位移后的重新绘制
     */
    public void onObjectWidthChanged(N_BaseObject orgObj) {
        if(!mObjects.contains(orgObj)) return;

        mMovedObjs.add(orgObj);
        while(mMovedObjs.size() > 0) {
            adjustPosition(mMovedObjs.get(0));
            mMovedObjs.remove(0);
        }

        finishMove();
        updatePreviewFrame();
/*
        Bitmap tempBitmap;

        if(mPreviewFrame.right > mLastPreviewFrame.right) {
            tempBitmap = Bitmap.createBitmap(mPreviewFrame.width(), mPreviewFrame.height(), Configs.BITMAP_CONFIG);
        } else {
            tempBitmap = mPreviewBitmap;
        }

        Canvas canvas = new Canvas(tempBitmap);
        Paint paint = new Paint();
        paint.setAntiAlias(true);       // 去除锯齿
        paint.setFilterBitmap(true);    // 对位图进行滤波处理
        paint.setStyle(Paint.Style.FILL);
        paint.setColor(Color.WHITE);
        if(!tempBitmap.equals(mPreviewBitmap)) {
            canvas.drawBitmap(mPreviewBitmap, 0, 0, paint);
        }

        for(N_BaseObject obj : mObjects) {
            if(obj.needRedraw()) {
                canvas.drawRect(obj.mLastDrawFrame, paint);
                canvas.drawBitmap(obj.mPreviewBitmap, obj.mDrawFrame.left, obj.mDrawFrame.top, paint);
                for(N_BaseObject obj1 : mObjects) {
                    Rect rect = new Rect(obj1.mDrawFrame);
                    if(!obj1.needRedraw() && rect.intersect(obj.mLastDrawFrame)) {
                        Rect rect1 = new Rect(rect);
                        rect1.offsetTo(0, 0);
                        canvas.drawBitmap(obj1.mPreviewBitmap, rect1, rect, paint);
                    }
                    rect = new Rect(obj1.mDrawFrame);
                    if(!obj1.needRedraw() && rect.intersect(obj.mDrawFrame)) {
                        Rect rect1 = new Rect(rect);
                        rect1.offsetTo(0, 0);
                        canvas.drawBitmap(obj1.mPreviewBitmap, rect1, rect, paint);
                    }
                }
                obj.mLastDrawFrame.set(obj.mDrawFrame);
                obj.clearRedrawFlag();
            }
        }

        if(mPreviewFrame.right < mLastPreviewFrame.right) {
            mPreviewBitmap = Bitmap.createBitmap(tempBitmap, 0, 0, mPreviewFrame.width(), mPreviewFrame.height());
        } else {
            mPreviewBitmap = tempBitmap;
        }
        mLastPreviewFrame.set(mPreviewFrame);
*/
    }

    public N_BaseObject getParentObject(int parentIndex) {
        for(N_BaseObject obj : mObjects) {
            if(obj.getIndex() == parentIndex) {
                return obj;
            }
        }
        return null;
    }
}
