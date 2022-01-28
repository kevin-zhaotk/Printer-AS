package com.industry.printer.Rfid;

import com.industry.printer.Utils.ByteArrayUtils;
import com.industry.printer.Utils.Debug;

public class EncryptionMethod {

	public static EncryptionMethod mInstance = null;
	/*墨水量上下限*/
	public static final int INK_LEVEL_MAX = 100000;
	public static final int INK_LEVEL_MIN = 0;
	
	
	public static EncryptionMethod getInstance() {
		if (mInstance == null) {
			mInstance = new EncryptionMethod();
		}
		return mInstance;
	}
	public EncryptionMethod() {
		
	}
	
	/**
	 * 通过序列号获取密钥A
	 * 1、序列号的1-2byte经过按位取反存到密钥的1-2byte，
	 * 2、序列号的3-4byte经过按位取反存到密钥的4-5byte，
	 * 3、密钥的1、2、4、5  4个byte的异或校验值保存到密钥的byte3
	 * 4、密钥的前5个byte的校验和放在密钥的byte6

	 * @param sn
	 * @return
	 */
	public byte[] getKeyA(byte[] sn) {
		if (sn == null || sn.length != 4) {
			return null;
		}
		byte[] key = new byte[6];
		
		key[0] = (byte) (~(sn[0]) & 0x0ff );
		key[1] = (byte) (~(sn[1]) & 0x0ff );
		key[3] = (byte) (~(sn[2]) & 0x0ff );
		key[4] = (byte) (~(sn[3]) & 0x0ff );
		key[2] = (byte) ((key[0] ^ key[1] ^ key[3] ^ key[4]) & 0x0ff );
		// 计算校验和
		for (int i = 0; i < key.length-1; i++) {
			key[5] += (key[i] & 0x0ff);
		}
		byte tmp = 0;
		// 校验和要经过高低位倒序
		for (int i = 0; i < 8; i++) {
			if ((key[5] >> i & 0x01) == 0x01) {
				// H.M.Wang 2021-11-8 这个 0x01 << (8-i) 会导致串一位，如
				// 0001 0000 高低位倒置以后，应该是0000 1000(即0x08）
				// 当i=4时会走到原来的0001 0000的1的位置，但是0x01 << (8-i)，会生成 0001 0000（左移4位），从而造成串一位的结果
				// 由于生成卡的时候也是同样的算法，因此将错就错了，这里不用修改
				tmp |= 0x01 << (8-i);
			}
		}
		key[5] = (byte) (tmp & 0x0ff);
		return key;
	}
	
	/**
	 * 通过序列号获取密钥B
	 * @param sn
	 * @return
	 */
	public byte[] getKeyB(byte[] sn) {
		if (sn == null || sn.length == 0) {
			return null;
		}
		byte[] key = new byte[sn.length];
		for (int i = 0; i < sn.length; i++) {
			key[i] = (byte) ~(sn[i]);
		}
		for (int i = 0; i < key.length; i++) {
			key[i] = (byte) ((key[i]<<4 & 0x0f0) & (key[i] >> 4 & 0x0f));
		}
		return key;
	}
	
	
	/**
	 * 解密得到真实的墨水值
	 * 暂时将墨水值放在byte0（高）和byte1（低）
	 */
	public int decryptInkLevel(byte[] level) {
		if (level == null || level.length < 3) {
			return 0;
		}
		if (false) {
			byte checkSum = (byte) (level[1] + level[2] + level[3] + level[4]);
			if (checkSum != level[0]) {
				return 0;
			}
			byte l=0;
			l = level[3];
			level[3] = (byte) (~level[1] & 0x0ff);
			level[1] = (byte) (~l & 0x0ff);
			l = level[2];
			level[2] = (byte) (~level[4] & 0x0ff);
			level[4] = (byte) (~l & 0x0ff);
		}
		return (int) ((level[1] & 0x0ff) + (level[2] & 0x0ff)* 256 + (level[3] & 0x0ff)* Math.pow(256, 2) + (level[4] & 0x0ff)* Math.pow(256, 3));
		
	}
	
	/**
	 * 墨水量的加密算法:
	 * 		1, 按位取反
	 * 		2. 錯序：Byte3 （bit0~7） Byte4(bit8~15) Byte1(bit16~23) Byte2(bit24~31)
	 * 		3. 校驗和： Byte0
	 */
	public byte[] encryptInkLevel(int level) {
		if (level < INK_LEVEL_MIN || level > INK_LEVEL_MAX) {
			return null;
		}
		byte[] ink = new byte[16];
		if(true) {
			ink[0] = (byte) (level & 0x0ff); 
			ink[1] = (byte) ((level>>8) & 0x0ff);
			ink[2] = (byte) ((level>>(8*2)) & 0x0ff);
			ink[3] = (byte) ((level>>(8*3)) & 0x0ff);
// H.M.Wang 2021-8-23 设置ink16字节的其他部分为一定墨量值的组合或者倍数组合
// 注意，当向RFID写墨水值的时候该值的加密调用的是本encryptInkLevel函数，但是当读取墨水值的时候使用的是decryptInkMAX函数，而非decryptInkLevel函数，这一点要注意
			int level1 = level * 2;
			ink[4] = (byte) (level & 0x0ff);
			ink[5] = (byte) ((level>>8) & 0x0ff);
			ink[6] = (byte) (level1 & 0x0ff);
			ink[7] = (byte) ((level1>>8) & 0x0ff);
			ink[8] = (byte) (level1 & 0x0ff);
			ink[9] = (byte) ((level1>>8) & 0x0ff);
			ink[10] = (byte) (level & 0x0ff);
			ink[11] = (byte) ((level>>8) & 0x0ff);
			ink[12] = (byte) (level & 0x0ff);
			ink[13] = (byte) ((level>>8) & 0x0ff);
			ink[14] = (byte) (level1 & 0x0ff);
			ink[15] = (byte) ((level1>>8) & 0x0ff);
// End of H.M.Wang 2021-8-23 设置ink16字节的其他部分为一定墨量值的组合或者倍数组合
		} else {
			ink[3] = (byte) (level & 0x0ff); 
			ink[3] = (byte) (~ink[3] & 0x0ff);
			ink[4] = (byte) ((level>>8) & 0x0ff);
			ink[4] = (byte) (~ink[4] & 0x0ff);
			ink[1] = (byte) ((level>>(8*2)) & 0x0ff);
			ink[1] = (byte) (~ink[1] & 0x0ff);
			ink[2] = (byte) ((level>>(8*3)) & 0x0ff);
			ink[2] = (byte) (~ink[2] & 0x0ff);
			ink[0] = (byte) (ink[1] + ink[2] + ink[3] + ink[4]);
		}
		return ink;
	}
	
	
	public int decryptInkMAX(byte[] level) {
		if (level == null || level.length < 6) {
			return 0;
		}
		
		return (int) ((level[1] & 0x0ff) + (level[2] & 0x0ff)* 256 + (level[3] & 0x0ff)* Math.pow(256, 2) + (level[4] & 0x0ff)* Math.pow(256, 3) + (level[5] & 0x0ff)* Math.pow(256, 4));
		
	}
	
	
	
}
