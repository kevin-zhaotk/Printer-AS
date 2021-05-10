package com.industry.printer.object;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

import com.industry.printer.MessageTask;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;

import java.util.HashMap;
import java.util.Vector;

/**
 * Created by hmwan on 2020/2/12.
 */

public class HyperTextObject extends BaseObject {
    private static final String TAG = HyperTextObject.class.getSimpleName();

    private static final String HYPER_TAG               = "@";             // 超文本标识符

    private static final String HYPER_TEXT_YEAR         = "@Y";            // 两位年
    private static final String HYPER_TEXT_MONTH        = "@M";            // 两位月
    private static final String HYPER_TEXT_DATE         = "@D";            // 两位日
    private static final String HYPER_TEXT_HOUR         = "@H";            // 两位时
    private static final String HYPER_TEXT_MINUTE       = "@N";            // 两位分
    private static final String HYPER_TEXT_SECOND       = "@S";            // 两位秒
    private static final String HYPER_TEXT_SHIFT        = "@T";            // 两位班次
    private static final String HYPER_TEXT_WEEKDAY      = "@A";            // 星期几
    private static final String HYPER_TEXT_WEEK_OF_YEAR = "@W";            // 年星期数
    private static final String HYPER_TEXT_2_COUNTER    = "@2";            // 两位计数器
    private static final String HYPER_TEXT_3_COUNTER    = "@3C";           // 三位计数器
    private static final String HYPER_TEXT_4_COUNTER    = "@4CC";          // 4位计数器
    private static final String HYPER_TEXT_5_COUNTER    = "@5CCC";         // 5位计数器

    public Vector<BaseObject> mSubObjs = null;
    public HashMap<BaseObject, String> mSubObjsTag = null;

    private static final int SHIFT_MAX = 4;

    public int mCounterStart;
    public int mCounterEnd;
    public int mCounterIndex;
    public int mOffset;
    public int mShiftTimes[];
    public String mShiftValues[];

    public HyperTextObject(Context context, float x) {
        super(context, BaseObject.OBJECT_TYPE_HYPERTEXT, x);
        mSubObjs = new Vector<BaseObject>();
        mSubObjsTag = new HashMap<BaseObject, String>();
        mCounterStart = 0;
        mCounterEnd = 99999;
        mCounterIndex = 0;
        mShiftTimes = new int[SHIFT_MAX];
        mShiftTimes[0] = 0000;
        mShiftTimes[1] = 600;
        mShiftTimes[2] = 1200;
        mShiftTimes[3] = 1800;
        mShiftValues = new String[SHIFT_MAX];
        mShiftValues[0] = " 1";
        mShiftValues[1] = " 2";
        mShiftValues[2] = " 3";
        mShiftValues[3] = " 4";
        mOffset = 0;
    }

    public HyperTextObject(Context context) {
        this(context, 0);
    }

    @Override
    public void setContent(String cnt) {
        Debug.d(TAG, "setContent: [" + cnt + "]");
        super.setContent(cnt);
        parseContent(cnt);
    }

    @Override
    public void setTask(MessageTask task) {
        mTask = task;

//        if(null == mSubObjs) return;

        for (BaseObject obj : mSubObjs) {
            obj.setTask(task);
        }
    }

    @Override
    public String getContent()
    {
        return super.getContent();
    }

// H.M.Wang 2020-7-31 追加超文本内容，条码的内容可能是超文本
    public String getExpandedContent() {
        StringBuilder sb = new StringBuilder();
        sb.append("");
        for (BaseObject obj : mSubObjs) {
            sb.append(obj.getContent());
        }
        return sb.toString();
    }
// End of H.M.Wang 2020-7-31 追加超文本内容，条码的内容可能是超文本

    public Vector<BaseObject> getSubObjs()
    {
        return mSubObjs;
    }

