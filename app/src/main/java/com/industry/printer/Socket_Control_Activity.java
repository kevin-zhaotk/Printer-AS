package com.industry.printer;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketTimeoutException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.json.JSONException;
import org.json.JSONObject;

import com.industry.printer.Socket_Server.LinearLayoutBaseAdapter;
import com.industry.printer.Socket_Server.LinearLayoutViews;
import com.industry.printer.Socket_Server.LinearLayoutViews.OnItemClickListener;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Socket_Server.ObserverCallBack;
import com.industry.printer.Socket_Server.QueryDataThread;
import com.industry.printer.Socket_Server.SendDataThread;
import com.industry.printer.Socket_Server.SocketFilesServer;

import com.industry.printer.Socket_Server.ConnectDataThread;
import com.industry.printer.Socket_Server.DialogShow_Activity;
import com.industry.printer.Socket_Server.Db.Server_Socket_Database;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.DialogInterface.OnKeyListener;
import android.database.Cursor;
import android.graphics.Color;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.renderscript.Allocation;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

public class Socket_Control_Activity extends Activity {
	private TextView But_Print,Scan_device,But_pur,But_Stop;
	//private Button ButBack;
	private String sDeviceNumber = "";
	private String sInK = "";
	private String sCounts = "";
	private String sIp;
	private String sPort;

	public static final int RESULT_OK = 1;
	public static final int BACK_OK = 0;
	private Server_Socket_Database Db;
	private Cursor Cr;
	private AlertDialog dialog;
	private String InsertSql;
	private String UpDAtaSql;
	private String QuerySql;
	private int Flag = 0;
	private LoadingDialog_Activity ShowDialog;
	private static DialogShow_Activity.Builder builder;
	private Handler handler;
	private SocketFilesServer socketManager;
	private ArrayList<String> fileName = new ArrayList<String>();
	private ArrayList<String> safeFileName = new ArrayList<String>();
	private List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
	private static Map<String, Object> mapFilll =  new HashMap<String, Object>();
	boolean isFolder;
	private TextView txtink,txttimes;
	int nCounts = 0;
	private String[] sValues;
	private ArrayList<String> All_Divece_ID= new ArrayList<String>();
	private ArrayList<String> All_Print_Path= new ArrayList<String>();
	private String sSendpath;
	private String PrintPath;
	private MyAdapter adapter;
	private LinearLayoutViews forListView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.socket_control_activity);
		But_Print= (TextView) findViewById(R.id.But_Print);
		Scan_device = (TextView) findViewById(R.id.Scan_device);
		But_pur= (TextView) findViewById(R.id.But_pur);
		But_Stop= (TextView) findViewById(R.id.But_Stop);
		
		//ButBack = (Button) findViewById(R.id.ButBack);
		builder = new DialogShow_Activity.Builder(Socket_Control_Activity.this);
		ShowDialog = new LoadingDialog_Activity(this);
		Db = new Server_Socket_Database(Socket_Control_Activity.this);
		forListView = (LinearLayoutViews) findViewById(R.id.listview);
		//GetSdPath();
		BeginData();
		fillDataInfo();

		 adapter = new MyAdapter(this, list);
	
		forListView.setAdapter(adapter);
		txtink = (TextView) forListView.findViewById(R.id.txtink);
		txttimes = (TextView) forListView.findViewById(R.id.txttimes);
		
		But_Print.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				int i = -1;
			
				ShowDialog.show();
				ShowDialog.setCanceledOnTouchOutside(false);
				Cr = Db.AllData_SqlData("select * from printer_info");
				
				while (Cr.moveToNext()) {
					i++;
					All_Divece_ID.add(i, Cr.getString(Cr.getColumnIndex("device_id")));
					All_Print_Path.add(i, Cr.getString(Cr.getColumnIndex("printer_path")));

				}
				Db.close();
				Cr.close();
				
				
				for(int y=0;y<All_Divece_ID.size();y++)
				{
					sDeviceNumber=All_Divece_ID.get(y);
					PrintPath=All_Print_Path.get(y);
					MakeIP();
				
					
					
					
					new Thread(RepeatThreadSendPrint).start();
				}
					
					
				

			}

		});
		But_pur.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				int i = 0;
				
				ShowDialog.show();
				ShowDialog.setCanceledOnTouchOutside(false);
				while (Cr.moveToNext()) {
					i++;
					All_Print_Path.add(i, Cr.getString(Cr.getColumnIndex("device_id")));
					All_Print_Path.add(i, Cr.getString(Cr.getColumnIndex("printer_path")));

				}
				Db.close();
				Cr.close();
				
				
				for(int y=0;y<All_Divece_ID.size();y++)
				{
					sDeviceNumber=All_Divece_ID.get(y);
					PrintPath=All_Print_Path.get(y);
					MakeIP();
				
					
					
					
					new SendDataThread(sIp, 3550, "000B|0000|200|0|0|0000|0|0000|0D0A",
							Socket_Control_Activity.this, callbackData).start();
				}

			}

		});
		
		But_Stop.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
	int i = 0;
				
				ShowDialog.show();
				ShowDialog.setCanceledOnTouchOutside(false);
				while (Cr.moveToNext()) {
					i++;
					All_Print_Path.add(i, Cr.getString(Cr.getColumnIndex("device_id")));
					All_Print_Path.add(i, Cr.getString(Cr.getColumnIndex("printer_path")));

				}
				Db.close();
				Cr.close();
				
				
				for(int y=0;y<All_Divece_ID.size();y++)
				{
					sDeviceNumber=All_Divece_ID.get(y);
					PrintPath=All_Print_Path.get(y);
					MakeIP();
				
					
					
					
					new SendDataThread(sIp, 3550, "000B|0000|500|0|0|0000|0|0000|0D0A",
							Socket_Control_Activity.this, callbackData).start();
				}


			}

		});
		Scan_device.setOnClickListener(new OnClickListener() {

			@Override

			public void onClick(View arg0) {
				/*ShowDialog.show();
				 
				Db.deleteAll("device_info");
				Db.close();
				
				
				 //mThreadScanDevice.start();
			  new Thread(mThreadScanDevice).start();*/
				Dialog_SureCancel();

			}

		});


	}

