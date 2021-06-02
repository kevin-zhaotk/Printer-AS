package com.industry.printer;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.HorizontalScrollView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.TextView;

import com.industry.printer.Constants.Constants;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.FileUtil;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.DynamicText;
import com.industry.printer.object.GraphicObject;
import com.industry.printer.object.TextObject;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase;
import com.industry.printer.ui.CustomerDialog.LoadingDialog;
import com.industry.printer.ui.CustomerDialog.MessageBrowserDialog;
import com.industry.printer.ui.CustomerDialog.PictureBrowseDialog;
import com.industry.printer.ui.Items.PictureItem;

import org.w3c.dom.Text;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;

/**
 * Created by hmwan on 2021/5/14.
 */

public class Custom2TabActivity extends Fragment {
    private static final String TAG = Custom2TabActivity.class.getSimpleName();

    private Context mContext;

    private DataTransferThread mDataTransferThread = null;
    private String mObjPath = null;
    private MessageTask mMsgTask;
    private TextObject mObj;

    private Handler mCtrlHandler = null;

    private TextView mStartBtn;
    private TextView mStopBtn;
    private TextView mOpenBtn;
    private TextView mImportBtn;
    private TextView mPurgeBtn;

    private EditText mCntET;
    private TextView mOKBtn;

    private ImageView mPreView;
    private ProgressBar mProcBar;

    private LoadingDialog mProgressDialog;

    private TextView mCountTV;
    private int mPrintedCount = 0;

    final private int MSG_IMPORT_DONE = 101;
    final private int MSG_REFRESH_PRINT_CNT = 102;

