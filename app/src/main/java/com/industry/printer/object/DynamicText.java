package com.industry.printer.object;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.BinFromBitmap;

public class DynamicText extends BaseObject {
    private static final String TAG = DynamicText.class.getSimpleName();

    private int mBits;
    private int mDtIndex;
    private String mOriginalContent;

    public DynamicText(Context ctx, float x) {
        super(ctx, OBJECT_TYPE_DYN_TEXT, x);
        mBits = 5;
        mDtIndex = 0;
        mContent = "#####";
        mOriginalContent = "#####";
    }

    public DynamicText(Context context, BaseObject parent, float x) {
        this(context, x);
        mParent = parent;
    }

    @Override
    public String getMeatureString() {
        StringBuilder sb = new StringBuilder();
        for(int i=0; i<mBits; i++) {
            sb.append('#');
        }
        return sb.toString();
    }

    @Override
    public void setContent(String cnt) {
        mOriginalContent = cnt;
        StringBuilder sb = new StringBuilder();
        sb.append(cnt);
        for(int i=0; i<mBits-cnt.length(); i++) {
            sb.append(' ');
        }
        super.setContent(sb.toString().substring(0, mBits));
        Debug.d(TAG, "setContent: [" + cnt + "] -> [" + mContent + "]");
//        在measureText之前，必须setTypeface和setTextSize，否则可能不准确，由于原来代码当中，在draw函数里面重新测量宽度，这里如果设置宽度，由于如果上述两个设置内容有变，或者设置不全，会带来不良结果。因此这里取消设置，
//        mPaint.setTextSize(getfeed());
//        setWidth(mPaint.measureText(getMeatureString()));
    }

    @Override
    public String getContent() {
        return mContent;
    }

    public void setBits(int n) {
        Debug.d(TAG, "setBits: [" + n + "]");
        mBits = n;
        this.setContent(mOriginalContent);
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

        int width = (int)mPaint.measureText(getMeatureString());
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
            sb.append('#');
        }

        mCan.drawText(sb.toString(), 0, mHeight-fm.descent, mPaint);

        Bitmap result = Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);
        BinFromBitmap.recyleBitmap(bitmap);
        return result;
    }

    @Override
    public void generateVarbinFromMatrix(String f) {
        BinFileMaker maker = new BinFileMaker(mContext);
        maker.extract("0123456789ABCDEFGHIJKLMNOPQRSTUV !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~~");
        maker.save(f + getVarBinFileName());
    }

    /**
     * 根據content生成變量的bin
     * 1、32点信息的bin统一不支持缩放
     * 2、7点和16点锁定字库
     * @param ctx 內容
     * @param scaleW	單個字符的實際寬度
     * @param scaleH	字符實際高度
     * @param dstH  背景圖高度
     * @return
     */
    @Override
    public int makeVarBin(Context ctx, float scaleW, float scaleH, int dstH) {
        int dots[] = new int[1];
        int singleW;
        Paint paint = new Paint();
        int height = Math.round(mHeight * scaleH);
        paint.setTextSize(height);
        paint.setAntiAlias(true); //去除锯齿
        paint.setFilterBitmap(true); //对位图进行滤波处理

        try {
            paint.setTypeface(FontCache.get(ctx, mFont));
        } catch (Exception e) {

        }

//		Debug.d(TAG, "SaveTime: - Start makeVarBin : " + System.currentTimeMillis());
        int width = Math.round(paint.measureText("0"));
        Paint.FontMetrics fm = paint.getFontMetrics();

		/*draw Bitmap of single digit*/
        Bitmap bmp = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
        Canvas can = new Canvas(bmp);

        PrinterNozzle head = mTask.getNozzle();

        // H.M.Wang 修改下列两行
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//        if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
        if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
            head == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
            head == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
            head == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
            head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头

            singleW = width;
        } else {
            singleW = Math.round(mWidth * scaleW/mContent.length());
        }

		/*draw 0-9 totally 10 digits Bitmap*/

        /** if message isn`t high resolution, divid by 2 because the buffer bitmap is halfed, so the variable buffer should be half too*/
        MessageObject msgObj = mTask.getMsgObject();
        if (!msgObj.getResolution() ) {
            singleW = singleW / msgObj.getPNozzle().getFactorScale();
        }

        Debug.d(TAG, "--->singleW=" + singleW);

		/* 最終生成v.bin使用的bitmap */
        Bitmap gBmp = Bitmap.createBitmap(singleW*128, dstH, Configs.BITMAP_CONFIG);
        Canvas gCan = new Canvas(gBmp);

        gCan.drawColor(Color.WHITE);	/*white background*/
        StringBuilder sb = new StringBuilder();
        for(int i=0; i<128; i++)
        {
            char b;
            can.drawColor(Color.WHITE);
            if(i>=0 && i<=9) {
                b = (char)(0x30+i);
            } else if(i>=10 && i<=0x1F) {
                b = (char)('A'+i-10);
            } else if(i == 0x7F) {
                b = 0x7E;
            } else {
                b = (char)i;
            }
            sb.append(b);
            can.drawText(String.valueOf(b), 0, height - fm.descent, paint);
            gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, height, false), i*singleW, Math.round(getY() * scaleH), paint);
        }
        Debug.d(TAG, sb.toString());
        BinFromBitmap.recyleBitmap(bmp);

        BinFileMaker maker = new BinFileMaker(mContext);

        // H.M.Wang 追加一个是否移位的参数。修改喷头数
        dots = maker.extract(Bitmap.createScaledBitmap(gBmp, gBmp.getWidth(), dstH, false), head.mHeads,
                (mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH ||
                        mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL ||
                        mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE ||
                        mTask.getNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR));

        Debug.d(TAG, "--->id: " + mId + " index:  " + mIndex);
        maker.save(ConfigPath.getVBinAbsolute(mTask.getName(), mIndex));
        //
        BinFromBitmap.recyleBitmap(gBmp);
		/*根據變量內容的實際長度計算點數*/
        dots[0] = (dots[0]* getContent().length()/10) + 1;

//		Debug.d(TAG, "SaveTime: - End makeVarBin : " + System.currentTimeMillis());
        return dots[0];
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
                .append("00000000^00000000^00000000")
                .append("^")
                .append(String.format("%03d", mDtIndex))
                .append("^0000^0000^")
                .append(mFont)
                .append("^000^")
                .append(mContent);

        String str = builder.toString();

        Debug.d(TAG, "toString = [" + str + "]");

        return str;
    }
}
