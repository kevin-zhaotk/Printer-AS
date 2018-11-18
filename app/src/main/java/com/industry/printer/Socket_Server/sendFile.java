package com.industry.printer.Socket_Server;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.R.bool;
import android.content.Context;

/*public class sendFile 
{
	private FileInputStream fileInputStream;
	private int bufferSize=1024;
	private FileOutputStream outputStream;
	private FileInputStream inputStream;
	private String receiveFilePath;
	private Context context;
	private FileOutputStream fileOutputStream;
	private byte[] buffer = new byte[this.bufferSize];
	private int total = 0;
	private int receiveFileSize;
	private boolean receiveFileState;
	private String transferFileOver;
	public sendFile(Context context)
	{
		this.context=context;

	}
	public sendFile (String filePath)
	{
    
	try
    {
        File file = new File(filePath);

        //发送文件
        this.fileInputStream = new FileInputStream(file);
       
       // _Log.e(_Log.msg() + "我们正在发送文件先生，");
        while (true)
        {
            int length = this.fileInputStream.read(buffer);
            this.outputStream.write(buffer);
            this.outputStream.flush();
            if (length == -1)
            {
               // _Log.e(_Log.msg() + "这就是当-1的时候我们读文件结束了");
                break;
            }
            total += length;
        }
       //_Log.e(_Log.msg() + "我们按传输所计算的文件大小为：" + total + "bit");
    }
    catch (FileNotFoundException e)
    {
        e.printStackTrace();
    }
   
   // _Log.e(_Log.msg() + "文件发送结束我们应该等待客户端的确认，然后关闭等待对话框");




this.fileOutputStream = new FileOutputStream(this.receiveFilePath);
boolean first = true;
//_Log.e(_Log.msg() + "文件正在被接受");
while (true)
{
  if (!first)
  {
     // _Log.e(_Log.msg() + "正在等待接收吓一跳");
      total = this.inputStream.read(buffer);
      //_Log.e(_Log.msg() + "接收到的数组长度为+" + total);
      if (new File(this.receiveFilePath).length() >= this.receiveFileSize)
      {
          //_Log.e(_Log.msg() + "读取文件时因为到末尾时返回-1而结束的 = " + this.receiveFileSize);
          break;
      }
  }
  else
  {
      first = false;
  }
  //我们暂且认为这里FileOutputStream(path)会自动创建这个不存在的路径，如果
  this.fileOutputStream.write(buffer, 0, total);
}
this.fileOutputStream.close();
//_Log.e(_Log.msg() + "传输完成了");
//endregion传输文件完成
this.receiveFileState = false;
//_Log.e(_Log.msg() + "我们在1秒钟之后告知客户我们已经接受文件完成的这件事");
Thread.sleep(1000);
//sendFile(this.context,"files");//发送文件接收完成标志
//_Log.e(_Log.msg() + "我们已经说完了，就是不知道其受到没有");

//这个没有成功
if (new File(this.receiveFilePath).length() >= this.receiveFileSize)
      {
         // _Log.e(_Log.msg() + "读取文件时因为到末尾时返回-1而结束的 = " + this.receiveFileSize);
          break;
      }
//这个也没有成功
this.receiveFileSize -= total;
if (this.receiveFileSize <= 0)
{
//Log.e(_Log.msg() + "读取文件时因为到末尾时返回-1而结束的 = " + this.receiveFileSize);
break;
}
}
}*/
//还试过让发送方Socket发送完成文件后发送一个字符串命令，
//我让发送文件后，使其sleep(1000)，在发送命令，依然会和文件数据 混在一起
