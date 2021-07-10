/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2017-2018 HP Development Company, L.P.
   Made in U.S.A.
 */
 
 
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int			uint32_t;

 
// GPIO inputs/outputs: low word is bit MASK; high word is device (1 is Host HPIO)
typedef enum
{	
	GPIO_I_BROKEN_BAG		= 0x0004,
	GPIO_I_SUPPLY_PRSNT_L	= 0x0008,
	GPIO_O_INK_VALVE_ON		= 0x0010,
	GPIO_O_AIR_VALVE_ON		= 0x0020,
	GPIO_O_PUMP_ENABLE		= 0x0040,
	GPIO_I_AIR_PRESS_LOW	= 0x0080,
	GPIO_O_HEART_BEAT		= 0x0100,
	GPIO_O_LED_R			= 0x0200,
	GPIO_O_LED_Y			= 0x0400,
	GPIO_O_LED_G			= 0x0800,
	GPIO_O_AIR_VALVE_HOLD	= 0x1000,
	GPIO_O_INK_VALVE_HOLD	= 0x2000,
	GPIO_O_RESET			= 0x4000,
	// invalid
	GPIO_INVALID			= 0xFF0000,
	
	// bit combinations (all two port GPIO)
	COMBO_INK_BOTH			= (GPIO_O_INK_VALVE_ON | GPIO_O_INK_VALVE_HOLD),
	COMBO_AIR_BOTH			= (GPIO_O_AIR_VALVE_ON | GPIO_O_AIR_VALVE_HOLD),
	COMBO_AIR_PUMP_ALL		= (COMBO_AIR_BOTH | GPIO_O_PUMP_ENABLE),
	COMBO_INK_AIR_PUMP_ALL	= (COMBO_INK_BOTH | COMBO_AIR_BOTH | GPIO_O_PUMP_ENABLE),
	COMBO_ALL_GEN_BITS		= 0x007FFC,		// input AND output bits for IDS (General GPIO)
} IDS_GPIO_Desc;
 
#define LEVEL_INFO 0
#define LEVEL_WARN 1
#define LEVEL_ERR 2
#define IDS_NA -1

extern int IDS_Init(void (*ids_callback_func)(int ids, int level, const char *message));
extern void IDS_Shutdown(void);
extern void IDS_LockAccess(void);
extern void IDS_LockAccessAndSelectIDS(int IDS);
extern void IDS_UnlockAccess(void);
extern void IDS_UnlockAccessAndDeSelectBothIDS(void);
extern int IDS_Present(int IDS);

extern int IDS_I2C_RawRead(int IDS, const uint8_t I2CAddress, uint8_t *Data, size_t Length);
extern uint8_t IDS_I2C_ReadByte(int IDS, const uint8_t I2CAddress);
extern uint8_t IDS_I2C_ReadByteFromRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register);
extern uint16_t IDS_I2C_ReadWordFromRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register);
extern int IDS_I2C_ReadTwoBytesFromRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, uint8_t *TwoBytes);

extern int IDS_I2C_RawWrite(int IDS, const uint8_t I2CAddress, const uint8_t *Data, const size_t Length);
extern int IDS_I2C_WriteByte(int IDS, const uint8_t I2CAddress, const uint8_t Byte);
extern int IDS_I2C_WriteTwoBytes(int IDS, const uint8_t I2CAddress, const uint8_t FirstByte, const uint8_t SecondByte);
extern int IDS_I2C_WriteByteToRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, const uint8_t Byte);
extern int IDS_I2C_WriteWordToRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, const uint16_t Word);
extern int IDS_I2C_WriteTwoBytesToRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, const uint8_t Byte1, const uint8_t Byte2);

extern int IDS_DAC_SetSetpointPSI(int IDS, float PSI);
extern void IDS_ClearPILSBuffer(int IDS);
extern void IDS_MonitorPILS(int IDS);
extern void IDS_MonitorPressure(int IDS);
extern void IDS_MonitorOff(int IDS);
extern void IDS_MonitorOn(int IDS);
extern float IDS_LastAvgPILS(int IDS);
extern float IDS_LastPressure(int IDS);

extern uint8_t IDS_GPIO_ReadBit(int IDS, IDS_GPIO_Desc GPIOBit);
extern void IDS_GPIO_SetBits(int IDS, IDS_GPIO_Desc GPIODesc);
extern void IDS_GPIO_ClearBits(int IDS, IDS_GPIO_Desc GPIODesc);
extern int IDS_GPIO_GetBitDesc(char *Name);
extern uint16_t IDS_GPIO_DescToBitMask(IDS_GPIO_Desc GPIODesc);
extern int IDS_GPIO_Present(int IDS);
extern uint16_t IDS_GPIO_GetAllBits(int IDS);
extern void IDS_LED_On(int IDS, int LED);
extern void IDS_LED_Off(int IDS, int LED);
extern void IDS_LED_Blink(int IDS, int LED);
extern void IDS_LED_AllOff(void);
extern void ResetMCU(void);

extern int IDS_RTC_SetRTCFromSystemClock(void);
extern struct tm* IDS_RTC_GetRTCTime(void);
extern char* IDS_RTC_GetRTCTimeString(void);
extern struct tm* IDS_RTC_SetSystemClockFromRTC(void);

