package com.industry.printer.object;

import java.util.Arrays;
import java.util.Calendar;
import java.util.HashMap;

import org.apache.http.util.ByteArrayBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.graphics.Rect;
import android.text.TextUtils;

import com.industry.printer.MessageTask;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.PrinterApplication;
import com.industry.printer.R;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.cache.FontCache;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.BinFromBitmap;

public class BaseObject{
	private static final String TAG = BaseObject.class.getSimpleName();

	public static final String OBJECT_TYPE_TEXT		="001";
	public static final String OBJECT_TYPE_CNT		="002";
	public static final String OBJECT_TYPE_RT_YEAR	="003";
	public static final String OBJECT_TYPE_RT_MON	="004";
	public static final String OBJECT_TYPE_RT_DATE	="005";
	public static final String OBJECT_TYPE_RT_HOUR	="006";
	public static final String OBJECT_TYPE_RT_MIN	="007";
	public static final String OBJECT_TYPE_YEAR		="008";
	public static final String OBJECT_TYPE_SHIFT	="009";
	public static final String OBJECT_TYPE_DL_YEAR	="013";
	public static final String OBJECT_TYPE_DL_MON	="014";
	public static final String OBJECT_TYPE_DL_DATE	="015";
	public static final String OBJECT_TYPE_JULIAN	="025";
	public static final String OBJECT_TYPE_GRAPHIC	="026";
	public static final String OBJECT_TYPE_BARCODE	="027";
	public static final String OBJECT_TYPE_LINE		="028";
	public static final String OBJECT_TYPE_RECT		="029";
	public static final String OBJECT_TYPE_ELLIPSE	="030";
	public static final String OBJECT_TYPE_MsgName	="031";
	public static final String OBJECT_TYPE_RT		="032";
	public static final String OBJECT_TYPE_QR		="033";
	public static final String OBJECT_TYPE_WEEKDAY  ="034";
	public static final String OBJECT_TYPE_WEEKS	="035";
	public static final String OBJECT_TYPE_RT_SECOND="036";
	public static final String OBJECT_TYPE_LETTERHOUR ="037";
	public static final String OBJECT_TYPE_DYN_TEXT ="040";
	
	public static final String OBJECT_TYPE_WEEKOFYEAR = "041";

// H.M.Wang 2020-2-12 追加超文本对象
	public static final String OBJECT_TYPE_HYPERTEXT = "043";
// End of H.M.Wang 2020-2-12 追加超文本对象

	public Context mContext;
	
	public String mId;
	public int mIndex;
	public String mTypeName;
	public Paint mPaint;
	public Canvas mCan;
	public String mFont;
	public float mSize;
	public float mWidth;
	public float mHeight;
	public float mXcor;
	public float mYcor;
	public float mXcor_end;
	public float mYcor_end;
	public boolean mDragable;
	public int mLineWidth;
	public boolean mIsSelected;
	public String mContent;
	public String mName;
	public int mDotsPerClm;
	/*内容来源 是否U盤*/
	public boolean mSource;

	public int mOffset;
	public BaseObject mParent;

	protected boolean mReverse;
	/* 
	 * 是否需要重新绘制bitmap 
	 * 需要重新绘制bitmap的几种情况：1、宽高变化；2、字体修改； 3，内容变化
	 */
	protected boolean isNeedRedraw;
	protected Bitmap	mBitmap;
	protected Bitmap	mBitmapSelected;
	
	public HashMap<String, byte[]> mVBuffer;
	public MessageTask mTask;

	// H.M.Wang 从RealtimeObject移至此处，为的是是的TextObject也能用到
	protected float mRatio = 1.0f;

	//	public static final String DEFAULT_FONT = "0T+";//
	public static final String DEFAULT_FONT = "1+";// addbylk	
	/**
	 * supported fonts
	 */
////	public static String[] mFonts = FontCache.getFonts(PrinterApplication.getInstance());
	
	public BaseObject(Context context, String id, float x)
	{
		this(context, id);
		setX(x);
		
		initName();
	}

	public BaseObject(Context ctx, String id)
	{
		//super(context);
		mContext = ctx;
		mId=id;
		mIndex = 0;
		mXcor=0;
		mYcor=0;
		mXcor_end=0;
		mYcor_end=0;
		mDragable = true;
		isNeedRedraw = true;
		mSource = false;
		mReverse = false;
		// 參數40：列高
		mDotsPerClm = 152;//SystemConfigFile.getInstance(mContext).getParam(39);

		mParent = null;

// H.M.Wang 2019-9-26 直接设置高，因为setHeight里面做了很多事情，不适合初始化的时候调用
//		setHeight(Configs.gDots);
		mHeight = Configs.gDots;
		mWidth = 0;

////		boolean findFont = false;
////		Debug.d(TAG, "--->fonts: " + mFonts);
////		if (mFonts != null && mFonts.length > 0) {
////			for (String font : mFonts) {
////				Debug.d(TAG, "--->font: " + font);
////				if (DEFAULT_FONT.equalsIgnoreCase(font)) {
////					Debug.d(TAG, "--->font match: " + font);
////					mFont = DEFAULT_FONT;
////					findFont = true;
////					break;
////				}
////			}
////			if (!findFont) {
////				mFont = mFonts[0];
////			}
////		} else {
			mFont = DEFAULT_FONT;
////		}

		initPaint();
		setSelected(true);	
		Debug.d(TAG, "--->new baseobject: " + isNeedRedraw);
		// setHeight(Configs.gDots);
		setLineWidth(5);
		// H.M.Wang 2019-9-25 调用setContent会在CounterObject的时候造成死机，因为此时mStart和mBits都没有初始化
//		setContent("");
		mContent = "";

		mVBuffer = new HashMap<String, byte[]>();

	}

	private void initName() {
		Debug.d(TAG, "--->mcontext: " + mContext);
		if(this instanceof MessageObject)
			mName = mContext.getString(R.string.object_msg_name);
		else if(this instanceof TextObject)
			mName = mContext.getString(R.string.object_text);
		else if(this instanceof CounterObject)
			mName = mContext.getString(R.string.object_counter);
		else if(this instanceof BarcodeObject)
			mName = mContext.getString(R.string.object_bar);
		else if(this instanceof GraphicObject)
			mName = mContext.getString(R.string.object_pic);
		else if(this instanceof JulianDayObject)
			mName = mContext.getString(R.string.object_julian);
		else if(this instanceof RealtimeObject)
			mName = mContext.getString(R.string.object_realtime);
		else if(this instanceof LineObject)
			mName = mContext.getString(R.string.object_line);
		else if(this instanceof RectObject)
			mName = mContext.getString(R.string.object_rect);
		else if(this instanceof EllipseObject)
			mName = mContext.getString(R.string.object_ellipse);
		else if(this instanceof ShiftObject)
			mName = mContext.getString(R.string.object_shift);
		else if(this instanceof RealtimeSecond)
			mName = mContext.getString(R.string.object_second);
// H.M.Wang 2020-2-17 追加HyperText控件
		else if(this instanceof HyperTextObject)
			mName = mContext.getString(R.string.object_hypertext);
// End of H.M.Wang 2020-2-17 追加HyperText控件
// H.M.Wang 2020-6-10 追加DynamicText控件
		else if(this instanceof DynamicText)
			mName = mContext.getString(R.string.object_dynamictext);
// End of H.M.Wang 2020-6-10 追加DynamicText控件
		else if (this instanceof LetterHourObject)
			mName = mContext.getString(R.string.object_charHour);
		else if (this instanceof WeekOfYearObject) {
			mName = mContext.getString(R.string.object_weekofyear);
		} else if (this instanceof WeekDayObject) {
			mName = mContext.getString(R.string.object_weekday);
		}
		else {
			mName = "unKnown";
		}
		Debug.d(TAG, "--->mName: " + mName);
	}
	