Runnable mThreadScanDevice = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			ScanDevice();
			ScanHander.sendEmptyMessage(0);
			try {
				Thread.sleep(3000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
		}
	};
	/*Thread mThreadScanDevice = new Thread() {
		@Override
		public void run() {
           
			
            
			ScanDevice();
			ScanHander.sendEmptyMessage(0);

		}
	};*/
	private Handler ScanHander=new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case 0: {
				//CleanList();
				UpDataList();
				
			}
				break;
			default:
				break;
			}
		}
	};
	
Runnable RepeatThreadSendPrint = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			 
				hander.sendEmptyMessage(0);

			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
		}
	};		
Runnable mThreadSendFiles = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			   String ss=safeFileName.get(0).toString();
	            sSendpath=ss.substring(ss.indexOf("MSG"),ss.lastIndexOf("/"));
	            //ss=sSendpath;
				
	            
				SendFile(fileName, safeFileName, sIp, 9999);
				hander.sendEmptyMessage(0);

			try {
				Thread.sleep(3000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
		}
	};

	/*Thread mThreadLoadApps = new Thread() {
		@Override
		public void run() {
            String ss=safeFileName.get(0).toString();
            Print_Path=ss.substring(ss.indexOf("printer"),ss.lastIndexOf("/"));
            ss=sSendpath;
			
            
			SendFile(fileName, safeFileName, sIp, 9999);
			hander.sendEmptyMessage(0);

		}
	};*/


	private Handler hander = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case 0: {
				
				new SendDataThread(sIp, 3550, "000B|0000|100|/"+PrintPath+"/|0|3702|0000|0|0000|0D0A",
						Socket_Control_Activity.this, callbackData).start();
			}
				break;
			default:
				break;
			}
		}
	};

