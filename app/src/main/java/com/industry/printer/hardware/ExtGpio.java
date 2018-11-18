package com.industry.printer.hardware;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import com.industry.printer.Utils.Debug;

public class ExtGpio {

	/**
	 * RFID选择
	 */
	private static final String EXT_GPIO_FILE = "/dev/ext-gpio";
	
	private static final int GPIO_PLAY = 0x01;
	private static final int GPIO_PLAY_ERR = 0x02;
	private static final int GPIO_RFID_CARD1 = 0x03;
	private static final int GPIO_RFID_CARD2 = 0x04;
	private static final int GPIO_RFID_CARD3 = 0x05;
	private static final int GPIO_RFID_CARD4 = 0x06;
	private static final int GPIO_RFID_CARD5 = 0x07;
	private static final int GPIO_RFID_CARD6 = 0x08;
	private static final int GPIO_RFID_CARD7 = 0x09;
	private static final int GPIO_RFID_CARD8 = 0x0A;
	private static final int GPIO_WRITE = 0x10;
	
	
	// RFID卡1對應的3-8譯碼器編碼
	private static final int RFID_CARD1_CODE = 3;
	// RFID卡2對應的3-8譯碼器編碼
	private static final int RFID_CARD2_CODE = 4;
	
	public static final int RFID_CARD1 = 0;
	public static final int RFID_CARD2 = 1;
	public static final int RFID_CARD3 = 2;
	public static final int RFID_CARD4 = 3;
	public static final int RFID_CARD5 = 4;
	public static final int RFID_CARD6 = 5;
	public static final int RFID_CARD7 = 6;
	public static final int RFID_CARD8 = 7;
	
	public static int mFd = 0;
	public static void rfidSwitch(int sw) {
		int fd = open();
		if (sw == RFID_CARD1) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD1, 0);
		} else if (sw == RFID_CARD2) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD2, 0);
		} else if (sw == RFID_CARD3) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD3, 0);
		} else if (sw == RFID_CARD4) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD4, 0);
		} else if (sw == RFID_CARD5) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD5, 0);
		} else if (sw == RFID_CARD6) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD6, 0);
		} else if (sw == RFID_CARD7) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD7, 0);
		} else if (sw == RFID_CARD8) {
			FpgaGpioOperation.ioctl(fd, GPIO_RFID_CARD8, 0);
		}
	}
	
	/**
	 * 
	 * @param group
	 * @param index
	 * @param value
	 */
	public static void writeGpio(char group, int index, int value) {
		int fd = open();
		int g = group - 'a';
		int v = ((g & 0x0f) << 12) | ((index & 0x0ff) << 4) | value;
		Debug.d("ExtGpio", "--->writeGpio: fd= " + fd + "  group=" + group + "  index=" + index + " value=" + value + "  v=" + v);
		FpgaGpioOperation.ioctl(fd, GPIO_WRITE, v);
	}
	
	public static void playClick() {
		int fd = open();
		FpgaGpioOperation.ioctl(fd, GPIO_PLAY, 0);
	}
	
	public static void playError() {
		int fd = open();
		FpgaGpioOperation.ioctl(fd, GPIO_PLAY_ERR, 0);
	}
	
	public static int open() {
		if (mFd <= 0) {
			mFd = FpgaGpioOperation.open(EXT_GPIO_FILE);
		}
		return mFd;
	}
	
	public static boolean writeSysfs() {
        String path = "/sys/devices/platform/ext-gpio/playClick";
        if (!new File(path).exists()) {
            Debug.e("", "File not found: " + path);
            return false;
        }

        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(path), 2);
            try {
                writer.write("1");
            } finally {
                writer.close();
            }
            return true;

        } catch (IOException e) {
            Debug.e("", "IO Exception when write: " + path, e);
            return false;
        }
    }

}
