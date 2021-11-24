package com.industry.printer.Serial;

        import android.content.Context;

        import com.industry.printer.Utils.StreamTransport;

        import org.apache.http.util.ByteArrayBuffer;

        import java.nio.charset.Charset;

/**
 * Created by hmwan on 2020/10/30.
 */

public class Scaner2Protocol extends SerialProtocol {
    public static String TAG = SerialProtocol5.class.getSimpleName();

    public final static String TEXT_SEPERATOR         = "#";          // 串口接收文本的间隔符

    public Scaner2Protocol(/*StreamTransport st*/SerialPort serialPort, Context ctx){
        super(serialPort, ctx);
    }

    @Override
    protected int parseFrame(ByteArrayBuffer recvMsg) {
        return ERROR_SUCESS;
    }

    @Override
    protected byte[] createFrame(int cmd, int ack, int devStatus, int cmdStatus, byte[] msg) {
        return msg;
    }

    @Override
    public void handleCommand(SerialHandler.OnSerialPortCommandListenner normalCmdListenner, SerialHandler.OnSerialPortCommandListenner printCmdListenner, ByteArrayBuffer bab) {
        setListeners(normalCmdListenner, printCmdListenner);

        int result = parseFrame(bab);
        if (result == ERROR_SUCESS) {
            byte[] recvData = bab.toByteArray();
            procCommands(result, recvData);
        }
    }

    @Override
    public void sendCommandProcessResult(int cmd, int ack, int devStatus, int cmdStatus, String message) {
        byte[] retMsg = createFrame(cmd, ack, devStatus, cmdStatus, message.getBytes(Charset.forName("UTF-8")));
        super.sendCommandProcessResult(retMsg);
    }
}
