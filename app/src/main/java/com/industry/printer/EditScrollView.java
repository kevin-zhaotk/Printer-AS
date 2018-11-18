package com.industry.printer;

import com.industry.printer.Utils.Debug;
import com.industry.printer.data.BinCreater;
import com.industry.printer.data.BinFromBitmap;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.MessageObject;

import android.R.color;
import android.R.integer;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Paint.Style;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.widget.HorizontalScrollView;
import android.widget.ScrollView;

public class EditScrollView extends View {
	
	public static final String TAG="EditScrollView";
	
	private int mScreenW;
	private int mScreenH;
	public Paint p;
	public HorizontalScrollView mParent;
	public Context mContext;
	public MessageTask mTask;
	/* 防止鍵盤彈出時刷新界面，導致文本寬度被設定爲默認寬度 */
	private boolean needDraw = false;
	
	public EditScrollView(Context context) {
		super(context);
		mContext = context;
		// TODO Auto-generated constructor stub
		p = new Paint();
		mParent= (HorizontalScrollView) this.getParent();
		getWindowPixels();
		Debug.d(TAG, "==>EditScrollView 1");
	}
	
	public EditScrollView(Context context, AttributeSet attrs)
	{
		super(context, attrs);
		mContext = context;
		//mParent= (HorizontalScrollView) this.getParent().getParent();
		p = new Paint();
		getWindowPixels();
		Debug.d(TAG, "==>EditScrollView 2");
	}
	public EditScrollView(Context context, AttributeSet attrs, int defStyle)
	{
		super(context, attrs,defStyle);
		//mParent= (HorizontalScrollView) this.getParent().getParent();
		p = new Paint();
		getWindowPixels();
		Debug.d(TAG, "==>EditScrollView 3");
	}
	@Override  
	protected void onDraw(Canvas canvas) {
		Debug.d(TAG, "====>onDraw needDraw = " + needDraw);
		if (!needDraw) {
			return;
		}
		needDraw = false;
		int scrollx = 0;
		if (mParent != null) {
			scrollx = mParent.getScrollX();
		}
		MessageObject msgObject = mTask.getMsgObject();
		if (msgObject != null) {
			p.setColor(Color.GRAY);
			int type = msgObject.getType();
			switch (type) {
			case 0:
			case 1:
				break;
			case 2:
			case 4:
				canvas.drawLine(0, 76, 5000, 76, p);
				break;
			case 5:
				canvas.drawLine(0, 50, 5000, 50, p);
				canvas.drawLine(0, 101, 5000, 101, p);
				break;
			case 6:
				canvas.drawLine(0, 38, 5000, 38, p);
				canvas.drawLine(0, 76, 5000, 76, p);
				canvas.drawLine(0, 114, 5000, 114, p);
				break;
			default:
				break;
			}
		}
		// p.setColor(0x000000);
		
		Debug.d(TAG, "--->scrollx: " + scrollx + ",  mScreenW: " + mScreenW);
		for(BaseObject obj : mTask.getObjects())
		{
			Debug.d(TAG, "index=" + obj.getIndex() + "  c: " + obj.getContent());
			/* 只有当cursor选中时才显示十字标线  */
			if(obj instanceof MessageObject) {
				if (!obj.getSelected()) {
					continue;
				}
				
				float[] points = {
					/* 画水平线 */
					0, obj.getY(), 5000, obj.getY(),
					/* 画垂直线 */
					obj.getX(), 0, obj.getX(), 280
				};
				p.setColor(Color.BLUE);
				p.setStrokeWidth(1);
				canvas.drawLines(points, p);
				continue;
			} 
			/* 不在显示区域内的对象可以不画，优化效率  */
//			if ((obj.getXEnd() < getScrollX()) || (obj.getX() > getScrollX() + mScreenW)) {
//				Debug.d(TAG, "index=" + obj.getIndex() + "  c: " + obj.getContent() + "  x=" + obj.getX() + " end=" + obj.getXEnd());
//				Debug.d(TAG, "scrollx=" + getScrollX());
//				continue;
//			}
			if(mContext == null)
				Debug.d(TAG, "$$$$$$$$$$context=null");
			
			Bitmap bitmap = obj.getScaledBitmap(mContext);
			if (bitmap == null) {
				Debug.d(TAG, "--->obj: " + obj.getContent());
				continue;
			}
			// canvas.drawBitmap(bitmap, obj.getX(), obj.getY(), p);
			dispImg(canvas, bitmap, obj.getX(), obj.getY());
			if (obj.getSelected()) {
				p.setStyle(Style.STROKE);
				canvas.drawRect(new RectF(obj.getX(), obj.getY(), obj.getXEnd(), obj.getYEnd()), p);
			}
			
		}
		// p.setColor(Color.BLACK);
		// canvas.drawLine(0, 153, getW(), 153, p);
		Debug.d(TAG, "<<<==onDraw");
		 //mParent.fling(100);
	} 
	
	private void dispImg(Canvas canvas, Bitmap bmp, float x, float y) {
		int end = 0, cutWidth = 0;
		for (;;) {
			if (end + 2000 > bmp.getWidth()) {
				cutWidth = bmp.getWidth() - end;
			} else {
				cutWidth =1200;
			}
			Bitmap child = Bitmap.createBitmap(bmp, end, 0, cutWidth, bmp.getHeight());
			canvas.drawBitmap(child, x+end, y, p);
			end += cutWidth;
			if (end >= bmp.getWidth() - 1) {
				break;
			}
		}
		
	}
	
	public void setParent(View view) {
		mParent = (HorizontalScrollView) view;
	}
	
	public void setTask(MessageTask task) {
		mTask = task;
	}

	private int getW() {
		DisplayMetrics metrics = getResources().getDisplayMetrics();
		float density = metrics.density;
		return (int) (5000 * density);
	}
	
	public void getWindowPixels() {
		DisplayMetrics metrics = getResources().getDisplayMetrics();
		float density = metrics.density;
		float w = metrics.widthPixels;
		float h = metrics.heightPixels;
		
		// mScreenW = (int) (w * density + 0.5f);
		// mScreenH = (int) (h * density + 0.5f);
		mScreenW = (int) (w + 0.5f);
	}
	
	public void beginDraw() {
		needDraw = true;
	}
	
	public void endDraw() {
		needDraw = false;
	}
}