    public Handler mHandler = new Handler(){
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EditTabSmallActivity.HANDLER_MESSAGE_SAVE_SUCCESS:
                    refreshView();
//                    mPreView.setImageBitmap(mDataTransferThread.getCurData().getBgPreview());
//                    mProcBar.setVisibility(View.GONE);
//                    mPreView.setVisibility(View.VISIBLE);
                    break;
                case MSG_REFRESH_PRINT_CNT:
                    mCountTV.setText("" + mPrintedCount);
                    break;
                case MSG_IMPORT_DONE:
                    mProgressDialog.dismiss();
                    break;
                default:
                    break;
            }
        }
    };

    public Custom2TabActivity() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        //requestWindowFeature(Window.FEATURE_NO_TITLE);
        return inflater.inflate(R.layout.custom2_frame, container, false);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        mPrintedCount = 0;
        mContext = this.getActivity();
        mDataTransferThread = DataTransferThread.getInstance(mContext);

        if(mDataTransferThread.isRunning()) {
            if(null != mCtrlHandler) {
                mCtrlHandler.sendEmptyMessage(ControlTabActivity.MESSAGE_PRINT_STOP);
            }
        }

        mStartBtn = (TextView) getView().findViewById(R.id.btn_print);
        mStartBtn.setOnClickListener(new View.OnClickListener() {
             @Override
             public void onClick(View v) {
                 if(null == mCtrlHandler) return;
                 if(StringUtil.isEmpty(mObjPath)) return;

                 Message ctrlMsg = mCtrlHandler.obtainMessage(ControlTabActivity.MESSAGE_OPEN_PREVIEW);
                 Bundle bundle = new Bundle();
                 bundle.putString("file", mObjPath);
                 bundle.putBoolean("printAfterLoad", true);
                 ctrlMsg.setData(bundle);
                 mCtrlHandler.sendMessage(ctrlMsg);

                 mDataTransferThread.setCusCallback(new DataTransferThread.CustomCallback() {
                     @Override
                     public void onComplete(int index) {
                         mPrintedCount++;
                         mHandler.sendEmptyMessage(MSG_REFRESH_PRINT_CNT);
                     }
                 });
                 mStopBtn.setEnabled(true);
                 mStartBtn.setEnabled(false);
                 mCntET.setEnabled(false);
                 mOKBtn.setEnabled(false);
                 mOpenBtn.setEnabled(false);
                 mPurgeBtn.setEnabled(false);
             }
         });
        mStartBtn.setEnabled(true);

        mStopBtn = (TextView) getView().findViewById(R.id.btn_stop);
        mStopBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null == mCtrlHandler) return;
                mCtrlHandler.sendEmptyMessage(ControlTabActivity.MESSAGE_PRINT_STOP);
                mStopBtn.setEnabled(false);
                mStartBtn.setEnabled(true);
                mCntET.setEnabled(true);
                mOKBtn.setEnabled(true);
                mOpenBtn.setEnabled(true);
                mPurgeBtn.setEnabled(true);
            }
        });
        mStopBtn.setEnabled(false);

        mOpenBtn = (TextView) getView().findViewById(R.id.btn_open);
        mOpenBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MessageBrowserDialog dialog = new MessageBrowserDialog(mContext, MessageBrowserDialog.OpenFrom.OPEN_PRINT, mObjPath);
                dialog.setOnPositiveClickedListener(new CustomerDialogBase.OnPositiveListener() {

                    @Override
                    public void onClick() {
                        if(null == mCtrlHandler) return;

                        ArrayList<String> f = MessageBrowserDialog.getSelected();
                        if (f==null || f.size() == 0 || f.size() > 1) {
                            return;
                        }

                        setObjPath(f.get(0));
                        refreshView();
                    }

                    @Override
                    public void onClick(String content) {
                        // TODO Auto-generated method stub
                    }
                });
                dialog.show();
            }
        });

        mImportBtn = (TextView) getView().findViewById(R.id.btn_import);
        mImportBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final ArrayList<String> usbs = ConfigPath.getMountedUsb();
                if (usbs.size() <= 0) {
                    ToastUtil.show(mContext, R.string.toast_plug_usb);
                    return;
                }
                mProgressDialog = LoadingDialog.show(mContext, R.string.strCopying);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            FileUtil.copyDirectiory(usbs.get(0) + Configs.SYSTEM_CONFIG_MSG_PATH, Configs.TLK_PATH_FLASH);
                        } catch(IOException e) {
                            ToastUtil.show(mContext, R.string.toast_plug_usb);
                        } finally {
                            mHandler.sendEmptyMessage(MSG_IMPORT_DONE);
                        }
                    }
                }).start();

            }
        });

        mPurgeBtn = (TextView) getView().findViewById(R.id.btn_flush);
        mPurgeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null == mDataTransferThread) return;

                if(mDataTransferThread.isPurging) {
                    ToastUtil.show(mContext, R.string.str_under_purging);
                    return;
                }

                AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
                builder.setTitle(R.string.str_btn_clean)
                        .setMessage(R.string.str_purge_confirm)
                        .setPositiveButton(R.string.str_ok, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                mDataTransferThread.purge(mContext);
                                dialog.dismiss();
                            }
                        })
                        .setNegativeButton(R.string.str_btn_cancel, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        })
                        .create()
                        .show();
            }
        });

        mCntET = (EditText) getView().findViewById(R.id.text_item);

        mOKBtn = (TextView) getView().findViewById(R.id.btn_ok);
        mOKBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null == mCtrlHandler) return;
                if(StringUtil.isEmpty(mObjPath)) return;

                mProcBar.setVisibility(View.VISIBLE);

                mObj.setContent(mCntET.getText().toString());
                mMsgTask.save(mHandler);
            }
        });

        mPreView = (ImageView) getView().findViewById(R.id.tv_preview_img);
        mPreView.setVisibility(View.GONE);
        mProcBar = (ProgressBar) getView().findViewById(R.id.pb_progressing);
        mProcBar.setVisibility(View.VISIBLE);
        mCountTV = (TextView) getView().findViewById(R.id.tv_printed_count);
        mCountTV.setText("" + mPrintedCount);
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    private Bitmap getProperSizeBmp(Bitmap src) {
        if (src != null) {
            int width = src.getWidth();
            int height = src.getHeight();
            if (width > 4096 || height > 4096) {
                int inSampleSize;
                int heightRatio = Math.round((float) height / (float) 4096);
                int widthRatio = Math.round((float) width / (float) 4096);
                inSampleSize = heightRatio < widthRatio ? widthRatio : heightRatio;
                if (inSampleSize == 1) {
                    inSampleSize = 2;
                }
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                src.compress(Bitmap.CompressFormat.JPEG, 100, baos);
                ByteArrayInputStream isBm = new ByteArrayInputStream(baos.toByteArray());
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inSampleSize = inSampleSize;
                options.inJustDecodeBounds = false;
                src = BitmapFactory.decodeStream(isBm, null, options);
            }
        }
        return src;
    }

    public void refreshView() {
        mCountTV.setText("" + mPrintedCount);
        mCntET.setText(mObj.getContent());
        Bitmap bmp = BitmapFactory.decodeFile(mMsgTask.getPreview());
        bmp = getProperSizeBmp(bmp);
        mPreView.setImageBitmap(bmp);
        mPreView.setVisibility(View.VISIBLE);
        mProcBar.setVisibility(View.GONE);
    }

    public void setObjPath(String path) {
        Debug.d(TAG, "path = " + path);
        if(null == path) return;
        mObjPath = path;
        mMsgTask = new MessageTask(mContext, mObjPath);

        for(BaseObject obj : mMsgTask.getObjects()) {
            if(obj instanceof TextObject) {
                mObj = (TextObject)obj;
                break;
            }
        }
    }

    public void setCtrlHandler(Handler handler) {
        mCtrlHandler = handler;
    }
}
