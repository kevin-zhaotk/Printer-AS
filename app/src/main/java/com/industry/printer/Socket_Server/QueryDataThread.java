package com.industry.printer.Socket_Server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
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
import android.util.Log;

public class QueryDataThread extends Thread {
    private Socket socket;
    private String ip="";//接收方的IP
    private int port=0;//接收方的端口号
    private Server_Socket_Database Db;
    private Cursor rc;
    private String sCommand;
    private ObserverCallBack callBack;
    private String[] sValues;
 
    public QueryDataThread(String ip, int port,String sCommand,Context context,ObserverCallBack callBack) {
       this.ip = ip;
       this.port = port;
       this.sCommand=sCommand;
      this.callBack=callBack;
       Db=new Server_Socket_Database(context);
    }

    @Override
    public void run() {
    	
        try {
            socket = new Socket();
            socket.connect(new InetSocketAddress(ip, port),1000);//设置超时时间
            
        } catch (UnknownHostException e) {
          
            Log.d("error", "SendDataThread.init() has UnknownHostException" + e.getMessage());
        } catch (SocketTimeoutException e) {
          
            Log.d("error", "SendDataThread.init() has TimeoutException:" + e.getMessage());
        }catch (IOException e){
            Log.d("error", "SendDataThread.init() has IOException:" + e.getMessage());
        }
        if (socket != null&&socket.isConnected()) {
   
        	OutputStream outputName = null;
			try {
				outputName = socket.getOutputStream();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			OutputStreamWriter outputWriter = new OutputStreamWriter(outputName);
			BufferedWriter bwName = new BufferedWriter(outputWriter);
			try {
				bwName.write(this.sCommand);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			try {
				bwName.flush();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
        	
        	try {
            	InputStream nameStream = socket.getInputStream();
				InputStreamReader streamReader = new InputStreamReader(nameStream);
			    BufferedReader br = new BufferedReader(streamReader);
				String RecevInfor = br.readLine();
		
				sValues = RecevInfor.split("##");
				
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
			callBack.back(1, 1,sValues);
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	//callBack.back(1, 2,sValues);
    }
	
   public void addDB()
   {
	  int nCounts=0;
	   rc=Db.getAll("device_info");
	   rc.moveToLast();
	   rc.moveToFirst();
	   nCounts=rc.getCount()+1;
	   
	   
	   String InsertSql = "insert into device_info values(null,'"+nCounts+"','"+"SoftwinerEvb"+"','"+0+"','";
	  String QuerySql = "select * from device_info where device_ip='"+ip+"'";
	  InsertSql +=ip+"','"+port+"');";
	  
	  String UpDAtaSql = "update device_info set device_id ='" + nCounts + "',device_ip='" + ip
				+ "',device_port='" + port + "'" + " where device_id='" + nCounts + "'";
		
 	  Db.InsertData(InsertSql,UpDAtaSql,QuerySql);
     
   }
}