	public String getTitle() {
		return mName;
	}
	
	public String getId()
	{
		return mId;
	}
	
	public void initPaint()
	{
		mPaint = new Paint();
		mPaint.setTextSize(getfeed());
		try {
			// mPaint.setTypeface(FontCache.get(mContext, "fonts/"+mFont));
		} catch (Exception e) {
		}
	}
	public Bitmap getScaledBitmap(Context context)
	{
		/* 使用
		Debug.d(TAG,"getScaledBitmap  mWidth="+mWidth+", mHeight="+mHeight);
		Bitmap bmp = getBitmap(context);
		if (mWidth <= 0) {
			mWidth = bmp.getWidth();
		}
		Bitmap scaledBmp = Bitmap.createScaledBitmap(bmp, (int)mWidth, (int)mHeight, true);
		return scaledBmp;
		*/
		if (isNeedRedraw) {
			// drawSelected();
			drawNormal();
		}
		Debug.d(TAG, "--->redraw?" + isNeedRedraw);
		isNeedRedraw = false;
//		if (mIsSelected) {
//			return mBitmapSelected;
//		}
		return mBitmap;
	}
	
	protected void drawNormal() {
		// Debug.d(TAG, "--->drawNormal");
		mPaint.setColor(Color.BLACK);
		mBitmap = draw();
	}
	
	protected void drawSelected() {
		// mPaint.setColor(Color.RED);
		// Debug.d(TAG, "--->drawSelected");
		mPaint.setColor(Color.BLACK);	
		mBitmapSelected = draw();
	}
	
	public void setReverse(boolean reverse) {
		Debug.e(TAG, "--->setReverse: " + reverse + "  fail because we close this function at 20190619");
		return;
//		mReverse = reverse;
	}
	
	public boolean getReverse() {
		return mReverse;
	}

    // H.M.Wang 追加该函数。用来获取初始的横向缩放比例
	public void setXRatio() {
		mRatio = 1.0f;
		Debug.d(TAG, "mRatio = " + mRatio);
	}

