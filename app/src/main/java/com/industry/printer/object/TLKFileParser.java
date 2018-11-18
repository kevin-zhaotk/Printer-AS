package com.industry.printer.object;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.nio.Buffer;
import java.util.ArrayList;
import java.util.Vector;

import com.industry.printer.EditTabActivity;
import com.industry.printer.MessageTask;
import com.industry.printer.MessageTask.MessageType;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.FileFormat.TlkFile;
import com.industry.printer.Utils.ConfigPath;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.exception.PermissionDeniedException;
import com.industry.printer.object.BaseObject;
import com.industry.printer.object.LetterHourObject;

import android.R.bool;
import android.R.integer;
import android.content.Context;
import android.util.Log;

public class TLKFileParser  extends TlkFile{
	
	public static final String TAG="TLKFileParser";
	
	private int mDots[];
	private float mProportion = 1;
	
	public TLKFileParser(Context context, String file) {
		super(context, file);
		mDots = new int[8];
		Debug.d(TAG, "--->file: " + file);
		
	}
	
	/*
	public void parse(Context context, String name, Vector<BaseObject> objlist)
	{
		int i;
		BaseObject pObj;
		mContext = context;
		if(objlist == null)
			objlist = new Vector<BaseObject>();
		objlist.clear();
		File file = new File(name);
		if(file.isDirectory())
		{
			Debug.d(TAG, "this is a directory");
			return;
		}
		try{
			 InputStream instream = new FileInputStream(file); 
			 if(instream != null)
			 {
				 InputStreamReader inputreader = new InputStreamReader(instream,"UTF-8");
                 BufferedReader buffreader = new BufferedReader(inputreader);
                 String line;
                 
                 while (( line = buffreader.readLine()) != null) {
                     Debug.d(TAG, "line="+line);
                     pObj = parseLine(line);
                     if (pObj == null) {
                    	 continue;
                     }
                     objlist.add(pObj);
                     if(pObj instanceof RealtimeObject)
                     {
                    	 i = ((RealtimeObject) pObj).mSubObjs.size();
                    	 
                    	 for(int j=0; j<i ; j++)
                    	 {
                    		 line = buffreader.readLine();
                    		 Debug.d(TAG, "line="+line);
                    		 for(int k=0; k<i; k++)
                    		 {
                    			 BaseObject obj =  ((RealtimeObject) pObj).mSubObjs.get(k);
                    			 if(obj.getId().equals(line.substring(4, 7)))
                    			 {
                    				 ((RealtimeObject) pObj).mSubObjs.get(j).setIndex(Integer.parseInt(line.substring(0, 3)));
                    				 Debug.d(TAG, "pObj "+((RealtimeObject) pObj).mSubObjs.get(j).getId()+",index="+((RealtimeObject) pObj).mSubObjs.get(j).getIndex()); 
                            		 break;
                    			 }
                    		 }
                    		 
                    	 }
                     }
                 }
                 instream.close();
			 }
		}catch(Exception e)
		{
			Debug.d(TAG, "parse error: "+e.getMessage());
		}
	}
	*/
	public void parse(Context context, MessageTask task, ArrayList<BaseObject> objlist) throws PermissionDeniedException
	{
		int i;
		BaseObject pObj = null;
		mContext = context;
		if(objlist == null) {
			Debug.d(TAG, "objlist is null");
			return;
		}
		objlist.clear();
		File file = new File(mPath);
		if(file.isDirectory())
		{
			Debug.d(TAG, "File:" + mPath + " is directory");
			return;
		} else if (!file.exists()) {
			Debug.d(TAG, "File:" + mPath + " is not exist");
		}
		Debug.e(TAG, "==========>begin parse tlk");
		try{
			 InputStream instream = new FileInputStream(file); 
			 if(instream != null)
			 {
				 InputStreamReader inputreader = new InputStreamReader(instream,"UTF-8");
                 BufferedReader buffreader = new BufferedReader(inputreader);
                 String line;
                 
                 while (( line = buffreader.readLine()) != null) {
                     //Debug.d(TAG, "line="+line);
                     if (StringUtil.isEmpty(line.trim())) {
						continue;
					}
                     pObj = parseLine(task, line);
                     if (pObj == null) {
                    	 continue;
                     }
                     /** The first object must be MessageObject, create a default one otherwise */
                     if (objlist.size() == 0 && !(pObj instanceof MessageObject) ) {
                    	MessageObject msg = new MessageObject(context, 0);
                    	int type = SystemConfigFile.getInstance(context).getParam(30);
         				msg.setType(type);
         				//((MessageObject) obj).setDotCount(Integer.parseInt(attr[13]));
         				// mDots = Integer.parseInt(attr[13]);
         				objlist.add(msg);
                     }
                     objlist.add(pObj);
                     Debug.d(TAG, "--->objlist size= " + objlist.size());
                     if(pObj instanceof RealtimeObject)
                     {
                    	 i = ((RealtimeObject) pObj).mSubObjs.size();
                    	 for(int j=0; j<i ; j++)
                    	 {
                    		 line = buffreader.readLine();
                    		 while(StringUtil.isEmpty(line)) {
								line = buffreader.readLine();
							}
                    		 // Debug.d(TAG, "line="+line);
                    		 BaseObject obj =  ((RealtimeObject) pObj).mSubObjs.get(j);
                    		 parseSubObject(obj, line);
                    	 }
                     }
                 }
                 instream.close();
			 }
		}catch(IOException e)
		{
			Debug.d(TAG, "parse error: "+e.getMessage());
			if (e.getMessage().contains("Permission denied")) {
				throw new PermissionDeniedException();
			}
		}
		Debug.e(TAG, "<==========end parse tlk");
	}
	
