package com.industry.printer.hardware;

import android.content.Context;

import com.industry.printer.BinInfo;
import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.PHeader.PrinterNozzle;
import com.industry.printer.Utils.Configs;
import com.industry.printer.Utils.Debug;
import com.industry.printer.data.DataTask;

/**
 * @author kevin
 *         用于操作Fpga Gpio的类
 */
public class FpgaGpioOperation {


    /**
     * IOCMD
     */
    public static final int FPGA_CMD_SETTING = 0x01;
    public static final int FPGA_CMD_SENDDATA = 0x02;
    public static final int FPGA_CMD_SYNCDATA = 0x03;
    public static final int FPGA_CMD_STARTPRINT = 0x04;
    public static final int FPGA_CMD_STOPPRINT = 0x05;
    public static final int FPGA_CMD_CLEAN = 0x06;
// H.M.Wang 2020-12-25 追加两个命令
    public static final int FPGA_CMD_DATAGENRE = 0x07;
    public static final int FPGA_CMD_BUCKETSIZE = 0x08;
    public static final int FPGA_CMD_DISPLOG = 0x09;
    public static final int FPGA_CMD_SOFTPHO = 0x0A;
// H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
    public static final int FPGA_CMD_GET_DPI_VER = 0x0B;
// End of H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
// H.M.Wang 2022-3-21 修改为设置是否反向生成打印缓冲区
// H.M.Wang 2021-9-24 追加输入设置参数
//    public static final int FPGA_CMD_INPUT_PROC = 0x0C;
    public static final int FPGA_CMD_MIRROR = 0x0C;
// End of H.M.Wang 2021-9-24 追加输入设置参数
// End of H.M.Wang 2022-3-21 修改为设置是否反向生成打印缓冲区


    // H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
    public static final int DPI_VERSION_NONE  = 0;
    public static final int DPI_VERSION_150   = 1;
    public static final int DPI_VERSION_300   = 2;
// End of H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令

    public static final int DATA_GENRE_UPDATE   = 0;
    public static final int DATA_GENRE_NEW      = 1;
    public static final int DATA_GENRE_IGNORE   = 0;
// End of H.M.Wang 2020-12-25 追加两个命令

    /**
     * 0x00 输出数据状态
     * 0x01 设置状态
     * 0x02 保留
     * 0x03 清空状态
     */
    public static final int FPGA_STATE_OUTPUT = 0x00;
    public static final int FPGA_STATE_SETTING = 0x01;
    public static final int FPGA_STATE_RESERVED = 0x02;
    public static final int FPGA_STATE_CLEAN = 0x03;
    public static final int FPGA_STATE_PURGE = 0x05;

    public static final String FPGA_DRIVER_FILE = "/dev/fpga-gpio";
    public static int mFd = 0;
    /**
     * GPIO JNI APIs
     **/
    /**
     * 打开GPIO设备文件
     *
     * @param dev GPIO驱动设备文件
     * @return
     */
    static public native int open(String dev);

    /**
     * 向GPIO写入数据
     *
     * @param fd     设备句柄
     * @param buffer 要写到GPIO的数据buffer
     * @param count  写入数据长度，单位 sizeof（char）
     * @return
     */
    static public native int write(int fd, char[] buffer, int count);

    /**
     * 讀取FPGA數據
     *
     * @param fd
     * @return
     */
    static public native int read(int fd);

    /**
     * 向GPIO写入数据
     *
     * @param fd 设备句柄
     * @return
     */
    static public native int ioctl(int fd, int cmd, long arg);

    /**
     * 查询GPIO是否可写
     *
     * @param fd 设备句柄
     * @return
     */
    static public native int poll(int fd);

    /**
     * 关闭GPIO驱动设备文件
     *
     * @param fd 设备句柄
     * @return
     */
    static public native int close(int fd);


    //TAG
    public final static String TAG = FpgaGpioOperation.class.getSimpleName();


    public static volatile FpgaGpioOperation mInstance;

    public static FpgaGpioOperation getInstance() {
        if (mInstance == null) {
            synchronized (FpgaGpioOperation.class) {
                if (mInstance == null) {
                    mInstance = new FpgaGpioOperation();
                }
            }
        }
        return mInstance;
    }

    public FpgaGpioOperation() {

    }

    public static int open() {
        if (mFd <= 0) {
            mFd = open(FPGA_DRIVER_FILE);
        }
        return mFd;
    }

