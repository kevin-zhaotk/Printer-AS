package com.industry.printer.object;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

import com.industry.printer.PHeader.N_NozzleManager;
import com.industry.printer.R;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.data.NativeGraphicJni;

/**
 * Created by hmwan on 2021/10/11.
 */

public class N_TextObject extends N_BaseObject {
    private static final String TAG = N_TextObject.class.getSimpleName();

    protected String mFont;                         // 字体

    public N_TextObject(Context ctx, N_MessageTask task) {
        super(ctx, task);
        mId = N_ObjectType.STATIC_TEXT;
        mName = mContext.getString(R.string.object_text);
        mFont = N_NozzleManager.getSystemNozzle().getDefalutFont();
    }

    public N_TextObject(Context ctx, N_MessageTask task, String[] tlkLine) {
        this(ctx, task);
        fromTlkString(tlkLine);
    }

    /*
        按照给定的高度，和颜色，画出给定字符串内容的图片，并且返回该图片。这个是一个工具性的函数，凡是需要讲文字转化为图片的需求，都可以使用该函数
     */
    private Bitmap drawText(int height, String cnt, int color) {
        Bitmap bitmap;

        Paint paint = new Paint();
        paint.setColor(color);

        try {
            paint.setTypeface(FontCache.get(mContext, mFont));
        } catch (Exception e) {
            Debug.d(TAG, "--->e: " + e.getMessage());
        }

        paint.setTextSize(height);
        paint.setAntiAlias(true);       // 去除锯齿
        paint.setFilterBitmap(true);    // 对位图进行滤波处理

        Paint.FontMetrics fontMetrics = paint.getFontMetrics();

        int width = (int)Math.ceil(paint.measureText(cnt));

        bitmap = Bitmap.createBitmap(width , height, Configs.BITMAP_CONFIG);

        Canvas canvas = new Canvas(bitmap);
        canvas.drawText(mContent, 0, height - fontMetrics.descent, paint);

        return bitmap;
    }

    private Bitmap createBitmap(String cnt, int color, int dstWidth, int dstHeight) {
        Bitmap retBitmap = null;

        Bitmap bmp = drawText(dstHeight, cnt, color);
        if(null == bmp) return null;

        switch (mDrawType) {
            case DRAW_TYPE_OVERFLOW:
//                mDrawFrame.set(mCurRect.left, mCurRect.top, mCurRect.left + bmp.getWidth(), mCurRect.bottom);
                retBitmap = bmp;
                break;
            case DRAW_TYPE_CLIP:
//                mDrawFrame.set(mCurRect.left, mCurRect.top, mCurRect.left + Math.min(mCurRect.width(), bmp.getWidth()), mCurRect.bottom);
                retBitmap = Bitmap.createBitmap(bmp, 0, 0, dstWidth, dstHeight);
                break;
            default:
//            case DRAW_TYPE_FIT:
//                mDrawFrame.set(mCurRect);
                retBitmap = Bitmap.createScaledBitmap(bmp, dstWidth, dstHeight, false);
                break;
        }

        if(!bmp.equals(retBitmap)) {
            bmp.recycle();
        }

        return retBitmap;
    }


    protected void createPreviewBitmap(int color) {
        mPreviewBitmap = createBitmap(
            mContent,
            color,
            (int)Math.ceil(mTask.getPreviewHeightRatio() * mCurRect.width()),
            (int)Math.ceil(mTask.getPreviewHeightRatio() * mCurRect.height()));

        mPreviewNeedReCreate = false;
        mPreviewNeedRedraw = true;
    }

    @Override
    public Bitmap getPreview() {
        if(mPreviewNeedReCreate) {
            createPreviewBitmap(Color.BLACK);
        }
        return mPreviewBitmap;
    }