	private void parseSubObject(BaseObject object, String str) {
		if (StringUtil.isEmpty(str)) {
			return;
		}
		String [] attr = str.split("\\^",0);
		if (attr == null || attr.length != 22) {
			return ;
		}
		object.setIndex(StringUtil.parseInt(attr[0]));
		if (object instanceof TextObject) {
			object.setX(StringUtil.parseInt(attr[2])/(2*mProportion));
			object.setWidth(StringUtil.parseInt(attr[4])/(2 * mProportion)-StringUtil.parseInt(attr[2])/(2 * mProportion));
		} else {
			Debug.d(TAG, "--->x = " + (StringUtil.parseInt(attr[2]) / mProportion));
			object.setX(StringUtil.parseInt(attr[2]) / mProportion);
			object.setWidth(StringUtil.parseInt(attr[4])/mProportion-StringUtil.parseInt(attr[2])/mProportion);
			Debug.d(TAG, "--->xEnd = " + object.getXEnd());
		}
		
	}
	
	public BaseObject parseLine(MessageTask task, String str)
	{
		Debug.d(TAG, "*************************");
		BaseObject obj = null;
		String [] attr = str.split("\\^",0);
		if (attr == null || attr.length != 22) {
			return null;
		}
		
		Debug.d(TAG, "attr[1]="+attr[1]);
		try {
			if (BaseObject.OBJECT_TYPE_MsgName.equals(attr[1]))        //msg name
			{
				obj = new MessageObject(mContext, 0);
			/*参数8表示打印头类型*/
				int type = Integer.parseInt(attr[8]);
				((MessageObject) obj).setType(type);
				int resolution = Integer.parseInt(attr[9]);
				((MessageObject) obj).setHighResolution(resolution);
				((MessageObject) obj).setDotCount(Integer.parseInt(attr[13]));
				mDots[0] = Integer.parseInt(attr[10]);
				mDots[1] = Integer.parseInt(attr[11]);
				mDots[2] = Integer.parseInt(attr[12]);
				mDots[3] = Integer.parseInt(attr[14]);
				mDots[4] = Integer.parseInt(attr[15]);
				mDots[5] = Integer.parseInt(attr[16]);
				mDots[6] = Integer.parseInt(attr[17]);
				mDots[7] = Integer.parseInt(attr[18]);
				adjustDotCount(Integer.parseInt(attr[13]));
				((MessageObject) obj).setDotCountPer(mDots);
				setDotsPerClm(type);
			} else if (BaseObject.OBJECT_TYPE_BARCODE.equals(attr[1])
					|| BaseObject.OBJECT_TYPE_QR.equals(attr[1]))    //barcode
			{
				obj = new BarcodeObject(mContext, 0);
				if (BaseObject.OBJECT_TYPE_QR.equals(attr[1])) {
					int code = Integer.parseInt(attr[9]);
					if (code == 0) {
						((BarcodeObject) obj).setCode("QR");
					} else if (code == 1) {
						((BarcodeObject) obj).setCode("DM");
					} else {
						((BarcodeObject) obj).setCode("QR");
					}
					((BarcodeObject) obj).setContent(attr[21]);
				} else {
					((BarcodeObject) obj).setCode(Integer.parseInt(attr[9]));
					((BarcodeObject) obj).setContent(attr[12]);
					((BarcodeObject) obj).setTextsize(Integer.parseInt(attr[21]));
				}
				int isShow = Integer.parseInt(attr[11]);
				((BarcodeObject) obj).setShow(isShow == 0 ? false : true);
				int source = Integer.parseInt(attr[13]);
				Debug.d(TAG, "--->source = " + source);
				obj.setSource(source == 1);
			} else if (BaseObject.OBJECT_TYPE_CNT.equals(attr[1]))        //cnt
			{
				obj = new CounterObject(mContext, 0);
				((CounterObject) obj).setBits(Integer.parseInt(attr[8]));
				((CounterObject) obj).setRange(Integer.parseInt(attr[14]), Integer.parseInt(attr[13]));
				SystemConfigFile conf = SystemConfigFile.getInstance(mContext);
				((CounterObject) obj).setValue(conf.getParam(17));
			} else if (BaseObject.OBJECT_TYPE_ELLIPSE.equals(attr[1]))    //ellipse
			{
				obj = new EllipseObject(mContext, 0);
				((EllipseObject) obj).setLineWidth(Integer.parseInt(attr[8]));
				((EllipseObject) obj).setLineType(Integer.parseInt(attr[9]));
			} else if (BaseObject.OBJECT_TYPE_GRAPHIC.equals(attr[1]))    //graphic
			{
				obj = new GraphicObject(mContext, 0);
				((GraphicObject) obj).setImage(getDirectory() + "/" + attr[12]);
			} else if (BaseObject.OBJECT_TYPE_JULIAN.equals(attr[1]))        //julian day
			{
				obj = new JulianDayObject(mContext, 0);
			} else if (BaseObject.OBJECT_TYPE_LINE.equals(attr[1]))            //line
			{
				obj = new LineObject(mContext, 0);
				((LineObject) obj).setLineWidth(Integer.parseInt(attr[8]));
				((LineObject) obj).setLineType(Integer.parseInt(attr[9]));
			} else if (BaseObject.OBJECT_TYPE_RECT.equals(attr[1]))            //rect
			{
				obj = new RectObject(mContext, 0);
				((RectObject) obj).setLineWidth(Integer.parseInt(attr[8]));
				((RectObject) obj).setLineType(Integer.parseInt(attr[9]));

			} else if (BaseObject.OBJECT_TYPE_RT.equals(attr[1]))                //realtime
			{
				Debug.d(TAG, "Real time object");
				obj = new RealtimeObject(mContext, 0);
				((RealtimeObject) obj).setFormat(attr[21]);
				((RealtimeObject) obj).setOffset(Integer.parseInt(attr[13]));
			} else if (BaseObject.OBJECT_TYPE_TEXT.equals(attr[1]))            //text
			{
				obj = new TextObject(mContext, 0);
				try {
					obj.setContent(new String(attr[21].getBytes(), "UTF-8"));
				} catch (UnsupportedEncodingException e) {
					e.printStackTrace();
				}
			} else if (BaseObject.OBJECT_TYPE_RT_SECOND.equals(attr[1])) {
				obj = new RTSecondObject(mContext, 0);
			} else if (BaseObject.OBJECT_TYPE_SHIFT.equals(attr[1])) {
				Debug.d(TAG, "--->shift object");
				obj = new ShiftObject(mContext, 0);
				((ShiftObject) obj).setBits(Integer.parseInt(attr[8]));
				((ShiftObject) obj).setShift(0, attr[13]);

				for (int i = 0; i < 4; i++) {
					// int time = Integer.parseInt(attr[13 + i]);
					((ShiftObject) obj).setShift(i, attr[13 + i]);
					((ShiftObject) obj).setValue(i, attr[9 + i]);
				}
			} else if (BaseObject.OBJECT_TYPE_LETTERHOUR.equalsIgnoreCase(attr[1])) {
				obj = new LetterHourObject(mContext, 0);

			} else if (BaseObject.OBJECT_TYPE_WEEKOFYEAR.equalsIgnoreCase(attr[1])) {
				obj = new WeekOfYearObject(mContext);
			} else if (BaseObject.OBJECT_TYPE_WEEKDAY.equalsIgnoreCase(attr[1])) {
				obj = new WeekDayObject(mContext);
			} else {
				Debug.d(TAG, "Unknown object type: " + attr[1]);
				return null;
			}
		} catch (Exception e) {
			Debug.e(TAG, "--->parseLine: " + e.getMessage());
			return null;
		}
		// 设置object的task
		obj.setTask(task);
		Debug.d(TAG, "--->proportion = " + mProportion);
		if(!(obj instanceof MessageObject))
		{
			try {
				
			obj.setIndex(Integer.parseInt(attr[0]));
			if((obj instanceof CounterObject)||
					obj instanceof JulianDayObject ||
					obj instanceof ShiftObject ||
					obj instanceof WeekOfYearObject ||
					obj instanceof WeekDayObject)
			{
				obj.setX(StringUtil.parseInt(attr[2])/mProportion);
				obj.setWidth(StringUtil.parseInt(attr[4])/mProportion-StringUtil.parseInt(attr[2])/mProportion);
			}
			else
			{
				obj.setX(StringUtil.parseInt(attr[2])/(2*mProportion));
				obj.setWidth(StringUtil.parseInt(attr[4])/(2*mProportion)-StringUtil.parseInt(attr[2])/(2*mProportion));
			}
			
			obj.setY(StringUtil.parseInt(attr[3])/(2*mProportion));
			
			obj.setHeight(StringUtil.parseInt(attr[5])/(2*mProportion)-StringUtil.parseInt(attr[3])/(2*mProportion));
			obj.setDragable(Boolean.parseBoolean(attr[7]));
			Debug.d(TAG, "--->attr[111] = " + attr[11]);
			obj.setReverse(StringUtil.parseBool(attr[11]));
			obj.setFont(attr[19]);
			} catch (Exception e) {
				Debug.d(TAG, "e: " + e.getCause());
			}
		}
		
		if (obj instanceof MessageObject) {
			obj.setContent(attr[21]);
		}
		return obj;
	}
	
	

	
	/**
	 * 获取打印对象的内容缩略信息
	 * 暂时只支持文本对象，后续会添加对变量的支持
	 */
	
