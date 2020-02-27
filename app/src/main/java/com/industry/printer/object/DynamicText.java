package com.industry.printer.object;

import android.content.Context;

import com.industry.printer.Utils.Debug;

public class DynamicText extends BaseObject {
    private static final String TAG = DynamicText.class.getSimpleName();

    public int start;
    public int count;

    public DynamicText(Context ctx, float x) {
        super(ctx, OBJECT_TYPE_DYN_TEXT,x);
        count = 8;
        start = 0;
    }

    /**
     * parse tlk formatted string resulting DynamicText Object
     * @param ctx
     * @param tlk
     * @return
     */
    public static DynamicText fromTlk(Context ctx, String tlk) {
        DynamicText dt = new DynamicText(ctx, 0);

        String[] items = tlk.split("\\^");
        if (items == null || items.length != 22) {
            return null;
        }
        dt.mXcor = Float.parseFloat(items[2]);
        dt.mXcor_end = Float.parseFloat(items[4]);
        dt.mYcor = Float.parseFloat(items[3]);
        dt.mYcor_end = Float.parseFloat(items[5]);
        dt.mHeight = Float.parseFloat(items[6]);
        int dragble = Integer.parseInt(items[7]);
        dt.mDragable = dragble == 0 ? false : true;
        dt.count = Integer.parseInt(items[8]);
        dt.start = Integer.parseInt(items[12]);
        return dt;
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
                .append(BaseObject.floatToFormatString(mHeight, 3))
                .append("^")
                .append(BaseObject.boolToFormatString(mDragable, 3))
                .append("^")
                .append(BaseObject.intToFormatString(count, 3))
                .append("^000^000^000^")
                .append(BaseObject.intToFormatString(start, 3))
                .append("^")
                .append("00000000^00000000^0000^0000^0000^0000^")
                .append(mFont)
                .append("^000^")
                .append(mContent);


        String str = builder.toString();

        Debug.d(TAG, "toString = [" + str + "]");

        return str;
    }
}
