package com.industry.printer.ui.CustomerDialog;

import com.industry.printer.R;
import com.industry.printer.ui.CustomerAdapter.SettingsListAdapter;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;

/**
 * Created by hmwan on 2020/4/24.
 */

public class EncoderPPREditDialog extends Dialog implements android.view.View.OnClickListener {
    private static final String TAG = CounterEditDialog.class.getSimpleName();

    private Context                 mContext;

    private EditText                mEncoderPPREdit;

    private TextView                mConfirm;
    private TextView                mCancel;

    private Handler                 mHandler;
    private String                  mValue;

    public EncoderPPREditDialog(Context context, Handler handler, String value) {
        super(context, R.style.Dialog_Fullscreen);
        mContext = context;
        mHandler = handler;
        mValue = value;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.encoder_ppr_edit_dialog);

        mEncoderPPREdit = (EditText) findViewById(R.id.printEncoderPPREdit);
        mEncoderPPREdit.setText(mValue);
        mEncoderPPREdit.setSelection(mValue.length());

        mConfirm = (TextView) findViewById(R.id.confirm);
        mCancel = (TextView) findViewById(R.id.cancel);
        mConfirm.setOnClickListener(this);
        mCancel.setOnClickListener(this);
    }

    @Override
    public void onClick(View arg0) {
        switch (arg0.getId()) {
            case R.id.confirm:
                Message msg = mHandler.obtainMessage(SettingsListAdapter.MSG_ENCODER_PPR_SET);
                try {
                    msg.arg1 = Integer.valueOf(mEncoderPPREdit.getText().toString());
                } catch(Exception e) {
                    msg.arg1 = Integer.valueOf(mValue);
                }
                mHandler.sendMessage(msg);
                dismiss();
                break;
            case R.id.cancel:
                dismiss();
            default:
                break;
        }
    }
}
