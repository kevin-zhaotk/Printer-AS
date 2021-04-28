package com.industry.printer.ui.CustomerDialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;

import com.industry.printer.R;
import com.industry.printer.Utils.ToastUtil;

/**
 * Created by hmwan on 2021/4/28.
 */

public class GroupNamingDialog  extends Dialog {
    private static final String TAG = CounterEditDialog.class.getSimpleName();

    private Context mContext;
    private String mSuffixName = "";

    private EditText mNameEV;
    private TextView mConfirm;
    private TextView mCancel;

    public interface OnOKListener {
        public void onOK(String name);
    }
    private OnOKListener mOKListener;

    public GroupNamingDialog(Context ctx, String name) {
        super(ctx, R.style.Dialog);
        mContext = ctx;
        mSuffixName = name;
        mOKListener = null;
    }

    public void setOKListener(OnOKListener l) {
        mOKListener = l;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.dialog_group_naming);

        mNameEV = (EditText) findViewById(R.id.group_name);
        mNameEV.setText(mSuffixName);
        mNameEV.selectAll();
        mConfirm = (TextView) findViewById(R.id.confirm);
        mCancel = (TextView) findViewById(R.id.cancel);

        mConfirm.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mNameEV.getText().toString().isEmpty()) {
                    ToastUtil.show(mContext, R.string.toast_content_empty);
                } else {
                    dismiss();
                    if (null != mOKListener) {
                        mOKListener.onOK(mNameEV.getText().toString());

                    }
                }
            }
        });

        mCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
            }
        });
    }
}
