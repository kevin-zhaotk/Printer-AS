package com.industry.printer.Socket_Server;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Environment;
import android.widget.Toast;


public class Printer_Database {
	
	/** 数据库名 */
	public static Paths_Create Paths=new Paths_Create();
	//private static final String SDCARD_ROOT //.Environment.getExternalStorageDirectory().toString();
	public static final String DB_NAME =Paths.getSDPath()+ "printer.db"; 
	//public static final String DB_NAME ="QjFund.db"; 
	private static SQLiteDatabase db; 
	private static Printer_Dbase dbManager;

	private static Printer_Database instance;
	//________________________________接口调用函数__________________________________
	public static Printer_Database getInstance(Context context) {
		if (instance == null) {
			instance = new Printer_Database(context);
		}
		return instance;
	}

	public Printer_Database(Context context) {
		if (dbManager == null) {
			dbManager = new Printer_Dbase(context);
		}
	}
	//_______________________________________________________________________
	public void close() {
		db.close();
	}
	
	public void open() {
		db = dbManager.getWritableDatabase();
	}
	
	public SQLiteDatabase getDB() {
		if (db == null ) {
			return dbManager.getWritableDatabase();
		}
		return db;
	}
	
	public int deleteByItem(String tbName, String key, String value) {
		return db.delete(tbName, key + " = ? ", new String[] { value });
	}

	public boolean deleteAll(String tbName) {
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
		boolean result = true;
		try
		{
			db.execSQL("DELETE FROM " + tbName); 
			result = true;
		}catch(Exception e)
		{
			result = false;
			//Toast.makeText(ct, "error:" + e.getMessage(), Toast.LENGTH_LONG).show();
		}
		
		 
		return result;
	}
	
	
	public boolean InsertData(String Sql,String QureySql)
	{
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
		boolean result = true;
		try
		{
			Cursor cr=AllData_SqlData(QureySql);
			if(cr.getCount()>0)
			{
				return false;
			}
			else
			{
			db.execSQL(Sql);
			result = true;
			}
		}catch(Exception e)
		{
			result = false;
			//Toast.makeText(ct, "error:" + e.getMessage(), Toast.LENGTH_LONG).show();
		}
		return result;
	}	

	public int update(String tbName, ContentValues values, String whereClause, String[] whereArgs) {
		return db.update(tbName, values, whereClause + " = ? ", whereArgs);
	}

	
	public Cursor getAll(String tbName) {
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
		return db.query(tbName, null, null, null, null, null, null);
	}
	public String QuerySqlData(String StrSql)//
	{
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
		Cursor rs=db.rawQuery(StrSql,null);
		rs.moveToFirst();
		if(rs.getCount()>0)
		{
			
		
			
	    return rs.getString(rs  
                .getColumnIndex("system_device"));
	   }
		return "none";  
	}
	public Cursor AllData_SqlData(String StrSql)//
	{
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
	    
	    return db.rawQuery(StrSql,null); 
	  
	}
	public Cursor getByItem(String tbName, String key, String value, String orderBy){
		return db.query(tbName, null, key + " = ? ", new String[] { value }, null, null, orderBy);
	}
	
	public Cursor getByMoreItem(String tbName, String key, String[] value, String orderBy){
		return db.query(tbName, null, key, value, null, null, orderBy);
	}
	
	/**
	 * 根据某一条件进行相似查询
	 * @param key
	 * @return
	 */
	public Cursor getByItemLike(String tbName, String key, String value){
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
		String where = key + " like '%" + value + "%'";
		return db.query(tbName, null, where, null, null, null, null);
	}
	
	/**
	 * @param tbName
	 * @param colunm
	 * @param value
	 * @return
	 */
	public Cursor get(String sql) {
		return db.rawQuery(sql, null);
	}

}