int getFiles(String path) {
		File file = new File(path);
		try {
		File[] files = file.listFiles();
		
		for(int i =0;i<files.length;i++){
			fileName.add(files[i].getAbsoluteFile().getName());
			safeFileName.add(files[i].getAbsolutePath());
			}
		
		return 0;
		
		}
		catch (Exception e) {
			e.printStackTrace();
			// myApp.SetLogVents("99_进入e.printStackTrace();"+"\r\n");
			return -1;
		}
	}
	private void BeginData() {
		
		Map<String, Object> map;
		list.clear();
		for(int i=1;i<21;i++){
			map = new HashMap<String, Object>();
			map.put("Device", ""+i);
			map.put("Ink", "");
			map.put("Counts", "");
			map.put("device_ip", "");
			list.add(map);

		}
		
		
	}
	private void initData() {
		Cr = Db.AllData_SqlData("select * from device_info");
		Map<String, Object> map;
		list.clear();
		while (Cr.moveToNext()) {
			map = new HashMap<String, Object>();
			map.put("Device", Cr.getString(Cr.getColumnIndex("device_id")));
			map.put("Ink", Cr.getString(Cr.getColumnIndex("device_ink")));
			map.put("Counts", Cr.getString(Cr.getColumnIndex("device_print_counts")));
			list.add(map);

		}
		Db.close();
		Cr.close();
		
	}
	private void UpDataList() {
		Cr = Db.AllData_SqlData("select * from device_info");
		Map<String, Object> map = null;
		list.clear();
		forListView.changedIF.changed();
		CleanList();
		while (Cr.moveToNext()) {
			map = new HashMap<String, Object>();
			map.put("Device", Cr.getString(Cr.getColumnIndex("device_id")));
			map.put("Ink", Cr.getString(Cr.getColumnIndex("device_ink")));
			map.put("Counts", Cr.getString(Cr.getColumnIndex("device_print_counts")));
			list.add(map);

		}
		Db.close();
		Cr.close();
		 adapter = new MyAdapter(Socket_Control_Activity.this, list);
		
		forListView.setAdapter(adapter);
		ShowDialog.dismiss();
	}
	
	private void DataUpDataList()
	{
		list.clear();
		forListView.changedIF.changed();
		CleanList();
		BeginData();
		fillDataInfo();
		adapter = new MyAdapter(Socket_Control_Activity.this, list);
		forListView.setAdapter(adapter);
		
		
	}
	private void fillDataInfo() {
		
		int nRows=0;
		Cr = Db.AllData_SqlData("select * from device_info");
		
	
		
		while (Cr.moveToNext()) {
			
			
			//mapFilll.put("device_ip", Cr.getString(Cr.getColumnIndex("device_ip")));
			//mapFilll.put("Ink", Cr.getString(Cr.getColumnIndex("device_ink")));
			//mapFilll.put("Counts", Cr.getString(Cr.getColumnIndex("device_print_counts")));
			 ((Map)list.get(nRows)).put("device_id",Cr.getString(Cr.getColumnIndex("device_ip")));//
			
			 nRows++;
		}
		
		
		Db.close();
		Cr.close();
		
	}
	private void CleanList() {
		
		Map<String, Object> map = null;
		list.clear();
		
		
		adapter = new MyAdapter(Socket_Control_Activity.this, list);
		
		forListView.setAdapter(adapter);
		
		ShowDialog.dismiss();
	}

	class MyAdapter extends LinearLayoutBaseAdapter {
		public MyAdapter(Context context, List<Map<String, Object>> list) {
			super(context, list);
		}

		protected View getView(int position) {
			View view = getLayoutInflater().inflate(R.layout.socket_control_activity_item, null);
			final TextView Device_id = (TextView) view.findViewById(R.id.txtDevice_number);
			TextView txtink = (TextView) view.findViewById(R.id.txtink);
			TextView txttimes = (TextView) view.findViewById(R.id.txttimes);
			TextView ip_add= (TextView) view.findViewById(R.id.ip_add);
			LinearLayout Devices=(LinearLayout)view.findViewById(R.id.Devices);
			TextView Linear_print= (TextView) view.findViewById(R.id.Linear_print);
			TextView Linear_Purget= (TextView) view.findViewById(R.id.Linear_Purget);
			TextView Linear_Stop= (TextView) view.findViewById(R.id.Linear_Stop);
			TextView Linear_Query= (TextView) view.findViewById(R.id.Linear_Query);
			TextView Linear_Set= (TextView) view.findViewById(R.id.Linear_Set);
			Device_id.setText((String) list.get(position).get("Device"));
			txtink.setText((String) list.get(position).get("Ink"));
			txttimes.setText((String) list.get(position).get("Counts"));
			ip_add.setText((String) list.get(position).get("device_id"));
			
			Devices.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					sDeviceNumber=Device_id.getText().toString();
					int i = 0;
					Intent intent = new Intent();
					intent.putExtra("Device", sDeviceNumber);
					setResult(RESULT_OK, intent);
					finish();
					// Flag=1;

				}

			});
			Linear_print.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					sDeviceNumber=Device_id.getText().toString();
					sIp="";
					MakeIP();
					if("".equals(sIp)||(sIp==null))
					{
						DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_ip_ed));
						
						return;
					}
					if(Configs.Device_Printer_Path==null||"".equals(Configs.Device_Printer_Path))
					{
						Cr = Db.AllData_SqlData("select * from printer_info where device_id='" + sDeviceNumber + "'");
						while (Cr.moveToNext()) {
							PrintPath=Cr.getString(Cr.getColumnIndex("printer_path"));
						}
						
						Db.close();
						Cr.close();
						if(PrintPath==null||"".equals(PrintPath))
						{
							DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_print_documents));
							
							return;
						}
						else
						{
						ShowDialog.show();
						ShowDialog.setCanceledOnTouchOutside(false);
						new Thread(RepeatThreadSendPrint).start();
						return;
						}
						
					}
					
					String ss=Configs.Device_Printer_Path;
					PrintPath=ss.substring(ss.indexOf("/")+1,ss.lastIndexOf("/"));
					sDeviceNumber=Device_id.getText().toString();
					
					ShowDialog.show();
					
					
					/*Intent intent = new Intent();
					intent.putExtra("Device", sDeviceNumber);
					setResult(RESULT_OK, intent);
					finish();*/
					
					//return;
					// Flag=1;
					Print_Files();

				}

			});
			Linear_Purget.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					sDeviceNumber=Device_id.getText().toString();
					sIp="";
					ShowDialog.show();
					
					MakeIP();
					
					if("".equals(sIp)||(sIp==null))
					{
						
						ShowDialog.dismiss();
						DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_ip_ed));
						
						
						return;
					}
					new SendDataThread(sIp, 3550, "000B|0000|200|0|0|0000|0|0000|0D0A",
							Socket_Control_Activity.this, callbackData).start();//

				}

			});

			Linear_Stop.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					sDeviceNumber=Device_id.getText().toString();
					sIp="";
					ShowDialog.show();
					
					MakeIP();
					if("".equals(sIp)||(sIp==null))
					{
						
						ShowDialog.dismiss();
						DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_ip_ed));
						
						return;
					}
					new SendDataThread(sIp, 3550, "000B|0000|500|0|0|0000|0|0000|0D0A",
							Socket_Control_Activity.this, callbackData).start();//
					// Flag=1;

				}

			});
			Linear_Query.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					sDeviceNumber=Device_id.getText().toString();
					sIp="";
					ShowDialog.show();
					
					MakeIP();
					
					if("".equals(sIp)||(sIp==null))
					{
						ShowDialog.dismiss();
						DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_ip_ed));
						
						
						
						return;
					}
					new QueryDataThread(sIp, 3550, "000B|0000|400|0|0|0000|0|0000|0D0A",
							Socket_Control_Activity.this, callbackData).start();//
					// Flag=1;

				}

			});
			Linear_Set.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					sDeviceNumber=Device_id.getText().toString();
					sIp="";
					
					MakeIP();
					
					inputTitleDialog(sDeviceNumber, sCounts, sInK, Socket_Control_Activity.this);
					Flag = 1;
				}

			});
			return view;
		}

	}

	private void ScanDevice() {
		Socket socket = new Socket();

		String mLocalIP = getIPAddressStr(this);
		String[] prefixArr = mLocalIP.split("\\.");
		mLocalIP = prefixArr[0] + "." + prefixArr[1] + "." + prefixArr[2] + ".";
		String TmpIp = "";

		for (int i = 1; i < 255; i++) {

			TmpIp = mLocalIP + i;

			new ConnectDataThread(TmpIp, 3550, Socket_Control_Activity.this).start();// 消息发送方启动线程发送消息

		}
		
	}

	public String getIPAddressStr(Context context) {

		int ipAddress = getIPAddress(context);
		if (ipAddress == 0) {
			return "";
		} else {
			return int2ip(ipAddress);
		}
	}

	private static int getIPAddress(Context context) {

		WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);

		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		if (wifiInfo != null) {
			return wifiInfo.getIpAddress();
		}
		return 0;
	}

	private static String int2ip(int ipInt) {
		StringBuilder sb = new StringBuilder();
		sb.append(ipInt & 0xFF).append(".");
		sb.append((ipInt >> 8) & 0xFF).append(".");
		sb.append((ipInt >> 16) & 0xFF).append(".");
		sb.append((ipInt >> 24) & 0xFF);
		return sb.toString();
	}

	public void inputTitleDialog(final String position, String count, String inks, Context context) {
		dialog = new AlertDialog.Builder(this).create();
		dialog.setView(LayoutInflater.from(this).inflate(R.layout.dialogshow_two_button, null));
		
		dialog.show();
		dialog.setCanceledOnTouchOutside(false);
		// dialog.getWindow().setContentView(R.layout.alert_dialog);
		Button BtnSave = (Button) dialog.findViewById(R.id.BtnSave);
		Button BtnCancel = (Button) dialog.findViewById(R.id.BtnCancel);
		TextView Devce_Nubmer = (TextView) dialog.findViewById(R.id.Devce_Nubmer);
		final EditText Ip_Add;
		final EditText Ports;
		Flag = 0;
		Ip_Add = (EditText) dialog.findViewById(R.id.Ip_Add);
		Ports = (EditText) dialog.findViewById(R.id.Ports);
		Ip_Add.setText(sIp);
		Ports.setText("3550");
		Ports.setEnabled(false);
		Devce_Nubmer.setText(context.getString(R.string.str_device_number) + sDeviceNumber);
		InsertSql = "insert into device_info values(null,'" + position + "','" + count + "','" + inks + "','";
		
		QuerySql="select * from device_info where device_id ='" + position + "'";
		BtnSave.setOnClickListener(new OnClickListener() {

			public void onClick(View arg0) {
				String str = Ip_Add.getText().toString();
				if (isNullEmptyBlank(str)) {
					Ip_Add.setError("Ip address can not be empty");
					return;
				}
				String strPort = Ports.getText().toString();
				if (isNullEmptyBlank(strPort)) {
					Ip_Add.setError("The port can not be empty");
					return;
				}
				InsertSql += Ip_Add.getText().toString() + "','" + Ports.getText().toString() + "');";
				UpDAtaSql = "update device_info set device_id ='" + position + "',device_ip='" + Ip_Add.getText().toString()
						+ "',device_port='" + Ports.getText().toString() + "'" + " where device_id='" + position + "'";
				
				Db.InsertData(InsertSql,UpDAtaSql, QuerySql);
				DataUpDataList();
				dialog.dismiss();

			}
		});
		BtnCancel.setOnClickListener(new OnClickListener() {

			public void onClick(View arg0) {
				
				dialog.dismiss();

			}
		});

	}

	private static boolean isNullEmptyBlank(String str) {
		if (str == null || "".equals(str) || "".equals(str.trim()))
			return true;
		return false;
	}

	private ObserverCallBack callbackData = new ObserverCallBack() {
		public void back(int data, int url,String[] Values) {
			
			switch (url) {
			case 1: // 进行数据解析

				try {
					// Index index = new Index();
					// index = index.convertHttpHead(context,data);
					//// App.index = index;
					// Log.i(tag, index.toString());
					// myApp.SetLogVents("98_进入callbackData;"+data+"\r\n");
					// writeLog.recordLog("Login_"+data);
					sValues=Values;
					Message msg = new Message();
					msg.what = 1;
//					msg.obj = index;
					mHandler.sendMessage(msg);
					
				} catch (Exception e) {
					e.printStackTrace();
					// myApp.SetLogVents("99_进入e.printStackTrace();"+"\r\n");
				}
				break;
			case 2: 
			{
				
				Message msg = new Message();
				msg.what = 2;
				// msg.obj = 2;
				mHandler.sendMessage(msg);
				break;
			}
			default:
				Message msg = new Message();
				msg.what = url;
				mHandler.sendMessage(msg);
				break;
			}
		}
	};


	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			Intent intent = null;
			switch (msg.what) {
			case 1:
			
				try {
					JsonData(sValues);
				} catch (JSONException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					
				}
				//Toast.makeText(Fina_Login_Activity.this, "测试数据 数据编号�?+datas, Toast.LENGTH_SHORT).show();
				break;
			
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
//				setListViewHeightBasedOnChildren(mListView);
//				setListViewHeightBasedOnChildren(mListView);
				break;
			case 8:
//				Bitmap bmp = (Bitmap)msg.obj;
//				SendMsg.weixin(context, mCategory, bmp);
				break;
			default:
				//Toast.makeText(Fina_Login_Activity.this, "测试数据 数据编号�?+msg.what, Toast.LENGTH_SHORT).show();
				break;
			}
		};
	};



 