    public static void close() {
        if (mFd > 0) {
            close(mFd);
        }
    }

    public int read() {
        open();
        return read(mFd);
    }

    /**
     * writeData 下发打印数据接口
     * 每次在启动打印的时候设置为输出，在打印过程中不允许修改PG0 PG1状态
     *
     * @param type 数据类型，设置or打印数据
     * @param data
     * @param len
     * @return
     */
// H.M.Wang 2020-12-25 追加两个命令
//    public static synchronized int writeData(int type, char data[], int len) {
    public static synchronized int writeData(int dataGenre, int type, char data[], int len) {
// End of H.M.Wang 2020-12-25 追加两个命令

        int fd = open();
        if (fd <= 0) {
            return -1;
        }
        if (type < FPGA_STATE_OUTPUT || type > FPGA_STATE_PURGE) {
            Debug.d(TAG, "===>wrong data type");
            return -1;
        }
// H.M.Wang 2020-12-25 追加两个命令
        ioctl(fd, FPGA_CMD_DATAGENRE, dataGenre);   // 0:update; 1:new data
        Debug.d(TAG, "FPGA_CMD_DATAGENRE -> GENRE = " + dataGenre);
// End of H.M.Wang 2020-12-25 追加两个命令
        ioctl(fd, FPGA_CMD_SETTING, type);
        Debug.d(TAG, "FPGA_CMD_SETTING -> TYPE = " + type);
// H.M.Wang 2022-3-19 当type为FPGA_STATE_PURGE的时候，设置ExtGpio的FpgaState为Output，ioctl(fd, FPGA_CMD_SETTING, type)用来控制apk发送数据的用途，ExtGpio.setFpgaState用来切换FPGA的工作状态
        if(type == FPGA_STATE_PURGE) {
            ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_OUTPUT);
        }
// End of H.M.Wang 2022-3-19 当type为FPGA_STATE_PURGE的时候，设置ExtGpio的FpgaState为Output，ioctl(fd, FPGA_CMD_SETTING, type)用来控制apk发送数据的用途，ExtGpio.setFpgaState用来切换FPGA的工作状态

        Debug.d(TAG, "--->writeData len=" + len);
        int wlen = write(fd, data, len);
        if (wlen != len) {
            //close(fd);
            return -1;
        }
        // close(fd);
        return wlen;
    }

    public static int getPrintedCount() {
        int ret = 0;
        int fd = open();
        if (fd <= 0) {
            return -1;
        }

        ret = read(fd);
        return ret;
    }

    /**
     * pollState 轮训内核buffer状态
     * 由于该函数会调用native的poll函数，native的poll函数会一直阻塞直到内核kernel Buffer状态为空，
     * 所以不能在UI线程内调用该函数，请在单独的Thread中调用，防止ANR
     *
     * @return
     */
    public static int pollState() {
        int ret = -1;
        int fd = open();
        if (fd <= 0) {
            return -1;
        }

        ret = poll(fd);
        return ret;
    }

    /**
     * clean 下发清空数据命令到FPGA
     */
    public static void clean() {
        int fd = open();
        if (fd <= 0) {
            Debug.d(TAG, "===>open fpga file error");
            return;
        }
        ioctl(fd, FPGA_CMD_CLEAN, 0);
// H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_CLEAN);
// End of H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        Debug.d(TAG, "FPGA_CMD_CLEAN");
        // close(fd);
    }

    public static final int SETTING_TYPE_NORMAL = 1;
    public static final int SETTING_TYPE_PURGE1 = 2;
    public static final int SETTING_TYPE_PURGE2 = 3;

    /**
     * updateSettings 下发系统设置
     * 如果要下发设置数据，必须先停止打印
     * FPGA驅動接收32個參數，其中前24個參數是下發給FPGA設備的，後8個給驅動備用
     * 參數24： 表示列高（經過補償後的字節數），用於加重處理
     *
     * @param context
     */

    public static void updateSettings(Context context, DataTask task, int type) {

/*
		if (DataTransferThread.getInstance().isRunning()) {
			Debug.d(TAG, "===>print Thread is running now, please stop it then update settings");
			return;
		}
*/
        int fd = open();
        if (fd <= 0) {
            return;
        }
        char data[] = new char[Configs.gParams];
        SystemConfigFile config = SystemConfigFile.getInstance(context);
// H.M.Wang 2021-12-31 在打自己的时候，将分辨率参数强制设为150，（其实我认为300dpi的img应该设为300，150dpi的img应该设为150）
        if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_16_DOT ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32_DOT ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64_DOT ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32DN ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32SN ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64SN ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_96DN) {
            config.setParam(2, 150);
        }