    public void parseContent(String cnt) {
        Debug.d(TAG, "parseContent: [" + cnt + "]");

        if(null == cnt) return;

        mSubObjs.clear();
        mSubObjsTag.clear();

        int procStart = 0, searchStart = 0, foundPos = 0;
        HashMap<String, String> addedMap = new HashMap<String, String>();

        String testStr = new String(cnt);
        testStr = testStr.toUpperCase();

        do {
            foundPos = testStr.indexOf(HYPER_TAG, searchStart);
            if(foundPos >= 0) {
                String tag = "";
                BaseObject obj = null;

                if(testStr.startsWith(HYPER_TEXT_5_COUNTER, foundPos)) {
                    tag = HYPER_TEXT_5_COUNTER;
                    if(!addedMap.containsKey(CounterObject.class.getSimpleName())) {
                        addedMap.put(CounterObject.class.getSimpleName(), "");
                        obj = new CounterObject(mContext, this, 0);
                        ((CounterObject)obj).setBits(5);
//                        ((CounterObject)obj).setStart(mCounterStart);
//                        ((CounterObject)obj).setEnd(mCounterEnd);
                        ((CounterObject)obj).setRange(mCounterStart, mCounterEnd);;
                        ((CounterObject)obj).setCounterIndex(mCounterIndex);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_4_COUNTER, foundPos)) {
                    tag = HYPER_TEXT_4_COUNTER;
                    if(!addedMap.containsKey(CounterObject.class.getSimpleName())) {
                        addedMap.put(CounterObject.class.getSimpleName(), "");
                        obj = new CounterObject(mContext, this, 0);
                        ((CounterObject)obj).setBits(4);
//                        ((CounterObject)obj).setStart(mCounterStart);
//                        ((CounterObject)obj).setEnd(mCounterEnd);
                        ((CounterObject)obj).setRange(mCounterStart, mCounterEnd);;
                        ((CounterObject)obj).setCounterIndex(mCounterIndex);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_3_COUNTER, foundPos)) {
                    tag = HYPER_TEXT_3_COUNTER;
                    if(!addedMap.containsKey(CounterObject.class.getSimpleName())) {
                        addedMap.put(CounterObject.class.getSimpleName(), "");
                        obj = new CounterObject(mContext, this, 0);
                        ((CounterObject)obj).setBits(3);
//                        ((CounterObject)obj).setStart(mCounterStart);
//                        ((CounterObject)obj).setEnd(mCounterEnd);
                        ((CounterObject)obj).setRange(mCounterStart, mCounterEnd);;
                        ((CounterObject)obj).setCounterIndex(mCounterIndex);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_2_COUNTER, foundPos)) {
                    tag = HYPER_TEXT_2_COUNTER;
                    if(!addedMap.containsKey(CounterObject.class.getSimpleName())) {
                        addedMap.put(CounterObject.class.getSimpleName(), "");
                        obj = new CounterObject(mContext, this, 0);
                        ((CounterObject)obj).setBits(2);
//                        ((CounterObject)obj).setStart(mCounterStart);
//                        ((CounterObject)obj).setEnd(mCounterEnd);
                        ((CounterObject)obj).setRange(mCounterStart, mCounterEnd);;
                        ((CounterObject)obj).setCounterIndex(mCounterIndex);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_WEEK_OF_YEAR, foundPos)) {
                    tag = HYPER_TEXT_WEEK_OF_YEAR;
                    if(!addedMap.containsKey(WeekOfYearObject.class.getSimpleName())) {
                        addedMap.put(WeekOfYearObject.class.getSimpleName(), "");
                        obj = new WeekOfYearObject(mContext, this, 0);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_WEEKDAY, foundPos)) {
                    tag = HYPER_TEXT_WEEKDAY;
                    if(!addedMap.containsKey(WeekDayObject.class.getSimpleName())) {
                        addedMap.put(WeekDayObject.class.getSimpleName(), "");
                        obj = new WeekDayObject(mContext, this, 0);
                        ((WeekDayObject)obj).setBits(2);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_SHIFT, foundPos)) {
                    tag = HYPER_TEXT_SHIFT;
                    if(!addedMap.containsKey(ShiftObject.class.getSimpleName())) {
                        addedMap.put(ShiftObject.class.getSimpleName(), "");
                        obj = new ShiftObject(mContext, this, 0);
                        ((ShiftObject)obj).setBits(2);
                        ((ShiftObject)obj).setShift(0, String.valueOf(mShiftTimes[0]));
                        ((ShiftObject)obj).setShift(1, String.valueOf(mShiftTimes[1]));
                        ((ShiftObject)obj).setShift(2, String.valueOf(mShiftTimes[2]));
                        ((ShiftObject)obj).setShift(3, String.valueOf(mShiftTimes[3]));
                        ((ShiftObject)obj).setValue(0, mShiftValues[0]);
                        ((ShiftObject)obj).setValue(1, mShiftValues[1]);
                        ((ShiftObject)obj).setValue(2, mShiftValues[2]);
                        ((ShiftObject)obj).setValue(3, mShiftValues[3]);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_SECOND, foundPos)) {
                    tag = HYPER_TEXT_SECOND;
                    if(!addedMap.containsKey(RealtimeSecond.class.getSimpleName())) {
                        addedMap.put(RealtimeSecond.class.getSimpleName(), "");
                        obj = new RealtimeSecond(mContext, this, 0);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_MINUTE, foundPos)) {
                    tag = HYPER_TEXT_MINUTE;
                    if(!addedMap.containsKey(RealtimeMinute.class.getSimpleName())) {
                        addedMap.put(RealtimeMinute.class.getSimpleName(), "");
                        obj = new RealtimeMinute(mContext, this, 0);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_HOUR, foundPos)) {
                    tag = HYPER_TEXT_HOUR;
                    if(!addedMap.containsKey(RealtimeHour.class.getSimpleName())) {
                        addedMap.put(RealtimeHour.class.getSimpleName(), "");
                        obj = new RealtimeHour(mContext, this, 0);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_DATE, foundPos)) {
                    tag = HYPER_TEXT_DATE;
                    if(!addedMap.containsKey(RealtimeDate.class.getSimpleName())) {
                        addedMap.put(RealtimeDate.class.getSimpleName(), "");
                        obj = new RealtimeDate(mContext, this, 0);
                        obj.setOffset(mOffset);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_MONTH, foundPos)) {
                    tag = HYPER_TEXT_MONTH;
                    if(!addedMap.containsKey(RealtimeMonth.class.getSimpleName())) {
                        addedMap.put(RealtimeMonth.class.getSimpleName(), "");
                        obj = new RealtimeMonth(mContext, this, 0);
                        obj.setOffset(mOffset);
                    }
                } else if(testStr.startsWith(HYPER_TEXT_YEAR, foundPos)) {
                    tag = HYPER_TEXT_YEAR;
                    if(!addedMap.containsKey(RealtimeYear.class.getSimpleName())) {
                        addedMap.put(RealtimeYear.class.getSimpleName(), "");
                        obj = new RealtimeYear(mContext, this, 0, false);
                        obj.setOffset(mOffset);
                    }
                } else {
                    searchStart++;
                    continue;
                }

                searchStart = foundPos + tag.length();
                if(null != obj) {
                    if(procStart < foundPos) {
//                        Debug.d(TAG, "Text: [" + cnt.substring(procStart, foundPos) + "]");
                        TextObject tObj = new TextObject(mContext, this, 0);
                        tObj.setContent(cnt.substring(procStart, foundPos));
                        mSubObjs.add(tObj);
                    }

                    mSubObjsTag.put(obj, tag);
                    mSubObjs.add(obj);
//                    Debug.d(TAG, "Variable: [" + cnt.substring(foundPos, searchStart) + "]");
                    procStart = searchStart;
                }
            } else {
                if(procStart < cnt.length()) {
//                    Debug.d(TAG, "Text: [" + cnt.substring(procStart) + "]");
                    TextObject tObj = new TextObject(mContext, this, 0);
                    tObj.setContent(cnt.substring(procStart));
                    mSubObjs.add(tObj);
                }
                break;
            }
        } while(procStart < cnt.length());

        setTask(mTask); // 给子元素设置Task，这个关系到FONT的选取，同时关系到宽度的计算
        meature();      // 计算每个元素的宽度
    }