public void JsonData(String[] s) throws JSONException
	{
		
	       analyticJson(s);  
	       
	}
 public void analyticJson(String[] result) throws JSONException{  
	 sIp="";
	 if(result==null)
	 {   ShowDialog.dismiss();
		 return;
	 }
	 txtink.setText(result[1].toString());
	 txtink.setTextColor(Color.parseColor("#fff600")); 
		txttimes.setText(result[2].toString());
		txttimes.setTextColor(Color.parseColor("#fff600")); 
		
		ShowDialog.dismiss();
		}
	
	
	/**
	 * 处理UI线程中数�?
	 */
	/*private Handler mHandler = new Handler() {
		
		public void handleMessage(Message msg) {
			
			Intent intent = null;
			switch (msg.what) {
			case 1: {
				// Flag=0;
				// Toast.makeText(Fina_Login_Activity.this, "测试数据 数据编号�?+datas,
				// Toast.LENGTH_SHORT).show();
				ShowDialog.dismiss();
				break;
			}
			case 2:
			{
				//String ss=sValues[2].toString();
				//ss=sValues[3].toString();
				
				//txtink.setText(sValues[2].toString());
				//txttimes.setText(sValues[3].toString());
				ShowDialog.dismiss();
				break;
			}
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				// setListViewHeightBasedOnChildren(mListView);
				// setListViewHeightBasedOnChildren(mListView);
				break;
			case 8:
				// Bitmap bmp = (Bitmap)msg.obj;
				// SendMsg.weixin(context, mCategory, bmp);
				break;
			default:
				// Toast.makeText(Fina_Login_Activity.this, "测试数据 数据编号�?+msg.what,
				// Toast.LENGTH_SHORT).show();
				break;
			}
			
		};
	};*/

	OnKeyListener keylistener = new DialogInterface.OnKeyListener() {
		public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
			if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
				return true;
			} else {
				return false;
			}
		}
	};

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// 选择了文件发送
		//if (resultCode == RESULT_OK) {
		/*if (resultCode == 1) {
			fileName = data.getStringArrayListExtra("fileName");
			safeFileName = data.getStringArrayListExtra("safeFileName");
			
			Cr = Db.AllData_SqlData("select * from printer_info where device_id='" + sDeviceNumber + "'");
			while (Cr.moveToNext()) {
				fileName=getFiles(Cr.getString(Cr.getColumnIndex("printer_path")));
			}
			
			
			Cr = Db.AllData_SqlData("select * from device_info where device_id='" + sDeviceNumber + "'");
			ShowDialog.show();
			while (Cr.moveToNext()) {

				sIp = Cr.getString(Cr.getColumnIndex("device_ip"));
				sPort = Cr.getString(Cr.getColumnIndex("device_port"));

			}
			Db.close();
			Cr.close();

			ShowDialog.setCanceledOnTouchOutside(false);
			new Thread(mThreadSendFiles).start();
		}*/
	}
    private void All_Print_Files(String Device_id)
    {
    	String Print_Path = "";
    		

		Cr = Db.AllData_SqlData("select * from printer_info where device_id='" + Device_id + "'");
		while (Cr.moveToNext()) {
			Print_Path=Cr.getString(Cr.getColumnIndex("printer_path"));
		}
		
		Db.close();
		Cr.close();
    	
    		if((Print_Path==null)||("".equals(Print_Path)))
    		{
    			DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_print_documents));
				
				return;
    			
    		}
    		
    			getFiles(Print_Path);
    			
    		
    		ShowDialog.setCanceledOnTouchOutside(false);
    		new Thread(mThreadSendFiles).start();
    	
    }
    private void Send_SetSystem_Files() {
		String SavaPaths;
		/*Intent intent = new Intent(getApplicationContext(), FilesViewActivity.class);
		// intent.setClass();
		startActivityForResult(intent, 0);*/
		String TmPath="";
		//Configs.Device_Printer_Path="1/22";
		String SDCarePath=Environment.getExternalStorageDirectory().toString();
		 String path=SDCarePath+"/system/";
		 
		
		
		
		
			int nRet=getFiles(path);
			if(nRet==-1)
			{
				return;
			}
			else {
			
		
				ShowDialog.setCanceledOnTouchOutside(false);
				new Thread(mThreadSendFiles).start();
				 Print_Files();
				
			}
	}
	
	private void Print_Files() {
		String SavaPaths;
		/*Intent intent = new Intent(getApplicationContext(), FilesViewActivity.class);
		// intent.setClass();
		startActivityForResult(intent, 0);*/
		String TmPath="";
		//Configs.Device_Printer_Path="1/22";
		String SDCarePath=Environment.getExternalStorageDirectory().toString();
		 String path=SDCarePath+"/MSG/"+Configs.Device_Printer_Path;
		 
		
		
		Cr = Db.AllData_SqlData("select * from printer_info where device_id='" + sDeviceNumber + "'");
		while (Cr.moveToNext()) {
			TmPath=Cr.getString(Cr.getColumnIndex("printer_path"));
		}
		
		Db.close();
		Cr.close();
		if((Configs.Device_Printer_Path==null)&&("".equals(TmPath)))
		{
			ShowDialog.dismiss();
			DialogShows(Socket_Control_Activity.this,getString(R.string.str_dialog_surecancel),getString(R.string.str_print_documents));
			return;
		}
		else if(!"".equals(Configs.Device_Printer_Path))
		{
			getFiles(path);
			SavaPaths=path;
		}
		else 
		{
			getFiles(TmPath);
			SavaPaths=TmPath;
		}
		FillPrintData(PrintPath);
		Configs.Device_Printer_Path="";
		SavaPaths="";
		TmPath="";
		
		ShowDialog.setCanceledOnTouchOutside(false);
		new Thread(mThreadSendFiles).start();
	}
	
	private void FillPrintData(String Paths)
	{
		InsertSql = "insert into printer_info values(null,'"+sDeviceNumber+ "','" + Paths + "','0');";
		UpDAtaSql = "update printer_info set device_id ='" + sDeviceNumber + "',printer_path='" + Paths
				 + "' where device_id='" + sDeviceNumber + "'";
		QuerySql="select * from printer_info where device_id='" + sDeviceNumber + "'";
		Db.InsertData(InsertSql,UpDAtaSql, QuerySql);
	}
	public void SendFile(ArrayList<String> fileName, ArrayList<String> path, String ipAddress, int port) {
		try {
			for (int i = 0; i < fileName.size(); i++) {
				Socket name = new Socket(ipAddress, port);
				OutputStream outputName = name.getOutputStream();
				OutputStreamWriter outputWriter = new OutputStreamWriter(outputName);
				BufferedWriter bwName = new BufferedWriter(outputWriter);

				bwName.write(fileName.get(i) + "#" + path.get(i).substring(path.get(i).indexOf("MSG")));
				bwName.close();
				outputWriter.close();
				outputName.close();
				name.close();
				// SendMessage(0, "正在发送" + fileName.get(i));

				Socket data = new Socket(ipAddress, port);
				OutputStream outputData = data.getOutputStream();
				FileInputStream fileInput = new FileInputStream(path.get(i));
				int size = -1;
				byte[] buffer = new byte[1024];
				while ((size = fileInput.read(buffer, 0, 1024)) != -1) {
					outputData.write(buffer, 0, size);
				}
				outputData.close();
				fileInput.close();
				data.close();
				// SendMessage(0, fileName.get(i) + " 发送完成");
			}

		} catch (Exception e) {
			// SendMessage(0, "发送错误:\n" + e.getMessage());
		}

	}

	public void MakeIP() {
		
		Cr = Db.AllData_SqlData("select * from device_info where device_id='" + sDeviceNumber + "'");
		
		while (Cr.moveToNext()) {

			sIp = Cr.getString(Cr.getColumnIndex("device_ip"));
			sPort = Cr.getString(Cr.getColumnIndex("device_port"));

		}
		
		Db.close();
		Cr.close();
		
	}	
