package com.industry.printer.object;

/**
 * Created by hmwan on 2021/10/11.
 */

public interface N_ObjectType {
    String NOT_DEFINED          = "";           // 无
    String STATIC_TEXT          = "001";        // 静态文本。运行时不可修改
    String COUNTER  		    = "002";        // 计数器
    String RT_YEAR	            = "003";        // 实时时间的年
    String RT_MON	            = "004";        // 实时时间的月
    String RT_DATE	            = "005";        // 实时时间的日
    String RT_HOUR	            = "006";        // 实时时间的时
    String RT_MIN	            = "007";        // 实时时间的分
    String YEAR		            = "008";        // 年
    String SHIFT	            = "009";        // 班次
    String DL_YEAR	            = "013";        //
    String DL_MON	            = "014";        //
    String DL_DATE	            = "015";        //
    String JULIAN	            = "025";        //
    String GRAPHIC	            = "026";        // 图片
    String BARCODE	            = "027";        // 条码
    String LINE		            = "028";        //
    String RECT		            = "029";        //
    String ELLIPSE	            = "030";        //
    String MESSAGE	            = "031";        // 信息头行
    String REAL_TIME		    = "032";        // 实时时间
    String QRCODE	            = "033";        // 二维码
    String WEEKDAY              = "034";        //
    String WEEKS	            = "035";        //
    String RT_SECOND            = "036";        // 实时时间的秒
    String LETTERHOUR           = "037";        //
    String DYNAMIC_TEXT         = "040";        // 动态文本
    String WEEKOFYEAR           = "041";        //
    String HYPERTEXT            = "043";        // 超文本
}
