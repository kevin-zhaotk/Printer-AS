//
// Created by hmwan on 2020/4/2.
//

#ifndef PRINTER_AS_GIT_LEVEL_MEMORY_ACCESS_H
#define PRINTER_AS_GIT_LEVEL_MEMORY_ACCESS_H

#include <stdint.h>

#define LEVEL_I2C_OK        0
#define LEVEL_I2C_FAILED    -1
//#define LEVEL_I2C_UNDER_RANGE_ERROR         -10
//#define LEVEL_I2C_OVER_RANGE_ERROR          -11
//#define LEVEL_I2C_TIMEOUT_ERROR             -12
//#define LEVEL_I2C_AMPLITUDE_ERROR           -13

// Definitions for Channel Data Error
#define CHANNEL_DATA_UNDER_RANGE_ERROR       0x8000
#define CHANNEL_DATA_OVER_RANGE_ERROR        0x4000
#define CHANNEL_DATA_TIMEOUT_ERROR           0x2000
#define CHANNEL_DATA_AMPLITUDE_ERROR         0x1000

#define CHANNEL_INDICATE_MASK                0xC000
#define DATA_AREA_MASK                       (~CHANNEL_INDICATE_MASK)

// Definitions for Status
#define STATUS_FOR_CHANNEL_0                 0x0000
#define STATUS_FOR_CHANNEL_1                 0x4000
#define STATUS_FOR_CHANNEL_2                 0x8000
#define STATUS_FOR_CHANNEL_3                 0xC000

#define STATUS_UNDER_RANGE_ERROR             0x2000
#define STATUS_OVER_RANGE_ERROR              0x1000
#define STATUS_WATCHDOG_TIMEOUT_ERROR        0x0800
#define STATUS_SENSOR_AMP_HIGH_ERROR         0x0400
#define STATUS_SENSOR_AMP_LOW_ERROR          0x0200
#define STATUS_ZERO_COUNT_ERROR              0x0100
#define STATUS_DATA_READY                    0x0040
#define STATUS_UNREAD_CONVERSION_0           0x0008
#define STATUS_UNREAD_CONVERSION_1           0x0004
#define STATUS_UNREAD_CONVERSION_2           0x0002
#define STATUS_UNREAD_CONVERSION_3           0x0001

// Definitions for Error Config
#define REPORT_URERR_TO_CHDATA               0x8000
#define REPORT_ORERR_TO_CHDATA               0x4000
#define REPORT_WDERR_TO_CHDATA               0x2000
#define REPORT_AHERR_TO_CHDATA               0x1000
#define REPORT_ALERR_TO_CHDATA               0x0800

#define REPORT_URERR_TO_INTB                 0x0080
#define REPORT_ORERR_TO_INTB                 0x0040
#define REPORT_WDERR_TO_INTB                 0x0020
#define REPORT_AHERR_TO_INTB                 0x0010
#define REPORT_ALERR_TO_INTB                 0x0008
#define REPORT_DATA_READ_TO_INTB             0x0001

// Definitions for Config
#define CONFIG_ACTIVE_CHAIN                 0xC000
#define CONFIG_ACTIVE_CHAIN_CH0             0x0000          // 检查时：(a & CONFIG_ACTIVE_CHAIN) == CONFIG_ACTIVE_CHAIN_CH0
                                                            // 设置时：((a & (~CONFIG_ACTIVE_CHAIN)) | CONFIG_ACTIVE_CHAIN_CH0)
#define CONFIG_ACTIVE_CHAIN_CH1             0x4000
#define CONFIG_ACTIVE_CHAIN_CH2             0x8000
#define CONFIG_ACTIVE_CHAIN_CH3             0xC000

#define CONFIG_SLEEP_MODE                   0x2000
#define CONFIG_SLEEP_MODE_ENABLE            CONFIG_SLEEP_MODE
#define CONFIG_ACTIVE_MODE_ENABLE           (~CONFIG_SLEEP_MODE_ENABLE)

#define CONFIG_SENSOR_RP_OVERRIDE           0x1000
#define CONFIG_SENSOR_RP_OVERRIDE_ENABLE    CONFIG_SENSOR_RP_OVERRIDE

#define CONFIG_SENSOR_ACTIVATION_MODE_SELECTION    0x0800
#define CONFIG_LOW_POWER_ACTIVATION_MODE           CONFIG_SENSOR_ACTIVATION_MODE_SELECTION
#define CONFIG_FULL_CURRENT_ACTIVATION_MODE        0x0000

#define CONFIG_AUTO_SENSOR_AMP_CORRECTION_DISABLE  0x0400
#define CONFIG_AUTO_SENSOR_AMP_CORRECTION_DISABLED CONFIG_AUTO_SENSOR_AMP_CORRECTION_DISABLE
#define CONFIG_AUTO_SENSOR_AMP_CORRECTION_ENABLED  0x0000