public static void DialogShows(Context context,String Titles,String Message) {
		
		builder.setMessage( Message);
		builder.setTitle(Titles);
		builder.setPositiveButton(context.getString(R.string.str_device_but_sure), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				//设置你的操作事项
			}
		});

		

		builder.create().show();
	}

public void Dialog_SureCancel() {
	dialog = new AlertDialog.Builder(this).create();
	dialog.setView(LayoutInflater.from(this).inflate(R.layout.dialog_surecancel, null));
	
	dialog.show();
	dialog.setCanceledOnTouchOutside(false);
	// dialog.getWindow().setContentView(R.layout.alert_dialog);
	Button BtnSave = (Button) dialog.findViewById(R.id.BtnSave);
	Button BtnCancel = (Button) dialog.findViewById(R.id.BtnCancel);
	
	
	
	BtnSave.setOnClickListener(new OnClickListener() {

		public void onClick(View arg0) {
			ShowDialog.show();
			 
			Db.deleteAll("device_info");
			Db.close();
			
			
			 //mThreadScanDevice.start();
		  new Thread(mThreadScanDevice).start();
			dialog.dismiss();

		}
	});
	BtnCancel.setOnClickListener(new OnClickListener() {

		public void onClick(View arg0) {
			
			dialog.dismiss();

		}
	});

}
public void onBackPressed() {    
	super.onBackPressed();    
	           
}    
   