	private Bitmap draw() {
		Bitmap bitmap;
		mPaint.setTextSize(getfeed());
		mPaint.setAntiAlias(true); //去除锯齿  
		mPaint.setFilterBitmap(true); //对位图进行滤波处理
		/*try {
			AssetFileDescriptor fd = mContext.getAssets().openFd("fonts/"+mFont);
			if (fd != null) {
				fd.close();
			} else {
				mFont = DEFAULT_FONT;
			}
		} catch (Exception e) {
			mFont = DEFAULT_FONT;
		}*/
		/*String f = "fonts/"+mFont;
		if (!new File("file://android_assets/" + f).exists()) {
			mFont = DEFAULT_FONT;
		}*/
////		boolean isCorrect = false;
		// Debug.d(TAG,"--->getBitmap font = " + mFont);
//		String[] mFonts = FontCache.getFonts();
////		for (String font : mFonts) {
////			Debug.d(TAG,"font = " + font + "； mFont = " + mFont);
////			if (font.equals(mFont)) {
////				isCorrect = true;
////				break;
////			}
////		}
////		if (!isCorrect) {
////			mFont = DEFAULT_FONT;
////		}
// H.M.Wang 2020-9-2 调用setFont函数，使得每次mFont的变化，都会产生重新计算宽度
//		mFont = verifyFont();
		setFont(verifyFont());
// End of H.M.Wang 2020-9-2 调用setFont函数，使得每次mFont的变化，都会产生重新计算宽度
		Debug.d(TAG,"--->draw font = " + mFont +"  h: " + mHeight);
		try {
			mPaint.setTypeface(FontCache.get(mContext, mFont));
		} catch (Exception e) {
			Debug.d(TAG, "--->e: " + e.getMessage());
		}

		// H.M.Wang 修改，一对应文本的放大缩小倍率
		int width = Math.round(mPaint.measureText(getMeatureString()));
// H.M.Wang2019-9-26 去掉画图时对倍率的调整
// H.M.Wang2019-9-27 恢复该调整
		float ratio;
		if(mWidth != 0) {
			ratio = mWidth / width;
		} else {
			ratio = 1.0f;
		}

		Debug.e(TAG, "--->content: " + getContent() + "  width=" + width + "  ratio=" + ratio);
		PrinterNozzle type = mTask != null? mTask.getNozzle() : PrinterNozzle.MESSAGE_TYPE_12_7;

		// H.M.Wang 修改下列两行
//		if (mWidth == 0 || type == PrinterNozzle.MESSAGE_TYPE_16_DOT || type == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
//		if (mWidth == 0 || type == PrinterNozzle.MESSAGE_TYPE_16_DOT || type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT || type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {

// H.M.Wang 2020-9-2 暂时恢复大字机时不调整宽度的设置，因为32点生成32高的计数器时，宽度计算不正确
// H.M.Wang 2020-8-13 取消大字机不考虑倍率的判断，否则会由于真实时间的字符串宽度和预计算的宽度
		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
			type == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
			type == PrinterNozzle.MESSAGE_TYPE_32DN ||
			type == PrinterNozzle.MESSAGE_TYPE_32SN ||
			type == PrinterNozzle.MESSAGE_TYPE_64SN ||
			type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
			setWidth(1.0f * width);
		} else {
			// H.M.Wang 修改
			setWidth(1.0f * width * ratio);
		}
// End of H.M.Wang 2020-9-2 暂时恢复大字机时不调整宽度的设置，因为32点生成32高的计数器时，宽度计算不正确
		bitmap = Bitmap.createBitmap(width , Math.round(mHeight), Configs.BITMAP_CONFIG);
		Debug.e(TAG,"===--->getBitmap width=" + mWidth + ", mHeight=" + mHeight);
		mCan = new Canvas(bitmap);
		FontMetrics fm = mPaint.getFontMetrics();
		Debug.e(TAG, "--->asent: " + fm.ascent + ",  bottom: " + fm.bottom + ", descent: " + fm.descent + ", top: " + fm.top);
        // float tY = (y - getFontHeight(p))/2+getFontLeading(p);
		mCan.drawText(mContent, 0, mHeight-fm.descent, mPaint);
//		if (mHeight <= 4 * MessageObject.PIXELS_PER_MM) {
//			setWidth(width * 1.25f);
//		}
		return Bitmap.createScaledBitmap(bitmap, Math.round(mWidth), Math.round(mHeight), false);
	}
	
	
	/**
	 * draw content to a bitmap
	 * @param ctx context
	 * @param content content
	 * @param ctH	real height of content
	 * @param ctW		total width
	 * @param font
	 * @return
	 */
	public Bitmap makeBinBitmap(Context ctx, String content, int ctW, int ctH, String font) {
		if (ctW <= 0 || ctH <= 0) {
			return null;
		}
		Bitmap bitmap;
		Paint paint = new Paint();
//		paint.setColor(Color.BLACK);
		paint.setTextSize(ctH);
		paint.setAntiAlias(true); //去除锯齿
		paint.setFilterBitmap(true); //对位图进行滤波处理
		boolean isCorrect = false;
		
		try {
			paint.setTypeface(FontCache.get(mContext, mFont));
		} catch (Exception e) {
			
		}
		int width = Math.round(paint.measureText(content));
		Debug.d(TAG, "--->bmp: " + width);
//		StringBuffer buffer = new StringBuffer();
//		for (int i = 0 ; i < 10; i++) {
//			buffer.append("a");
//			width = (int)paint.measureText(buffer.toString());
//			Debug.d(TAG, "--->bmp: " + width);
//		}
//
//		Rect rect = new Rect();
//		paint.getTextBounds(getContent(), 0, getContent().length(), rect);
//		int w = getTextWidth(paint, getContent());
		bitmap = Bitmap.createBitmap(width , ctH, Configs.BITMAP_CONFIG);
		Debug.d(TAG, "--->bmp: " + bitmap.getWidth() +  "  " + bitmap.getHeight() );
		Canvas canvas = new Canvas(bitmap);
		FontMetrics fm = paint.getFontMetrics();
		int adjust = Math.round(fm.descent);
//		if (adjust < 4 && ctH > 32) {
//			adjust = 4;
//		}
		canvas.drawText(content, 0, ctH-adjust, paint);
		PrinterNozzle head = mTask.getNozzle();
		Debug.d(TAG, "--->content: " + content + "  descent=" + fm.descent + "  ascent= " + fm.ascent + " botom= " + fm.bottom + " top = " + fm.top + " leading = " + fm.leading);

		// H.M.Wang 修改下列两行
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
			head == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
			head == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
			head == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
// H.M.Wang 2020-8-26 追加64SN打印头
            head == PrinterNozzle.MESSAGE_TYPE_64SN ||
// End of H.M.Wang 2020-8-26 追加64SN打印头
			head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头

			return bitmap;
		}

		return Bitmap.createScaledBitmap(bitmap, ctW, ctH, true);
	}

	public static int getTextWidth(Paint paint, String str) {
		int iRet = 0;
		if (str != null && str.length() > 0) {
			int len = str.length();
			float[] widths = new float[len];
			paint.getTextWidths(str, widths);
			for (int j = 0; j < len; j++) {
				iRet += (int) Math.ceil(widths[j]);
			}
		}
		return iRet;
	}

	private int getfeedsent() {
		return (int)(mHeight/10 * 11/20 + 1);
	}
	
	protected Bitmap getBitmap(Context context)
	{
		//mPaint.setColor(Color.RED);
		//Debug.d(TAG,"getBitmap mContent="+mContent);
		mPaint.setTypeface(FontCache.get(context, mFont));
		int width = Math.round(mPaint.measureText(getContent()));
		int height = Math.round(mPaint.getTextSize());
		
		Bitmap bmp = Bitmap.createBitmap(width , Configs.gDots, Configs.BITMAP_CONFIG);
		Debug.d(TAG,"getBitmap width="+width+", height="+height+ ", mHeight="+mHeight);
		mCan = new Canvas(bmp);
		mCan.drawText(mContent, 0, height-30, mPaint);
		//BinCreater.saveBitmap(bmp, "bg.png");
		return bmp;
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
		int width = Math.round(paint.measureText("8"));
		FontMetrics fm = paint.getFontMetrics();
		
		/*draw Bitmap of single digit*/
		Bitmap bmp = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
		Canvas can = new Canvas(bmp);

		PrinterNozzle head = mTask.getNozzle();

		// H.M.Wang 修改下列两行
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT || head == PrinterNozzle.MESSAGE_TYPE_32_DOT || head == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		if (head == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
			head == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
			head == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
			head == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
// H.M.Wang 2020-8-26 追加64SN打印头
            head == PrinterNozzle.MESSAGE_TYPE_64SN ||
// End of H.M.Wang 2020-8-26 追加64SN打印头
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
		Bitmap gBmp = Bitmap.createBitmap(singleW*10, dstH, Configs.BITMAP_CONFIG);
		Canvas gCan = new Canvas(gBmp);
		
		gCan.drawColor(Color.WHITE);	/*white background*/
		for(int i =0; i<=9; i++)
		{
			can.drawColor(Color.WHITE);
			can.drawText(String.valueOf(i), 0, height - fm.descent, paint);
			// H.M.Wang 修改 20190905
//			gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, height, false), i*singleW, (int)getY() * scaleH, paint);
			gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, height, false), i*singleW, Math.round(getY() * scaleH), paint);
		}

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
	public int drawVarBitmap()
	{
		Debug.d(TAG, "***************begin  id: " + mId + " index:  " + mIndex);
		if (TextUtils.isEmpty(mContent)) {
			Debug.d(TAG, "--->drawVarBitmap = " + mContent);
			return 0;
		}
		int heads = mTask.getHeads() == 0 ? 1 : mTask.getHeads();

		int[] dots = new int[1];
		//mPaint.setTextSize(mHeight);
		int singleW; //the width value of each char
		int height = Math.round(mPaint.getTextSize());
		int width = Math.round(mPaint.measureText("8"));
		FontMetrics fm = mPaint.getFontMetrics();
		float wDiv = (float) (2.0/heads);
		MessageObject msg = mTask.getMsgObject();
		/*對320高的buffer進行單獨處理*/
		if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH)) {
// H.M.Wang 修改
//			wDiv = 1;
			wDiv = 1.0f * 304/308;
		} else if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL)) {
// H.M.Wang 修改
//			wDiv = 0.5f;
			wDiv = 2.0f * 304/308;
		// H.M.Wang 追加下列4行
		} else if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE)) {
// H.M.Wang 修改
//			wDiv = 0.333333333333f;
			wDiv = 3.0f * 304/308;
		} else if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR)) {
// H.M.Wang 修改
//			wDiv = 0.25f;
			wDiv = 4.0f * 304/308;
		}
		/*draw Bitmap of single digit*/
		Bitmap bmp = Bitmap.createBitmap(width, Math.round(mHeight), Configs.BITMAP_CONFIG);
		Canvas can = new Canvas(bmp);
		
		// Debug.d(TAG, "--->id = " + mId + " Width=" + mWidth);
		/*draw 0-9 totally 10 digits Bitmap*/
		singleW = Math.round(mWidth/mContent.length());
		//Debug.d(TAG, "--->singleW=" + singleW);
		singleW = Math.round((singleW/wDiv));
		//Debug.d(TAG, "--->singleW/div=" + singleW);
		Bitmap gBmp = Bitmap.createBitmap(singleW*10, Configs.gDots * mTask.getHeads(), Configs.BITMAP_CONFIG);
		Canvas gCan = new Canvas(gBmp);
		gCan.drawColor(Color.WHITE);	/*white background*/
		for(int i =0; i<=9; i++)
		{
			/*draw background to white firstly*/
			can.drawColor(Color.WHITE);
			can.drawText(String.valueOf(i), 0, mHeight-fm.descent, mPaint);
			// Bitmap b = Bitmap.createScaledBitmap(bmp, singleW, (int)mHeight, true);
			gCan.drawBitmap(Bitmap.createScaledBitmap(bmp, singleW, Math.round(mHeight * mTask.getHeads()), false), i*singleW, Math.round(getY()) * mTask.getHeads(), mPaint);
		}
		BinFromBitmap.recyleBitmap(bmp);
		/*對320高的buffer進行單獨處理*/
		if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH)) {
			gBmp = Bitmap.createScaledBitmap(gBmp, gBmp.getWidth(), 308, true);
			Bitmap b = Bitmap.createBitmap(gBmp.getWidth(), 320, Configs.BITMAP_CONFIG);
			can.setBitmap(b);
			can.drawColor(Color.WHITE);
			can.drawBitmap(gBmp, 0, 0, mPaint);
			gBmp.recycle();
			gBmp = b;
		} else if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_DUAL)) {
			gBmp = Bitmap.createScaledBitmap(gBmp, gBmp.getWidth(), 308*2, true);
			Bitmap b = Bitmap.createBitmap(gBmp.getWidth(), 320*2, Configs.BITMAP_CONFIG);
			can.setBitmap(b);
			can.drawColor(Color.WHITE);
			int h = gBmp.getHeight()/2;
			/*先把gBmp的上半部分0~307高貼到620高的上半部分（0~319）*/
			can.drawBitmap(gBmp, new Rect(0, 0, gBmp.getWidth(), h), new Rect(0, 0, b.getWidth(), h), null);
			
			/*先把gBmp的下半部分308~615高貼到620高的下半部分（320~619）*/
			// can.drawBitmap(Bitmap.createBitmap(gBmp, 0, 308, gBmp.getWidth(), 308), 0, 320, mPaint);
			can.drawBitmap(gBmp, new Rect(0, h, gBmp.getWidth(), h*2), new Rect(0, 320, b.getWidth(), 320 + h), null);
			gBmp.recycle();
			gBmp = b;

		// H.M.Wang 追加下列18行
		} else if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_TRIPLE)) {
			gBmp = Bitmap.createScaledBitmap(gBmp, gBmp.getWidth(), 308*3, true);
			Bitmap b = Bitmap.createBitmap(gBmp.getWidth(), 320*3, Configs.BITMAP_CONFIG);
			can.setBitmap(b);
			can.drawColor(Color.WHITE);
			int h = gBmp.getHeight()/3;

			/*先把gBmp的上三分之一部分0~307高貼到960高的上三分之一部分（0~319）*/
			can.drawBitmap(gBmp, new Rect(0, 0, gBmp.getWidth(), h), new Rect(0, 0, b.getWidth(), h), null);

			/*先把gBmp的中间三分之一部分308~615高貼到960高的中间三分之一部分（320~639）*/
			can.drawBitmap(gBmp, new Rect(0, h, gBmp.getWidth(), h*2), new Rect(0, 320, b.getWidth(), 320 + h), null);

			/*先把gBmp的下三分之一部分616~923高貼到960高的下三分之一部分（640~959）*/
			can.drawBitmap(gBmp, new Rect(0, h*2, gBmp.getWidth(), h*3), new Rect(0, 640, b.getWidth(), 640 + h), null);

			gBmp.recycle();
			gBmp = b;

		// H.M.Wang 追加下列21行
		} else if (msg != null && (msg.getPNozzle() == PrinterNozzle.MESSAGE_TYPE_1_INCH_FOUR)) {
			gBmp = Bitmap.createScaledBitmap(gBmp, gBmp.getWidth(), 308*4, true);
			Bitmap b = Bitmap.createBitmap(gBmp.getWidth(), 320*4, Configs.BITMAP_CONFIG);
			can.setBitmap(b);
			can.drawColor(Color.WHITE);
			int h = gBmp.getHeight()/4;

			/*先把gBmp的上四分之一部分0~307高貼到1280高的上四分之一部分（0~319）*/
			can.drawBitmap(gBmp, new Rect(0, 0, gBmp.getWidth(), h), new Rect(0, 0, b.getWidth(), h), null);

			/*先把gBmp的中间四分之二部分308~615高貼到1280高的中间四分之二部分（320~639）*/
			can.drawBitmap(gBmp, new Rect(0, h, gBmp.getWidth(), h*2), new Rect(0, 320, b.getWidth(), 320 + h), null);

			/*先把gBmp的中间四分之三部分616~923高貼到1280高的中间四分之三部分（640~959）*/
			can.drawBitmap(gBmp, new Rect(0, h*2, gBmp.getWidth(), h*3), new Rect(0, 640, b.getWidth(), 640 + h), null);

			/*先把gBmp的下四分之一部分924~1231高貼到1280高的下四分之一部分（960~1279）*/
			can.drawBitmap(gBmp, new Rect(0, h*3, gBmp.getWidth(), h*4), new Rect(0, 960, b.getWidth(), 960 + h), null);

			gBmp.recycle();
			gBmp = b;
		}
		BinFileMaker maker = new BinFileMaker(mContext);

		// H.M.Wang 追加一个是否移位的参数
		dots = maker.extract(gBmp, 1, false);

		Debug.d(TAG, "***************id: " + mId + " index:  " + mIndex);
		maker.save(ConfigPath.getVBinAbsolute(mTask.getName(), mIndex));
		//
		BinFromBitmap.recyleBitmap(gBmp);
		/*根據變量內容的實際長度計算點數*/
		dots[0] = (dots[0] * getContent().length()/10) + 1;
		return dots[0];
	}
	/**
	 * generateVarBuffer - generate the variable bin buffer, Contained in the HashMap
	 * 设计： 
	 */
	public void generateVarBuffer()
	{
		if (TextUtils.isEmpty(mContent)) {
			return;
		}
		//mPaint.setTextSize(mHeight);
		int singleW; //the width value of each char
		int height = Math.round(mPaint.getTextSize());
		int width = Math.round(mPaint.measureText("8"));
		/*draw Bitmap of single digit*/
		Bitmap bmp = Bitmap.createBitmap(width, Configs.gDots, Configs.BITMAP_CONFIG);
		Canvas can = new Canvas(bmp);
		
		/*draw 0-9 totally 10 digits Bitmap*/
		singleW = Math.round(mWidth/mContent.length());

		/*A full-size empty bitmap, width:singleW; height: Configs.gDots*/
		Bitmap bg = Bitmap.createBitmap(singleW, Configs.gDots, Configs.BITMAP_CONFIG); 
		mCan = new Canvas(bg);
		
		for(int i =0; i<=9; i++)
		{
			/*draw background to white firstly*/
			can.drawColor(Color.WHITE);
			mCan.drawColor(Color.WHITE);
			can.drawText(String.valueOf(i), 0, height-30, mPaint);
			Bitmap b = Bitmap.createScaledBitmap(bmp, singleW, Math.round(mHeight), true);
			mCan.drawBitmap(b, 0, getY(), mPaint);
			// Bitmap scaledBg = Bitmap.createScaledBitmap(bg, singleW, Configs.gDots, true);
			BinFileMaker maker = new BinFileMaker(mContext);

			// H.M.Wang 追加一个是否移位的参数
			maker.extract(bmp, 1, false);

			//byte[] buffer = new byte[BinCreater.mBmpBits.length];
			byte[] buffer = Arrays.copyOf(maker.getBuffer(), maker.getBuffer().length);
			BinFromBitmap.recyleBitmap(b);
			mVBuffer.put(String.valueOf(i), buffer);
		}
		BinFromBitmap.recyleBitmap(bmp);
		BinFromBitmap.recyleBitmap(bg);
	}
	
	public void generateVarbinFromMatrix(String f) {
		BinFileMaker maker = new BinFileMaker(mContext);
		maker.extract("0123456789");
		maker.save(f + getVarBinFileName());
	}
	
	
	public Canvas getCanvas()
	{
		return mCan;
	}
	
	public void setHeight(float size)
	{
		if (this instanceof BarcodeObject && size < 4.0*MessageObject.PIXELS_PER_MM) {
			size = 4.0f * MessageObject.PIXELS_PER_MM;
		}/* else if (size < MessageObject.mBaseList[0] * MessageObject.PIXELS_PER_MM) {
			size = MessageObject.mBaseList[0] * MessageObject.PIXELS_PER_MM;
		}*/ else if (size > 152) {
			size = 152;
		}
// H.M.Wang 2019-9-26 考虑高的变化对倍率的影响
		mRatio *= mHeight / size;
		mHeight = size;
		tuningHeightOfSpecialHeadtype();
		mYcor_end = mYcor + mHeight;
		Debug.d(TAG, "--->id " + mId  + "  height=" + mHeight + "   yEnd= " + mYcor_end);
		isNeedRedraw = true;
		
	}

	// H.M.Wang 2019-9-26 从TextObject和BarcodeObject移至此，对所有元素有效。RealtimeObject元素及其其元素除外
	public void wide() {
		float ratio = (getWidth() + 5) / getWidth();
		mRatio *= ratio;

		setWidth(getWidth()*ratio);
		isNeedRedraw = true;
	}

	// H.M.Wang 2019-9-26 从TextObject和BarcodeObject移至此，对所有元素有效。RealtimeObject元素及其其元素除外
	public void narrow() {
		float ratio = (getWidth() - 5) / getWidth();
		mRatio *= ratio;

		setWidth(getWidth()*ratio);
		isNeedRedraw = true;
	}

	/**
	 * 根据不同喷头类型处理一些需要特殊对待的尺寸
	 */
	private void tuningHeightOfSpecialHeadtype() {
		if (mTask == null) {
			return;
		}
		PrinterNozzle type = mTask.getNozzle();
		String dspH = getDisplayHeight();
		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
			Debug.d(TAG, "--->display H = " + dspH + "   mHeight: " + mHeight);
// H.M.Wang 2020-4-15 追加"5x5"字体
			if (MessageObject.mDotSizes[0].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 5 / 16;
			} else if (MessageObject.mDotSizes[1].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 8 / 16;
// H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            } else if (MessageObject.mDotSizes[2].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 10 / 16;
            } else if (MessageObject.mDotSizes[3].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 12 / 16;
            } else if (MessageObject.mDotSizes[4].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 14 / 16;
// End of H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			} else {
				mHeight = 152f * 16 / 16;
			}
        // H.M.Wang 2019-09-11 MessageObject.mDotSizes 修改为 MessageObject.mDot_32_Size
// H.M.Wang 2020-7-23 追加32DN打印头
//		} else if (type == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-8-17 追加32SN打印头
//		} else if (type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_32DN) {
		} else if (type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_32DN || type == PrinterNozzle.MESSAGE_TYPE_32SN) {
// End of H.M.Wang 2020-8-17 追加32SN打印头
// End of H.M.Wang 2020-7-23 追加32DN打印头
			Debug.d(TAG, "--->display H = " + dspH + "   mHeight: " + mHeight);
			if (MessageObject.mDot_32_Size[0].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 5 / 32;
			} else if (MessageObject.mDot_32_Size[1].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 8 / 32;
// H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
//			} else if (MessageObject.mDot_32_Size[1].equalsIgnoreCase(dspH)) {
            } else if (MessageObject.mDot_32_Size[2].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 10 / 32;
            } else if (MessageObject.mDot_32_Size[3].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 12 / 32;
            } else if (MessageObject.mDot_32_Size[4].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 14 / 32;
            } else if (MessageObject.mDot_32_Size[5].equalsIgnoreCase(dspH) ||
						MessageObject.mDot_32_Size[6].equalsIgnoreCase(dspH) ||
						MessageObject.mDot_32_Size[7].equalsIgnoreCase(dspH)) {
// End of H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
				mHeight = 152f * 16 / 32;
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (MessageObject.mDot_32_Size[8].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 19 / 32;
			} else if (MessageObject.mDot_32_Size[9].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 21 / 32;
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else {
				mHeight = 152f * 32 / 32;
			}

		// H.M.Wang 追加下列11行
		// H.M.Wang 2019-09-11 MessageObject.mDotSizes 修改为 MessageObject.mDot_64_Size
// H.M.Wang 2020-8-26 追加64SN打印头
//        } else if (type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		} else if (type == PrinterNozzle.MESSAGE_TYPE_64_DOT || type == PrinterNozzle.MESSAGE_TYPE_64SN) {
// End of H.M.Wang 2020-8-26 追加64SN打印头
			Debug.d(TAG, "--->display H = " + dspH + "   mHeight: " + mHeight);
			if (MessageObject.mDot_64_Size[0].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 5 / 64;
			} else if (MessageObject.mDot_64_Size[1].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 8 / 64;
// H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
            } else if (MessageObject.mDot_64_Size[2].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 10 / 64;
            } else if (MessageObject.mDot_64_Size[3].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 12 / 64;
            } else if (MessageObject.mDot_64_Size[4].equalsIgnoreCase(dspH)) {
                mHeight = 152f * 14 / 64;
			} else if (MessageObject.mDot_64_Size[5].equalsIgnoreCase(dspH) ||
					MessageObject.mDot_64_Size[6].equalsIgnoreCase(dspH) ||
					MessageObject.mDot_64_Size[7].equalsIgnoreCase(dspH)) {
// End of H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
				mHeight = 152f * 16 / 64;
// H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (MessageObject.mDot_64_Size[8].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 19 / 64;
			} else if (MessageObject.mDot_64_Size[9].equalsIgnoreCase(dspH)) {
				mHeight = 152f * 21 / 64;
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if (MessageObject.mDot_64_Size[10].equalsIgnoreCase(dspH)) {
// End of H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
				mHeight = 152f * 24 / 64;
// H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
			} else if (MessageObject.mDot_64_Size[11].equalsIgnoreCase(dspH)) {
// End of H.M.Wang 2020-1-31 追加"10x8", "12x9", "14x10"字体，高度不跟16x12走
				mHeight = 152f * 32 / 64;
			} else {
				mHeight = 152f * 64 / 64;
			}
		}
// End of H.M.Wang 2020-4-15 追加"5x5"字体
	}
	
	public void resizeByHeight() {
		mPaint.setTextSize(getfeed());

		// H.M.Wang 修改为测量标准字符串，而非实际内容
//		String s = getContent();
		String s = getMeatureString();
		if (s == null) {
			return;
		}
		float width = mPaint.measureText(s);

		Debug.d(TAG, "Messure: s=" + s + "; width=" + width + "; height=" + getfeed());
//		if (getHeight() <= 4 * MessageObject.PIXELS_PER_MM) {
//			width = width * 1.25f;
//		}
		setWidth(width);
	}

	public void setHeight(String size)
	{
		float height = mTask.getMsgObject().getPixels(size);
		setHeight(height);
	}

	public String getDisplayHeight() {
		Debug.d(TAG, "--->getDisplayHeight: " + mHeight);
//		return mTask.getMsgObject().getDisplayFs(mHeight);
		String dispString =  mTask.getMsgObject().getDisplayFs(mHeight);
// H.M.Wang 2020-4-15 追加"5x5"字体
		if(dispString.startsWith(MessageObject.mDotSizes[5]) || dispString.startsWith(MessageObject.mDot_32_Size[5]) || dispString.startsWith(MessageObject.mDot_64_Size[5])) {
// End of H.M.Wang 2020-4-15 追加"5x5"字体
			String[] dispStrings = dispString.split(",");
			if(null != dispStrings) {
				if(mFont.equals("7")) {
					dispString = dispStrings.length > 0 ? dispStrings[0] : "16x12";
				} else if(mFont.equals("7L")) {
					dispString = dispStrings.length > 1 ? dispStrings[1] : "16x12";
				} else if(mFont.equals("7R")) {
					dispString = dispStrings.length > 2 ? dispStrings[2] : "16x12";
				} else {
					dispString = "16x12";
				}
			} else {
				dispString = "16x12";
			}
		} else {
			dispString = dispString;
		}
		return dispString;
	}
	
	public float getHeight()
	{
		return mHeight;
	}
	
	public void setWidth(float size)
	{
		if(size<0)
			size=0;
		mWidth = size;
		mXcor_end = mXcor + mWidth;
//		isNeedRedraw = true;
		Debug.d(TAG, "===>setWidth x= " + mXcor + ", mWidth=" + mWidth + ", xend=" + mXcor_end);
	}

	public float getWidth()
	{
		return mWidth;
	}
	
	public void setX(float x)
	{
		mXcor=x;
		mXcor_end = mXcor + mWidth; 
		Debug.d(TAG, "===>setX Content = " + mContent + ", x= " + mXcor + ", mWidth=" + mWidth + ", xend=" + mXcor_end);
	}
	
	public float getX()
	{
		return mXcor;
	}
	
	public float getXEnd()
	{
		return mXcor_end;
	}
	
	public void setY(float y)
	{
		mYcor = y;
		mYcor_end = mYcor + mHeight;
		Debug.d(TAG, "--->height=" + mHeight + "   yEnd= " + mYcor_end);
	}
	
	public float getY()
	{
		return mYcor;
	}
	
	public float getYEnd()
	{
		Debug.d(TAG, "--->height=" + mHeight + "   yEnd= " + mYcor_end);
		return mYcor_end;
	}
	
	
	public void setSelected(boolean s)
	{
		mIsSelected = s;
		if(mIsSelected)
		//	mPaint.setColor(Color.RED);//addbylk
			mPaint.setColor(Color.BLACK);	
		else
			mPaint.setColor(Color.BLACK);
	}
	
	public boolean getSelected()
	{
		return mIsSelected;
	}

	public void setContent(String content)
	{
		Debug.d(TAG, "setContent: [" + content + "]");

		if(mContent!=null && mContent.equals(content))
			return;
		mContent = content;
		/*mPaint.setTextSize(getfeed());
		mWidth = mPaint.measureText(mContent);
		mXcor_end = mXcor + mWidth;
		Debug.d(TAG,"content="+mContent+", mXcor = "+mXcor+", mWidth ="+mWidth + ",mHeight="+mHeight);
		mYcor_end = mYcor + mHeight;*/
//		mWidth = 0;
		isNeedRedraw = true;
	}

	public String getContent()
	{
		return mContent;
	}
	
	public void setDragable(boolean drag)
	{
		mDragable = drag;
	}
	
	public boolean getDragable()
	{
		return mDragable;
	}
	
	public void setFont(String font)
	{
// H.M.Wang 2020-8-31 只有字体发生变化的时候再重新设置字体，并且重新计算宽度
//		if(font== null)
		if(font== null || font == mFont)
// End of H.M.Wang 2020-8-31 只有字体发生变化的时候再重新设置字体，并且重新计算宽度
			return;

		mFont = font;
		try {
			mPaint.setTypeface(FontCache.get(mContext, mFont));
// H.M.Wang 2020-8-31 临时追加重新计算宽度，可能有副作用
			setWidth(mPaint.measureText(getContent()));
// End of H.M.Wang 2020-8-31 临时追加重新计算宽度，可能有副作用
		} catch (Exception e) {}
		isNeedRedraw = true;
		Debug.d(TAG, "--->setFont: " + mFont);
	}
	
	public String getFont()
	{
// H.M.Wang 2020-9-2 调用setFont函数，使得每次mFont的变化，都会产生重新计算宽度
//		mFont = verifyFont();
		setFont(verifyFont());
// End of H.M.Wang 2020-9-2 调用setFont函数，使得每次mFont的变化，都会产生重新计算宽度
		return mFont;
	}
	public void setLineWidth(int width)
	{
		mLineWidth=width;
		if(mPaint != null)
			mPaint.setStrokeWidth(mLineWidth);
		isNeedRedraw = true;
	}
	
	public float getLineWidth()
	{
		return mLineWidth;
	}
	
	public void setDotsPerClm(int dots) {
		mDotsPerClm = dots;
	}
	
	public int getDotsPerClm() {
		return mDotsPerClm;
	}
	

	public boolean getSource() {
		return mSource ;
	}
	
	public void setSource(boolean dynamic) {
		mSource  = dynamic;
	}

	// H.M.Wang 2019-9-24 原来的实现未考虑负数的情况，导致f为负数时，变换结果不正确（如-1，变为000-1）
	public static String floatToFormatString(float f, int n)
	{
		String format = "%0" + n + "d";
		return String.format(format, Math.round(f));
	}
/*
	public static String floatToFormatString(float f, int n)
	{
		String str = "";
		Float ff = new Float(f);
		//Integer i = Integer.valueOf(ff.intValue());
		int cn = String.valueOf(ff.intValue()).length();
		for(int i=0; i < n-cn && n-cn>=0; i++)
		{
			str=str+"0";
		}
		str += ff.intValue();
		// Debug.d(TAG,"floatToFormatString str ="+str);
		return str;
	}
*/

	// H.M.Wang 2019-9-24 原来的实现未考虑负数的情况，导致f为负数时，变换结果不正确（如-1，变为000-1）
	public static String intToFormatString(int i, int n)
	{
		String format = "%0" + n + "d";
		return String.format(format, i);
	}
/*
	public static String intToFormatString(int f, int n)
	{
		String str = "";
		//Float ff = new Float(f);
		//Integer i = Integer.valueOf(ff.intValue());
		int cn = String.valueOf(f).length();
		for(int i=0; i < n-cn && n-cn>=0; i++)
		{
			str=str+"0";
		}
		str += f;
		// Debug.d(TAG,"intToFormatString str ="+str);
		return str;
	}
*/
	public static String boolToFormatString(boolean b, int n)
	{
		String str = "";
		for(int i=0; i < n-1; i++)
		{
			str=str+"0";
		}
		if(b==true)
			str += 1;
		else
			str += 0;
		// Debug.d(TAG,"boolToFormatString str ="+str);
		return str;
	}
	
	public void setIndex(int index)
	{
		mIndex = index;
	}

	public int getIndex()
	{
		return mIndex;
	}

	public void setOffset(int offset) {
		mOffset = offset;
	}

	public int getOffset() {
		return mOffset;
	}

	protected String getBinFileName() {
		return "/1.bin";
	}
	
	protected String getVarBinFileName() {
		return "/v" + mIndex + ".bin";
	}
	
	
	public byte[] getBufferFromContent()
	{
		int n=0;
		Debug.d(TAG, "===>getBufferFromContent id="+mId+", content="+mContent);
		if (mVBuffer == null) {
			return new byte[152];
		}
		int lenth=mVBuffer.get("0").length;
		ByteArrayBuffer buffer = new ByteArrayBuffer(mContent.length()*lenth);
		Debug.d(TAG, "--->Arraybuffer len="+buffer.length());
		for(int i=0;i<mContent.length(); i++){
			try {
				n = Integer.parseInt(mContent.substring(i, i + 1));
			} catch (Exception e) {
				n = 1;
			}
			byte[] b=mVBuffer.get(String.valueOf(n));
			buffer.append(b, 0, b.length);
		}
		Debug.d(TAG, "--->Arraybuffer len="+buffer.length());
		return buffer.buffer();
	}
	
	/**
	 * 根據系統設置參數17，確定每天的幾點開始才是新的一天
	 * @return
	 */
	protected long timeDelay() {
		long delay=0;
		int hold = SystemConfigFile.getInstance(mContext).getParam(SystemConfigFile.INDEX_DAY_START);
		Calendar c = Calendar.getInstance();
		if ( c.get(Calendar.HOUR_OF_DAY)*100 + c.get(Calendar.MINUTE) < hold) {
			delay = RealtimeObject.MS_DAY;
		}
		Debug.d(TAG, "--->delay:" + delay);
		return delay;
	}
	
	/**
	 * 设置当前object所在的Task
	 * @param task
	 */
	public void setTask(MessageTask task) {
		mTask = task;
		if (task == null || mTask == task) {
			return ;
		}
		PrinterNozzle type = mTask.getNozzle();
		// H.M.Wang 2020-1-25 Why??????????????????????
		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
			setHeight(152/2);
			mFont = MessageObject.mDotSizes[0];
		} else {
			setHeight(Configs.gDots);
		}

	}
	
	/**
	 * 当前object所在的Task
	 * @return MessageTask
	 */
	public MessageTask getTask() {
		Debug.d(TAG, "--->getTask: " + mTask);
		return mTask;
	}
	
	public boolean isNeedDraw() {
        return isNeedRedraw;
	}
	
	public float getProportion() {
		return 1;
	}

	// H.M.Wang 追加返回测量标准字符串的函数，如果子类没有定义该函数，则直接返回实际内容
	public String getMeatureString() {
		return mContent;
	}

	protected void meature() {
		Debug.d(TAG, "meature content = " + getMeatureString() + "; font = " + mFont);

		try {
			mPaint.setTypeface(FontCache.get(mContext, mFont));
		} catch (Exception e) {}
		mPaint.setTextSize(getfeed());

		// H.M.Wang 修改为测量标准字符串，而非实际内容
//		int width = (int) mPaint.measureText(getContent());
		int width = Math.round(mPaint.measureText(getMeatureString()));
		Debug.d(TAG, "meature: s=" + getMeatureString() + "; width=" + width + "; height=" + getfeed());

//		if (mHeight <= 4 * MessageObject.PIXELS_PER_MM) {
//			mWidth = width * 1.25f;
//		} else {
//			mWidth = width;
//		}
		mWidth = width;
		Debug.d(TAG, "meature mHeight = " + mHeight + "  mWidth = " + mWidth);
	}
	//////addbylk 		 
	public Bitmap getpreviewbmp()
	{
		return  Bitmap.createBitmap(10 , Configs.gDots, Configs.BITMAP_CONFIG);
	}
	public int getfeed() {
		return Math.round(mHeight);
	}
	
	
	
	/**
	 * ajust font library if print-head is either 16-dot or 32-dot
	 * if print-head 16-dot: font-4 is bound to 7dot object, font-7 is bound to 16dot object
	 * @return
	 */
	protected String verifyFont() {
		Debug.d(TAG, "--->verifyFont: " + mFont);
		PrinterNozzle type = mTask.getNozzle();
		String font = "";

// H.M.Wang 2020-8-8 在每个判断高度得if语句里面，准确计算的数值后面都+1，目的是为了解决在保存时，由于四舍五入会导致数值偏差，再次读入时会发生错误。如：
//	64点14x10的文本，其高度为33.25。保存时放大2倍保存四舍五入整数，结果就变成了67，带重新读入时，计算回来的高度就是33.5了。如果严格判断就不属于14x10字体了，而被错误的判断为16x12的字体了
//  其他的不准确值可以类推此现象。这里+1以后，不会造成判断失误，因为数值相差的都比较远，大于1，因此不会有不良影响
// End of 2020-8-8 在每个判断高度得if语句里面，准确计算的数值后面都+1

		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
// H.M.Wang 2020-4-15 追加"5x5"字体
			if (mHeight <= 152f * 5 / 16 + 1) {					// 47.5
				font = "5x5";
			} else if (mHeight <= 152f * 8 / 16 + 1) {			// 76
				font = "4";
// H.M.Wang 2020-1-30 追加"10x8", "12x9", "14x10"字体
//			} else {
            } else if(mHeight <= 152f * 10 / 16 + 1){			// 95
                font = "10";
            } else if(mHeight <= 152f * 12 / 16 + 1){			// 114
                font = "12";
            } else if(mHeight <= 152f * 14 / 16 + 1){			// 133
                font = "14";
            } else if(mHeight <= 152f * 16 / 16 + 1){			// 152
				if(mFont.equals("7L")) {
					font = "7L";
				} else if (mFont.equals("7R")) {
					font = "7R";
				} else {
					font = "7";
				}
			} else {
				font = mFont;
// End of H.M.Wang 2020-1-30 追加"10x8", "12x9", "14x10"字体
			}
// H.M.Wang 2020-7-23 追加32DN打印头
//		} else if (type == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-8-17 追加32SN打印头
//		} else if (type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_32DN) {
		} else if (type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_32DN || type == PrinterNozzle.MESSAGE_TYPE_32SN) {
// End of H.M.Wang 2020-8-17 追加32SN打印头
// End of H.M.Wang 2020-7-23 追加32DN打印头
			if (mHeight <= 152f * 5 / 32 + 1) {					// 23.75
				font = "5x5";
			} else if (mHeight <= 152f * 8 / 32 + 1) {			// 38
				font = "4";
// H.M.Wang 2020-1-30 追加"10x8", "12x9", "14x10"字体
            } else if(mHeight <= 152f * 10 / 32 + 1){			// 47.5
                font = "10";
            } else if(mHeight <= 152f * 12 / 32 + 1){			// 57
                font = "12";
            } else if(mHeight <= 152f * 14 / 32 + 1){			// 66.5
                font = "14";
            } else if(mHeight <= 152f * 16 / 32 + 1){			// 76
				if(mFont.equals("7L")) {
					font = "7L";
				} else if (mFont.equals("7R")) {
					font = "7R";
				} else {
					font = "7";
				}
// End of H.M.Wang 2020-1-30 追加"10x8", "12x9", "14x10"字体
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if(mHeight <= 152f * 19 / 32 + 1){			// 90.25
				font = "19";
			} else if(mHeight <= 152f * 21 / 32 + 1){			// 99.75
				font = "21";
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else {
				font = mFont;
			}

		// H.M.Wang 追加下列8行
// H.M.Wang 2020-8-26 追加64SN打印头
//        } else if (type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		} else if (type == PrinterNozzle.MESSAGE_TYPE_64_DOT | type == PrinterNozzle.MESSAGE_TYPE_64SN) {
// End of H.M.Wang 2020-8-26 追加64SN打印头
			if (mHeight <= 152f * 5 / 64 + 1) {					// 11.875
				font = "5x5";
			} else if (mHeight <= 152f * 8 / 64 + 1) {			// 19
				font = "4";
// H.M.Wang 2020-1-30 追加"10x8", "12x9", "14x10"字体
            } else if(mHeight <= 152f * 10 / 64 + 1){			// 23.75
                font = "10";
            } else if(mHeight <= 152f * 12 / 64 + 1){			// 28.5
                font = "12";
            } else if(mHeight <= 152f * 14 / 64 + 1){			// 33.25
                font = "14";
            } else if(mHeight <= 152f * 16 / 64 + 1){			// 38
				if(mFont.equals("7L")) {
					font = "7L";
				} else if (mFont.equals("7R")) {
					font = "7R";
				} else {
					font = "7";
				}
// End of H.M.Wang 2020-1-30 追加"10x8", "12x9", "14x10"字体
// H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else if(mHeight <= 152f * 19 / 64 + 1){			// 45.125
				font = "19";
			} else if(mHeight <= 152f * 21 / 64 + 1){			// 49.875
				font = "21";
// End of H.M.Wang 2020-5-29 追加"19x13", "21x14"字体
			} else {
				font = mFont;
			}
// End of H.M.Wang 2020-4-15 追加"5x5"字体
		} else {
			font = mFont;
		}
		Debug.d(TAG, "--->verifyFont: " + font);
		return font;
	}
	
	/**
	 * 判断是否可以修改字库，16点和32点的喷头目前是固定字库：7点对象 - 4.ttf / 16点对象 - 7.ttf
	 * @return
	 */
	public boolean fixedFont() {
		PrinterNozzle type = mTask.getNozzle();
		boolean isFixed = false;
		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT) {
			isFixed = true;
		}

		if (isFixed) {
			return isFixed;
		}
		String  height = getDisplayHeight();
		if (MessageObject.mDotSizes[0].equalsIgnoreCase(height) ||
				MessageObject.mDotSizes[1].equalsIgnoreCase(height) ||
				MessageObject.mDotSizes[2].equalsIgnoreCase(height) ||
				MessageObject.mDotSizes[3].equalsIgnoreCase(height) ||
				MessageObject.mDotSizes[4].equalsIgnoreCase(height) ||
				MessageObject.mDotSizes[5].equalsIgnoreCase(height) ||
				MessageObject.mDotSizes[6].equalsIgnoreCase(height)) {
			isFixed = true;
		}
		return isFixed;
	}

	/**
	 * 判断当前对象的宽高是否可修改
	 * @return
	 */
	public boolean whChangable() {
		MessageObject msg = mTask.getMsgObject();
		if (msg == null) {
			return false;
		}
		
		PrinterNozzle type = msg.getPNozzle();

		// H.M.Wang 修改下列2行
//		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT || type == PrinterNozzle.MESSAGE_TYPE_32_DOT) {
// H.M.Wang 2020-7-23 追加32DN打印头
//		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT || type == PrinterNozzle.MESSAGE_TYPE_32_DOT || type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
		if (type == PrinterNozzle.MESSAGE_TYPE_16_DOT ||
			type == PrinterNozzle.MESSAGE_TYPE_32_DOT ||
			type == PrinterNozzle.MESSAGE_TYPE_32DN ||
// H.M.Wang 2020-8-17 追加32SN打印头
			type == PrinterNozzle.MESSAGE_TYPE_32SN ||
// End of H.M.Wang 2020-8-17 追加32SN打印头
// H.M.Wang 2020-8-26 追加64SN打印头
            type == PrinterNozzle.MESSAGE_TYPE_64SN ||
// End of H.M.Wang 2020-8-26 追加64SN打印头
			type == PrinterNozzle.MESSAGE_TYPE_64_DOT) {
// End of H.M.Wang 2020-7-23 追加32DN打印头
			return false;
		} else {
			return true;
		}
	}

	/**
	 * judge if this object need vx.bin file
	 * @return
	 */
	public boolean needVBin() {
		if ((this instanceof CounterObject) ||
				(this instanceof RealtimeYear) ||
				(this instanceof RealtimeMonth) ||
				(this instanceof RealtimeDate) ||
				(this instanceof RealtimeHour) ||
				(this instanceof RealtimeMinute) ||
				(this instanceof JulianDayObject) ||
				(this instanceof WeekDayObject) ||
				(this instanceof WeekOfYearObject) ||
				(this instanceof WeeksObject) ||
				(this instanceof RealtimeSecond) ||
				(this instanceof LetterHourObject)) {
			return true;
		} else {
			return false;
		}
	}
}
