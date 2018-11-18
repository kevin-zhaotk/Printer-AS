package com.industry.printer.Socket_Server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

import android.os.Environment;
import android.os.Handler;
import android.os.Message;

public class SocketFilesServer {
	private ServerSocket server;
	private Handler handler = null;
	public SocketFilesServer(Handler handler){
		this.handler = handler;
		int port = 9999;
		while(port > 9000){
			try {
				server = new ServerSocket(port);
				break;
			} catch (Exception e) {
				port--;
			}
		}
		SendMessage(1, port);
		Thread receiveFileThread = new Thread(new Runnable(){
			@Override
			public void run() {
				while(true){//接收文件
					ReceiveFile();
				}
			}
		});
		receiveFileThread.start();
	}
	void SendMessage(int what, Object obj){
		if (handler != null){
			Message.obtain(handler, what, obj).sendToTarget();
		}
	}
	//接收文件
	void ReceiveFile(){
		try{
			//接收文件名
			Socket name = server.accept();
			InputStream nameStream = name.getInputStream();
			InputStreamReader streamReader = new InputStreamReader(nameStream);
			BufferedReader br = new BufferedReader(streamReader);
			String fileName = br.readLine();
			
			String savePath=fileName.substring(fileName.indexOf("/")+1,fileName.lastIndexOf("/"));
			fileName=fileName.substring(0, fileName.indexOf("#"));
        	String[] sApath = savePath.split("\\/");
			
			br.close();
			streamReader.close();
			nameStream.close();
			name.close();
			//SendMessage(0, "正在接收:" + fileName);
			//接收文件数据
			Socket data = server.accept();
			InputStream dataStream = data.getInputStream();
			String SsavePath= Environment.getExternalStorageDirectory().getPath() + "/MSG/";// + fileName;
			String LastPath="";
			//String SDCarePath=Environment.getExternalStorageDirectory().toString();
	       // final String path=SDCarePath+"/wj/"+ fileName;;
	        
			//for(int i=0;i<sApath.length;i++)
			//{
				//LastPath+=sApath[i].toString()+"/";
				
			File dir = new File(SsavePath+sApath[1]);//LastPath); // 创建文件的存储路径
			if (!dir.exists()) {
				dir.mkdirs();
			}
			//}
			
			
			
			FileOutputStream file = new FileOutputStream(SsavePath+sApath[1]+"/"+ fileName, false);
			byte[] buffer = new byte[1024];
			int size = -1;
			while ((size = dataStream.read(buffer)) != -1){
				file.write(buffer, 0 ,size);
			}
			file.close();
			dataStream.close();
			data.close();
			SendMessage(0, fileName + " 接收完成");
		}catch(Exception e){
			SendMessage(0, "接收错误:\n" + e.getMessage());
		}
	}
	
}
