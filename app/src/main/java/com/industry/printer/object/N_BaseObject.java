package com.industry.printer.object;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Message;

import com.industry.printer.MessageTask;
import com.industry.printer.PHeader.N_NozzleManager;
import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

import java.util.Arrays;

/**
 * Created by hmwan on 2021/10/11.
 */

abstract public class N_BaseObject {
    private static final String TAG = N_BaseObject.class.getSimpleName();

    protected final int NUMBER_OF_TLK_INDEXS                    = 22;                                   // TLK每行项目数量

    protected final int TLK_INDEX_0                             = 0;                                    // 0. 号元素
    protected final int TLK_INDEX_1                             = 1;                                    // 1. 号元素
    protected final int TLK_INDEX_2                             = 2;                                    // 2. 号元素
    protected final int TLK_INDEX_3                             = 3;                                    // 3. 号元素
    protected final int TLK_INDEX_4                             = 4;                                    // 4. 号元素
    protected final int TLK_INDEX_5                             = 5;                                    // 5. 号元素
    protected final int TLK_INDEX_6                             = 6;                                    // 6. 号元素
    protected final int TLK_INDEX_7                             = 7;                                    // 7. 号元素
    protected final int TLK_INDEX_8                             = 8;                                    // 8. 号元素
    protected final int TLK_INDEX_9                             = 9;                                    // 9. 号元素
    protected final int TLK_INDEX_10                            = 10;                                   // 0. 号元素
    protected final int TLK_INDEX_11                            = 11;                                   // 11. 号元素
    protected final int TLK_INDEX_12                            = 12;                                   // 12. 号元素
    protected final int TLK_INDEX_13                            = 13;                                   // 13. 号元素
    protected final int TLK_INDEX_14                            = 14;                                   // 14. 号元素
    protected final int TLK_INDEX_15                            = 15;                                   // 15. 号元素
    protected final int TLK_INDEX_16                            = 16;                                   // 16. 号元素
    protected final int TLK_INDEX_17                            = 17;                                   // 17. 号元素
    protected final int TLK_INDEX_18                            = 18;                                   // 18. 号元素
    protected final int TLK_INDEX_19                            = 19;                                   // 19. 号元素
    protected final int TLK_INDEX_20                            = 20;                                   // 20. 号元素
    protected final int TLK_INDEX_21                            = 21;                                   // 21. 号元素

    protected final int TLK_INDEX_OBJINDEX                      = TLK_INDEX_0;                          // 0. 序号
    protected final int TLK_INDEX_ID                            = TLK_INDEX_1;                          // 1. 种类
    protected final int TLK_INDEX_FRAME_LEFT                    = TLK_INDEX_2;                          // 2. Left坐标（乘2）
    protected final int TLK_INDEX_FRAME_TOP                     = TLK_INDEX_3;                          // 3. Top坐标（乘2）
    protected final int TLK_INDEX_FRAME_RIGHT                   = TLK_INDEX_4;                          // 4. Right坐标（乘2）
    protected final int TLK_INDEX_FRAME_BOTTOM                  = TLK_INDEX_5;                          // 5. Bottom坐标（乘2）
    protected final int TLK_INDEX_DRAGABLE                      = TLK_INDEX_7;                          // 7. 是否可以拖拽
    protected final int TLK_INDEX_REVERSE                       = TLK_INDEX_11;                         // 11. 是否可以反转
    protected final int TLK_INDEX_PARENT                        = TLK_INDEX_17;                         // 17. 父Object

    protected Context mContext;

    // 变量宽度类型的定义
    public final static int WIDTH_TYPE_FIXED        = 0;	    // 宽度类型：固定
    public final static int WIDTH_TYPE_CONTENT      = 1;		// 宽度类型：随内容变化

    // 变量定位类型的定义
    public final static int POSITION_TYPE_FIXED     = 0;		// 定位类型：固定位置
    public final static int POSITION_TYPE_FLOAT     = 1;		// 定位类型：流动位置（根据前面的变量的右边进行位置调整）

