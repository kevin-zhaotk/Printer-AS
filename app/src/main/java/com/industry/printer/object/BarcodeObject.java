package com.industry.printer.object;

import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;

import com.google.zxing.BarcodeFormat;
import com.google.zxing.EncodeHintType;
import com.google.zxing.MultiFormatWriter;
import com.google.zxing.WriterException;
import com.google.zxing.common.BitMatrix;
import com.google.zxing.datamatrix.DataMatrixWriter;
//import com.google.zxing.datamatrix.DataMatrixWriter;
import com.google.zxing.oned.EAN13Writer;
import com.google.zxing.qrcode.decoder.ErrorCorrectionLevel;
import com.google.zxing.qrcode.encoder.ByteMatrix;
import com.google.zxing.qrcode.encoder.Encoder;
import com.google.zxing.qrcode.encoder.QRCode;
import com.industry.printer.FileFormat.QRReader;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFileMaker;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.object.BaseObject;

import com.industry.printer.BinInfo;
import com.industry.printer.R;																																																																								
					
import android.R.bool;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.View.MeasureSpec;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import android.widget.TextView;


public class BarcodeObject extends BaseObject {

	private static final int QUIET_ZONE_SIZE = 4;
	private static final int STROKE_WIDTH = 3;
	
	public String mFormat;
	public int mCode;
	public boolean mShow;
	public int mTextSize;
	
	public Bitmap mBinmap;
	
	private Map<String, Integer> code_format;
	private Map<Integer, String> format_code;
	
	public BarcodeObject(Context context, float x) {
		super(context, BaseObject.OBJECT_TYPE_BARCODE, x);
		// TODO Auto-generated constructor stub
		mShow = true;
		mCode = 3;
		mTextSize = 20;
		mFormat="CODE_128";
		if (mSource == true) {
			setContent("123456789");
		} else {
			setContent("");
		}
		mWidth=0;
		
	}

	public void setCode(String code)
	{
		
		mId = BaseObject.OBJECT_TYPE_BARCODE;
		if ("EAN8".equals(code)) {
			mCode = 0;
		} else if ("EAN13".equals(code)) {
			mCode = 1;
		} else if ("EAN128".equals(code)) {
			mCode = 2;
		} else if ("CODE_128".equals(code)) {
			mCode = 3;
		} else if ("CODE_39".equals(code)) {
			mCode = 5;
		} else if ("ITF_14".equals(code)) {
			mCode = 6;
		} else if ("UPC_A".equals(code)) {
			mCode = 7;
		} else if ("QR".equals(code)) {
			mCode = 0;
			mId = BaseObject.OBJECT_TYPE_QR;
		} else if ("DM".equals(code)) {
			mCode = 8;
			mId = BaseObject.OBJECT_TYPE_QR;
		} else {
			return;
		}
		mFormat = code;
		isNeedRedraw = true;
	}
	
	public void setCode(int code)
	{
		if (code == 0) {
			mCode = 0;
			mFormat = "EAN8";
		} else if (code == 1) {
			mCode = 1;
			mFormat = "EAN13";
		} else if (code == 2) {
			mCode = 2;
			mFormat = "EAN128";
		} else if (code == 3) {
			mCode = 3;
			mFormat = "CODE_128";
		} else if (code == 5) {
			mCode = 5;
			mFormat = "CODE_39";
		} else if (code == 6) {
			mCode = 6;
			mFormat = "ITF_14";
		} else if (code == 7) {
			mCode = 7;
			mFormat = "UPC_A";
		} else if (code == 8) {
			mCode = 8;
			mFormat = "DM";
		}
		mId = BaseObject.OBJECT_TYPE_BARCODE;
		isNeedRedraw = true;
	}
	
	public String getCode()
	{
		return mFormat;
	}
	
	public boolean isQRCode() {
		return "QR".equals(mFormat);
	}
	
	public void setShow(boolean show)
	{
		mShow = show;
	}
	public boolean getShow()
	{
		return mShow;
	}
	
	public void setTextsize(int size) {
		if (size == mTextSize) {
			return;
		}
		if (size > 0 && size < 100) {
			mTextSize = size;
			isNeedRedraw = true;
		}
	}
	
	public int getTextsize() {
		return mTextSize;
	}
	@Override
	public void setContent(String content)
	{
		mContent=content;
		isNeedRedraw = true;
	}
	
