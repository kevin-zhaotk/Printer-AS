package com.industry.printer.PHeader;

import com.industry.printer.FileFormat.SystemConfigFile;

/**
 * Created by hmwan on 2021/9/22.
 */

public class N_NozzleManager {
    private static N_Nozzle mNozzle = null;

    public static N_Nozzle getPrinterNozzle() {
        SystemConfigFile configFile = SystemConfigFile.getInstance();
        if(null == configFile) return null;

        if(null != mNozzle && mNozzle.getIndex() == configFile.getParam(SystemConfigFile.INDEX_HEAD_TYPE)) return mNozzle;

        switch (configFile.getParam(SystemConfigFile.INDEX_HEAD_TYPE)) {
            case N_Nozzle.Index.NOZZLE_INDEX_127:
                mNozzle = new N_Nozzle127(N_Nozzle.Index.NOZZLE_INDEX_127);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_127x2:
                mNozzle = new N_Nozzle127x2(N_Nozzle.Index.NOZZLE_INDEX_127x2);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_127x3:
                mNozzle = new N_Nozzle127x3(N_Nozzle.Index.NOZZLE_INDEX_127x3);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_127x4:
                mNozzle = new N_Nozzle127x4(N_Nozzle.Index.NOZZLE_INDEX_127x4);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_R6X48:
                mNozzle = new N_NozzleR6X48(N_Nozzle.Index.NOZZLE_INDEX_R6X48);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_R6X50:
                mNozzle = new N_NozzleR6X50(N_Nozzle.Index.NOZZLE_INDEX_R6X50);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_1_INCH:
                mNozzle = new N_Nozzle254(N_Nozzle.Index.NOZZLE_INDEX_1_INCH);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_1_INCH_DUAL:
                mNozzle = new N_Nozzle254x2(N_Nozzle.Index.NOZZLE_INDEX_1_INCH_DUAL);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_1_INCH_TRIPLE:
                mNozzle = new N_Nozzle254x3(N_Nozzle.Index.NOZZLE_INDEX_1_INCH_TRIPLE);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_1_INCH_FOUR:
                mNozzle = new N_Nozzle254x4(N_Nozzle.Index.NOZZLE_INDEX_1_INCH_FOUR);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_9MM:
                mNozzle = new N_Nozzle9MM(N_Nozzle.Index.NOZZLE_INDEX_9MM);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_E6X48:
                mNozzle = new N_NozzleE6X48(N_Nozzle.Index.NOZZLE_INDEX_E6X48);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_E6X50:
                mNozzle = new N_NozzleE6X50(N_Nozzle.Index.NOZZLE_INDEX_E6X50);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_E6X1:
                mNozzle = new N_NozzleE6X1(N_Nozzle.Index.NOZZLE_INDEX_E6X1);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_E5X48:
                mNozzle = new N_NozzleE5X48(N_Nozzle.Index.NOZZLE_INDEX_E5X48);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_E5X50:
                mNozzle = new N_NozzleE5X50(N_Nozzle.Index.NOZZLE_INDEX_E5X50);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_16_DOT:
                mNozzle = new N_Nozzle16Dot(N_Nozzle.Index.NOZZLE_INDEX_16_DOT);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_32_DOT:
                mNozzle = new N_Nozzle32Dot(N_Nozzle.Index.NOZZLE_INDEX_32_DOT);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_32DN:
                mNozzle = new N_Nozzle32DN(N_Nozzle.Index.NOZZLE_INDEX_32DN);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_32SN:
                mNozzle = new N_Nozzle32SN(N_Nozzle.Index.NOZZLE_INDEX_32SN);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_64_DOT:
                mNozzle = new N_Nozzle64Dot(N_Nozzle.Index.NOZZLE_INDEX_64_DOT);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_64SN:
                mNozzle = new N_Nozzle64SN(N_Nozzle.Index.NOZZLE_INDEX_64SN);
                break;
            case N_Nozzle.Index.NOZZLE_INDEX_96DN:
                mNozzle = new N_Nozzle96DN(N_Nozzle.Index.NOZZLE_INDEX_96DN);
                break;
            default:
                mNozzle = null;
                break;
        }

        return mNozzle;
    }
}
