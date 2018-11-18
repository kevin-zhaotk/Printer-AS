package com.industry.printer.Utils;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteOpenHelper;

public class PrinterDBHelper extends SQLiteOpenHelper {

	public static PrinterDBHelper dbHelper;
	private static final String DATABASE_NAME = "printer_database";
	private static final String TABLE_COUNT_NAME = "sys_config_count";
	private static final String FIRST_BOOT_TABLE = "first_boot_table";
	private static final int DATABASE_VERSION = 1;
	
	private static final String DB_CREATE_SQL = "create table if not exists sys_config_count(name varchar primary key,"
			+ "value integer)";
	
	private static final String TABLE_FIRSTBOOT_CREATE_SQL = "create table if not exists first_boot_table(property varchar primary key,"
			+ "state varchar)";
	
	public static PrinterDBHelper getInstance(Context context) {
		if (dbHelper == null) {
			dbHelper = new PrinterDBHelper(context);
		}
		return dbHelper;
	}
	
	public PrinterDBHelper(Context context, String name, CursorFactory factory,
			int version) {
		super(context, name, factory, version);
	}
	
	public PrinterDBHelper(Context context, String name, int version) {
		this(context, name, null, version);
	}

	public PrinterDBHelper(Context context) {
		this(context, DATABASE_NAME, DATABASE_VERSION);
	}
	@Override
	public void onCreate(SQLiteDatabase db) {
		db.execSQL(DB_CREATE_SQL);
		db.execSQL(TABLE_FIRSTBOOT_CREATE_SQL);
		// 添加默认记录值
		// db.execSQL("insert into printer_database(name, value) values('count', 0)");
		ContentValues values = new ContentValues();
		values.put("name", "count");
		values.put("value", 0);
		db.insert(TABLE_COUNT_NAME, null, values);
		
		values.clear();
		values.put("property", "firstboot");
		values.put("state", "true");
		db.insert(FIRST_BOOT_TABLE, null, values);
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

	}

	
	public void updateCount(Context context, int count) {
		SQLiteDatabase db = this.getWritableDatabase();
		ContentValues values = new ContentValues();
		values.clear();
		values.put("name", "count");
		values.put("value", count);
		String where = "name = 'count'";
		db.update(TABLE_COUNT_NAME, values, null, null);
	}
	
	public int getCount(Context context) {
		SQLiteDatabase db = this.getReadableDatabase();
		String[] columns = {"value"}; 
		Cursor cursor = db.query(TABLE_COUNT_NAME, columns, null, null, null, null, null);
		Debug.d("", "===>" + cursor.getColumnIndex("value"));
		cursor.getColumnIndex("value");
		cursor.moveToFirst();
		int count = cursor.getInt(0);
		return count;
	}
	
	public void setFirstBoot(Context context, boolean stat) {
		SQLiteDatabase db = this.getReadableDatabase();
		ContentValues values = new ContentValues();
		values.clear();
		values.put("property", "firstboot");
		if (stat) {
			values.put("state", "true");
		} else {
			values.put("state", "false");
		}
		db.update(FIRST_BOOT_TABLE, values, null, null);
		
	}
	
	public boolean getFirstBoot(Context context) {
		SQLiteDatabase db = this.getReadableDatabase();
		String[] columns = {"state"}; 
		Cursor cursor = db.query(FIRST_BOOT_TABLE, columns, null, null, null, null, null);
		cursor.getColumnIndex("state");
		cursor.moveToFirst();
		String stat = cursor.getString(0);
		return stat.equals("true");
	}
}
