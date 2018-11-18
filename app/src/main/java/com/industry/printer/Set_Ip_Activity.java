package com.industry.printer;

import com.industry.printer.Socket_Server.Db.Server_Socket_Database;

import android.app.Activity;
import android.database.Cursor;
import android.os.Bundle;

public class Set_Ip_Activity extends Activity {
	private Server_Socket_Database Db;
	private Cursor Cr;
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.dialogshow_two_button);
		Db = new Server_Socket_Database(Set_Ip_Activity.this);
	}
	
}