// End of H.M.Wang 2021-12-31 在打自己的时候，将分辨率参数强制设为150，（其实我认为300dpi的img应该设为300，150dpi的img应该设为150）

//		config.paramTrans();
        //		RFIDManager manager = RFIDManager.getInstance(context);
//		RFIDDevice device = manager.getDevice(0);

        IInkDevice device = InkManagerFactory.inkManager(context);
        Paramter paramter = Paramter.getInstance();
        int feature4 = 0;
        int feature5 = 0;
        if (device != null) {
            feature4 = device.getFeature(0, 4);
            feature5 = device.getFeature(0, 5);
        }
        paramter.paramTrans(config.mParam, feature4, feature5, config.getPNozzle().mHeads);
        for (int i = 0; i < 24; i++) {
            data[i] = (char) paramter.getFPGAParam(i);
        }
        // S10 lower 4 bits represent print-header type
        int index = (char) config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
        data[9] = (char) PrinterNozzle.getInstance(index).mType;

        if (type != SETTING_TYPE_NORMAL) {
            data[1] = 4;
            data[3] = 100 * 4;
            data[4] = 1000;
// H.M.Wang 2021-10-22 修改清洗，重复打印设置改为2000ms，这样防止在清洗完成后还连续产生PH14
//            data[5] = 100 * 4;
            data[5] = 500 * 4;
// End of H.M.Wang 2021-10-22 修改清洗，重复打印设置改为2000ms，这样防止在清洗完成后还连续产生PH14
// H.M.Wang 2021-4-1 当清洗时，将bold设为头数，以避免清洗变淡
            data[15] = (char) (config.getPNozzle().mHeads);
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X48 ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X50 ||
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E5X48 ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E5X50 ||
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X48 ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X50) {
// H.M.Wang 2021-10-20 E5,E6头的清洗加重值，从16改为128。2021-10-22 回复为16
                data[15] = 16;
// End of H.M.Wang 2021-10-20 E5,E6头的清洗加重值，从16改为128。2021-10-22 回复为16
            }
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X1) {
// H.M.Wang 2021-10-20 E5,E6头的清洗加重值，从16改为128。2021-10-22 回复为16
                data[15] = 16;
// End of H.M.Wang 2021-10-20 E5,E6头的清洗加重值，从16改为128。2021-10-22 回复为16
            }
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_16_DOT ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32_DOT ||

                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64_DOT ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32DN ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32SN ||
// H.M.Wang 2021-8-16 追加96DN头
//                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64SN) {
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64SN ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_96DN) {
// End of H.M.Wang 2021-8-16 追加96DN头
// H.M.Wang 2021-12-31 将data[15]原来强制设为8改为2（其实2是对应于300dpi的，150dpi应该是1
                data[15] = 2;
// End of H.M.Wang 2021-12-31 将data[15]原来强制设为8改为2（其实2是对应于300dpi的，150dpi应该是1
            }
// End of H.M.Wang 2021-4-1 当清洗时，将bold设为头数，以避免清洗变淡
// H.M.Wang 2021-4-22 如果打印头的类型是打字机，则取消加重的设置。如果img为300dpi的话，强制设置为300dpi，如果img为150dpi的话，设置为150dpi
        } else {
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_16_DOT ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32_DOT ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64_DOT ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32DN ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_32SN ||
// H.M.Wang 2021-8-16 追加96DN头
//                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64SN) {
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_64SN ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_96DN) {
// End of H.M.Wang 2021-8-16 追加96DN头
                data[15] = (char) (Configs.GetDpiVersion() == DPI_VERSION_300 ? 2 : 1);
// End of H.M.Wang 2021-4-22 如果打印头的类型是打字机，则取消加重的设置。如果img为300dpi的话，强制设置为300dpi，如果img为150dpi的话，设置为150dpi
            }
