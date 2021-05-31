package com.industry.printer;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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
import android.widget.TextView;

import com.industry.printer.Constants.Constants;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.ToastUtil;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.DynamicText;
import com.industry.printer.object.GraphicObject;
import com.industry.printer.object.TextObject;
import com.industry.printer.ui.CustomerDialog.CustomerDialogBase;
import com.industry.printer.ui.CustomerDialog.MessageBrowserDialog;
import com.industry.printer.ui.CustomerDialog.PictureBrowseDialog;
import com.industry.printer.ui.Items.PictureItem;

import java.util.ArrayList;

/**
 * Created by hmwan on 2021/5/14.
 */

public class CustomTabActivity extends Fragment {
    private static final String TAG = CustomTabActivity.class.getSimpleName();

    private Context mContext;

    private TextView mPrevBtn;
    private TextView mPurgeBtn;
    private TextView mOpenBtn;

    private ListView mPicLV;
    private ListView mTxtLV;

    private PicLVAdapter     mPicAdapter;
    private TxtLVAdapter     mTxtAdapter;

    private String mObjPath = null;
    private Handler mCtrlHandler = null;

    private PopupWindow mPopupWindow;
    private ImageView mPreView;
    private ProgressBar mProcBar;
    private TextView mCountTV;
    private int mPrintedCount = 0;
    private TextView mCancelBtn;
    private TextView mPurgeBtnPrev;

    private DataTransferThread mDataTransferThread = null;

    final private int MSG_PRINTER_LAUNCHED = 100;
    final private int MSG_PRINTER_STOPPED = 101;
    final private int MSG_REFRESH_PRINT_CNT = 102;

    private MessageTask mMsgTask;

