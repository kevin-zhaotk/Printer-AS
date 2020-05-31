package com.industry.printer.ui.CustomerDialog;
 

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;
 



import com.industry.printer.R;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.PlatformInfo;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.object.TLKFileParser;
import com.industry.printer.ui.CustomerAdapter.ListViewButtonAdapter;
import com.industry.printer.ui.CustomerAdapter.MessageListAdater;
import com.industry.printer.ui.KZListView.KZListView;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.opengl.Visibility;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.AbsListView;
import android.widget.AbsListView.OnScrollListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

//// public class MessageBrowserDialog extends CustomerDialogBase implements android.view.View.OnClickListener, OnItemClickListener, AdapterView.OnItemLongClickListener, OnTouchListener, OnScrollListener, TextWatcher {
public class MessageBrowserDialog extends CustomerDialogBase implements android.view.View.OnClickListener, OnItemClickListener, AdapterView.OnItemLongClickListener, TextWatcher {
	private final String TAG = MessageBrowserDialog.class.getSimpleName();

	public TextView mConfirm;
	public TextView mCancel;
	public RelativeLayout mPagePrev;
	public RelativeLayout mPageNext;
	public TextView mDelete;
	public TextView mGroup;
	public RelativeLayout mLoadingLy;

	public EditText mSearch;
	private static ArrayList<String> mTitles;

	private String[] mTotalContents = null;
	private ArrayList<Integer> mTotalIndex = null;
	private ArrayList<Integer> mDispIndex = null;

	public KZListView mMessageList;
	public View mVSelected;

	public MessageListAdater mFileAdapter;

	/**
	 * multi select
	 */
	private boolean mMultiMode = false;
	private OpenFrom mFrom;
	private String mCurrentMsg = null;

	private static final int MSG_FILTER_CHANGED = 1;
	private static final int MSG_LOADED = 2;
	private static final int MSG_UPDATE_FILTER_STATUS = 3;

	public enum OpenFrom {
		OPEN_EDIT,
		OPEN_PRINT
	}