	@Override
	public void setSource(boolean dynamic) {
		mSource  = dynamic;
		if (dynamic) {
			mName = mContext.getString(R.string.object_dynamic_qr);
			setContent("dynamic");
		} else {
			mName = mContext.getString(R.string.object_bar);
		}
		
	}
	
	@Override
	public String getTitle() {
		if (is2D()) {
			if (mSource) {
				mName = mContext.getString(R.string.object_dynamic_qr);
			} else {
				mName = mContext.getString(R.string.object_qr);
			}
		} else {
			mName = mFormat + " " + mContext.getString(R.string.object_bar);
		}
		return mName;
	}
	
	private static final String CODE = "utf-8"; 
	
	public Bitmap getScaledBitmap(Context context) {
		if (!isNeedRedraw) {
			return mBitmap; 			
		}
		
		isNeedRedraw = false;
		check();
		if (!is2D()) {
			if (mWidth <= 0) {
				mWidth = mContent.length() * 70;
			}
			mBitmap = draw(mContent, (int)mWidth, (int)mHeight);
		} else {
			mWidth = mHeight;
			if (mFormat.equalsIgnoreCase("DM") || mFormat.equalsIgnoreCase("DATA_MATRIX")) {
				mBitmap = drawDataMatrix(mContent, (int) mWidth, (int) mHeight);
			} else {
				mBitmap = drawQR(mContent, (int) mWidth, (int) mHeight);
			}
		}
		// mBitmap = draw(mContent, (int)mWidth, (int)mHeight);
		setWidth(mWidth);
		return mBitmap;
	}
	
	
	@Override
	public Bitmap getpreviewbmp() {
		if (mBinmap == null || mWidth == 0 || mHeight == 0) {
			getScaledBitmap(mContext);
		}
		return Bitmap.createScaledBitmap(mBitmap, (int) mWidth, (int) mHeight, false);
	}

	@Override
	public Bitmap makeBinBitmap(Context ctx, String content, int ctW, int ctH, String font) {
		if (is2D()) {
			if (mFormat.equalsIgnoreCase("DM") || mFormat.equalsIgnoreCase("DATA_MATRIX")) {
				return drawDataMatrix(content, ctW, ctH);
			} else {
				return drawQR(content, ctW, ctH);
			}
		} else {
			return draw(content, ctW, ctH);
		}
	}
	
	private Bitmap drawQR(String content, int w, int h) {
		try {
			BitMatrix matrix = encode(content, w, h, null);
			int tl[] = matrix.getTopLeftOnBit();
			int width = matrix.getWidth();
			int height = matrix.getHeight();
			Debug.d("BarcodeObject", "mWidth="+ w +", width="+width + "   height=" + height);
			int[] pixels = new int[width * height];
			for (int y = 0; y < height; y++) 
			{
				for (int x = 0; x < width; x++) 
				{
					if (matrix.get(x, y)) 
					{
						pixels[y * width + x] = mReverse ? 0xffffffff : 0xff000000;
					} else {
						pixels[y * width + x] = mReverse ? 0xff000000 : 0xffffffff;
					}
				}
			}
			/* 条码/二维码的四个边缘空出20像素作为白边 */
			Bitmap bitmap = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
			
			bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
			return bitmap;
		} catch (Exception e) {
		}
		return null;
	}

	private Bitmap drawDataMatrix(String content, int w, int h) {
		
		DataMatrixWriter writer = new DataMatrixWriter();
		
		BitMatrix matrix = writer.encode(content, getBarcodeFormat(mFormat), w, h);
		int width = matrix.getWidth();
		int height = matrix.getHeight();
		int[] pixels = new int[width * height];

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (matrix.get(x, y))
				{
					pixels[y * width + x] = mReverse ? 0xffffffff : 0xff000000;
				} else {
					pixels[y * width + x] = mReverse ? 0xff000000 : 0xffffffff;
				}
			}
		}
		/* 条码/二维码的四个边缘空出20像素作为白边 */
		Bitmap bitmap = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
