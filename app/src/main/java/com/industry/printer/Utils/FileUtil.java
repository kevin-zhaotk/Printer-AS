package com.industry.printer.Utils;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.text.TextUtils;

public class FileUtil {

	private static final String TAG = FileUtil.class.getSimpleName();
	/**
	 * copy file
	 */
	public static void copyFile(String oldPath, String newPath) {
		try {
			int bread=0;
			File oldFile = new File(oldPath);
			if (oldFile.exists()) {
				InputStream inStream = new FileInputStream(oldFile);
				FileOutputStream outStream = new FileOutputStream(newPath);
				byte[] buffer = new byte[1024];
				while ((bread = inStream.read(buffer)) != -1) {
					outStream.write(buffer, 0, bread);
				}
				inStream.close();
				outStream.flush();
				outStream.close();
			}
			
		} catch (Exception e) {
			Debug.e(TAG, "--->err:" + e.getCause());
		}
	}
 
	// 复制文件   
	public static void copyFile(File sourceFile,File targetFile) throws IOException{  
		
		if (targetFile.exists()) {
			targetFile.delete();
		}
		Debug.d(TAG, "--->copyFile: " + sourceFile + "-- to --> " + targetFile);
        // 新建文件输入流并对它进行缓冲   
        FileInputStream input = new FileInputStream(sourceFile);  
        BufferedInputStream inBuff=new BufferedInputStream(input);  
  
        // 新建文件输出流并对它进行缓冲   
        FileOutputStream output = new FileOutputStream(targetFile);  
        BufferedOutputStream outBuff=new BufferedOutputStream(output);  
          
        // 缓冲数组   
        byte[] b = new byte[1024 * 5];  
        int len;  
        while ((len =inBuff.read(b)) != -1) {  
            outBuff.write(b, 0, len);  
        }  
        // 刷新此缓冲的输出流   
        outBuff.flush();  
          
        //关闭流   
        inBuff.close();  
        outBuff.close();  
        output.close();  
        input.close();  
    }  
    /**
     * copy directory 
     * @param sourceDir
     * @param targetDir
     * @throws IOException
     */
    public static void copyDirectiory(String sourceDir, String targetDir)  throws IOException {
    	Debug.d(TAG, "--->copyDirectory src: " + sourceDir + "  target: " + targetDir);
    	
    	File src = new File(sourceDir);
    	if (src.isFile()) {
			copyFile(sourceDir, targetDir);
			return;
		}
        // make directory  
        (new File(targetDir)).mkdirs();  
        File[] file = (new File(sourceDir)).listFiles();
        if (file == null || file.length <= 0) {
			return;
		}
        for (int i = 0; i < file.length; i++) { 
            if (file[i].isFile()) {  
                // source file
                File sourceFile=file[i];  
                // target file
               File targetFile=new File(new File(targetDir).getAbsolutePath() + File.separator + file[i].getName());  
                copyFile(sourceFile,targetFile);  
            }  
            if (file[i].isDirectory()) {  
                // source directory 
                String dir1 = sourceDir + (sourceDir.endsWith("/") ? "":"/") + file[i].getName();  
                // target directory   
                String dir2=targetDir + (targetDir.endsWith("/") ? "" : "/")+ file[i].getName();  
                copyDirectiory(dir1, dir2);  
            }  
        }  
    } 
    /**
     * copy from source directory to target directory, and delete the target directory before copy
     * @param sourceDir
     * @param targetDir
     */
    public static void copyClean(String sourceDir, String targetDir) {
    	File target = new File(targetDir);
		if (target.exists()) {
			FileUtil.deleteFolder(target.getAbsolutePath());
		}
		File src = new File(sourceDir);
		try {
			if (src.isFile()) {
				copyFile(sourceDir, targetDir);
			} else {
				copyDirectiory(sourceDir, targetDir);
			}
		} catch (Exception e) {
			Debug.e(TAG, "--->exception: " + e.getMessage());
		}
    }
    