	public Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
				case MSG_FILTER_CHANGED:
					Bundle bundle = msg.getData();
					String title = bundle.getString("title");
					filter(title);
					break;
				case MSG_UPDATE_FILTER_STATUS:
					mFileAdapter.setContentIndex(mDispIndex);
					break;
				case MSG_LOADED:
					mFileAdapter.setmTotalContents(mTotalContents);
					mDispIndex = new ArrayList<Integer>(mTotalIndex);
					mFileAdapter.setContentIndex(mDispIndex);
					int scrollToPosition = find(mDispIndex, mCurrentMsg);
					mMessageList.setSelection(scrollToPosition);
					mFileAdapter.setSelected(scrollToPosition);
					hideLoading();
					break;
				default:
					break;
			}
		}
	};

	public MessageBrowserDialog(Context context) {
		super(context, R.style.Dialog_Fullscreen);
		mFrom = OpenFrom.OPEN_EDIT;
		mVSelected = null;
		mTitles = new ArrayList<String>();
		mTotalIndex = new ArrayList<Integer>();
		mDispIndex = new ArrayList<Integer>();

		mFileAdapter = new MessageListAdater(context,
				R.layout.message_item_layout,
				new String[]{"title", "abstract", ""},
				new int[]{R.id.tv_msg_title, R.id.ll_preview, R.id.image_selected});
	}

	public MessageBrowserDialog(Context context, OpenFrom from) {
		this(context);
		mFrom = from;
	}

	public MessageBrowserDialog(Context context, OpenFrom from, String msg) {
		this(context);
		mFrom = from;
		mCurrentMsg = msg;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.setContentView(R.layout.message_list_layout);

		mConfirm = (TextView) findViewById(R.id.btn_ok_message_list);
		mConfirm.setOnClickListener(this);

		mCancel = (TextView) findViewById(R.id.btn_cancel_message_list);
		mCancel.setOnClickListener(this);

		mDelete = (TextView) findViewById(R.id.btn_delete);
		mDelete.setOnClickListener(this);

		mGroup = (TextView) findViewById(R.id.btn_multi_select);
		mGroup.setOnClickListener(this);
		if (mFrom == OpenFrom.OPEN_EDIT) {
			mGroup.setVisibility(View.GONE);
		}

		mPagePrev = (RelativeLayout) findViewById(R.id.btn_page_prev);
		mPagePrev.setOnClickListener(this);

		mPageNext = (RelativeLayout) findViewById(R.id.btn_page_next);
		mPageNext.setOnClickListener(this);

		mSearch = (EditText) findViewById(R.id.et_search);
		mSearch.addTextChangedListener(this);
		mMessageList = (KZListView) findViewById(R.id.message_listview);
		mMessageList.setOnItemClickListener(this);

		mMessageList.setAdapter(mFileAdapter);

		mMessageList.setOnItemLongClickListener(this);
////		mMessageList.setOnTouchListener(this);
////		mMessageList.setOnScrollListener(this);

		mLoadingLy = (RelativeLayout) findViewById(R.id.loading);

		mTitles.clear();
		loadMessages();

		mTitles.add(mCurrentMsg);
		setupViews();

	}

	private void setupViews() {
		if (PlatformInfo.PRODUCT_FRIENDLY_4412.equals(PlatformInfo.getProduct())) {
			mPagePrev.setVisibility(View.GONE);
			mPageNext.setVisibility(View.GONE);
		}
	}

	private void deleteTlkDir(String title) {
		File file = new File(ConfigPath.getTlkDir(title));
		if (file.exists()) {
			File[] list = file.listFiles();
			for (int i = 0; i < list.length; i++) {
				File f = list[i];
				f.delete();
			}
		}
		file.delete();
	}

	private void deleteItem(Integer selected) {
		String title = mTotalContents[selected];
		Debug.d(TAG, "Delete: " + title + "@" + selected);
		deleteTlkDir(title);
		mTotalIndex.remove(selected);
		mTotalContents[selected.intValue()] = "";
		mDispIndex.remove(selected);
	}

	@Override
	public void onClick(View arg0) {
		switch (arg0.getId()) {
			case R.id.btn_ok_message_list:
				if (mMultiMode) {
					mTitles.clear();
					ArrayList<Integer> selected = mFileAdapter.getSelected();
					for(Integer idx : selected) {
						String title = mTotalContents[idx];
						if (title.startsWith("G-")) {
							continue;
						}
						mTitles.add(title);
					}
				}
				dismiss();
				if (pListener != null) {
					pListener.onClick();
				}
				break;
			case R.id.btn_cancel_message_list:
				dismiss();
				if (nListener != null) {
					nListener.onClick();
				}
				break;
			case R.id.btn_page_prev:
				mMessageList.smoothScrollBy(-200, 50);
				break;
			case R.id.btn_page_next:
				mMessageList.smoothScrollBy(200, 50);
				break;
			case R.id.btn_delete:
				if (mMultiMode) {
					for(Integer i : mFileAdapter.getSelected()) {
						deleteItem(i);
					}
					clearMultiSelect();
				} else {
					deleteItem(mFileAdapter.getSelectedSingle());
				}
				mFileAdapter.setContentIndex(mDispIndex);
				break;
				
			case R.id.btn_multi_select:
				switchMultiSelect();
				break;
		}
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
		Debug.d(TAG, "--->onItemClick");
		mFileAdapter.setSelected(position);
		mFileAdapter.notifyDataSetChanged();
		if (!mMultiMode) {
			mTitles.clear();
			mTitles.add(mTotalContents[mDispIndex.get(position)]);
		} else {
			String title = mTotalContents[mDispIndex.get(position)];
			if (title == null || title.startsWith("G-")) {
				return;
			}
			if (mTitles.contains(title)) {
				mTitles.remove(title);
			} else {
				mTitles.add(title);
			}
		}
	}

	private void switchMode(boolean mode) {
		mMultiMode = mode;
		mTitles.clear();
		mFileAdapter.setMultiMode(mMultiMode);
		mFileAdapter.notifyDataSetChanged();
	}

	private void clearMultiSelect() {
		switchMode(false);
	}

	private void switchMultiSelect() {
		switchMode(!mMultiMode);
	}

	@Override
	public boolean onItemLongClick(AdapterView<?> adapterView, View view, int i, long l) {
		switchMode(true);
		return true;
	}

	private ArrayList<Integer> sort(String[] src) {
		ArrayList<Integer> indexs = new ArrayList<Integer>();

// H.M.Wang 2020-5-19 增加文件名长度优先的排序，长度一致时按字符排序
		for (int i=0; i<src.length; i++) {
			int start = 0;
			int end = indexs.size();

			int j = start;
			do {
				if (j >= end) {
					indexs.add(end, i);
					break;
				}

				int lenSrc1 = src[i].length();
				int lenSrc2 = src[indexs.get(j)].length();

				if(lenSrc1 < lenSrc2) {
					end = j;
					j = (start + end) / 2;
				} else if(lenSrc1 > lenSrc2) {
					start = j;
					j = (start + end) / 2;
				} else {
					int et = src[i].compareTo(src[indexs.get(j)]);
					if (et == 0) {
						indexs.add(j, i);
						break;
					} else if (et < 0) {        // 欲插入字符串比当前字符串小
						end = j;
						j = (start + end) / 2;
					} else if (et > 0) {        // 欲插入字符串比当前字符串大
						start = j;
						j = (start + end) / 2;
					}
				}
				if (start + 1 == end) {
					indexs.add(end, i);
					break;
				}
			} while (true);
		}
// End of H.M.Wang 2020-5-19 增加文件名长度优先的排序，长度一致时按字符排序
		return indexs;
	}

	private int find(ArrayList<Integer> index, String find) {
		if(null == find || find.isEmpty()) {
			return 0;
		}

		int foundPos = 0;

		int start = 0;
		int end = index.size();

		int i = start;
		do {
			int et = find.compareTo(mTotalContents[index.get(i)]);
			if (et == 0) {
				foundPos = i;
				break;
			} else if (et < 0) {
				end = i;
				i = (start + end) / 2;
			} else if (et > 0) {
				start = i;
				i = (start + end) / 2;
			}
		} while (start + 1 < end);

		return foundPos;
	}

	@SuppressWarnings("unchecked")
	public void loadMessages() {
		showLoading();
		new Thread(new Runnable() {
			@Override
			public void run() {
				String tlkPath = ConfigPath.getTlkPath();
				if (tlkPath == null) {
					return;
				}

				Debug.d(TAG, "--->load message begin");
				File rootpath = new File(tlkPath);

				mTotalContents = rootpath.list(new FilenameFilter() {
					@Override
					public boolean accept(File arg0, String arg1) {
						if (mFrom == OpenFrom.OPEN_EDIT && arg1.startsWith("G-")) {
							return false;
						}
						if(!arg0.isDirectory()) {
							return false;
						}
						return true;
					}
				});
				if (mTotalContents == null) {
					return;
				}

				mTotalIndex = sort(mTotalContents);
				mHandler.sendEmptyMessage(MSG_LOADED);
			}
		}).start();
	}

	private void showLoading() {
		mLoadingLy.setVisibility(View.VISIBLE);
	}

	private void hideLoading() {
		mLoadingLy.setVisibility(View.GONE);
	}

	public static ArrayList<String> getSelected() {

		if (mTitles == null) {
			return new ArrayList<String>();
		} else {
			return mTitles;
		}
	}

