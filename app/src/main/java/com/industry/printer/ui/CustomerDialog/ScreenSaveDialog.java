package com.industry.printer.ui.CustomerDialog;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.view.View;
import android.view.Window;
import android.view.animation.Animation;
import android.widget.ImageView;

import com.industry.printer.R;
import com.industry.printer.Utils.Debug;

import java.util.Timer;
import java.util.TimerTask;

public class ScreenSaveDialog extends Dialog implements DialogInterface.OnDismissListener, View.OnClickListener {

    private static final int MESSAGE_TIMER = 101;

    private ImageView imageView;
    private Bitmap mBmp;
    private Animation mAnimation;
    private boolean flipped;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_TIMER:
                    flipped = !flipped;
                    showScreenSaver(flipped);
                    break;
                default:
                    break;
            }
        }
    };

    public ScreenSaveDialog(final Context context, Bitmap bitmap) {
        super(context, R.style.Dialog_Fullscreen);
        mBmp = bitmap;
        flipped = true;

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.layout_screen_save);

        imageView = (ImageView) findViewById(R.id.image);
//        imageView.setImageBitmap(mBmp);
        this.setOnDismissListener(this);
        imageView.setOnClickListener(this);

        showScreenSaver(flipped);

    }

    @Override
    public void onDismiss(DialogInterface dialog) {


        mHandler.removeMessages(MESSAGE_TIMER);
        if (mBmp != null && !mBmp.isRecycled()) {
            mBmp.recycle();
        }
        mBmp = null;
    }

    @Override
    public void onClick(View v) {
        this.dismiss();
    }

    private void showScreenSaver(boolean flip) {
        Matrix matrix = new Matrix();
        Debug.d("XXX", "--->showScreenSaver: " + flip + " -- width: " + mBmp.getWidth() + " height: " + mBmp.getHeight());
        int xScale = flip ? -1 : 1;
        matrix.postScale(xScale, 1);
        Bitmap bmp = Bitmap.createBitmap(mBmp, 0, 0, mBmp.getWidth(), mBmp.getHeight(), matrix, false);
        imageView.setImageBitmap(bmp);
        mHandler.sendEmptyMessageDelayed(MESSAGE_TIMER, 5*1000);
    }

}
