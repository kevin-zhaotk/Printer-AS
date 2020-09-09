package com.industry.printer.Utils;

import android.os.AsyncTask;
import android.os.Handler;

@Deprecated
public class IPAsyncTask extends AsyncTask<Void, Void, Void> {

	private Handler mCallback;
	private int 	mMessage;
	private Runnable mRunnable;
	
	public IPAsyncTask(Handler callback, int message, Runnable runnable) {
		mCallback = callback;
		mMessage = message;
		mRunnable = runnable;
	}
	
	@Override
	protected Void doInBackground(Void... params) {
		if (mRunnable != null) {
			mRunnable.run();
		}
		return null;
	}

	@Override
	protected void onPostExecute(Void result) {
		if (mCallback != null) {
			mCallback.sendEmptyMessage(mMessage);
		}
	}
	
	
	public interface Runnable {
		public void run();
	}
}