    @Override
    public byte[] getPrintBuffer() {
        Bitmap printBitmap = createBitmap(
                mContent,
                Color.BLACK,
                (int)Math.ceil(mTask.getPrintHeightRatio() * mCurRect.width()),
                (int)Math.ceil(mTask.getPrintHeightRatio() * mCurRect.height()));

        mPrintImageNeedReCreate = false;
        mPrintImageNeedPaste = true;

        int columns = printBitmap.getWidth();
        int printTop = (int)Math.ceil(mTask.getPrintHeightRatio() * mCurRect.top);
        int topDiff = (int)Math.ceil(mTask.getPrintHeightRatio() * mCurRect.top) % 8;        // 取原printTop与小于他的最大的8的倍数之间的差
        int[] pixels = new int[printBitmap.getHeight() * printBitmap.getWidth()];
        printBitmap.getPixels(pixels, 0, printBitmap.getWidth(), 0, 0, printBitmap.getWidth(), printBitmap.getHeight());
        // pixels = Nozzle.ShiftImage(printTop, printBitmap.getWidth(), pixels);
        Rect printBinRect = new Rect(
                0,
                0,
                printBitmap.getWidth(),
                pixels.length / printBitmap.getWidth());
        printBinRect.offset(
                (int)Math.ceil(mTask.getPrintHeightRatio() * mCurRect.left),
                printTop - topDiff);
        byte[] printBin = NativeGraphicJni.Binarize(pixels, printBitmap.getWidth(), pixels.length / printBitmap.getHeight(), 1, 240);

        return mPrintBin;
    }

    private int measureTextWidth(String cnt, String font, int height) {
        Paint paint = new Paint();

        try {
            paint.setTypeface(FontCache.get(mContext, font));
        } catch (Exception e) {
            Debug.d(TAG, "--->e: " + e.getMessage());
        }

        paint.setTextSize(height);

        return (int)Math.ceil(paint.measureText(cnt));
    }

    private void adjustWidth() {
        int width = measureTextWidth(mContent, mFont, mCurRect.height());

        if((mWidthType == WIDTH_TYPE_CONTENT && width != mCurRect.width()) ||
           (mWidthType == WIDTH_TYPE_FIXED && 0 == mCurRect.width()))  {
//            mLastFrame.set(mCurRect);
            mCurRect.right = mCurRect.left + width;

            if(null != mTask) {
                mTask.onObjectWidthChanged(this);
            }
        }
    }

    public void setContent(String cnt) {
        if(mContent.equals(cnt)) return;

        mContent = cnt;
        markNeedReCreate();

        adjustWidth();
    }

    public String getContent() {
        return mContent;
    }

    public void setFont(String font) {
        if(mFont.equals(font)) return;

        mFont = font;
        markNeedReCreate();

        adjustWidth();
    }

    public String getFont() {
        return mFont;
    }

    protected final int TLK_INDEX_CONTENT_LEN                   = 8;                                   // 8. 内容长度
    protected final int TLK_INDEX_FONT                          = 19;                                  // 19. 字体
    protected final int TLK_INDEX_CONTENT                       = 21;                                  // 21. 内容

    @Override
    public void fromTlkString(String[] items) {
        if(items.length >= NUMBER_OF_TLK_INDEXS) {
            try {
                mIndex = Integer.parseInt(items[TLK_INDEX_OBJINDEX]);
                mId = items[TLK_INDEX_ID];
                mDragable = items[TLK_INDEX_DRAGABLE].equals("1") ? true : false;
                mReverse = items[TLK_INDEX_REVERSE].equals("1") ? true : false;
                mParent = Integer.parseInt(items[TLK_INDEX_PARENT]) > 0 ? mTask.getParentObject(Integer.parseInt(items[TLK_INDEX_PARENT])) : null;
                mCurRect.set(
                    Integer.parseInt(items[TLK_INDEX_FRAME_LEFT]) / 2,
                    Integer.parseInt(items[TLK_INDEX_FRAME_TOP]) / 2,
                    Integer.parseInt(items[TLK_INDEX_FRAME_RIGHT]) / 2,
                    Integer.parseInt(items[TLK_INDEX_FRAME_BOTTOM]) / 2
                );
                mFont = items[TLK_INDEX_FONT];
                mContent = items[TLK_INDEX_CONTENT];
            } catch(NumberFormatException e) {
                Debug.e(TAG, e.getMessage());
            }
        }
    }