// H.M.Wang 2021-5-22 25.4x(1-4)头，打印的时候，S18[4]强制设为0
// H.M.Wang 2021-5-20 25.4x(1-4)头，打印的时候，S18[4]强制设为1
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_1_INCH ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_1_INCH_DUAL ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_1_INCH_TRIPLE ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_1_INCH_FOUR) {
//                data[17] |= 0x0010;
                data[17] &= 0xFF0F;
            }
// End of H.M.Wang 2021-5-20 25.4x(1-4)头，打印的时候，S18[4]强制设为1
// End of H.M.Wang 2021-5-22 25.4x(1-4)头，打印的时候，S18[4]强制设为0
// H.M.Wang 2021-5-22 打印的时候，E6不能允许选反表， 即S18[3:0]取0
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X48 ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X50) {
                data[17] &= 0xFFF0;
            }
// End of H.M.Wang 2021-5-22 打印的时候，E6不能允许选反表， 即S18[3:0]取0
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X1) {
                data[17] &= 0xFFF0;
            }
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
            if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E5X48 ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E5X50) {
                data[17] &= 0xFFF0;
            }
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
// H.M.Wang 2021-11-18 追加根据双列打印对参数的修改
            if (config.getParam(SystemConfigFile.INDEX_DUAL_COLUMNS) > 0) { // >0 代表启用双列打印
                if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_12_7) {
                    data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_1_INCH;
                } else if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_25_4) {
                    data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_1_INCH_DUAL;
                } else if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_38_1) {
                    data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_1_INCH_TRIPLE;
                } else if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_50_8) {
                    data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_1_INCH_FOUR;
                }
//                data[17] &= 0xFFF0;
                data[17] |= 0x0010;

//                Debug.d(TAG, "data[17] = " + Integer.toHexString(data[17]));
            }
// End of H.M.Wang 2021-11-18 追加根据双列打印对参数的修改
        }

        if (type == SETTING_TYPE_PURGE1) {
// H.M.Wang 2021-12-29 修改S5，S15，S21，S22，S23为下列固定值
///////            data[4] = (char) (data[4] * 2);
            data[20] = 50;
            data[21] = 50;
            data[22] = 1000;
            data[14] = 500;
// H.M.Wang 2022-3-4 data[4]设为200
//            data[4] = 100;
            data[4] = 200;
// End of H.M.Wang 2022-3-4 data[4]设为200
            data[5] = 1500;
// H.M.Wang 2022-3-17 data[5]减半，追加data[7]减半
            data[5] /= 2;
            data[7] /= 2;
// End of H.M.Wang 2022-3-17 data[5]减半，追加data[7]减半
// End of H.M.Wang 2021-12-29 修改S5，S15，S21，S22，S23为下列固定值

// H.M.Wang 2021-3-30 当清洗时，头类型设为25.4x4
            data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_1_INCH_FOUR;
// End of H.M.Wang 2021-3-30 当清洗时，头类型设为25.4x4
// H.M.Wang 2021-5-20 清洗的时候，都选反表，S18[3:0]都设为1
//            data[17] = (char) (data[17] | 0x010);
            data[17] = (char) (data[17] | 0x001F);
// End of H.M.Wang 2021-5-20 清洗的时候，都选反表，S18[3:0]都设为1
        } else if (type == SETTING_TYPE_PURGE2) {
            data[4] = (char) (data[4] * 2);
            data[17] = (char) (data[17] & 0xffef);
        }
		/* else {
			data[1] = (char) SystemConfigFile.mParam2;
			data[3] = (char) SystemConfigFile.mParam4;
			data[4] = (char) SystemConfigFile.mParam5;
			data[5] = (char) SystemConfigFile.mParam6;
			data[15] = (char) SystemConfigFile.mResv16;
		}
		data[2] = (char) SystemConfigFile.mParam3;
		data[6] = (char) SystemConfigFile.mParam7;
		data[7] = (char) SystemConfigFile.mParam8;
		Debug.d(TAG, "===>data7:" + Integer.toHexString(data[7]));
		data[8] = (char) SystemConfigFile.mParam9;
		data[9] = (char) SystemConfigFile.mParam10;
		data[10] = (char) SystemConfigFile.mResv11;
		data[11] = (char) SystemConfigFile.mResv12;
		data[12] = (char) SystemConfigFile.mResv13;
		data[13] = (char) SystemConfigFile.mResv14;
		data[14] = (char) SystemConfigFile.mResv15;
		
		data[16] = (char) SystemConfigFile.mResv17;
		data[17] = (char) SystemConfigFile.mResv18;
		data[18] = (char) SystemConfigFile.mResv19;
		data[19] = (char) SystemConfigFile.mResv20;
		data[20] = (char) SystemConfigFile.mResv21;
		data[21] = (char) SystemConfigFile.mResv22;
		data[22] = (char) SystemConfigFile.mResv23;
		data[23] = (char) SystemConfigFile.mResv24;
		*/

        if (task != null) {
            BinInfo info = task.getInfo();
            data[24] = (char) info.getBytesFeed();
        }
