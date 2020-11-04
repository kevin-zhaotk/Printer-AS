package com.industry.printer.ui.CustomerDialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.industry.printer.R;

/**
 * Created by hmwan on 2020/11/4.
 */

public class RemoteMsgPrompt extends Dialog {

    private Context mContext;
    private RelativeLayout mTotalView;
    private TextView mMessage;

    public RemoteMsgPrompt(Context context) {
        super(context, R.style.Dialog);
        mContext = context;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.setContentView(R.layout.dialog_remote_msg_prompt);

        mMessage = (TextView) findViewById(R.id.RemoteMsgTV);
        mTotalView = (RelativeLayout) findViewById(R.id.RemoteMsgTotal);

        mTotalView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                hide();
            }
        });
    }

    public void setMessage(String msg) {
        mMessage.setText(msg);
    }
}