////	private boolean isTouch = true;

////	@Override
////	public boolean onTouch(View arg0, MotionEvent arg1) {
////		switch (arg1.getAction()) {
////			case MotionEvent.ACTION_DOWN:
////				//mMessageList.dispatchTouchEvent(arg1);
////				break;
////			case MotionEvent.ACTION_MOVE:
////				isTouch =false;
////				break;
////			case MotionEvent.ACTION_UP:
////				isTouch = true;
////				break;
////		}
////		return false;
////	}

////		float mdownx,mdowny;
//		@Override
//		public boolean onTouch(View arg0, MotionEvent arg1) {
//			// TODO Auto-generated method stub
//
//			float curX,curY;
//			switch( arg1.getAction())
//			{
//			case MotionEvent.ACTION_DOWN:
//				Debug.e(TAG, "------ACTION_DOWN"+ arg1.getX());
//				mdownx=arg1.getX();
//				mdowny=arg1.getY();
//
//				break;
//			case MotionEvent.ACTION_MOVE:
//				Debug.e(TAG, "------ACTION_MOVE"+ arg1.getX());
//				curX=arg1.getX();
//				curY=arg1.getY();
//				if(( Math.abs(curX-mdownx )  )>  (Math.abs(curY-mdowny ))  )//横向滑动
//				{
//					mFileAdapter.Scroll( (int)( mdownx-curX ) );
//					mFileAdapter.notifyDataSetChanged();
//				}
//				mdownx = curX;
//				return false;
//				//break;
//			case MotionEvent.ACTION_UP:
//				Debug.e(TAG, "------ACTION_UP"+ arg1.getX());
//				curX=arg1.getX();
//				curY=arg1.getY();
//
//
//				if(( Math.abs(curX-mdownx )  )>  (Math.abs(curY-mdowny ))  )//横向滑动
//				{
//					return true;
//				}
//
//
//
//				// mFileAdapter.Scroll(1);
//			//	mFileAdapter.notifyDataSetChanged();
//				break;
//
//			}
//
//			return false;
//		}