// H.M.Wang 2020-5-7 12.7R5头的时候，设置头的数，强制设置打印头类型为12.7->3x25.4->12.7
        // S17
// H.M.Wang 2020-5-9 12.7R5d打印头类型不参与信息编辑，因此不通过信息的打印头类型判断其是否为12.7R5的信息，而是通过参数来规定现有信息的打印行为
// H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//		if(config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_12_7_R5) {
        if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X48 ||
                config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_R6X50) {
// End of H.M.Wang 2020-5-21 12.7R5头改为RX48，追加RX50头
//		final int headIndex = config.getParam(SystemConfigFile.INDEX_HEAD_TYPE);
//		PrinterNozzle head = PrinterNozzle.getInstance(headIndex);
//		if(head == PrinterNozzle.MESSAGE_TYPE_12_7_R5) {
// End of H.M.Wang 2020-5-9 12.7R5d打印头类型不参与信息编辑，因此不通过信息的打印头类型判断其是否为12.7R5的信息，而是通过参数来规定现有信息的打印行为
            data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_12_7;
            data[16] &= 0xfc7f;        // Bit9-7
            data[16] |= 0x0280;        // 6个头
            data[24] *= 6;
        }
// End of H.M.Wang 2020-5-7 12.7R5头的时候，设置头的数

// H.M.Wang 2021-3-6 追加E6X48,E6X50头
        if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X48 ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X50) {
            data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_9MM;
            data[16] &= 0xfc7f;        // Bit9-7
            data[16] |= 0x0280;        // 6个头
            data[24] *= 6;
        }
// End of H.M.Wang 2021-3-6 追加E6X48,E6X50头
        if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E6X1) {
            data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_9MM;
            data[16] &= 0xfc7f;        // Bit9-7
            data[16] |= 0x0280;        // 6个头
            data[24] *= 6;
        }
// H.M.Wang 2021-8-25 追加E5X48和E5X50头类型
        if (config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E5X48 ||
            config.getParam(SystemConfigFile.INDEX_HEAD_TYPE) == PrinterNozzle.MessageType.NOZZLE_INDEX_E5X50) {
            data[9] = (char) PrinterNozzle.NozzleType.NOZZLE_TYPE_9MM;
            data[16] &= 0xfc7f;        // Bit9-7
            data[16] |= 0x0280;        // 按6个头设置，下发6个头的数据，但是FPGA会会略掉最后一个头的数据
            data[24] *= 6;
        }
// End of H.M.Wang 2021-8-25 追加E5X48和E5X50头类型

        //是否雙列打印
// H.M.Wang 2021-11-17 修改参数61为双列位移设项
//        data[25] = (char) config.getParam(31 - 1);
// H.M.Wang 2021-12-9 这个参数是int型的，不能只取一个字节
        data[25] = (char) config.getParam(SystemConfigFile.INDEX_DUAL_COLUMNS);
// End of H.M.Wang 2021-12-9 这个参数是int型的，不能只取一个字节
// End of H.M.Wang 2021-11-17 修改参数61为双列位移设项
        //雙列偏移量
// H.M.Wang 2021-11-19 增加打印方向参数传递
//        data[26] = (char) config.getParam(32 - 1);
        data[26] = (char) config.getParam(1);
