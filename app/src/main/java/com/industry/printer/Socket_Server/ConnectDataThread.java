package com.industry.printer.Socket_Server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import com.industry.printer.Socket_Server.Db.Server_Socket_Database;

import android.content.Context;
import android.database.Cursor;
import android.os.Environment;
import android.util.Log;

/**
 * 数据发送线程
 */
public class ConnectDataThread extends Thread {
    private Socket socket;
    private String ip="";//接收方的IP
    private int port=0;//接收方的端口号
    private Server_Socket_Database Db;
    Cursor rc;

    public ConnectDataThread(String ip, int port,Context context) {
        this.ip = ip;
       
        this.port = port;
    	Db=new Server_Socket_Database(context);
    }

    @Override
    public void run() {
        try {
            socket = new Socket();
            socket.connect(new InetSocketAddress(this.ip, this.port),500);//设置超时时间
        } catch (UnknownHostException e) {
          
            Log.d("error", "SendDataThread.init() has UnknownHostException" + e.getMessage());
        } catch (SocketTimeoutException e) {
          
            Log.d("error", "SendDataThread.init() has TimeoutException:" + e.getMessage());
        }catch (IOException e){
            Log.d("error", "SendDataThread.init() has IOException:" + e.getMessage());
        }
        if (socket != null&&socket.isConnected()) {
            try {
            	InputStream nameStream = socket.getInputStream();
				InputStreamReader streamReader = new InputStreamReader(nameStream);
			    BufferedReader br = new BufferedReader(streamReader);
				String RecevInfor = br.readLine();
		
				if(RecevInfor.indexOf("SoftwinerEvb")>=0) { 
			
					addDB(RecevInfor);
				}
				socket.close();
				nameStream.close();
				streamReader.close();
				br.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    	try {
			socket.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
	
   public void addDB(String Results)
   {
	  String Tmps=Results;
	   int nCounts=0;
	   rc=Db.getAll("device_info");
	   rc.moveToLast();
	   rc.moveToFirst();
	   nCounts=rc.getCount()+1;
	   String[] sValues = Tmps.split("##");
	 
	   String InsertSql = "insert into device_info values(null,'"+nCounts+"','"+sValues[1].toString()+"','"+sValues[2].toString()+"','";
	  String QuerySql = "select * from device_info where device_ip='"+ip+"'";
	  InsertSql +=ip+"','"+port+"');";
	  String UpDAtaSql = "update device_info set device_id ='" + nCounts + "',device_ip='" + ip
				+ "',device_port='" + port + "'" + " where device_id='" + nCounts + "'";
 	  Db.InsertData(InsertSql,UpDAtaSql,QuerySql);
 	  CreateDeviceDir(""+nCounts);
     
   }
 public void CreateDeviceDir(String Sdir)
 {
	 String SsavePath= Environment.getExternalStorageDirectory().getPath() + "/printer/"+ Sdir;
		
		
			
		File dir = new File(SsavePath); // 创建文件的存储路径
		if (!dir.exists()) {
			dir.mkdirs();
		}
		
 }
}