    @Override
    public void setFont(String font) {
        super.setFont(font);

        if(mSubObjs == null) return;
        for(BaseObject o : mSubObjs) {
            o.setFont(font);
        }

        isNeedRedraw = true;
        meature();
    }

    public void setIndex(int index)
    {
        mIndex = index++;
        for (BaseObject object : mSubObjs) {
            object.setIndex(index++);
        }
    }

    @Override
    protected void meature() {
        float width = 0f;

        for (BaseObject o : mSubObjs) {
            o.meature();
            o.setX(width);
            o.setWidth(o.getWidth() * mRatio);
//            o.setWidth(o.getWidth());
            width += o.getWidth();
        }
        super.setWidth(width);
        Debug.d(TAG, "Width: " + super.getWidth());
    };

    @Override
    public void setX(float x) {
        float xx = x;

        super.setX(xx);

        if(null == mSubObjs) return;

        for(BaseObject o : mSubObjs) {
            o.setX(xx);
            xx += o.getWidth();
        }
    }

    @Override
    public void setY(float y) {
        super.setY(y);

//        if(null == mSubObjs) return;

        for(BaseObject o : mSubObjs) {
            o.setY(y);
        }
    }

    @Override
    public void setHeight(float size) {
        if(mHeight == size) return;

        super.setHeight(size);
        size = getHeight();

        Debug.d(TAG, "--->setHeight: " + size);

        for(BaseObject o : mSubObjs) {
            o.setHeight(size);
        }

        isNeedRedraw = true;
//        meature();
    }

//    @Override
//    public void setHeight(String size) {
//        float height = 1.0f * mTask.getMsgObject().getPixels(size);
//        this.setHeight(height);
//    }

// H.M.Wang 2020-9-3 在BaseObject类中的draw函数会重新计算变量的宽度。如果没有这个函数，会以当前内容作为参考文本，如果是RealtimeObject。原来是按着"YY-MM-DD"计算的宽度，后来就按"20-09-03"计算了，宽度发生变化，结果会发生内容覆盖
//	追加此函数以避免
    @Override
    public String getMeatureString() {
        StringBuilder sb = new StringBuilder();
        for (BaseObject o : mSubObjs) {
            sb.append(o.getMeatureString());
        }
        return sb.toString();
    }
// End of H.M.Wang 2020-9-3 在BaseObject类中的draw函数会重新计算变量的宽度。如果没有这个函数，会以当前内容作为参考文本，如果是RealtimeObject。原来是按着"YY-MM-DD"计算的宽度，后来就按"20-09-03"计算了，宽度发生变化，结果会发生内容覆盖
//	追加此函数以避免

