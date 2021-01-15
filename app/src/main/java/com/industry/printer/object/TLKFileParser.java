package com.industry.printer.object;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import com.industry.printer.MessageTask;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.FileFormat.TlkFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.Utils.StringUtil;
import com.industry.printer.exception.PermissionDeniedException;
import com.industry.printer.exception.TlkNotFoundException;

import android.content.Context;

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
	public void parse(Context context, MessageTask task, ArrayList<BaseObject> objlist) throws PermissionDeniedException, TlkNotFoundException
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
                     if(pObj instanceof RealtimeObject) {
						 i = ((RealtimeObject) pObj).mSubObjs.size();
						 for (int j = 0; j < i; j++) {
							 line = buffreader.readLine();
							 while (StringUtil.isEmpty(line)) {
								 line = buffreader.readLine();
							 }
							 // Debug.d(TAG, "line="+line);
							 BaseObject obj = ((RealtimeObject) pObj).mSubObjs.get(j);
							 parseSubObject(obj, line);
						 }
// H.M.Wang 2020-2-17 追加HyperText控件
					 } else if(pObj instanceof HyperTextObject) {
						 int num = ((HyperTextObject) pObj).mSubObjs.size();
						 for(int j=0; j<num ; j++) {
							 line = buffreader.readLine();
							 while(StringUtil.isEmpty(line)) {
								 line = buffreader.readLine();
							 }
							 // Debug.d(TAG, "line="+line);
							 BaseObject obj =  ((HyperTextObject) pObj).mSubObjs.get(j);
							 parseSubObject(obj, line);
						 }
// End of H.M.Wang 2020-2-17 追加HyperText控件
                     }
                 }
                 instream.close();
			 }
		}catch(IOException e)
		{
			Debug.d(TAG, "parse error: "+e.getMessage());
			if (e.getMessage().contains("Permission denied")) {
				throw new PermissionDeniedException();
			} else {
				throw new TlkNotFoundException();
			}
		}
		Debug.e(TAG, "<==========end parse tlk");
	}
	
	private void parseSubObject(BaseObject object, String str) {
		if (StringUtil.isEmpty(str)) {
			return;
		}
		String [] attr = str.split("\\^",0);
		if (attr == null || attr.length < 22) {
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
		if (attr == null || attr.length < 22) {
			return null;
		}

		Debug.d(TAG, "attr[1]="+attr[1]);
		try {
			if (BaseObject.OBJECT_TYPE_MsgName.equals(attr[1]))        //msg name
			{
				obj = new MessageObject(mContext, 0);
			/*参数8表示打印头类型*/
				int type = Integer.parseInt(attr[8]);
// H.M.Wang 2020-7-27 修改打印头的获取方式，不依据信息中的打印头信息获取，而是依据系统参数设置获取
//				((MessageObject) obj).setType(PrinterNozzle.getByType(type).mIndex);
				SystemConfigFile conf = SystemConfigFile.getInstance(mContext);
				((MessageObject) obj).setType(conf.getParam(SystemConfigFile.INDEX_HEAD_TYPE));
// End of H.M.Wang 2020-7-27 修改打印头的获取方式，不依据信息中的打印头信息获取，而是依据系统参数设置获取
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
// H.M.Wang 2020-7-27 修改打印头的获取方式，不依据信息中的打印头信息获取，而是依据系统参数设置获取
//				PrinterNozzle nozzle = PrinterNozzle.getByType(type);
				PrinterNozzle nozzle = conf.getPNozzle();
// End of H.M.Wang 2020-7-27 修改打印头的获取方式，不依据信息中的打印头信息获取，而是依据系统参数设置获取
				setDotsPerClm(nozzle);
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
// H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
				int withFrame = Integer.parseInt(attr[20]);
				((BarcodeObject) obj).setWithFrame(withFrame == 0 ? false : true);
// End of H.M.Wang 2020-2-25 追加ITF_14边框有无的设置
			} else if (BaseObject.OBJECT_TYPE_CNT.equals(attr[1]))        //cnt
			{
				obj = new CounterObject(mContext, 0);
				((CounterObject) obj).setBits(Integer.parseInt(attr[8]));
				((CounterObject) obj).setRange(Integer.parseInt(attr[13]), Integer.parseInt(attr[14]));
				SystemConfigFile conf = SystemConfigFile.getInstance(mContext);
				int index = Integer.parseInt(attr[16]);
				if (index < 0 || index >= 10) {
					index = 0;
				}
				((CounterObject) obj).setCounterIndex(index);
// 值已经在setCounterIndex里面设置，无需再次设置 ((CounterObject) obj).setValue(conf.getParam(SystemConfigFile.INDEX_COUNT_1 + index));
				((CounterObject) obj).setSteplen(Integer.parseInt(attr[15]));
			} else if (BaseObject.OBJECT_TYPE_ELLIPSE.equals(attr[1]))    //ellipse
			{
				obj = new EllipseObject(mContext, 0);
				((EllipseObject) obj).setLineWidth(Integer.parseInt(attr[8]));
				((EllipseObject) obj).setLineType(Integer.parseInt(attr[9]));
			} else if (BaseObject.OBJECT_TYPE_GRAPHIC.equals(attr[1]))    //graphic
			{
				obj = new GraphicObject(mContext, 0);
				obj.setTask(task);
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
// H.M.Wang 2020-2-17 追加HyperText控件
			} else if (BaseObject.OBJECT_TYPE_HYPERTEXT.equals(attr[1])) {               //hypertext
				Debug.d(TAG, "Hypertext object");
				obj = new HyperTextObject(mContext, 0);
				((HyperTextObject) obj).setContent(attr[21]);
				((HyperTextObject) obj).setCounterIndex(attr[8]);
				((HyperTextObject) obj).setCounterStart(attr[17]);
				((HyperTextObject) obj).setCounterEnd(attr[18]);
				((HyperTextObject) obj).setShiftValue(0, attr[9]);
				((HyperTextObject) obj).setShiftValue(1, attr[10]);
				((HyperTextObject) obj).setShiftValue(2, attr[11]);
				((HyperTextObject) obj).setShiftValue(3, attr[12]);
				((HyperTextObject) obj).setShiftTime(0, attr[13]);
				((HyperTextObject) obj).setShiftTime(1, attr[14]);
				((HyperTextObject) obj).setShiftTime(2, attr[15]);
				((HyperTextObject) obj).setShiftTime(3, attr[16]);
				((HyperTextObject) obj).setDateOffset(attr[20]);
// End of H.M.Wang 2020-2-17 追加HyperText控件
			} else if (BaseObject.OBJECT_TYPE_TEXT.equals(attr[1]))            //text
			{
				obj = new TextObject(mContext, 0);
				try {
					obj.setContent(new String(attr[21].getBytes(), "UTF-8"));
				} catch (UnsupportedEncodingException e) {
					e.printStackTrace();
				}
			} else if (BaseObject.OBJECT_TYPE_RT_SECOND.equals(attr[1])) {
				obj = new RealtimeSecond(mContext, 0);
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
			} else if (BaseObject.OBJECT_TYPE_DYN_TEXT.equalsIgnoreCase(attr[1])) {
/*				// H.M.Wang 2019-12-6 PC制作的面向串口的动态文本，暂时以OBJECT_TYPE_DYN_TEXT作为标识，适用计数器类进行处理
//				obj = DynamicText.fromTlk(mContext, str);
				obj = new CounterObject(mContext, 0);
				((CounterObject) obj).setBits(Integer.parseInt(attr[8]));
				((CounterObject) obj).setRemoteContent("");
				((CounterObject) obj).setRange(0, 0);
				((CounterObject) obj).mCounterIndex = 0;
				((CounterObject) obj).setValue(0);
				((CounterObject) obj).setSteplen(0);
				// End. -----------------
*/
				obj = new DynamicText(mContext, 0);
				((DynamicText) obj).setDtIndex(Integer.parseInt(attr[16]));
				((DynamicText) obj).setBits(Integer.parseInt(attr[8]));
				obj.setContent(new String(attr[21].getBytes(), "UTF-8"));
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
				obj.setFont(attr[19]);

				obj.setIndex(Integer.parseInt(attr[0]));
// 必须先设置Y值和高，然后在设置X和宽，否则图片变形
				obj.setY(StringUtil.parseInt(attr[3])/(2*mProportion));
				obj.setHeight(StringUtil.parseInt(attr[5])/(2*mProportion)-StringUtil.parseInt(attr[3])/(2*mProportion));

				if((obj instanceof CounterObject)||
						obj instanceof DynamicText ||
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
// 必须先设置Y值和高，然后在设置X和宽，否则图片变形
//				obj.setY(StringUtil.parseInt(attr[3])/(2*mProportion));
//				obj.setHeight(StringUtil.parseInt(attr[5])/(2*mProportion)-StringUtil.parseInt(attr[3])/(2*mProportion));
				obj.setDragable(Boolean.parseBoolean(attr[7]));
//				Debug.d(TAG, "--->attr[111] = " + attr[11]);
				obj.setReverse(StringUtil.parseBool(attr[11]));

				// H.M.Wang追加一行，设置初始横向缩放比例
				obj.setXRatio();
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
// H.M.Wang 2020-2-17 追加HyperText控件
					} else if (pObj instanceof HyperTextObject) {
						objString = pObj.getContent();
						int lines = ((HyperTextObject) pObj).getSubObjs().size();
						for (int i = 0; i <lines; i++) {
							buffreader.readLine();
						}
// End of H.M.Wang 2020-2-17 追加HyperText控件
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
	
	private float setDotsPerClm(PrinterNozzle type) {
		float dots = 0;
		Debug.d(TAG, "--->setDotsPerClm type = " + type);
		switch (type) {
			case MESSAGE_TYPE_12_7:
			case MESSAGE_TYPE_25_4:
			case MESSAGE_TYPE_38_1:
			case MESSAGE_TYPE_50_8:
				dots = 152f;
				mProportion = dots/Configs.gDots;
				break;
			case MESSAGE_TYPE_1_INCH:
				dots = 320f;
				mProportion = 1f;
				break;
			case MESSAGE_TYPE_1_INCH_DUAL:
				dots = 640f;
				mProportion = 1.0f;
				break;

            // H.M.Wang 追加下列8行
            case MESSAGE_TYPE_1_INCH_TRIPLE:
                dots = 960f;
                mProportion = 1.0f;
                break;
            case MESSAGE_TYPE_1_INCH_FOUR:
                dots = 1280f;
                mProportion = 1.0f;
                break;

			case MESSAGE_TYPE_16_DOT:
			case MESSAGE_TYPE_32_DOT:
// H.M.Wang 2020-7-23 追加32DN打印头
			case MESSAGE_TYPE_32DN:
// End of H.M.Wang 2020-7-23 追加32DN打印头
// H.M.Wang 2020-8-17 追加32SN打印头
			case MESSAGE_TYPE_32SN:
// End of H.M.Wang 2020-8-17 追加32SN打印头

			// H.M.Wang 追加下列一行
			case MESSAGE_TYPE_64_DOT:
// H.M.Wang 2020-8-26 追加64SN打印头
			case MESSAGE_TYPE_64SN:
// End of H.M.Wang 2020-8-26 追加64SN打印头

				dots = 152f;
				mProportion = dots/Configs.gDots;
				break;
			default:
				dots = 152f;
				mProportion = dots/Configs.gDots;
				break;
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