//		Bitmap bitmap = Bitmap.createBitmap(w, h, Configs.BITMAP_CONFIG);
		bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
		return bitmap;
	}
	
	private Bitmap draw(String content, int w, int h) {
		BitMatrix matrix=null;
		int margin = 0;
//		if (h <= mTextSize) {
//			h = mTextSize + 10;
//		}
		int textH = (h * mTextSize) / 100;
		Paint paint = new Paint();
		Debug.d("BarcodeObject", "--->draw w : " + w + "  h: " + h + "  textH = " + textH);
		try {
			MultiFormatWriter writer = new MultiFormatWriter();
			Hashtable<EncodeHintType, String> hints = new Hashtable<EncodeHintType, String>();  
            hints.put(EncodeHintType.CHARACTER_SET, CODE);
            // hints.put(EncodeHintType.MARGIN, margin);
            BarcodeFormat format = getBarcodeFormat(mFormat);
            
            Debug.d(TAG, "--->content: " + mContent + "   format:" + mFormat);
            /* 条形码的宽度设置:每个数字占70pix列  */
/*			if ("EAN13".equals(mFormat)) {
				content = checkSum();
			} else if ("EAN8".equals(mFormat)) {
				content = checkLen(8);
			} else if ("ITF_14".equals(mFormat)) {
				content = checkLen(14);
			} else if ("UPC_A".equals(mFormat)) {
				content = checkLen(11,12);
			}
*/			
            content = check();
            matrix = writer.encode(content,
				        format, w, h - textH- 5, null);

			int tl[] = matrix.getTopLeftOnBit();
			int width = matrix.getWidth();
			int height = matrix.getHeight();
			Debug.d("BarcodeObject", "mWidth="+ w +", width="+width + "   height=" + height);
			// setWidth(width);
			int[] pixels = new int[width * height];
			for (int y = 0; y < height; y++) 
			{
				for (int x = 0; x < width; x++) 
				{
					if (matrix.get(x, y)) 
					{
						pixels[y * width + x] = 0xff000000;
					} else {
						pixels[y * width + x] = 0xffffffff;
					}
				}
			}
			/* 条码/二维码的四个边缘空出20像素作为白边 */
			Bitmap bitmap = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
			bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
			
			if ("ITF_14".equals(mFormat)) {
				Canvas cvs = new Canvas(bitmap);
				paint.setStrokeWidth(STROKE_WIDTH * 2);
				cvs.drawLine(/* top */  0, STROKE_WIDTH, bitmap.getWidth(), STROKE_WIDTH, paint);
				cvs.drawLine(/* left */ STROKE_WIDTH, 0, STROKE_WIDTH, bitmap.getHeight(), paint);
				cvs.drawLine(/* right */bitmap.getWidth() - STROKE_WIDTH, 0, bitmap.getWidth() - STROKE_WIDTH, bitmap.getHeight(), paint);
				cvs.drawLine(/* bottom*/0, bitmap.getHeight() - STROKE_WIDTH,bitmap.getWidth(), bitmap.getHeight() - STROKE_WIDTH, paint);
			}
			if(mShow)
			{
				// 用於生成bin的bitmap
				Bitmap bmp = Bitmap.createBitmap(width, h, Configs.BITMAP_CONFIG);
				Bitmap code = createCodeBitmapFromDraw(content, width-tl[0]*2, textH + 5);
				Debug.d(TAG, "===>code width=" + code.getWidth());
				//BinCreater.saveBitmap(code, "barcode.png");
				Canvas can = new Canvas(bmp);
				can.drawBitmap(bitmap, 0, 0, paint);
				can.drawBitmap(code, tl[0], height, paint);
				BinFromBitmap.recyleBitmap(bitmap);
				BinFromBitmap.recyleBitmap(code);
				bitmap = bmp;
			}
			
			return bitmap;

		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	public BitMatrix encode(String contents, int width, int height, Map<EncodeHintType, ?> hints)
            throws WriterException {

        if (contents.isEmpty()) {
            throw new IllegalArgumentException("Found empty contents");
        }

        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Requested dimensions are too small: " + width + 'x' + height);
        }

        ErrorCorrectionLevel errorCorrectionLevel = ErrorCorrectionLevel.L;
        int quietZone = QUIET_ZONE_SIZE;
        if (hints != null) {
            ErrorCorrectionLevel requestedECLevel = (ErrorCorrectionLevel) hints.get(EncodeHintType.ERROR_CORRECTION);
            if (requestedECLevel != null) {
                errorCorrectionLevel = requestedECLevel;
            }
            Integer quietZoneInt = (Integer) hints.get(EncodeHintType.MARGIN);
            if (quietZoneInt != null) {
                quietZone = quietZoneInt;
            }
        }
        try {
        	contents = new String(contents.getBytes("UTF-8"), "ISO-8859-1");
        } catch (UnsupportedEncodingException e) {
			Debug.d(TAG, "--->e: " + e.getMessage());
		}
        QRCode code = Encoder.encode(contents, errorCorrectionLevel, hints);
        return renderResult(code, width, height, quietZone);
    }

    private BitMatrix drasDM() {
		return null;
	}
    // Note that the input matrix uses 0 == white, 1 == black, while the output
    // matrix uses
    // 0 == black, 255 == white (i.e. an 8 bit greyscale bitmap).
    //修改如下代码
    private static BitMatrix renderResult(QRCode code, int width, int height, int quietZone) {
        ByteMatrix input = code.getMatrix();
        if (input == null) {
            throw new IllegalStateException();
        }
        int inputWidth = input.getWidth();
        int inputHeight = input.getHeight();
        int qrWidth = inputWidth ;
        int qrHeight = inputHeight;
        int outputWidth = Math.max(width, qrWidth);
        int outputHeight = Math.max(height, qrHeight);

        int multiple = Math.min(outputWidth / qrWidth, outputHeight / qrHeight);
        // Padding includes both the quiet zone and the extra white pixels to
        // accommodate the requested
        // dimensions. For example, if input is 25x25 the QR will be 33x33
        // including the quiet zone.
        // If the requested size is 200x160, the multiple will be 4, for a QR of
        // 132x132. These will
        // handle all the padding from 100x100 (the actual QR) up to 200x160.

        int leftPadding = (outputWidth - (inputWidth * multiple)) / 2;
        int topPadding = (outputHeight - (inputHeight * multiple)) / 2;

        if(leftPadding >= 0 ) {
            outputWidth = outputWidth - 2 * leftPadding ;
            leftPadding = 0;
        }
        if(topPadding >= 0) {
            outputHeight = outputHeight - 2 * topPadding;
            topPadding = 0;
        }

        BitMatrix output = new BitMatrix(outputWidth, outputHeight);

        for (int inputY = 0, outputY = topPadding; inputY < inputHeight; inputY++, outputY += multiple) {
            // Write the contents of this row of the barcode
            for (int inputX = 0, outputX = leftPadding; inputX < inputWidth; inputX++, outputX += multiple) {
                if (input.get(inputX, inputY) == 1) {
                    output.setRegion(outputX, outputY, multiple, multiple);
                }
            }
        }
        return output;
    }
	
	public Bitmap getPrintBitmap(int totalW, int totalH, int w, int h, int y) {
//		BitMatrix matrix=null;
//		Debug.d(TAG, "--->getPrintBitmap : totalW = " + totalW + "  w = " + w);
//		MultiFormatWriter writer = new MultiFormatWriter();
//		Hashtable<EncodeHintType, String> hints = new Hashtable<EncodeHintType, String>();  
//        hints.put(EncodeHintType.CHARACTER_SET, CODE);
//        try {
//			matrix = writer.encode(mContent,
//					BarcodeFormat.QR_CODE, w, w, hints);
//			matrix = deleteWhite(matrix);
//        } catch (Exception e) {
//        	return null;
//        }
//		int width = matrix.getWidth();
//		int height = matrix.getHeight();
//		int[] pixels = new int[width * height];
//		for (int y1 = 0; y1 < height; y1++) 
//		{
//			for (int x = 0; x < width; x++) 
//			{
//				if (matrix.get(x, y1)) 
//				{
//					pixels[y1 * width + x] = 0xff000000;
//				} else {
//					pixels[y1 * width + x] = 0xffffffff;
//				}
//			}
//		}
		Bitmap bg = Bitmap.createBitmap(totalW, totalH, Configs.BITMAP_CONFIG);
		Canvas canvas = new Canvas(bg);
//		Bitmap bitmap = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
		
//		bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
		Debug.d(TAG, "--->mFormat: " + mFormat);
		Bitmap bitmap = null;
		if ("QR".equalsIgnoreCase(mFormat)) {
			bitmap = drawQR(mContent, w, w);
		} else {
			bitmap = drawDataMatrix(mContent, w, w);
		}
		canvas.drawColor(Color.WHITE);
		canvas.drawBitmap(Bitmap.createScaledBitmap(bitmap, w, h, true), 0, y, mPaint);
		return bg;
	}
	
	public int[] getDotcount() {
		Bitmap bmp = getScaledBitmap(mContext);
		BinFileMaker maker = new BinFileMaker(mContext);
		int[] dots = maker.extract(bmp, mTask.getHeads());
		return dots;
	}

	protected Bitmap createCodeBitmapFromTextView(String contents,int width,int height, boolean isBin) {
		int heads = mTask.getHeads();
		if (heads == 0) {
			heads = 1;
		}
		float div = (float) (4.0/heads);
		Debug.d(TAG, "===>width=" + width);
		width = (int) (width/div);
		TextView tv=new TextView(mContext);
	    LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
        tv.setLayoutParams(layoutParams);
        tv.setText(contents);
        tv.setTextSize(15);
        tv.setHeight(height);
        tv.setGravity(Gravity.CENTER_HORIZONTAL);
        tv.setWidth(width);
        tv.setDrawingCacheEnabled(true);  
        tv.setTextColor(Color.BLACK);
        tv.measure(  
                MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED),  
                MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED));  
        tv.layout(0, 0, tv.getMeasuredWidth(),  
        		tv.getMeasuredHeight());
  
        tv.buildDrawingCache();  
        Bitmap bitmapCode=tv.getDrawingCache();
        Debug.d(TAG, "===>width=" + width + ", bmp width=" + bitmapCode.getWidth());
        return isBin?Bitmap.createScaledBitmap(bitmapCode, (int) (bitmapCode.getWidth()*div), bitmapCode.getHeight(), true) : bitmapCode;
	}
	
	protected Bitmap createCodeBitmapFromDraw(String content, int width, int height) {
		if (TextUtils.isEmpty(content)) {
			return null;
		}
		Paint paint = new Paint(); 
		
		paint.setTextSize(height - 5);
		paint.setTextScaleX(2);
		paint.setColor(Color.BLACK);
		Bitmap bitmap = Bitmap.createBitmap(width, height, Configs.BITMAP_CONFIG);
		Canvas canvas = new Canvas(bitmap);
		//每个字符占的宽度
		int perPix = width/content.length();
		//字符本身的宽度
		float numWid = paint.measureText("0");
		int left = (int) ((perPix - numWid)/2);
		for (int i = 0; i < content.length(); i++) {
			String n = content.substring(i, i+1);
			canvas.drawText(n, i*perPix + left, paint.getTextSize(), paint);
		}
		return bitmap;
	}
	
	public int getBestWidth()
	{
		int width=0;
		BitMatrix matrix=null;
		try{
			MultiFormatWriter writer = new MultiFormatWriter();
			BarcodeFormat format = getBarcodeFormat(mFormat);
			if(is2D())
			{
				Hashtable<EncodeHintType, String> hints = new Hashtable<EncodeHintType, String>();  
	            hints.put(EncodeHintType.CHARACTER_SET, CODE);
	            
				matrix = writer.encode(mContent,
					                format, (int)mWidth, (int)mHeight, null);
			} else {
				matrix = writer.encode(mContent,
		                format, (int)mWidth, (int)mHeight);
			}
			width = matrix.getWidth();
			int height = matrix.getHeight();
			Debug.d(TAG, "mWidth="+mWidth+", width="+width);
		}
		catch(Exception e)
		{
			Debug.e(TAG, "exception:"+e.getMessage());
		}
		return width;
	}
	
	private boolean is2D() {
		Debug.d(TAG, "is2D? " + mFormat);
		if (mFormat.equalsIgnoreCase("QR")
				|| mFormat.equalsIgnoreCase("DATA_MATRIX")
				|| mFormat.equalsIgnoreCase("DM")
				|| mFormat.equalsIgnoreCase("AZTEC")
				|| mFormat.equalsIgnoreCase("PDF_417")) {
			return true;
		}
		return false;
	}
	
	private BarcodeFormat getBarcodeFormat(String format) {
		int i;
		if ("CODE_128".equals(format)) {
			return BarcodeFormat.CODE_128;
		} else if ("CODE_39".equals(format)) {
			return BarcodeFormat.CODE_39;
		} else if ("CODE_93".equals(format)) {
			return BarcodeFormat.CODE_93;
		} else if ("CODABAR".equals(format)) {
			return BarcodeFormat.CODABAR;
		} else if ("EAN8".equals(format)) {
			return BarcodeFormat.EAN_8;
		} else if ("EAN13".equals(format)) {
			return BarcodeFormat.EAN_13;
		} else if ("UPC_E".equals(format)) {
			return BarcodeFormat.UPC_E;
		} else if ("UPC_A".equals(format)) {
			return BarcodeFormat.UPC_A;
		} else if ("ITF_14".equals(format)) {
			return BarcodeFormat.ITF;
		} else if ("RSS14".equals(format)) {
			return BarcodeFormat.RSS_14;
		} else if ("RSS_EXPANDED".equals(format)) {
			return BarcodeFormat.RSS_EXPANDED;
		} else if ("QR".equals(format)) {
			return BarcodeFormat.QR_CODE;
		} else if ("DATA_MATRIX".equals(format)) {
			return BarcodeFormat.DATA_MATRIX;
		} else if ("AZTEC".equals(format)) {
			return BarcodeFormat.AZTEC;
		} else if ("PDF_417".equals(format)) {
			return BarcodeFormat.PDF_417;
		} else if ("DM".equalsIgnoreCase(format)) {
			return BarcodeFormat.DATA_MATRIX;
		} else {
			return BarcodeFormat.CODE_128;
		}
		
	}
	
	
	private static BitMatrix deleteWhite(BitMatrix matrix) {
        int[] rec = matrix.getEnclosingRectangle();
        int resWidth = rec[2] + 1;
        int resHeight = rec[3] + 1;
        Debug.d("BarcodeObject", "--->deleteWhite: " + resWidth + " " + resHeight );
        BitMatrix resMatrix = new BitMatrix(resWidth, resHeight);
        resMatrix.clear();
        for (int i = 0; i < resWidth; i++) {
            for (int j = 0; j < resHeight; j++) {
                if (matrix.get(i + rec[0], j + rec[1]))
                    resMatrix.set(i, j);
            }
        }
        return resMatrix;
    }
	
	
	
	/**
	 * 計算EAN13的校驗和
	 * 奇数位和：6 + 0 + 2 + 4 + 6 + 8 = 26
	 * 偶数位和：9 + 1 + 3 + 5 + 7 + 9 = 34
	 * 将奇数位和与偶数位和的三倍相加：26 + 34 * 3 = 128
	 * 取结果的个位数：128的个位数为8
	 * 用10减去这个个位数：10 - 8 = 2
	 * @return
	 */
	private String checkSum() {
		String code = "";
		int odd = 0, even = 0;
		if (mContent.length() < 12) {
			String add = "";
			for (int i = 0; i < 12-mContent.length(); i++) {
				add += "0";
			}
			code = mContent + add;
		} else if (mContent.length() > 12) {
			code = mContent.substring(0, 12);
		} else {
			code = mContent; 
		}
		Debug.d(TAG, "--->content: " + mContent);
		mContent = code;
		Debug.d(TAG, "--->code: " + code);
		for (int i = 0; i < code.length(); i++) {
			try {
				if (i%2 == 0) {
					odd += Integer.parseInt(code.substring(i, i+1));
				} else {
					even += Integer.parseInt(code.substring(i, i+1));
				}
			} catch (Exception e){
				Debug.e(TAG, "--->" + e.getMessage());
			}
		}
		int temp = odd + even * 3;
		int sum = 10 - temp%10;
		if (sum >= 10) {
			sum = 0;
		}
		Debug.d(TAG, "--->sum: " + sum);
		code += sum;
		Debug.d(TAG, "--->code: " + code);
		return code;
	}
	
	private String check() {
		String content = mContent;
		if ("EAN13".equals(mFormat)) {
			content = checkSum();
		} else if ("EAN8".equals(mFormat)) {
			content = checkLen(8);
		} else if ("ITF_14".equals(mFormat)) {
			content = checkLen(14);
		} else if ("UPC_A".equals(mFormat)) {
			content = checkLen(11,12);
		} else {
			
		}
		mContent = content;
		return mContent;
	}
	
	/**
	 * EAN8只支持8位長度
	 * @return
	 */
	private String checkLen(int dstLen) {
		int len = mContent.length();
		if (len < dstLen) {
			for (int i = 0; i < dstLen - len; i++) {
				mContent += "0";
			}
		} else if (mContent.length() > dstLen) {
			return mContent.substring(0, dstLen);
		}
		return mContent;
	}
	
	private String checkLen(int min, int max) {
		int len = mContent.length();
		if (len < min) {
			for (int i = 0; i < min - len; i++) {
				mContent += "0";
			}
		} else if (len > max) {
			mContent = mContent.substring(0, max - 1);
		}
		return mContent;
	}
	
	public String toString()
	{
		int dots = SystemConfigFile.getInstance(mContext).getParam(39);
		float prop = dots/Configs.gDots;
		
		StringBuilder builder = new StringBuilder(mId);
		if (BaseObject.OBJECT_TYPE_QR.equalsIgnoreCase(mId)) {
			builder.append("^")
				.append(BaseObject.floatToFormatString(getX()*2*prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getY()*2*prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getXEnd()*2*prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getYEnd()*2*prop, 5))
				.append("^")
				.append(BaseObject.intToFormatString(0, 1))
				.append("^")
				.append(BaseObject.boolToFormatString(mDragable, 3))
				.append("^")
				.append("000^")
				.append("DM".equalsIgnoreCase(mFormat) ? "001" : "000")
				.append("^000^")
				.append(BaseObject.boolToFormatString(mReverse, 3))
				.append("^")
				.append("000")
				.append("^")
				.append(BaseObject.boolToFormatString(mSource, 8))
				.append("^")
				.append("00000000^00000000^00000000^00000000^00000000^00000000^00000000^")
				.append(mContent);
		} else {
			builder.append("^")
			.append(BaseObject.floatToFormatString(getX()*2*prop, 5))
			.append("^")
			.append(BaseObject.floatToFormatString(getY()*2*prop, 5))
			.append("^")
			.append(BaseObject.floatToFormatString(getXEnd()*2*prop, 5))
			.append("^")
			.append(BaseObject.floatToFormatString(getYEnd()*2*prop, 5))
			.append("^")
			.append(BaseObject.intToFormatString(0, 1))
			.append("^")
			.append(BaseObject.boolToFormatString(mDragable, 3))
			.append("^")
			.append(BaseObject.floatToFormatString(mContent.length(), 3))
			.append("^")
			.append(mCode)
			.append("^")
			.append("000^")
			.append(BaseObject.boolToFormatString(mShow, 3))
			.append("^")
			.append(mContent)
			.append("^")
			.append(BaseObject.boolToFormatString(mSource, 8))
			.append("^")
			.append("00000000^00000000^00000000^0000^0000^")
			.append(mFont)
			.append("^000^")
			.append(BaseObject.intToFormatString(mTextSize, 3));
		}
		
		String str = builder.toString();
		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		if (BaseObject.OBJECT_TYPE_QR.equalsIgnoreCase(mId)) {
//			str += mId+"^";
//			str += BaseObject.floatToFormatString(getX()*2*prop, 5)+"^";
//			str += BaseObject.floatToFormatString(getY()*2*prop, 5)+"^";
//			str += BaseObject.floatToFormatString(getXEnd()*2*prop, 5)+"^";
//			str += BaseObject.floatToFormatString(getYEnd()*2*prop, 5)+"^";
//			str += BaseObject.intToFormatString(0, 1)+"^";
//			str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//			str += "000^000^000^000^000^";
//			str += BaseObject.boolToFormatString(mSource, 8) + "^";
//			str += "00000000^00000000^00000000^00000000^00000000^00000000^00000000" + "^";
//			str += mContent;
//		} else {
//			str += mId+"^";
//			str += BaseObject.floatToFormatString(getX()*2*prop, 5)+"^";
//			str += BaseObject.floatToFormatString(getY()*2*prop, 5)+"^";
//			str += BaseObject.floatToFormatString(getXEnd()*2*prop, 5)+"^";
//			str += BaseObject.floatToFormatString(getYEnd()*2*prop, 5)+"^";
//			str += BaseObject.intToFormatString(0, 1)+"^";
//			str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//			str += BaseObject.floatToFormatString(mContent.length(), 3)+"^";
//			str += mCode +"^";
//			str += "000^";
//			str += BaseObject.boolToFormatString(mShow, 3)+"^";
//			str += mContent+"^";
//			str += BaseObject.boolToFormatString(mSource, 8) + "^";
//			str += "00000000^00000000^00000000^0000^0000^" + mFont + "^000" + "^";
//			str += BaseObject.intToFormatString(mTextSize, 3);
//		}
		System.out.println("file string ["+str+"]");
		return str;
	}
}