    // H.M.Wang 2019-9-25 该类对象重新根据高度重新设置宽度的时候，主要根据子项目的内容来设置，跟自己的内容无关
    @Override
    public void resizeByHeight() {
        float x = super.getX();

//        if(mSubObjs == null) return;

        for(BaseObject o : mSubObjs) {
            o.setX(x);
            o.resizeByHeight();
            x = o.getXEnd();
        }
        setWidth(x - getX());
    }

    public void wide() {
        float x = getX();
        float ratio = (getWidth() + 5) / getWidth();
        mRatio *= ratio;

//        if(mSubObjs == null) return;

        for(BaseObject o : mSubObjs) {
            o.setX(x);
            o.setWidth(o.getWidth() * ratio);
            x = o.getXEnd();
        }
        setWidth(x - getX());
        isNeedRedraw = true;
    }

    public void narrow() {
        float x = getX();
        float ratio = (getWidth() - 5) / getWidth();
        mRatio *= ratio;

//        if(mSubObjs == null) return;

        for(BaseObject o : mSubObjs) {
            o.setX(x);
            o.setWidth(o.getWidth() * ratio);
            x = o.getXEnd();
        }
        setWidth(x - getX());
        isNeedRedraw = true;
    }

    public void setCounterIndex(String str) {
        try {
            mCounterIndex = Integer.valueOf(str);
        } catch(NumberFormatException e){}

        for(BaseObject o : mSubObjs) {
            if(o instanceof CounterObject) {
                ((CounterObject) o).setCounterIndex(mCounterIndex);
            }
        }
    }

    public int getCounterIndex() {
        return mCounterIndex;
    }

    public void setCounterStart(String str) {
        try {
            mCounterStart = Integer.valueOf(str);
        } catch(NumberFormatException e){}

        for(BaseObject o : mSubObjs) {
            if(o instanceof CounterObject) {
                ((CounterObject) o).setRange(mCounterStart, mCounterEnd);
            }
        }
    }

    public int getCounterStart() {
        return mCounterStart;
    }

    public void setCounterEnd(String str) {
        try {
            mCounterEnd = Integer.valueOf(str);
        } catch(NumberFormatException e){}

        for(BaseObject o : mSubObjs) {
            if(o instanceof CounterObject) {
                ((CounterObject) o).setRange(mCounterStart, mCounterEnd);
            }
        }
    }