    public Handler mHandler = new Handler(){
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EditTabSmallActivity.HANDLER_MESSAGE_SAVE_SUCCESS:
                    if(null == mCtrlHandler) return;

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

                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            while(!mDataTransferThread.isRunning()) {
                                try{Thread.sleep(10);}catch(Exception e){};
                            }
                            mHandler.sendEmptyMessage(MSG_PRINTER_LAUNCHED);
                        }
                    }).start();

                    break;
                case MSG_PRINTER_LAUNCHED:
                    mPreView.setImageBitmap(mDataTransferThread.getCurData().getBgPreview());
                    mProcBar.setVisibility(View.GONE);
                    mPreView.setVisibility(View.VISIBLE);
                    break;
                case MSG_PRINTER_STOPPED:
                    mPopupWindow.dismiss();
                    break;
                case MSG_REFRESH_PRINT_CNT:
                    mCountTV.setText("" + mPrintedCount);
                    break;
                default:
                    break;
            }
        }
    };

    public CustomTabActivity() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        //requestWindowFeature(Window.FEATURE_NO_TITLE);
        return inflater.inflate(R.layout.custom_frame, container, false);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        mContext = this.getActivity();
        mDataTransferThread = DataTransferThread.getInstance(mContext);

        if(mDataTransferThread.isRunning()) {
            if(null != mCtrlHandler) {
                mCtrlHandler.sendEmptyMessage(ControlTabActivity.MESSAGE_PRINT_STOP);
            }
        }

        mPrevBtn = (TextView) getView().findViewById(R.id.btn_preview);
        mPrevBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null == mMsgTask) return;
                mPicAdapter.saveObjs();
                mTxtAdapter.saveObjs();
                mMsgTask.save(mHandler);

                View popupView = LayoutInflater.from(mContext).inflate(R.layout.custom_preview, null);

                mPopupWindow = new PopupWindow(popupView, ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT, true);
                mPopupWindow.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#fffefefe")));
                mPopupWindow.setOutsideTouchable(true);
                mPopupWindow.setTouchable(true);
                mPopupWindow.update();

                mCancelBtn = (TextView) popupView.findViewById(R.id.tv_cancel);
                mCancelBtn.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        mCtrlHandler.sendEmptyMessage(ControlTabActivity.MESSAGE_PRINT_STOP);
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                while(mDataTransferThread.isRunning()) {
                                    try{Thread.sleep(10);}catch(Exception e){};
                                }
                                mHandler.sendEmptyMessage(MSG_PRINTER_STOPPED);
                            }
                        }).start();
                    }
                });

                mPurgeBtnPrev = (TextView) getView().findViewById(R.id.tv_flush);
                mPurgeBtnPrev.setOnClickListener(new View.OnClickListener() {
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

                mPreView = (ImageView) popupView.findViewById(R.id.tv_preview_img);
                mPreView.setVisibility(View.GONE);
                mProcBar = (ProgressBar) popupView.findViewById(R.id.pb_progressing);
                mProcBar.setVisibility(View.VISIBLE);
                mCountTV = (TextView) popupView.findViewById(R.id.tv_printed_count);
                mCountTV.setText("" + mPrintedCount);

                mPopupWindow.showAtLocation(mPrevBtn, Gravity.NO_GRAVITY, 0, 0);
            }
        });

        mPurgeBtn = (TextView) getView().findViewById(R.id.tv_flush);
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

        mOpenBtn = (TextView) getView().findViewById(R.id.tv_binfile);
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
                        mPicAdapter.createObjs();
                        mPicLV.invalidate();
                        mPicAdapter.notifyDataSetChanged();
                        mTxtAdapter.createObjs();
                        mTxtLV.invalidate();
                        mTxtAdapter.notifyDataSetChanged();
                    }

                    @Override
                    public void onClick(String content) {
                        // TODO Auto-generated method stub

                    }

                });
                dialog.show();
            }
        });

        mPicLV = (ListView) getView().findViewById(R.id.pic_list);
        mPicLV.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final int finalPos = position;
                final PictureBrowseDialog dialog = new PictureBrowseDialog(mContext);
                dialog.setOnPositiveClickedListener(new CustomerDialogBase.OnPositiveListener() {

                    @Override
                    public void onClick(String content) {
                        dialog.dismiss();
                    }

                    @Override
                    public void onClick() {
                        PictureItem item = dialog.getSelect();
                        if (item == null) {
                            dialog.dismiss();
                            return;
                        }
                        mPicAdapter.setItemContent(finalPos, item.getPath());
                        mPicAdapter.notifyDataSetChanged();
                        dialog.dismiss();
                    }
                });
                dialog.setOnNagitiveClickedListener(new CustomerDialogBase.OnNagitiveListener() {
                    @Override
                    public void onClick() {
                        dialog.dismiss();
                    }
                });
                dialog.show();
            }
        });

        mTxtLV = (ListView) getView().findViewById(R.id.text_list);

        mPicLV.setDivider(null);
        mTxtLV.setDivider(null);

        mPicAdapter = new PicLVAdapter();
        mPicLV.setAdapter(mPicAdapter);
        mTxtAdapter = new TxtLVAdapter();
        mTxtLV.setAdapter(mTxtAdapter);
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public void refreshView() {
        mPicAdapter.createObjs();
        mTxtAdapter.createObjs();
        mPrintedCount = 0;
    }

    public void setObjPath(String path) {
        if(null == path) return;
        mObjPath = path;
        mMsgTask = new MessageTask(mContext, mObjPath);
    }

    public void setCtrlHandler(Handler handler) {
        mCtrlHandler = handler;
    }

    private class ObjCnt {
        public String mContent = "";
        public BaseObject mObj = null;
    }

    private class PicLVAdapter extends BaseAdapter {
        private LayoutInflater  mInflater;
        private ArrayList<ObjCnt> mObjCnts;

        public PicLVAdapter() {
            mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mObjCnts = new ArrayList<ObjCnt>();
            createObjs();
        }

        private void createObjs() {
            if(null == mMsgTask) return;
            mObjCnts.clear();
            for(BaseObject obj : mMsgTask.getObjects()) {
                if(obj instanceof GraphicObject) {
                    ObjCnt objCnt = new ObjCnt();
                    objCnt.mContent = Configs.TLK_PATH_FLASH  + "/" + mMsgTask.getName() + "/" + obj.getContent();
                    objCnt.mObj = obj;
                    mObjCnts.add(objCnt);
                }
            }
        }

        public void setItemContent(int pos, String cnt) {
            mObjCnts.get(pos).mContent = cnt;
        }

        public void saveObjs() {
            for(ObjCnt objCnt : mObjCnts) {
                ((GraphicObject)objCnt.mObj).setImage(objCnt.mContent);
            }
        }

        @Override
        public int getCount() {
            return mObjCnts.size();
        }

        @Override
        public Object getItem(int arg0) {
            return (arg0 >= mObjCnts.size() ? null :mObjCnts.get(arg0));
        }

        @Override
        public long getItemId(int arg0) {
            return arg0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.custom_frame_pic_list_item, null);
            }

            ImageView picIV = (ImageView)convertView.findViewById(R.id.pic_item);
            Bitmap lastBmp = (Bitmap)picIV.getTag();

            Bitmap bmp = BitmapFactory.decodeFile(mObjCnts.get(position).mContent);
            if(null != bmp) {
                picIV.setImageBitmap(bmp);
                picIV.setTag(bmp);
                if(null != lastBmp) {
                    lastBmp.recycle();
                }
            }

            return convertView;
        }
    }

    public class TxtLVAdapter extends BaseAdapter {
        private LayoutInflater  mInflater;
        private ArrayList<ObjCnt> mObjCnts;

        public TxtLVAdapter() {
            mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mObjCnts = new ArrayList<ObjCnt>();
            createObjs();
        }

        private void createObjs() {
            if(null == mMsgTask) return;

            mObjCnts.clear();
            for(BaseObject obj : mMsgTask.getObjects()) {
                if(obj instanceof TextObject) {
                    ObjCnt objCnt = new ObjCnt();
                    objCnt.mContent = obj.getContent();
                    objCnt.mObj = obj;
                    mObjCnts.add(objCnt);
                }
            }
        }

        public void saveObjs() {
            for(ObjCnt objCnt : mObjCnts) {
                Debug.d(TAG, objCnt.mContent);
                ((TextObject)objCnt.mObj).setContent(objCnt.mContent);
            }
        }

        @Override
        public int getCount() {
            return mObjCnts.size();
        }

        @Override
        public Object getItem(int arg0) {
            return (arg0 >= mObjCnts.size() ? null :mObjCnts.get(arg0));
        }

        @Override
        public long getItemId(int arg0) {
            return arg0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.custom_frame_txt_list_item, null);
                final EditText txtET = (EditText)convertView.findViewById(R.id.text_item);
                txtET.addTextChangedListener(new TextWatcher() {
                    @Override
                    public void beforeTextChanged(CharSequence s, int start, int count, int after) {

                    }

                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {

                    }

                    @Override
                    public void afterTextChanged(Editable s) {
                        int pos = (Integer)txtET.getTag();
                        mObjCnts.get(pos).mContent = s.toString();
                        Debug.d(TAG, mObjCnts.get(pos).mContent);
                    }
                });
            }

            EditText txtET = (EditText)convertView.findViewById(R.id.text_item);
            txtET.setTag(position);
            txtET.setText(mObjCnts.get(position).mContent);

            return convertView;
        }
    }
}