// End of H.M.Wang 2021-11-19 增加打印方向参数传递
/*
        data[0] = 1;
        data[1] = 4;
        data[2] = 10;
        data[3] = 61;
        data[4] = 170;
        data[5] = 1632;
        data[6] = 1;
        data[7] = 11;
        data[8] = 11;
        data[9] = 35;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = 0;
        data[15] = 2;
        data[16] = 656;
        data[17] = 16;
        data[18] = 112;
        data[19] = 200;
        data[20] = 0;
        data[21] = 0;
        data[22] = 0;
        data[23] = 1;
        data[24] = 20;
        data[25] = 17;
        data[26] = 0;
*/
        for (int i = 0; i < data.length; i++) {
            Debug.e(TAG, "--->mFPGAParam[" + i + "] = 0x" + Integer.toHexString(data[i]));
        }
        //时间参数放在最后3个
		/*
		Calendar c = Calendar.getInstance();
		int hour = c.get(Calendar.HOUR_OF_DAY);  
        int minute = c.get(Calendar.MINUTE);
        int second = c.get(Calendar.SECOND);
		data[Configs.gParams - 3] = (char)hour;
		data[Configs.gParams - 2] = (char)minute;
		data[Configs.gParams - 1] = (char)second;
		*/

// H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_SETTING);
// End of H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        writeData(DATA_GENRE_IGNORE, FPGA_STATE_SETTING, data, data.length * 2);
// H.M.Wang 2022-3-12 设置之后恢复CLEAN（双高）
// H.W.Wang 2022-3-17 暂时取消CLEAN设置
//        ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_CLEAN);
// End of H.W.Wang 2022-3-17 暂时取消CLEAN设置
// End of H.M.Wang 2022-3-12 设置之后恢复CLEAN（双高）
    }

    /**
     * 启动打印时调用，用于初始化内核轮训线程
     */
    public static void init(Context context) {
        int fd = open();
        if (fd <= 0) {
            return;
        }
		/*设置状态为输出*/
        // ioctl(fd, FPGA_CMD_SETTING, FPGA_STATE_OUTPUT);
		/*启动内核轮训线程*/
        SystemConfigFile config = SystemConfigFile.getInstance(context);
        Debug.d(TAG, "FPGA_CMD_BUCKETSIZE -> " + config.getParam(SystemConfigFile.INDEX_FIFO_SIZE));
        ioctl(fd, FPGA_CMD_BUCKETSIZE, config.getParam(SystemConfigFile.INDEX_FIFO_SIZE));
        Debug.d(TAG, "FPGA_CMD_STARTPRINT");
        ioctl(fd, FPGA_CMD_STARTPRINT, 0);
// H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_OUTPUT);
// End of H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
    }

    /**
     * 停止打印时调用，用于停止内核轮训线程
     */
    public static void uninit() {
        int fd = open();
        if (fd <= 0) {
            return;
        }

        Debug.d(TAG, "FPGA_CMD_STOPPRINT");
        ioctl(fd, FPGA_CMD_STOPPRINT, 0);
// H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_CLEAN);
// End of H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
    }

    public static void dispLog() {
        int fd = open();
        if (fd <= 0) {
            return;
        }

        Debug.d(TAG, "FPGA_CMD_DISPLOG");
        ioctl(fd, FPGA_CMD_DISPLOG, 0);
    }

    public static void softPho() {
        int fd = open();
        if (fd <= 0) {
            return;
        }

        Debug.d(TAG, "FPGA_CMD_SOFTPHO");
        ioctl(fd, FPGA_CMD_SOFTPHO, 0);
// H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
        ExtGpio.setFpgaState(ExtGpio.FPGA_STATE_SOFTPHO);
// End of H.M.Wang 2021-12-14 将FPGA的状态设置转移到EXT-GPIO驱动里面，目的是避免这两个驱动（FPGA驱动和EXT-GPIO驱动）都操作PG管脚组，并且无法互斥，而产生互相干扰
    }

// H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令
    public static int getDpiVersion() {
        int fd = open();
        if (fd <= 0) {
            return DPI_VERSION_150;
        }

        Debug.d(TAG, "FPGA_CMD_GET_DPI_VER");
        return ioctl(fd, FPGA_CMD_GET_DPI_VER, 0);
    }
// End of H.M.Wang 2021-4-9 追加ioctl的分辨率信息获取命令

// H.M.Wang 2022-3-21 修改为设置是否反向生成打印缓冲区
// H.M.Wang 2021-9-24 追加输入设置参数
    public static int setMirror(int mirror) {
        int fd = open();
        if (fd > 0) {
            Debug.d(TAG, "FPGA_CMD_MIRROR");
            return ioctl(fd, FPGA_CMD_MIRROR, mirror);
        }
        return -1;
    }
// End of H.M.Wang 2021-9-24 追加输入设置参数
// End of H.M.Wang 2022-3-21 修改为设置是否反向生成打印缓冲区
}