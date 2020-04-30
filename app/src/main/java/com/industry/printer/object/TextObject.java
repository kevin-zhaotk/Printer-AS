package com.industry.printer.object;

import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.Configs;
import com.industry.printer.cache.FontCache;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;

public class TextObject extends BaseObject {
	private static final String TAG = TextObject.class.getSimpleName();

	// H.M.Wang 追加时间对象的所属信息
//	public BaseObject mParent;

	public TextObject(Context context, float x) {
		super( context, BaseObject.OBJECT_TYPE_TEXT, x);
//		mParent = null;
	}

	public TextObject(Context context, BaseObject parent, float x) {
		this(context, x);
		mParent = parent;
	}

	@Override
	public void setContent(String content) {
// H.M.Wang 2020-4-20 在内容发生变化的情况下，设置新的内容，并且重新计算宽度，是保持比例
		if(mContent != null && !mContent.equals(content)) {
			super.setContent(content);
			meature();
		}
// End of H.M.Wang 2020-4-20 在内容发生变化的情况下，设置新的内容，并且重新计算宽度，是保持比例
// H.M.Wang 2020-4-19 不重新计算就会出现即使字数发生变化，总宽度不变的问题（如果原来是5个字，修改为1个字的话，后面的1个字会占原来的5个字的宽度
// 如果重新计算，如果对高度进行过手动调整，但宽度不调整的话，字数发生变化后，宽度也会按着高度的比率发生变化
// 总而言之，这里原来的逻辑有缺陷，最好的办法是再次确认需求之后，重新定义这里的逻辑，否则，逻辑互相牵制，无法达到最佳状态
//		meature();

		// H.M.Wang取消宽度归零的设置。如果有这行，用户对高度进行缩小以后，设置属性重新计算宽度，此时宽度也会变小
//		mWidth = 0;
	}
/*
	// H.M.Wang 修改。取消原来的子元素均等加减1的缩放方法，改为均等缩放
	public void wide() {
		float ratio = (getWidth() + 5) / getWidth();
		mRatio *= ratio;

		setWidth(getWidth()*ratio);
		isNeedRedraw = true;
	}

	// H.M.Wang 修改。取消原来的子元素均等加减1的缩放方法，改为均等缩放
	public void narrow() {
		float ratio = (getWidth() - 5) / getWidth();
		mRatio *= ratio;

        setWidth(getWidth()*ratio);
		isNeedRedraw = true;
	}
*/
	public String toString()
	{
		float prop = getProportion();
		StringBuilder builder = new StringBuilder(mId);
		
		builder.append("^")
				.append(BaseObject.floatToFormatString(getX()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getY()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getXEnd()*2 * prop, 5))
				.append("^")
				.append(BaseObject.floatToFormatString(getYEnd()*2 * prop, 5))
				.append("^")
				.append(BaseObject.intToFormatString(0, 1))
				.append("^")
				.append(BaseObject.boolToFormatString(mDragable, 3))
				.append("^")
				.append(BaseObject.intToFormatString(mContent.length(), 3))
				.append("^")
// H.M.Wang 2019-9-24 追加所属信息
				.append("000^000^000^000^00000000^00000000^00000000^00000000^" + (mParent == null ? "0000" : String.format("%03d", mParent.mIndex)) + "^0000^")
//				.append("000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^")
				.append(mFont)
				.append("^000^")
				.append(mContent);
				
		String str = builder.toString();
//		String str="";
//		//str += BaseObject.intToFormatString(mIndex, 3)+"^";
//		str += mId+"^";
//		str += BaseObject.floatToFormatString(getX()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getY()*2 * prop, 5)+"^";
//		str += BaseObject.floatToFormatString(getXEnd()*2 * prop, 5)+"^";
//		//str += BaseObject.floatToFormatString(getY() + (getYEnd()-getY())*2, 5)+"^";
//		str += BaseObject.floatToFormatString(getYEnd()*2 * prop, 5)+"^";
//		str += BaseObject.intToFormatString(0, 1)+"^";
//		str += BaseObject.boolToFormatString(mDragable, 3)+"^";
//		str += BaseObject.intToFormatString(mContent.length(), 3)+"^";
//		str += "000^000^000^000^00000000^00000000^00000000^00000000^0000^0000^" + mFont + "^000^"+mContent;
		Debug.d(TAG, "toString = [" + str + "]");
//		System.out.println("file string ["+str+"]");
		return str;
	}
//////addbylk 
	@Override	 
	public Bitmap getpreviewbmp()
	{	Debug.e(TAG, "1===== " + getContent() );
		Bitmap bitmap;
		mPaint.setTextSize(getfeed());
		mPaint.setAntiAlias(true);   
		mPaint.setFilterBitmap(true); 
	
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
		
		int width = (int)mPaint.measureText(getContent());

		Debug.d(TAG, "--->content: " + getContent() + "  width=" + width);
		if (mWidth == 0) {
			setWidth(width);
		}
		Debug.d(TAG, "2===== " + getContent() );
		bitmap = Bitmap.createBitmap(width , (int)mHeight, Configs.BITMAP_PRE_CONFIG);
		Debug.d(TAG,"--->getBitmap width="+mWidth+", mHeight="+mHeight);
		mCan = new Canvas(bitmap);
		FontMetrics fm = mPaint.getFontMetrics();
		Debug.d(TAG, "3===== " + getContent() );
		
				
		mCan.drawText(mContent , 0, mHeight-fm.descent, mPaint);
	
		return Bitmap.createScaledBitmap(bitmap, (int)mWidth, (int)mHeight, false);	
	}	
}