    public int getCounterEnd() {
        return mCounterEnd;
    }

// H.M.Wang 2020-7-31 追加超文本的计数器打印后调整
    public void goNext() {
        for(BaseObject o : mSubObjs) {
            if (o instanceof CounterObject) {
                ((CounterObject) o).goNext();
            }
        }
    }
// End of H.M.Wang 2020-7-31 追加超文本的计数器打印后调整

// H.M.Wang 2021-5-7 追加实际打印数调整函数
    public void goPrintedNext() {
        for(BaseObject o : mSubObjs) {
            if (o instanceof CounterObject) {
                ((CounterObject) o).goPrintedNext();
            }
        }
    }
// End of H.M.Wang 2021-5-7 追加实际打印数调整函数

    public void setShiftTime(int shift, String time) {
        for(BaseObject o : mSubObjs) {
            if(o instanceof ShiftObject) {
                ((ShiftObject) o).setShift(shift, time);
                mShiftTimes[shift] = ((ShiftObject) o).getShift(shift);
            }
        }
    }

    public int getShiftTime(int shift) {
        if(shift < 0 || shift > SHIFT_MAX) return 0;
        return mShiftTimes[shift];
    }

    public void setShiftValue(int shift, String value) {
        for(BaseObject o : mSubObjs) {
            if(o instanceof ShiftObject) {
                ((ShiftObject) o).setValue(shift, value);
                mShiftValues[shift] = ((ShiftObject) o).getValue(shift);
            }
        }
    }

    public String getShiftValue(int shift) {
        if(shift < 0 || shift > SHIFT_MAX) return "";
        return mShiftValues[shift];
    }

    public void setDateOffset(String str) {
        try {
            mOffset = Integer.valueOf(str);
        } catch(NumberFormatException e){}

        for(BaseObject o : mSubObjs) {
            if(o instanceof RealtimeDate) {
                ((RealtimeDate) o).setOffset(mOffset);
            }
            if(o instanceof RealtimeMonth) {
                ((RealtimeMonth) o).setOffset(mOffset);
            }
            if(o instanceof RealtimeYear) {
                ((RealtimeYear) o).setOffset(mOffset);
            }
        }
    }

    public int getDateOffset() {
        return mOffset;
    }