////		@Override
////		public void onScroll(AbsListView arg0, int firstVisibleItem, int visibleItemCount, int totalItemCount) {
////			if(firstVisibleItem==0){
////				Debug.e(TAG, "滑到顶部");
////				isTop = true;
////            } else {
////            	isTop = false;
////            }

////			if(visibleItemCount+firstVisibleItem==totalItemCount){
////            	Debug.e(TAG, "滑到底部");
////            	isBottom = true;
////            } else {
////            	isBottom = false;
////            }
////		}

////		@Override
////		public void onScrollStateChanged(AbsListView view, int state) {
////			switch (state) {
////			case OnScrollListener.SCROLL_STATE_IDLE:
////				Debug.d(TAG, "===>idle");

////				break;
////			case OnScrollListener.SCROLL_STATE_FLING:
////				Debug.d(TAG, "===>fling");
////				break;
////			case OnScrollListener.SCROLL_STATE_TOUCH_SCROLL:
////				Debug.d(TAG, "===>touch scroll");
////				break;
////			default:
////				break;
////			}
////		}
		

		/**
		 * 過濾信息名匹配的
		 * @param filter
		 */
		public void filter(final String filter) {
			if(filter.isEmpty()) {
				mDispIndex = new ArrayList<Integer>(mTotalIndex);
				mHandler.sendEmptyMessage(MSG_UPDATE_FILTER_STATUS);
			} else {
				new Thread(new Runnable() {
					@Override
					public void run() {
						mDispIndex.clear();
						long interval = System.currentTimeMillis();
						for(int i=0; i<mTotalIndex.size(); i++) {
							if(mTotalContents[mTotalIndex.get(i)].indexOf(filter) >=0 ) {
								mDispIndex.add(mTotalIndex.get(i));
							}
							if(System.currentTimeMillis() > interval + 50) {
								mHandler.sendEmptyMessage(MSG_UPDATE_FILTER_STATUS);
								try {Thread.sleep(100);} catch (Exception e) {};
								interval = System.currentTimeMillis();
							}
						}
						mHandler.sendEmptyMessage(MSG_UPDATE_FILTER_STATUS);
					}
				}).start();
			}
		}
		
		@Override
		public void afterTextChanged(Editable arg0) {
			String text = arg0.toString();
			Debug.d(TAG, "filter: " + text);
			mHandler.removeMessages(MSG_FILTER_CHANGED);
			Message msg = mHandler.obtainMessage(MSG_FILTER_CHANGED);
			Bundle bundle = new Bundle();
			bundle.putString("title", text);
			msg.setData(bundle);
			mHandler.sendMessageDelayed(msg, 2000);
		}

		@Override
		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			
		}

		@Override
		public void onTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			
		}
		
}
