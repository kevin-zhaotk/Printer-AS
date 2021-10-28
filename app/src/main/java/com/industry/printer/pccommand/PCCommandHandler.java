package com.industry.printer.pccommand;

import android.content.Context;
import android.os.Bundle;
import android.os.Message;
import android.os.SystemClock;

import com.industry.printer.Constants.Constants;
import com.industry.printer.DataTransferThread;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.R;
import com.industry.printer.Socket_Server.PCCommand;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.DataTask;
import com.industry.printer.hardware.FpgaGpioOperation;
import com.industry.printer.hardware.RTCDevice;

import java.io.File;
import java.util.Calendar;

/**
 * Created by hmwan on 2021/10/28.
 */

public class PCCommandHandler {
    private static final String TAG = PCCommandHandler.class.getSimpleName();

    private Context mContext = null;
    // H.M.Wang 2020-1-8 增加网络命令ID，在向PC报告打印状态的时候用来识别命令
    private String mPCCmdId = "";
    // End of H.M.Wang 2020-1-8 增加网络命令ID，在向PC报告打印状态的时候用来识别命令

    public interface HandleMessageReceiver {
        public void handledMessage(String msg);
    }
    private HandleMessageReceiver mHandledMessageReceiver = null;

    public PCCommandHandler(Context ctx) {
        mContext = ctx;
    }

    private void sendmsg(String msg) {
        if(null != mHandledMessageReceiver) {
            mHandledMessageReceiver.handledMessage(msg);
        }
    }

    public void handle(String msg, HandleMessageReceiver l) {
    }
}