    @Override
    protected void fillTlkArray(String[] items) {
        items[TLK_INDEX_OBJINDEX] = String.valueOf(mIndex);                                           // 0. 序号
        items[TLK_INDEX_ID] = mId;                                                                    // 1. 种类
        items[TLK_INDEX_FRAME_LEFT] = String.format("%05d", mCurRect.left * 2);                       // 2. Left坐标（乘2）
        items[TLK_INDEX_FRAME_TOP] = String.format("%05d", mCurRect.top * 2);                         // 3. Top坐标（乘2）
        items[TLK_INDEX_FRAME_RIGHT] = String.format("%05d", mCurRect.right * 2);                     // 4. Right坐标（乘2）
        items[TLK_INDEX_FRAME_BOTTOM] = String.format("%05d", mCurRect.bottom * 2);                   // 5. Bottom坐标（乘2）
        items[TLK_INDEX_6] = "0";                                                                     // 6. 不明
        items[TLK_INDEX_DRAGABLE] = String.format("%03d", mDragable ? "1" : "0");                     // 7. 是否可以拖拽
        items[TLK_INDEX_CONTENT_LEN] = String.format("%03d", mContent.length());                      // 8. 内容长度
        items[TLK_INDEX_9] = "000";                                                                   // 9. 不明
        items[TLK_INDEX_10] = "000";                                                                  // 10. 不明
        items[TLK_INDEX_REVERSE] = String.format("%03d", mReverse ? "1" : "0");                       // 11. Reverse
        items[TLK_INDEX_12] = "000";                                                                  // 12. 不明
        items[TLK_INDEX_13] = "00000000";                                                             // 13. 不明
        items[TLK_INDEX_14] = "00000000";                                                             // 14. 不明
        items[TLK_INDEX_15] = "00000000";                                                             // 15. 不明
        items[TLK_INDEX_16] = "00000000";                                                             // 16. 不明
        items[TLK_INDEX_PARENT] = String.valueOf(mParent.getIndex());                                 // 17. 父Object
        items[TLK_INDEX_18] = "0000";                                                                 // 18. 不明
        items[TLK_INDEX_FONT] = mFont;                                                                // 19. 字体
        items[TLK_INDEX_20] = "000";                                                                  // 20. 不明
        items[TLK_INDEX_CONTENT] = mContent;                                                          // 21. 内容

/*
        sb.append(mIndex).append("^")                                                                   // 0. 序号
          .append(mId).append("^")                                                                      // 1. 种类
          .append(String.format("%05d", mCurRect.left * 2)).append("^")                                 // 2. Left坐标（乘2）
          .append(String.format("%05d", mCurRect.top * 2)).append("^")                                  // 3. Top坐标（乘2）
          .append(String.format("%05d", mCurRect.right * 2)).append("^")                                // 4. Right坐标（乘2）
          .append(String.format("%05d", mCurRect.bottom * 2)).append("^")                               // 5. Bottom坐标（乘2）
          .append("0").append("^")                                                                      // 6. 不明
          .append(String.format("%03d", mDragable ? "1" : "0")).append("^")                             // 7. 是否可以拖拽
          .append(String.format("%03d", mContent.length())).append("^")                                 // 8. 内容长度
          .append("000").append("^")                                                                    // 9. 不明
          .append("000").append("^")                                                                    // 10. 不明
          .append("000").append("^")                                                                    // 11. Reverse
          .append("000").append("^")                                                                    // 12. 不明
          .append("00000000").append("^")                                                               // 13. 不明
          .append("00000000").append("^")                                                               // 14. 不明
          .append("00000000").append("^")                                                               // 15. 不明
          .append("00000000").append("^")                                                               // 16. 不明
          .append((mParent == null ? "0000" : String.format("%03d", mParent.mIndex))).append("^")       // 17. 父Object
          .append("0000").append("^")                                                                   // 18. 不明
          .append(mFont).append("^")                                                                    // 19. 字体
          .append("000").append("^")                                                                    // 20. 不明
          .append(mContent).append("^");                                                                // 21. 内容
*/
    }
}