#define CONFIG_REF_CLOCK_SRC                0x0200
#define CONFIG_REF_CLOCK_SRC_CLKIN          CONFIG_REF_CLOCK_SRC
#define CONFIG_REF_CLOCK_SRC_OSCILATOR      0x0000

#define CONFIG_INTB_DISABLE                 0x0080
#define CONFIG_INTB_DISABLE_NOT_ASSERT      CONFIG_INTB_DISABLE
#define CONFIG_INTB_DISABLE_ASSERT          0x0000

#define CONFIG_HIGH_CURRENT_SENSOR          0x0040
#define CONFIG_HIGH_CURRENT_GT_15MA         CONFIG_HIGH_CURRENT_SENSOR
#define CONFIG_HIGH_CURRENT_LT_15MA         0x0000

// Definitions for Mux_Config
#define MUX_CONFIG_AUTOSCAN_ENABLE          0x8000
#define MUX_CONFIG_AUTOSCAN_RR_SEQUENCE     MUX_CONFIG_AUTOSCAN_ENABLE
#define MUX_CONFIG_AUTOSCAN_ACTIVE_CHAN     0x0000

#define MUX_CONFIG_RR_SEQUENCE              0x6000
#define MUX_CONFIG_ACTIVE_CHAIN_CH01        0x0000
#define MUX_CONFIG_ACTIVE_CHAIN_CH012       0x2000
#define MUX_CONFIG_ACTIVE_CHAIN_CH0123      0x4000
#define MUX_CONFIG_ACTIVE_CHAIN_CH10        0x6000

#define MUX_CONFIG_DEGLITCH                 0x0007
#define MUX_CONFIG_DEGLITCH_1_0MHZ          0x0001
#define MUX_CONFIG_DEGLITCH_3_3MHZ          0x0004
#define CONFIG_ACTIVE_CHAIN_10MHZ           0x0005
#define CONFIG_ACTIVE_CHAIN_33MHZ           0x0007

// Definitions for Reset
#define RESET_BIT                           0x8000

// Definitions for Drive Current
#define DRIVE_CURRENT_IDRIVE                0xF800
#define DRIVE_CURRENT_INIT_IDRIVE           0x07C0

/*
typedef enum
{
    LEVEL_CHANNEL_0 = 0x00,
    LEVEL_CHANNEL_1,
    LEVEL_CHANNEL_2,
    LEVEL_CHANNEL_3
} LEVEL_CHANNEL_t;
*/
int readChannelData0(uint32_t *data);
int readChannelData1(uint32_t *data);
int readChannelData2(uint32_t *data);
int readChannelData3(uint32_t *data);

int readRCount0(uint16_t *data);
int writeRCount0(uint16_t *data);
int readRCount1(uint16_t *data);
int writeRCount1(uint16_t *data);
int readRCount2(uint16_t *data);
int writeRCount2(uint16_t *data);
int readRCount3(uint16_t *data);
int writeRCount3(uint16_t *data);

int readOffset0(uint16_t *data);
int writeOffset0(uint16_t *data);
int readOffset1(uint16_t *data);
int writeOffset1(uint16_t *data);
int readOffset2(uint16_t *data);
int writeOffset2(uint16_t *data);
int readOffset3(uint16_t *data);
int writeOffset3(uint16_t *data);

int readSettleCount0(uint16_t *data);
int writeSettleCount0(uint16_t *data);
int readSettleCount1(uint16_t *data);
int writeSettleCount1(uint16_t *data);
int readSettleCount2(uint16_t *data);
int writeSettleCount2(uint16_t *data);
int readSettleCount3(uint16_t *data);
int writeSettleCount3(uint16_t *data);

int readClockDivider0(uint16_t *data);
int writeClockDivider0(uint16_t *data);
int readClockDivider1(uint16_t *data);
int writeClockDivider1(uint16_t *data);
int readClockDivider2(uint16_t *data);
int writeClockDivider2(uint16_t *data);
int readClockDivider3(uint16_t *data);
int writeClockDivider3(uint16_t *data);

int readStatus(uint16_t *data);

int readErrorConfig(uint16_t *data);
int writeErrorConfig(uint16_t *data);

int readConfig(uint16_t *data);
int writeConfig(uint16_t *data);

int readMuxConfig(uint16_t *data);
int writeMuxConfig(uint16_t *data);

int readReset(uint16_t *data);
int writeReset(uint16_t *data);

int readDriveCurrent0(uint16_t *data);
int writeDriveCurrent0(uint16_t *data);
int readDriveCurrent1(uint16_t *data);
int writeDriveCurrent1(uint16_t *data);
int readDriveCurrent2(uint16_t *data);
int writeDriveCurrent2(uint16_t *data);
int readDriveCurrent3(uint16_t *data);
int writeDriveCurrent3(uint16_t *data);

int readManufactureID(uint16_t *data);

int readDeviceID(uint16_t *data);

#endif //PRINTER_AS_GIT_LEVEL_MEMORY_ACCESS_H