public boolean onKeyDown(int keyCode, KeyEvent event) {    
	if ((keyCode == KeyEvent.KEYCODE_BACK)) 
    {    
		   
		return false;    
    }
  else 
   {    
	return super.onKeyDown(keyCode, event);    
   }    
        
}    
   
protected void onDestroy() {    
super.onDestroy();    

}       
public String GetSdPath(){
	File sdDir = null;
	boolean sdCardExist = Environment.getExternalStorageState()
	.equals(android.os.Environment.MEDIA_MOUNTED); //判断
	if (sdCardExist)
	{
	   sdDir = Environment.getExternalStorageDirectory();//获取跟目�?
	}
	 
           
          
            File destDir = new File( sdDir.toString()+"/MSG/");
           
	        if (!destDir.exists()) {
	         destDir.mkdirs();
	        }
	         /*destDir = new File( sdDir.toString()+"/MSG/");
	           
	        if (!destDir.exists()) {
	         destDir.mkdirs();
	        }*/
	      
	return sdDir.toString()+"/MSG/";

}
/*private String getFileName(File[] files) {
	  String str = "";
	        if (files != null) { // 先判断目录是否为空，否则会报空指针  
	            for (File file : files) { 
	                if (file.isDirectory()){//检查此路径名的文件是否是一个目录(文件夹)
	                    Log.i("zeng", "若是文件目录。继续读1"
	                     +file.getName().toString()+file.getPath().toString()); 
	                    getFileName(file.listFiles()); 
	                    Log.i("zeng", "若是文件目录。继续读2"
	                     +file.getName().toString()+ file.getPath().toString()); 
	                } else { 
	                    String fileName = file.getName(); 
	                    if (fileName.endsWith(".txt")) {
	                        String s=fileName.substring(0,fileName.lastIndexOf(".")).toString(); 
	                        Log.i("zeng", "文件名txt：：   " + s); 
	                        str += fileName.substring(0,fileName.lastIndexOf("."))+"\n";
	                    } 
	                } 
	            }
	           
	        }
	        return str;
	    }
	 
	}*/
}
