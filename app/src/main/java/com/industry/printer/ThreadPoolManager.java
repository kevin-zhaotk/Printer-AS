package com.industry.printer;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ThreadPoolManager {
	
	public static ExecutorService mThreads = Executors.newFixedThreadPool(5);
	
	public static ExecutorService mSerialControlThread = Executors.newFixedThreadPool(1);
	
	public static ExecutorService mRfidThread = Executors.newFixedThreadPool(1);
	
		
}
