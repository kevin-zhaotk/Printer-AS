package com.industry.printer.object;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.R;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.hardware.RTCDevice;

public class DynamicText extends BaseObject {
    private static final String TAG = DynamicText.class.getSimpleName();

    private int mBits;
    private int mDtIndex;

    public DynamicText(Context ctx, float x) {
        super(ctx, OBJECT_TYPE_DYN_TEXT, x);
        mBits = 0;
        mContent = "";
    }

    public DynamicText(Context context, BaseObject parent, float x) {
        this(context, x);
        mParent = parent;
    }

    @Override
    public String getMeatureString() {
        StringBuilder sb = new StringBuilder();
        for(int i=0; i<mBits; i++) {
            sb.append('0');
        }
        return sb.toString();
    }

    @Override
    public void setContent(String cnt) {
        StringBuilder sb = new StringBuilder();
        sb.append(cnt);
        for(int i=0; i<mBits-cnt.length(); i++) {
            sb.append(' ');
        }
        super.setContent(sb.toString().substring(0, mBits));
        Debug.d(TAG, "setContent: [" + cnt + "] -> [" + mContent + "]");
        setWidth(mPaint.measureText(getContent()));
    }

    @Override
    public String getContent() {
        return mContent;
    }

    public void setBits(int n) {
        Debug.d(TAG, "setBits: [" + n + "]");
        mBits = n;
        setContent(mContent);
    }

    public int getBits()
    {
        return mBits;
    }

    public void setDtIndex(int n) {
        Debug.d(TAG, "setDtIndex: [" + n + "]");
        mDtIndex = n;
    }

    public int getDtIndex()
    {
        return mDtIndex;
    }

    @Override
    public Bitmap getpreviewbmp()
    {
        Bitmap bitmap;
        mPaint.setTextSize(getfeed());
        mPaint.setAntiAlias(true); //
        mPaint.setFilterBitmap(true); //

        try {
            mPaint.setTypeface(FontCache.get(mContext, mFont));
        } catch (Exception e) {}

        int width = (int)mPaint.measureText(getContent());
        Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
        if (mWidth == 0) {
            setWidth(width);
        }
        bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
        Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
        mCan = new Canvas(bitmap);
        Paint.FontMetrics fm = mPaint.getFontMetrics();
        mPaint.setColor(Color.BLUE);

        StringBuilder sb = new StringBuilder();
        for(int i=0; i<mBits; i++) {
            sb.append('d');
        }

        mCan.drawText(sb.toString(), 0, mHeight-fm.descent, mPaint);

        Bitmap result = Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);
        BinFromBitmap.recyleBitmap(bitmap);
        return result;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder(mId);
        float prop = getProportion();
        builder.append("^")
                .append(BaseObject.floatToFormatString(getX()*prop, 5))
                .append("^")
                .append(BaseObject.floatToFormatString(getY()*2 * prop, 5))
                .append("^")
                .append(BaseObject.floatToFormatString(getXEnd() * prop, 5))
                .append("^")
                .append(BaseObject.floatToFormatString(getYEnd()*2 * prop, 5))
                .append("^")
                .append(BaseObject.intToFormatString(0, 1))
                .append("^")
                .append(BaseObject.boolToFormatString(mDragable, 3))
                .append("^")
                .append(BaseObject.intToFormatString(mBits, 3))
                .append("^000^000^000^000")
                .append("^")
                .append("00000000^00000000^0000^0000")
                .append("^")
                .append((mParent == null ? "0000" : String.format("%03d", mParent.mIndex)) + "^0000^")
                .append(mFont)
                .append("^000^")
                .append(mContent);

        String str = builder.toString();

        Debug.d(TAG, "toString = [" + str + "]");

        return str;
    }
}
