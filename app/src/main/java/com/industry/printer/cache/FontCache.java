package com.industry.printer.cache;

import android.content.Context;
import android.graphics.Typeface;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.util.HashMap;

import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.FileUtil;

/**
 * Customized Font library loading cache for the purpose of avoiding memory leak
 * Bacause Typeface will allocate new memory block everytime when it create
 * a new instance of Typeface by invoking Typeface.createFromAsset method, this is a
 * known bug from google;
 * We improve this issue by managing the Typefaces using this FontCache Class;
 * Created by kevin on 2017/11/7.
 */

public class FontCache {

    // font library stay at asset if true, sdcard else.
    private static final boolean ASSETS = false;

    public static final HashMap FONT_CACHE = new HashMap<String, Typeface>();

    /**
     * return the typeface of the specified font name
     * @param ctx
     * @param font
     * @return
     */
    public static Typeface get(Context ctx, String font) {

        if (ASSETS) {
            return getFromAssets(ctx, font);
        } else {
            return getFromExternal(font);
        }
    }

    private static Typeface getFromExternal(String font) {
        synchronized (FONT_CACHE) {
            Typeface tf = (Typeface) FONT_CACHE.get(font);
            if (tf == null) {
                tf = Typeface.createFromFile(Configs.FONT_DIR + File.separator + font);
                FONT_CACHE.put(font, tf);
            }
            return tf;
        }
    }
    
	private static Typeface getFromAssets(Context ctx, String font) {
        synchronized (FONT_CACHE) {
            Typeface tf = (Typeface) FONT_CACHE.get(font);
            if (tf == null) {
                tf = Typeface.createFromAsset(ctx.getAssets(), Configs.FONT_PATH + File.separator + font);
                FONT_CACHE.put(font, tf);
            }
            return tf;
        }
    }

    public static String[] getFonts(Context ctx) {

        if (ASSETS) {
            return getFontsAsset(ctx);
        } else {
            return getFontsExternal();
        }
    }

	private static String[] getFontsExternal() {
		File dir = new File(Configs.FONT_DIR);
		File[] fonts = dir.listFiles(new FileFilter() {
			
			@Override
			public boolean accept(File arg0) {
//				if (arg0.getAbsolutePath().endsWith(".ttf")) {
//					return true;
//				}
//				return false;
				return true;
			}
		});
		if (fonts == null || fonts.length <= 0) {
			return null;
		}
		String[] fontName = new String[fonts.length];
		for (int i = 0; i < fontName.length; i++) {
			fontName[i] = FileUtil.getFileNameNoEx(fonts[i].getName());
		}
		return fontName;
	}

	private static String[] getFontsAsset(Context ctx) {

        String[] fontName = null;
        try {
            String[] fonts = ctx.getAssets().list(Configs.FONT_PATH);
            if (fonts == null || fonts.length <= 0) {
                return null;
            }
            fontName = new String[fonts.length];
            for (int i = 0; i < fontName.length; i++) {
                fontName[i] = FileUtil.getFileNameNoEx(fonts[i]);
            }
        } catch (IOException e) {

        } finally {
            return fontName;
        }
    }
}