    // 变量绘制（包括打印）方法的定义
    public final static int DRAW_TYPE_FIT           = 0;	    // 绘制方法：适合框体。将绘制内容填充到绘制空间，如果不足框体宽度，则放大；反之则缩小
    public final static int DRAW_TYPE_CLIP          = 1;	    // 绘制方法：切除。如果绘制部分超出变量的框体，则切掉多余的部分；否则保持原大小
    public final static int DRAW_TYPE_OVERFLOW      = 2;		// 绘制方法：溢出。如果绘制部分超出变量的框体，则在不更改宽度的情况下溢出描绘；否则保持原大小

    protected int mWidthType                        = WIDTH_TYPE_FIXED;		// 宽度类型，缺省为宽度固定
    protected int mPositionType                     = POSITION_TYPE_FIXED;	// 定位类型：缺省为固定位置
    protected int mDrawType                         = DRAW_TYPE_CLIP;		// 绘制方法：切除

    protected Rect mLastRect;                       // 变量上次的定位信息
    protected Rect mCurRect;                        // 变量当前的定位信息

    protected Rect mPreviewRect;			        // 变量当前的绘图范围
    protected Bitmap mPreviewBitmap = null;			// 预览图
//    protected boolean mPreviewUpdated = false;      // 预览图最新标识
    protected boolean mPreviewNeedReCreate          = false;    // 是否需要重新生成
    protected boolean mPreviewNeedRedraw            = false;    // 是否需要重新绘制


    protected Rect mPrintRect;	        			// 变量粘贴定位信息。主要是调整了top和bottom，使其能够被8整除，即保持整字节
    protected byte[] mPrintBin = null;			    // 打印区域二进制图
//    protected boolean mPrintImageUpdated = false;   // 打印缓冲区最新标识
    protected boolean mPrintImageNeedReCreate       = false;    // 是否需要重新生成
    protected boolean mPrintImageNeedPaste          = false;    // 是否需要重新绘制

    protected String mId;                           // Object的类型
    protected int mIndex;                           // Object的序号
    protected String mContent;                      // 字符串内容
    protected String mName;                         // Object的字符串名称，与类型对应
    protected boolean mDragable;                    // 是否可以拖拽。未被实际用到
    protected boolean mReverse;                     // 是否可以反转。未被实际用到
    protected N_BaseObject mParent;                 // 父变量

    protected N_MessageTask mTask;                  // 变量所属的任务

    public N_BaseObject(Context ctx, N_MessageTask task) {
        mContext = ctx;
        mTask = task;

        mPreviewNeedReCreate = true;
        mPreviewNeedRedraw = true;
        mPrintImageNeedReCreate = true;
        mPrintImageNeedPaste = true;

        mWidthType = WIDTH_TYPE_FIXED;
        mPositionType = POSITION_TYPE_FIXED;
        mDrawType = DRAW_TYPE_FIT;

        mCurRect = new Rect(0, 0, 0, N_MessageTask.STANDARD_HEIGHT);
        mLastRect = new Rect(mCurRect);

        mPreviewRect = new Rect(0, 0, 0, 0);
        mPrintRect = new Rect(0, 0, 0, 0);

        mPreviewBitmap = null;
        mPrintBin = null;

        mContent = "";
    }

    abstract public void fromTlkString(String[] tlkLine);
    abstract protected void fillTlkArray(String[] tlkLine);

