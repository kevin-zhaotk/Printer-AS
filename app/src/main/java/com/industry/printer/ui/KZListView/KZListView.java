package com.industry.printer.ui.KZListView;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.ListView;

import com.industry.printer.Utils.Debug;

/**
 *
 * @author 作者：易皇星
 *
 * @da2016年10月24日 时间：
 *
 * @toTODO 类描述： 解决 ListView中嵌套ScrollView，ScrollView拦截ListView的Item点击事件的解决办法
 *
 *
 *         在listview中嵌套ScrollView，发现横滑竖滑都正常，但是无法单击Listview的Item。查询Android分发机制后解决，继承Listview重写Listview的onInterceptTouchEvent。
 *
 *         onInterceptTouchEvent中总是调用listview的onTouchEvent保证listview的事件都执行，
 *         super.onInterceptTouchEvent(ev)不会拦截需要传递给ScrollView的横滑。
 */
public class KZListView extends ListView {

    private int flag = 0;

    private float StartX;

    private float StartY;

    public KZListView(Context context) {
        super(context);
        // TODO Auto-generated constructor stub
    }

    public KZListView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        // TODO Auto-generated constructor stub
    }

    public KZListView(Context context, AttributeSet attrs) {
        super(context, attrs);
        // TODO Auto-generated constructor stub
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent ev) {
        // 总是调用listview的touch事件处理
        onTouchEvent(ev);
        Debug.d("KZListView", "--->onInterceptTouchEvent: " + ev.getAction() + "  x = " + ev.getX()  + "  y = " + ev.getY() + "   flag = "  + flag);
        if (ev.getAction() == MotionEvent.ACTION_DOWN) {
            StartX = ev.getX();
            StartY = ev.getY();
            return false;
        }
        if (ev.getAction() == MotionEvent.ACTION_MOVE) {
            float ScollX = ev.getX() - StartX;
            float ScollY = ev.getY() - StartY;
            // 判断是横滑还是竖滑，竖滑的话拦截move事件和up事件（不拦截会由于listview和scrollview同时执行滑动卡顿）
            if (Math.abs(ScollX) < Math.abs(ScollY)) {
                flag = 1;
                return true;
            }
            return false;
        }
        if (ev.getAction() == MotionEvent.ACTION_UP) {
            if (flag == 1) {

                return true;
            }
            return false;
        }
        return super.onInterceptTouchEvent(ev);
    }

}
