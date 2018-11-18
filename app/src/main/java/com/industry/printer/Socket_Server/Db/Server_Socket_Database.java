package com.industry.printer.Socket_Server.Db;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Environment;
import android.widget.Toast;


public class Server_Socket_Database {
	
	/** 数据库名 */
	private static final String SDCARD_ROOT = Environment.getExternalStorageDirectory().toString();
	public static final String DB_NAME =SDCARD_ROOT+"/MSG/"+ "printer.db"; 
	
	private static SQLiteDatabase db; 
	private static Server_Socket_Create_Table dbManager;

	private static Server_Socket_Database instance;
	//________________________________APIs__________________________________
	public static Server_Socket_Database getInstance(Context context) {
		if (instance == null) {
			instance = new Server_Socket_Database(context);
		}
		return instance;
	}

	public Server_Socket_Database(Context context) {
		if (dbManager == null) {
			dbManager = new Server_Socket_Create_Table(context);
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
	
	
	public boolean InsertData(String InsertSql,String UpDAtaSql,String QuerySql)
	{
		if(db == null || !db.isOpen()) {
			db = dbManager.getWritableDatabase();
		}
		boolean result = true;
		try
		{
			Cursor cr=AllData_SqlData(QuerySql);
			if(cr.getCount()>0)
			{
				//ContentValues values = new ContentValues();
				//values.put("snumber","101003"); 
				//String whereClause = "id=?";  
				//String[] whereArgs={"1","2"};   
				//update("device_info",values,whereClause,whereArgs);
				//String sql = "update stu_table set snumber = 654321 where id = 1";   
				db.execSQL(UpDAtaSql);  
				return true;
			}
			else
			{
			db.execSQL(InsertSql);
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
