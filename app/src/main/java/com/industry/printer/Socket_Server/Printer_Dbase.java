package com.industry.printer.Socket_Server;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;


public class Printer_Dbase extends SQLiteOpenHelper {
	private static final int VERSION = 1;

	public Printer_Dbase(Context context) {
		super(context, Printer_Database.DB_NAME, null, VERSION);
	}

	@Override
	public void onCreate(SQLiteDatabase DbDatabase) {
		DbDatabase.execSQL("CREATE TABLE  IF NOT EXISTS " + "System" + " (" + "_id"
                + " INTEGER PRIMARY KEY AUTOINCREMENT," 
        		+ "system_version" + " TEXT,"
                + "system_device" + " TEXT," 
                + "system_ip" + " TEXT"
                + ");");
	/*DbDatabase.execSQL("CREATE TABLE  IF NOT EXISTS " + "User" + " (" + "_id"
            + " INTEGER PRIMARY KEY AUTOINCREMENT," 
    		+ "User_Account" + " TEXT,"
            + "User_Password" + " TEXT," 
            + "User_Date" + " TEXT,"
            + "User_Flag" + " TEXT" 
            + ");");*/
	//2014.11.3MAJINXIN修改
	/*DbDatabase.execSQL("CREATE TABLE  IF NOT EXISTS " + "Bank" + " (" + "_id"
            + " INTEGER PRIMARY KEY AUTOINCREMENT," 
    		+ "B_ISSHOW" + " TEXT,"	
            + "B_NAME" + " TEXT," 
            + "B_QUICK" + " TEXT,"
            + "B_FLAG" + " TEXT," 
            + "B_CODE" + " TEXT"
            + ");");*/
	}

	@Override
	public void onUpgrade(SQLiteDatabase arg0, int arg1, int arg2) {
		// TODO Auto-generated method stub
		
	}

	
	
}