    /*
        该函数中调用的fillTlkArray将访问到具体的Object类中的该函数，本类中该函数进定义为一个抽象类
     */
    public String toTlkString() {
        String[] tlkLine = new String[NUMBER_OF_TLK_INDEXS];
        fillTlkArray(tlkLine);          // 此函数到达响应类的相应函数

        StringBuilder sb = new StringBuilder();
        for(int i=0; i<NUMBER_OF_TLK_INDEXS; i++) {
            sb.append(tlkLine[i]).append("^");
        }
        sb.deleteCharAt(sb.length()-1);

        return sb.toString();
    }
/*
    private void initName() {
        if(mId.equals(N_ObjectType.STATIC_TEXT)) {
            mName = mContext.getString(R.string.object_text);
        } else if(mId.equals(N_ObjectType.COUNTER)) {
            mName = mContext.getString(R.string.object_counter);
        } else if(mId.equals(N_ObjectType.RT_YEAR)) {
        } else if(mId.equals(N_ObjectType.RT_MON)) {
        } else if(mId.equals(N_ObjectType.RT_DATE)) {
        } else if(mId.equals(N_ObjectType.RT_HOUR)) {
        } else if(mId.equals(N_ObjectType.RT_MIN)) {
        } else if(mId.equals(N_ObjectType.YEAR)) {
        } else if(mId.equals(N_ObjectType.SHIFT)) {
            mName = mContext.getString(R.string.object_shift);
        } else if(mId.equals(N_ObjectType.DL_YEAR)) {
        } else if(mId.equals(N_ObjectType.DL_MON)) {
        } else if(mId.equals(N_ObjectType.DL_DATE)) {
        } else if(mId.equals(N_ObjectType.JULIAN)) {
            mName = mContext.getString(R.string.object_julian);
        } else if(mId.equals(N_ObjectType.GRAPHIC)) {
            mName = mContext.getString(R.string.object_pic);
        } else if(mId.equals(N_ObjectType.BARCODE)) {
            mName = mContext.getString(R.string.object_bar);
        } else if(mId.equals(N_ObjectType.LINE)) {
            mName = mContext.getString(R.string.object_line);
        } else if(mId.equals(N_ObjectType.RECT)) {
            mName = mContext.getString(R.string.object_rect);
        } else if(mId.equals(N_ObjectType.ELLIPSE)) {
            mName = mContext.getString(R.string.object_ellipse);
        } else if(mId.equals(N_ObjectType.MESSAGE)) {
            mName = mContext.getString(R.string.object_msg_name);
        } else if(mId.equals(N_ObjectType.REAL_TIME)) {
            mName = mContext.getString(R.string.object_realtime);
        } else if(mId.equals(N_ObjectType.QRCODE)) {
        } else if(mId.equals(N_ObjectType.WEEKDAY)) {
            mName = mContext.getString(R.string.object_weekday);
        } else if(mId.equals(N_ObjectType.WEEKS)) {
        } else if(mId.equals(N_ObjectType.RT_SECOND)) {
            mName = mContext.getString(R.string.object_second);
        } else if(mId.equals(N_ObjectType.LETTERHOUR)) {
            mName = mContext.getString(R.string.object_charHour);
        } else if(mId.equals(N_ObjectType.DYNAMIC_TEXT)) {
            mName = mContext.getString(R.string.object_dynamictext);
        } else if(mId.equals(N_ObjectType.WEEKOFYEAR)) {
            mName = mContext.getString(R.string.object_weekofyear);
        } else if(mId.equals(N_ObjectType.HYPERTEXT)) {
            mName = mContext.getString(R.string.object_hypertext);
        } else {
            mName = "Unknown";
        }
        Debug.d(TAG, "--->mName: " + mName);
    }
*/
    abstract public Bitmap getPreview();
    abstract public byte[] getPrintBuffer();

    public boolean isPositionFixed() {
        return mPositionType == POSITION_TYPE_FIXED;			// 固定位置变量，不做考虑
    }

    private Rect getMovedRect() {
        return new Rect(
                Math.min(mLastRect.right, mCurRect.right),
                mCurRect.top,
                Math.max(mLastRect.right, mCurRect.right),
                mCurRect.bottom
        );
    }

    private boolean isMovedForward() {
        return mCurRect.right > mLastRect.right;
    }

    private boolean isMovedBackward() {
        return mCurRect.right < mLastRect.right;
    }

    private boolean intersectsForward(Rect movedRect) {
//			return Rect.intersects(mLastRect, movedRect) && mLastRect.left >= movedRect.left && mLastRect.left < movedRect.right;
        return mLastRect.bottom > movedRect.top && mLastRect.top < movedRect.bottom && mLastRect.left >= movedRect.left && mLastRect.left < movedRect.right;
    }

    private Rect getInterRectForward(Rect movedRect) {
        return new Rect(
                movedRect.left,
                Math.max(movedRect.top, mCurRect.top),
                mLastRect.left,
                Math.min(movedRect.bottom, mCurRect.bottom)
        );
    }