    @Override
    public Bitmap getScaledBitmap(Context context) {
        Debug.d(TAG, "getScaledBitmap Width=" + (int)(mXcor_end - mXcor) + ", Height = " + (int)mHeight);
        if (!isNeedRedraw) {
            return mBitmap;
        }

        if(null != mBitmap && !mBitmap.isRecycled()) {
            mBitmap.recycle();
        }

        if (mXcor_end - mXcor == 0) {
            meature();
        }

        mBitmap = Bitmap.createBitmap((int)(mXcor_end - mXcor) , (int)mHeight, Configs.BITMAP_CONFIG);
        mCan = new Canvas(mBitmap);

        for(BaseObject o : mSubObjs) {
            try {
                Bitmap b = o.getScaledBitmap(context);
                mCan.drawBitmap(b, o.getX()-getX(), 0, mPaint);
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

        isNeedRedraw = false;

        Debug.d(TAG, "getScaledBitmap Width=" + (int)(mXcor_end - mXcor) + ", Height = " + (int)mHeight);
        return mBitmap;
    }

    public Bitmap getBgBitmap(Context context, float scaleW, float scaleH) {
        Debug.d(TAG, "getBitmap width = " + (mXcor_end - mXcor) + ", mHeight = " + mHeight);

        Bitmap bmp = Bitmap.createBitmap((int)(mXcor_end * scaleW - mXcor * scaleW) , (int)(mHeight * scaleH), Configs.BITMAP_CONFIG);
        mCan = new Canvas(bmp);
        mCan.drawColor(Color.WHITE);
        for(BaseObject o : mSubObjs) {
            Debug.d(TAG, "--->obj: " + o.mId);
            //constant
            if(o instanceof TextObject) {
                Bitmap b = o.makeBinBitmap(context, o.getContent(), (int)(o.getWidth() * scaleW), (int)(o.getHeight() * scaleH), o.getFont());
                if (b == null) {
                    continue;
                }
                mCan.drawBitmap(b, (int)(o.getX() * scaleW - getX() * scaleW), 0, mPaint);
            }
        }
        return bmp;
    }

    @Override
    public Bitmap getpreviewbmp() {
        Debug.i(TAG, "getpreviewbmp");
        Bitmap bitmap = Bitmap.createBitmap(Math.round(mXcor_end - mXcor) , Math.round(mHeight), Configs.BITMAP_PRE_CONFIG);
        Canvas can = new Canvas(bitmap);

        Paint paint = new Paint();
        paint.setTextSize(getfeed());
        paint.setAntiAlias(true);
        paint.setFilterBitmap(true);

        try {
            paint.setTypeface(FontCache.get(mContext, mFont));
        } catch (Exception e) {}

        Paint.FontMetrics fm = paint.getFontMetrics();

        for(BaseObject o : mSubObjs) {
            try {
                if(mSubObjsTag.containsKey(o)) {
                    float width = (int)paint.measureText(mSubObjsTag.get(o));
                    Bitmap b = Bitmap.createBitmap(Math.round(width) , Math.round(mHeight), Configs.BITMAP_PRE_CONFIG);
                    Canvas c = new Canvas(b);

                    paint.setColor(Color.BLUE);
                    c.drawText(mSubObjsTag.get(o) , 0, mHeight-fm.descent, paint);

                    can.drawBitmap(Bitmap.createScaledBitmap(b, Math.round(o.getWidth()), Math.round(mHeight), false), o.getX()-getX(), 0, paint);
                } else {
                    can.drawBitmap(o.getpreviewbmp(), o.getX()-getX(), 0, paint);
                }
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

        Debug.d(TAG, "getpreviewbmp Width=" + (int)(mXcor_end - mXcor) + ", Height = " + (int)mHeight);
        return bitmap;
    }

    @Override
    public void generateVarbinFromMatrix(String f) {
        for (BaseObject object : mSubObjs) {
            if (object.mId.equals(BaseObject.OBJECT_TYPE_TEXT)) {
                continue;
            }
            object.generateVarbinFromMatrix(f);

        }
    }

    @Override
    public int makeVarBin(Context ctx, float scaleW, float scaleH, int dstH) {
        int dot = 0;
        for(BaseObject o : mSubObjs) {
            Debug.d(TAG, "--->obj: " + o.mId);
            if(o instanceof TextObject) {
                continue;
            } else	{
                dot += o.makeVarBin(ctx, scaleW, scaleH, dstH);
            }
        }
        return dot;
    }

    public String toString()
    {
        float prop = getProportion();
        String str="";
        str += mId+"^";                                                         // Tag 1    对象编号
        str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";          // Tag 2    X开始坐标
        str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";          // Tag 3    Y开始坐标
        str += BaseObject.floatToFormatString(getXEnd()*2  * prop, 5)+"^";      // Tag 4    X终止坐标
        str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";       // Tag 5    Y终止坐标
        str += BaseObject.intToFormatString(0, 1)+"^";                          // Tag 6    字符大小
        str += BaseObject.boolToFormatString(mDragable, 3)+"^";                 // Tag 7    支持拖拉标识
        str += BaseObject.intToFormatString(getCounterIndex(), 3) + "^";        // Tag 8    计数器号
        str += getShiftValue(0) + "^";                                          // Tag 9    班次值1
        str += getShiftValue(1) + "^";                                          // Tag 10   班次值2
        str += getShiftValue(2) + "^";                                          // Tag 11   班次值3
        str += getShiftValue(3) + "^";                                          // Tag 12   班次值4
        str += BaseObject.intToFormatString(getShiftTime(0), 8) + "^";          // Tag 13   班次时间1
        str += BaseObject.intToFormatString(getShiftTime(1), 8) + "^";          // Tag 14   班次时间2
        str += BaseObject.intToFormatString(getShiftTime(2), 8) + "^";          // Tag 15   班次时间3
        str += BaseObject.intToFormatString(getShiftTime(3), 8) + "^";          // Tag 16   班次时间4
        str += getCounterStart() + "^";                                         // Tag 17   计数器起始值
        str += getCounterEnd() + "^";                                           // Tag 18   计数器终止值
        str += mFont + "^";                                                     // Tag 19   字体
//        str += "<Counter No.>^<Shift1>^<Shift2>^<Shift3>^<Shift4>^";
//        str += "<Value1>^<Value2>^<Value3>^<Value4>^<Counter Start>^<Counter End>^<Offset>(Font)^000^" + mContent;
        str += getDateOffset() + "^";                                           // Tag 20   天数偏移
        str += mContent + "^";                                                  // Tag 21   内容

        Debug.d(TAG, "toString = [" + str + "]");

        return str;
    }

}
