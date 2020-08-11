package com.industry.printer.object;

import java.text.SimpleDateFormat;
import java.util.Date;

import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.text.format.Time;

public class RealtimeSecond extends BaseObject {
    private static final String TAG = RealtimeSecond.class.getSimpleName();

    public RealtimeSecond(Context context, float x) {
        super( context, BaseObject.OBJECT_TYPE_RT_SECOND, x);
        // TODO Auto-generated constructor stub
    }

    public RealtimeSecond(Context context, BaseObject parent, float x) {
        this(context, x);
        mParent = parent;
    }

    @Override
    public String getMeatureString() {
        return "00";
    }

    @Override
    public String getContent()
    {
//        Time t = new Time();
//        t.set(System.currentTimeMillis());
//        setContent(BaseObject.intToFormatString(t.second, 2));
//        Debug.d(TAG, ">>getContent: " + mContent);
//        return mContent;

        SimpleDateFormat dateFormat = new SimpleDateFormat("ss");
        setContent(dateFormat.format(new Date()));
        Debug.d(TAG, ">>getContent: " + mContent);
        return mContent;
    }

    @Override
    public Bitmap getpreviewbmp()
    {
        Debug.d(TAG, "1===== " + getContent() );
        Bitmap bitmap;

        mPaint.setTextSize(getfeed());
        mPaint.setAntiAlias(true); //
        mPaint.setFilterBitmap(true); //

        boolean isCorrect = false;
        // Debug.d(TAG,"--->getBitmap font = " + mFont);
////		for (String font : mFonts) {
////			if (font.equals(mFont)) {
////				isCorrect = true;
////				break;
////			}
////		}
////		if (!isCorrect) {
////			mFont = DEFAULT_FONT;
////		}
        try {
            mPaint.setTypeface(FontCache.get(mContext, mFont));
        } catch (Exception e) {}


        String str_new_content="";
        str_new_content =	mContent;
        str_new_content =	str_new_content.replace('0', 'S');
        str_new_content =	str_new_content.replace('1', 'S');
        str_new_content =	str_new_content.replace('2', 'S');
        str_new_content =	str_new_content.replace('3', 'S');
        str_new_content =	str_new_content.replace('4', 'S');
        str_new_content =	str_new_content.replace('5', 'S');
        str_new_content =	str_new_content.replace('6', 'S');
        str_new_content =	str_new_content.replace('7', 'S');
        str_new_content =	str_new_content.replace('8', 'S');
        str_new_content =	str_new_content.replace('9', 'S');

        int width = (int)mPaint.measureText(str_new_content);//addbylk �����ߴ�
        Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);

        bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
        Debug.d(TAG,"--->getBitmap width="+width+", mHeight="+mHeight);
        mCan = new Canvas(bitmap);
        Paint.FontMetrics fm = mPaint.getFontMetrics();
        mPaint.setColor(Color.BLUE);//

        mCan.drawText(str_new_content , 0, mHeight-fm.descent, mPaint);

        return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);
    }

    public String toString()
    {

        float prop = getProportion();
        StringBuilder builder = new StringBuilder(mId);

        builder.append("^")
                .append(BaseObject.floatToFormatString(getX() * prop, 5))
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
// H.M.Wang 2019-9-24 追加所属信息
                .append("000^000^000^000^000^00000000^00000000^00000000^00000000^" + (mParent == null ? "0000" : String.format("%03d", mParent.mIndex)) + "^0000^")
//				.append("000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^")
                .append(mFont)
                .append("^000^000");
        String str = builder.toString();

//        float prop = getProportion();
//        String str="";
//        //str += BaseObject.intToFormatString(mIndex, 3)+"^";
//        str += mId+"^";
//        str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
//        str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//        str += BaseObject.floatToFormatString(getXEnd()*2 * prop, 5)+"^";
//        //str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
//        str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//        str += BaseObject.intToFormatString(0, 1)+"^";
//        str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//        str += "000^000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^" + mFont + "^000^000";
        Debug.d(TAG, "toString = [" + str + "]");
//        Debug.d(TAG,"file string ["+str+"]");
        return str;
    }
}
