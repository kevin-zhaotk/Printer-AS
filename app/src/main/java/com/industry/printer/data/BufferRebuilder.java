package com.industry.printer.data;

import com.industry.printer.FileFormat.SystemConfigFile;
import com.industry.printer.Utils.Debug;

/**
 * Created by hmwan on 2020/3/3.
 */

public class BufferRebuilder {
    public static final String TAG = BufferRebuilder.class.getSimpleName();

    private byte[] mByteBuffer;             // 打印数据缓存区，传入的数据为char(2个字节)，需要转化为byte(1个字节)的。转换时，char的高位低位在前，高位在后；处理完成合成char时原路重构
    private int mColNum = 0;                // 打印缓存区的列数
//    private int mBytesPerColumn = 0;        // 每列的字节数；mColNum * mBytesPerColumn === mByteBuffer.length
    private int mBlockNum = 0;              // 每列中包含的块的数量，每个块为一个处理的数据单元
//    private int mBytesPerBlock = 0;         // 每个块的字节数；mBlockNum * mBytesPerBlock === mBytesPerColumn

    public BufferRebuilder(char[] src, int colCharNum, int blockNum) {
//..        Debug.d(TAG, "src.length = " + src.length + "; colCharNum = " + colCharNum + "; blockNum = " + blockNum);
        try {
            mColNum = src.length / colCharNum;
//            mBytesPerColumn = colCharNum * 2;
// H.M.Wang 2021-9-1 防止不能整除而产生目标空间不够大而出现OutOfBounds的异常
//            mByteBuffer = new byte[mColNum * colCharNum * 2];
            mByteBuffer = new byte[src.length * 2];
// End of H.M.Wang 2021-9-1 防止不能整除而产生目标空间不够大而出现OutOfBounds的异常

            mBlockNum = blockNum;
//            mBytesPerBlock = mBytesPerColumn / mBlockNum;

//            StringBuilder sb = new StringBuilder();
            for(int i=0; i<src.length; i++) {
                char tmp = src[i];
//                String str = "0000" + Integer.toHexString(tmp);
//                sb.append(str.substring(str.length()-4)  + " ");
                mByteBuffer[2*i] = (byte)(tmp & 0x00ff);
                mByteBuffer[2*i+1] = (byte)((tmp & 0xff00) >> 8);
            }
//            Debug.d(TAG, "[" + sb.toString() + "]");
//            Debug.d(TAG, "[" + ByteArrayUtils.toHexString(mByteBuffer) + "]");
//            Debug.d(TAG, "BufLength: " + mByteBuffer.length + "; Columns: " + mColNum + "; Bytes per Column: " + mBytesPerColumn + "; Blocks: " + mBlockNum + "; Bytes per Block: " + mBytesPerBlock);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public BufferRebuilder shift(int[] shifts) {
        try {
//            Debug.i(TAG, "shift");
            if(shifts.length != mBlockNum) {
                Debug.e(TAG, "Block number doesn't match!");
                return this;
            }

            int addedCols = 0;
            for(int s: shifts) {
//                Debug.d(TAG, "shift: " + s);
                addedCols = Math.max(s, addedCols);
            }
            if(addedCols == 0)  {
//..                Debug.i(TAG, "No shift required!");
                return this;
            }

            int bytesPerColumn = mByteBuffer.length / mColNum;        // 每列的字节数
            int bytesPerBlock = bytesPerColumn / mBlockNum;
            int newColNum = mColNum + addedCols;
            byte[] newBuf = new byte[newColNum * bytesPerColumn];

            for(int i=0; i<mColNum; i++) {
                for(int j=0; j<mBlockNum; j++) {
                    System.arraycopy(mByteBuffer, i * bytesPerColumn + j * bytesPerBlock, newBuf, (i + shifts[j]) * bytesPerColumn + j * bytesPerBlock,  bytesPerBlock);
                }
            }

            mColNum = newColNum;
            mByteBuffer = newBuf;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return this;
    }

    public BufferRebuilder mirror(int[] mirrors) {
        try {
//            Debug.i(TAG, "mirror");
            if(mirrors.length != mBlockNum) {
                Debug.e(TAG, "Block number doesn't match!");
                return this;
            }

            boolean needed = false;
            for(int i=0; i<mirrors.length; i++) {
//                Debug.d(TAG, "mirror: " + mirrors[i]);
                if(mirrors[i] == SystemConfigFile.DIRECTION_REVERS) {
                    needed = true;
                }
            }
            if(!needed)  {
//..                Debug.i(TAG, "No mirror required!");
                return this;
            }

            int bytesPerColumn = mByteBuffer.length / mColNum;        // 每列的字节数
            int bytesPerBlock = bytesPerColumn / mBlockNum;

            byte[] newBuf = new byte[mByteBuffer.length];

            for(int i=0; i<mColNum; i++) {
                for(int j=0; j<mBlockNum; j++) {
                    if (mirrors[j] == SystemConfigFile.DIRECTION_REVERS) {
                        System.arraycopy(mByteBuffer, i * bytesPerColumn + j * bytesPerBlock, newBuf, (mColNum-1-i) * bytesPerColumn + j * bytesPerBlock,  bytesPerBlock);
                    } else {
                        System.arraycopy(mByteBuffer, i * bytesPerColumn + j * bytesPerBlock, newBuf, i * bytesPerColumn + j * bytesPerBlock,  bytesPerBlock);
                    }
                }
            }

            mByteBuffer = newBuf;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return this;
    }

    private byte revert(byte src) {
        byte dst = 0;
        for (int i=0; i<=6; i++) {
            if ((src & (0x01 << i)) > 0) {
                dst |= 0x01 << (6 - i);
            }
        }
        if ((src & 0x080) > 0) {
            dst |= 0x080;
        }

        return dst;
    }

    private byte[] revert(byte[] src) {
        byte[] dst = new byte[src.length];

        for (int i=0; i<src.length; i++) {
            byte tmp = 0x00;
            for(int j=0; j<8; j++) {
                if ((src[i] & (0x01 << j)) > 0) {
                    tmp |= (0x01 << (7 - j));
                }
            }
            dst[src.length-1-i] = tmp;
        }

        return dst;
    }

    public BufferRebuilder reverse(int pattern) {
        try {
//            Debug.i(TAG, "reverse pattern: " + pattern);
            if ((pattern & 0x0f) == 0x00) {
                return this;
            }

            int bytesPerColumn = mByteBuffer.length / mColNum;        // 每列的字节数

            if (bytesPerColumn != 4) {
                Debug.e(TAG, "Not proper bytes of column!");
                return this;
            }

            byte[] newBuf = new byte[mByteBuffer.length];

            // 4头整体反转
            for(int i=0; i<mColNum; i++) {
                if (pattern == 0x0f) {
                    byte[] tmp = new byte[bytesPerColumn];
                    System.arraycopy(mByteBuffer, i * bytesPerColumn, tmp, 0,  bytesPerColumn);
                    System.arraycopy(revert(tmp), 0, newBuf, i * bytesPerColumn,  bytesPerColumn);
                } else {
                    // 1-2反转
                    if ((pattern & 0x03) == 0x03) {
                        byte[] tmp = new byte[bytesPerColumn/2];
                        System.arraycopy(mByteBuffer, i * bytesPerColumn, tmp, 0,  bytesPerColumn/2);
                        System.arraycopy(revert(tmp), 0, newBuf, i * bytesPerColumn,  bytesPerColumn/2);
                    } else if ((pattern & 0x03) == 0x01) {		//仅1头反转
                        newBuf[i * bytesPerColumn] = revert(mByteBuffer[i * bytesPerColumn]);
                        newBuf[i * bytesPerColumn + 1] = mByteBuffer[i * bytesPerColumn + 1];
                    } else if ((pattern & 0x03) == 0x02) {		//仅2头反转
                        newBuf[i * bytesPerColumn] = mByteBuffer[i * bytesPerColumn];
                        newBuf[i * bytesPerColumn + 1] = revert(mByteBuffer[i * bytesPerColumn + 1]);
                    } else {
                        System.arraycopy(mByteBuffer, i * bytesPerColumn, newBuf, i * bytesPerColumn,  bytesPerColumn/2);
                    }
                    // 3-4反转
                    if ((pattern & 0x0C) == 0x0C) {
                        byte[] tmp = new byte[bytesPerColumn/2];
                        System.arraycopy(mByteBuffer, i * bytesPerColumn + bytesPerColumn/2, tmp, 0,  bytesPerColumn/2);
                        System.arraycopy(revert(tmp), 0, newBuf, i * bytesPerColumn + bytesPerColumn/2,  bytesPerColumn/2);
                    } else if ((pattern & 0x0C) == 0x04) {		//仅3头反转
                        newBuf[i * bytesPerColumn + 2] = revert(mByteBuffer[i * bytesPerColumn + 2]);
                        newBuf[i * bytesPerColumn + 3] = mByteBuffer[i * bytesPerColumn + 3];
                    } else if ((pattern & 0x0C) == 0x08) {		//仅4头反转
                        newBuf[i * bytesPerColumn + 2] = mByteBuffer[i * bytesPerColumn + 2];
                        newBuf[i * bytesPerColumn + 3] = revert(mByteBuffer[i * bytesPerColumn + 3]);
                    } else {
                        System.arraycopy(mByteBuffer, i * bytesPerColumn + bytesPerColumn/2, newBuf, i * bytesPerColumn + bytesPerColumn/2,  bytesPerColumn/2);
                    }
                }
            }

            mByteBuffer = newBuf;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return this;
    }

    public char[] getCharBuffer() {
        char[] dstBuf = new char[mByteBuffer.length/2];

        for(int i=0; i<mByteBuffer.length; i+=2) {
            dstBuf[i/2] = (char)(((mByteBuffer[i+1] << 8) & 0xff00) | (mByteBuffer[i] & 0x00ff));
        }

        return dstBuf;
    }

    public int getColumnNum() {
        return mColNum;
    }
}