    public static void deleteFolder(String filePath) {       
    	if (!TextUtils.isEmpty(filePath)) {  
    	    try {  
    	        File file = new File(filePath);  
    	        if (file.isDirectory()) { //目录  
    	            File files[] = file.listFiles();  
    	            for (int i = 0; i < files.length; i++) {
    	                deleteFolder(files[i].getAbsolutePath());  
    	            }  
    	        }  
    	           
    	        if (!file.isDirectory()) { //如果是文件，删除  
    	            file.delete();  
    	        } else { //目录  
    	           if (file.listFiles().length == 0) { //目录下没有文件或者目录，删除  
    	                file.delete();  
    	           }  
    	        }
    	    } catch (Exception e) {  
    	        e.printStackTrace();  
    	    }  
    	}  
    } 
    
    public static String getFileNameNoEx(String filename) {
    	if ((filename != null) && filename.length() > 0) {
			int dot = filename.lastIndexOf(".");
			if ((dot > -1) && (dot < filename.length())) {
				return filename.substring(0, dot);
			}
		}
    	return filename;
    }
    
    public static void releaseAssets(Context context, String assetsDir,  String releaseDir) {  
          
    	Debug.d(TAG, "context: " + context + ", " + assetsDir + ",  " + releaseDir);  
        if (TextUtils.isEmpty(releaseDir)) {  
            return;  
        } else if (releaseDir.endsWith("/")) {  
            releaseDir = releaseDir.substring(0, releaseDir.length() - 1);  
        }  
   
        if (TextUtils.isEmpty(assetsDir) || assetsDir.equals("/")) {  
            assetsDir = "";  
        } else if (assetsDir.endsWith("/")) {  
            assetsDir = assetsDir.substring(0, assetsDir.length() - 1);  
        }  
   
        AssetManager assets = context.getAssets();  
        try {  
            String[] fileNames = assets.list(assetsDir);//只能获取到文件(夹)名,所以还得判断是文件夹还是文件  
            if (fileNames.length > 0) {// is dir  
            	
                for (String name : fileNames) {  
                	Debug.d(TAG, "--->releaseAssets 0 : " + name);
                    if (!TextUtils.isEmpty(assetsDir)) {  
                        name = assetsDir + File.separator + name;//补全assets资源路径  
                    }  
//                    Log.i(, brian name= + name);  
                    String[] childNames = assets.list(name);//判断是文件还是文件夹  
                    if (!TextUtils.isEmpty(name) && childNames.length > 0) {  
                        checkFolderExists(releaseDir + File.separator + name);  
                        releaseAssets(context, name, releaseDir);//递归, 因为资源都是带着全路径,   
                                                                                           //所以不需要在递归是设置目标文件夹的路径  
                    } else {  
                        InputStream is = assets.open(name);  
//                        FileUtil.writeFile(releaseDir + File.separator + name, is); 
                        Debug.d(TAG, "--->releaseAssets : " + name);
                        writeFile(releaseDir + File.separator + name, is);  
                    }  
                }  
            } else {// is file  
                InputStream is = assets.open(assetsDir);  
                // 写入文件前, 需要提前级联创建好路径, 下面有代码贴出  
//                FileUtil.writeFile(releaseDir + File.separator + assetsDir, is);  
                writeFile(releaseDir + File.separator + assetsDir, is);  
            }  
   
        } catch (Exception e) {  
            Debug.d(TAG, "--->e: " + e.getMessage());
        }  
    }  
    
    private static boolean writeFile(String fileName, InputStream in) throws IOException  
    {  
    	Debug.d(TAG, "--->writeFile: " + fileName);
        boolean bRet = true;  
        try {  
            OutputStream os = new FileOutputStream(fileName);  
            byte[] buffer = new byte[4112];  
            int read;  
            while((read = in.read(buffer)) != -1)  
            {  
                os.write(buffer, 0, read);  
            }  
            in.close();  
            in = null;  
            os.flush();  
            os.close();  
            os = null;  
//          Log.v(TAG, "copyed file: " + fileName);  
        } catch (FileNotFoundException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
            bRet = false;  
        }  
        return bRet;  
    }  
      
    private static void checkFolderExists(String path)  
    {  
        File file = new File(path);  
        if((file.exists() && !file.isDirectory()) || !file.exists())  
        {  
            file.mkdirs();  
        }  
    }
}