    public boolean isForwardBlock(Rect affectRect) {
//			return Rect.intersects(mLastRect, affectRect) && mLastRect.left >= affectRect.left && mLastRect.right <= affectRect.right;
        return mLastRect.bottom > affectRect.top && mLastRect.top < affectRect.bottom && mLastRect.left >= affectRect.left && mLastRect.right <= affectRect.right;
    }

    private boolean intersectsBackward(Rect movedRect) {
        return mLastRect.bottom > movedRect.top && mLastRect.top < movedRect.bottom && mCurRect.left > movedRect.left;
    }

    private Rect getInterRectBackward(Rect movedRect) {
        return new Rect(
                movedRect.left,
                mCurRect.top,
                mLastRect.left,
                mCurRect.bottom
        );
    }

    public int getBackwardLine(Rect affectRect) {
        return mCurRect.bottom > affectRect.top && mCurRect.top < affectRect.bottom ?
                (mCurRect.right > affectRect.left && mCurRect.right < affectRect.right ?
                 mCurRect.right : affectRect.left) :
                affectRect.left;
    }

    protected void markNeedReCreate() {
        synchronized (this) {
            mPreviewNeedReCreate = true;
            mPrintImageNeedReCreate = true;
            markNeedRedraw();
        }
    }

    protected void markNeedRedraw() {
        synchronized (this) {
            mPreviewNeedRedraw = true;
            mPrintImageNeedPaste = true;
        }
    }

    private boolean moveForwardTo(int targetLine) {
        if(targetLine > mCurRect.left) {
            mLastRect.set(mCurRect);
            mCurRect.offsetTo(targetLine, mCurRect.top);
            markNeedRedraw();
            return true;
        }
        return false;
    }

    private boolean moveBackwardTo(int targetLine) {
        if(targetLine < mCurRect.left) {
            mLastRect.set(mCurRect);
            mCurRect.offsetTo(targetLine, mCurRect.top);
            markNeedRedraw();
            return true;
        }
        return false;
    }

    public boolean adjustPosition(N_BaseObject orgObj) {
        if(this.equals(orgObj)) return false;
        if(isPositionFixed()) return false;

        Rect movedRect = orgObj.getMovedRect();
        if(movedRect.isEmpty()) return false;

        if(orgObj.isMovedForward()) {
            if(intersectsForward(movedRect)) {
                if(!mTask.hasBlockObj(Arrays.asList(orgObj, this), getInterRectForward(movedRect))) {
                    if(moveForwardTo(movedRect.right)) {
                        return true;
                    }
                }
            }
        } else if(orgObj.isMovedBackward()) {
            if(intersectsBackward(movedRect)) {
                if(moveBackwardTo(mTask.getTargetLeft(Arrays.asList(orgObj, this), getInterRectBackward(movedRect)))) {
                    return true;
                }
            }
        }

        return false;
    }

    public void setPosition(int x, int y) {
        mCurRect.offset(x - mCurRect.left, y - mCurRect.top);
    }

    public void setHeight(int height) {
        mCurRect.bottom = Math.min(N_MessageTask.STANDARD_HEIGHT, mCurRect.top + height);
    }

    public Rect getCurrentRect() {
        return mCurRect;
    }

    public Rect getLastRect() {
        return mLastRect;
    }

    public Rect getPreviewRect() {
        return mPreviewRect;
    }

    public Rect getNextPreviewRect() {
        Rect newPreviewRect = new Rect(
            0,
            0,
            mPreviewBitmap.getWidth(),
            mPreviewBitmap.getHeight()
        );

        newPreviewRect.offset(
            (int)Math.ceil(mTask.getPreviewHeightRatio() * mCurRect.left),
            (int)Math.ceil(mTask.getPreviewHeightRatio() * mCurRect.top));

        return newPreviewRect;
    }

    public void finishMove() {
        mLastRect = mCurRect;
    }

    public void setTask(N_MessageTask task) {
        mTask = task;
    }

    public void moveTo(int left) {
        mCurRect.offset(left, 0);
    }

    public N_MessageTask getTask() {
        return mTask;
    }

    public void setIndex(int index) {
        mIndex = index;
    }

    public int getIndex()
    {
        return mIndex;
    }

    public String getName() {
        return mName;
    }
}