	public String getContentAbatract() {
		String content = "";
		BaseObject pObj;
		if (mPath == null || mPath.isEmpty()) {
			return null;
		}
		if (!new File(mPath + "/1.bin").exists()) {
			return null;
		}
		File file = new File(mPath+"/1.TLK");
		InputStream instream;
		try {
			instream = new FileInputStream(file);
			if(instream != null)
			 {
				InputStreamReader inputreader = new InputStreamReader(instream,"UTF-8");
	            BufferedReader buffreader = new BufferedReader(inputreader);
	            String line;
	            while ( (line = buffreader.readLine()) != null) {
	            	pObj = parseLine(null, line);
	            	String objString = "";
	            	if (pObj == null) {
						continue;
					}
	            	if (pObj instanceof TextObject) {
	            		objString = pObj.getContent();
	            	} else if (pObj instanceof RealtimeObject) {
						objString = pObj.getContent();
						int lines = ((RealtimeObject) pObj).getSubObjs().size();
						for (int i = 0; i <lines; i++) {
							buffreader.readLine();
						}
					} else if (pObj instanceof CounterObject) {
						objString = pObj.getContent();
					} else {
						continue;
					}
	            	content += objString;
	            }
			 }
			instream.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
		return content;
	}
	
	public int[] getDots() {
		return mDots;
	}
	
	private float setDotsPerClm(int type) {
		float dots = 0;
		Debug.d(TAG, "--->setDotsPerClm type = " + type);
		switch (type) {
			case MessageType.MESSAGE_TYPE_12_7:
			case MessageType.MESSAGE_TYPE_12_7_S:
			case MessageType.MESSAGE_TYPE_25_4:
			case MessageType.MESSAGE_TYPE_33:
			case MessageType.MESSAGE_TYPE_38_1:
			case MessageType.MESSAGE_TYPE_50_8:
				dots = 152f;
				break;
			case MessageType.MESSAGE_TYPE_16_3:
				dots = 128f;
				break;
			case MessageType.MESSAGE_TYPE_1_INCH:
			case MessageType.MESSAGE_TYPE_1_INCH_FAST:
				dots = 320f;
				break;
			case MessageType.MESSAGE_TYPE_1_INCH_DUAL_FAST:
			case MessageType.MESSAGE_TYPE_1_INCH_DUAL:
				dots = 640f;
				break;
			case MessageType.MESSAGE_TYPE_32_DOT:
				dots = 152f;
				break;
			default:
				dots = 152f;
				break;
		}
		Debug.d(TAG, "--->setDotsPerClm dots = " + dots);
		mProportion = dots/Configs.gDots;
		if (type == MessageType.MESSAGE_TYPE_1_INCH || type == MessageType.MESSAGE_TYPE_1_INCH_FAST ) {
			mProportion = 1f;
		} else if (type == MessageType.MESSAGE_TYPE_1_INCH_DUAL || type == MessageType.MESSAGE_TYPE_1_INCH_DUAL_FAST) {
			mProportion = 1.0f;
		} else if ( type == MessageType.MESSAGE_TYPE_16_3)
		{
			mProportion = 1.0f;			
		}
		return mProportion;
	}
	
	
	private void adjustDotCount(int dots) {
		boolean isZero = true;
		for (int i = 0; i < mDots.length; i++) {
			if (mDots[i] > 0) {
				isZero = false;
				break;
			}
		}
		
		if (isZero) {
			for (int i = 0; i < mDots.length; i++) {
				mDots[i] = dots;
			}
		}
		return;
	}
}
