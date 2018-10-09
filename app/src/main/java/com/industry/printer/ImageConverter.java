package com.industry.printer;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;

public class ImageConverter {
	
    public static Bitmap convertGreyImg(Bitmap img) { 
        int width = img.getWidth();
        int height = img.getHeight();       

        int []pixels = new int[width * height]; 

        img.getPixels(pixels, 0, width, 0, 0, width, height); 
        int alpha = 0xFF << 24;  
        for(int i = 0; i < height; i++)  { 
            for(int j = 0; j < width; j++) { 
                int grey = pixels[width * i + j]; 

                int red = ((grey  & 0x00FF0000 ) >> 16); 
                int green = ((grey & 0x0000FF00) >> 8); 
                int blue = (grey & 0x000000FF); 

                grey = (int)((float) red * 0.3 + (float)green * 0.59 + (float)blue * 0.11); 
                grey = alpha | (grey << 16) | (grey << 8) | grey; 
                pixels[width * i + j] = grey; 
            } 
        } 
        Bitmap result = Bitmap.createBitmap(width, height, Config.RGB_565); 
        result.setPixels(pixels, 0, width, 0, 0, width, height); 
        return result; 
    } 
}
