/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2017-2018 HP Development Company, L.P.
   Made in U.S.A.
 */
 
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "ids.h"



// Customization for hardware
#define I2C_DEVICE "/dev/i2c-1"
#define MUX_ADDR 0xEE



// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@ Routines starting with "IDS_" are considered PUBLIC routines
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



// #####################################################################
// # Defines, Constants, and Globals
// #####################################################################

// I2C addresses
#define GPIO_I2C_ADDRESS 0x40
#define DAC_I2C_ADDRESS 0x1A
#define ADC_I2C_ADDRESS 0x90
#define RTC_I2C_ADDRESS 0xDE

// Define when not specifying IDS in a command (assumes previously set)
#define CURRENT_IDS -1

// Defines for background monitor thread
#define READ_PER_SEC 10					// PILS (or Pressure) readings per second
#define PRESSURE_SEC_PER_READ 2			// In PILS mode, pressure seconds per read

// mutex used to lock hardware access
static pthread_mutex_t IDSMutex;




// #####################################################################
// # Prototypes 
// #####################################################################

int I2CInit(void);
int GPIOInit(int IDS);
int BlinkInit(void);
int MonitorInit(void);
int RTCInit(void);




// #####################################################################
// # Local utilities
// #####################################################################

void InitRecursiveMutex(pthread_mutex_t *pMutex)
{
    // NOTE: can be locked multiple times (recursive), but locks/unlocks must match for the thread
	pthread_mutexattr_t MutexAttr;
	pthread_mutexattr_init(&MutexAttr);
    pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(pMutex, &MutexAttr);
    pthread_mutexattr_destroy(&MutexAttr);
}



// @@@ Deselect both IDS from the MUX
// @@@ NOTE: only IDS 0 and 1 are supported
void DeSelectBothIDS(void)
{
	// NOTE: calls to WriteByte must NOT specify IDS, or will recurse
	IDS_I2C_WriteByte(CURRENT_IDS, MUX_ADDR, 0);
	usleep(1000);	// 1ms
}



// @@@ Select an IDS from the MUX
// @@@ NOTE: only IDS 0 and 1 are supported
void SelectIDS(int IDS)
{
	if (IDS < 0 || IDS > 1)
	{
		DeSelectBothIDS();
		return;
	}
	// NOTE: calls to WriteByte must NOT specify IDS, or will recurse
	IDS_I2C_WriteByte(CURRENT_IDS, MUX_ADDR, (uint8_t)(0x01 << IDS));
	usleep(1000);	// 1ms
}




// #####################################################################
// # Callbacks
// #####################################################################

void (*_ids_callback_func)(int ids, int level, const char *message) = NULL;

void ids_callback(int ids, int level, const char *message)
{
    if (NULL != _ids_callback_func)
		(*_ids_callback_func)(ids, level, message);
}




// #####################################################################
// # IDS General
// #####################################################################

// @@@ Initialize the IDS system including Smart Card library
int IDS_Init(void (*ids_callback_func)(int ids, int level, const char *message))
{	
	// @@@ Set callbacks @@@
	_ids_callback_func = ids_callback_func;

	// @@@ Set up IDS components @@@

    // Create the IDS mutex
    InitRecursiveMutex(&IDSMutex);
   
    // Init the I2C device access
    if (I2CInit() < 0)
	{
		fprintf(stderr, "IDS_Init cannot init I2C"); 
		return -1;
	}
	
	// Init IDS GPIO / Blink
	if (GPIOInit(0) < 0)
	{
		fprintf(stderr, "IDS_Init: cannot init GPIO on IDS0"); 
		return -1;
	}
	if (GPIOInit(1) < 0)
	{
		fprintf(stderr, "IDS_Init: cannot init GPIO on IDS1"); 
		return -1;
	}
	if (BlinkInit() < 0)
	{
		fprintf(stderr, "IDS_Init cannot init blink thread"); 
		return -1;
	}
		
	// Init IDS RTC (only IDS 0)
	if (RTCInit() < 0)
	{
		fprintf(stderr, "IDS_Init: cannot init RTC"); 
		return -1;
	}
		
   	// @@@ Start background monitoring thread @@@

	if (MonitorInit() < 0)
	{
		fprintf(stderr, "IDS_Init cannot init monitor thread"); 
		return -1;
	}

	return 0;
}



// @@@ Shut down the IDS system including Smart Card library
void IDS_Shutdown(void)
{
	IDS_LED_AllOff();
}



// @@@ Lock access (mutex) to hardware AND select an IDS
void IDS_LockAccessAndSelectIDS(int IDS)
{
	IDS_LockAccess();
	SelectIDS(IDS);
}



// @@@ Lock access (mutex) to hardware
void IDS_LockAccess(void)
{
	pthread_mutex_lock(&IDSMutex);
}



// @@@ UNlock access (mutex) to hardware AFTER deselecting both IDS
void IDS_UnlockAccessAndDeSelectBothIDS(void)
{
	DeSelectBothIDS();
	IDS_UnlockAccess();
}



// @@@ UNlock access (mutex) to hardware
void IDS_UnlockAccess(void)
{
	pthread_mutex_unlock(&IDSMutex);
}




// #####################################################################
// # I2C
// #####################################################################

static bool I2C_Initialized = false;
static int I2C_File = -1;



// @@@ Init (open) the I2C device
int I2CInit(void)
{
    // if already initialized, don't do it again
    if (I2C_Initialized)
        return 0;

    // open the device
    I2C_File = open(I2C_DEVICE, O_RDWR);
    if (I2C_File < 0)
    {
		fprintf(stderr, "I2CInit: open errno: %d\n", errno);
        return -1;          // failure
    }
     
    // Initialized
    I2C_Initialized = true;
    return 0;
}



// @@@ Set I2C address for next read/write
// @@@ NOTE: this is the "full" address (lowest bit will used for read/write)
uint8_t SetI2CAddress(uint8_t I2CAddress)
{
    int ret;

    // set I2C address
    ret = ioctl(I2C_File, I2C_SLAVE, (uint8_t)(I2CAddress >> 1));
    if (ret < 0) 
    {
		fprintf(stderr, "SetI2CAddress: ioctl errno: %d\n", errno);
		return -1;
	}
	
	return 0;
}



// @@@ Raw read from I2C device
int IDS_I2C_RawRead(int IDS, const uint8_t I2CAddress, uint8_t *Data, size_t Length)
{
    int ret = 0;
    
    // if IDS specified, LOCK/UNLOCK
   	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);

    // set I2C address
    if (SetI2CAddress(I2CAddress) < 0)
		ret = -1;
    	
	// read data
	if (ret >= 0)
	{
		ret = read(I2C_File, (void *)(Data), Length);
		//if (ret < 0) fprintf(stderr, "IDS_I2C_RawRead: read errno: %d\n", errno);
	}
    
    // UNLOCK
   	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();

    return (ret < 0 ? -1 : 0);		// return 0 or -1
}



// @@@ Read a single byte from I2C device (no command/register)
// @@@ NOTE: error not returned
uint8_t IDS_I2C_ReadByte(int IDS, const uint8_t I2CAddress)
{
	int ret;
	uint8_t byte = 0;
	ret = IDS_I2C_RawRead(IDS, I2CAddress, &byte, 1);
	if (ret < 0) ids_callback(IDS, LEVEL_ERR, "IDS_I2C_RawRead returned an error");
	return byte;
}



// @@@ Read a single byte from I2C device AFTER writing the register command
// @@@ NOTE: error not returned
uint8_t IDS_I2C_ReadByteFromRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register)
{
	int ret;
	uint8_t byte = 0;
	
	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);		// if IDS specified, LOCK/UNLOCK multiple commands
	
	ret = IDS_I2C_RawWrite(CURRENT_IDS, I2CAddress, &Register, 1);
	if (ret < 0) ids_callback(IDS, LEVEL_ERR, "IDS_I2C_RawWrite returned an error");
	if (ret >= 0)
	{
		ret = IDS_I2C_RawRead(CURRENT_IDS, I2CAddress, &byte, 1);
		if (ret < 0) ids_callback(IDS, LEVEL_ERR, "IDS_I2C_RawRead returned an error");
	}
		
	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	
	return byte;
}



// @@@ Read a word (2 bytes) from I2C device AFTER writing the register command
// @@@ NOTE: error not returned
uint16_t IDS_I2C_ReadWordFromRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register)
{
	int ret;
	uint8_t buff[2];
	
	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);		// if IDS specified, LOCK/UNLOCK multiple commands
	
	ret = IDS_I2C_RawWrite(CURRENT_IDS, I2CAddress, &Register, 1);
	if (ret < 0) ids_callback(IDS, LEVEL_ERR, "IDS_I2C_RawWrite returned an error");
	if (ret >= 0)
	{
		ret = IDS_I2C_RawRead(CURRENT_IDS, I2CAddress, buff, 2);
		if (ret < 0) ids_callback(IDS, LEVEL_ERR, "IDS_I2C_RawRead returned an error");
	}
		
	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	
	return ((uint16_t)buff[1] << 8 | (uint16_t)buff[0]);
}



// @@@ Read two bytes from I2C device AFTER writing the register command
int IDS_I2C_ReadTwoBytesFromRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, uint8_t *TwoBytes)
{
	int ret;
	
	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);		// if IDS specified, LOCK/UNLOCK multiple commands
	
	ret = IDS_I2C_RawWrite(CURRENT_IDS, I2CAddress, &Register, 1);
	if (ret >= 0)
		ret = IDS_I2C_RawRead(CURRENT_IDS, I2CAddress, TwoBytes, 2);
		
   	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK

    return (ret < 0 ? -1 : 0);							// return 0 or -1
}



// @@@ Raw write to I2C device
int IDS_I2C_RawWrite(int IDS, const uint8_t I2CAddress, const uint8_t *Data, const size_t Length)
{
    int ret = 0;
    
    // if IDS specified, LOCK/UNLOCK
   	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);

    // set I2C address
    if (SetI2CAddress(I2CAddress) < 0)
		ret = -1;
    	
	// write data
	if (ret >= 0)
	{
		ret = write(I2C_File, (const void *)Data, Length);
		if (ret < 0) fprintf(stderr, "IDS_I2C_RawWrite: write errno: %d\n", errno);
	}
    
    // UNLOCK
   	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();

    return (ret < 0 ? -1 : 0);		// return 0 or -1
}



// @@@ Write a single byte to I2C device (no command/register)
int IDS_I2C_WriteByte(int IDS, const uint8_t I2CAddress, const uint8_t Byte)
{
	return IDS_I2C_RawWrite(IDS, I2CAddress, &Byte, 1);
}



// @@@ Write two bytes to I2C device (no command/register)
int IDS_I2C_WriteTwoBytes(int IDS, const uint8_t I2CAddress, const uint8_t FirstByte, const uint8_t SecondByte)
{
	uint8_t buff[2];
	buff[0] = FirstByte;
	buff[1] = SecondByte;
	return IDS_I2C_RawWrite(IDS, I2CAddress, buff, 2);
}



// @@@ Write a single byte to a register in I2C device
int IDS_I2C_WriteByteToRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, const uint8_t Byte)
{
	uint8_t buff[2];
	buff[0] = Register;
	buff[1] = Byte;
	return IDS_I2C_RawWrite(IDS, I2CAddress, buff, 2);
}



// @@@ Write a word (2 bytes) to a register in I2C device
int IDS_I2C_WriteWordToRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, const uint16_t Word)
{
	uint8_t buff[3];
	buff[0] = Register;
	buff[1] = (uint8_t)(Word & 0xFF);
	buff[2] = (uint8_t)(Word >> 8);
	return IDS_I2C_RawWrite(IDS, I2CAddress, buff, 3);
}



// @@@ Write two bytes to a register in I2C device
int IDS_I2C_WriteTwoBytesToRegister(int IDS, const uint8_t I2CAddress, const uint8_t Register, const uint8_t Byte1, const uint8_t Byte2)
{
	uint8_t buff[3];
	buff[0] = Register;
	buff[1] = Byte1;
	buff[2] = Byte2;
	return IDS_I2C_RawWrite(IDS, I2CAddress, buff, 3);
}




// #####################################################################
// # GPIO
// #####################################################################

#define GPIO_CMD_INPUT 0x00
#define GPIO_CMD_OUTPUT 0x02
#define GPIO_CMD_CONFIG 0x06

#define GPIO_CONFIG_PORT0 0x8F			// IOOO IIII
#define GPIO_CONFIG_PORT1 0xC0			// IIOO OOOO
#define GPIO_RESET_CONFIG_PORT1 0x80	// I0OO OOOO

#define GPIO_DUMMY_BITS 0x0088		// return for non-configured supply (not present, etc.)

#define LED_ON_MS 500
#define LED_OFF_MS 1000

static pthread_t BlinkThread;
static int BlinkMask[2];
static bool BlinkOn = false; 



int GPIOInit(int IDS)
{
	int ret = 0;	
	IDS_LockAccessAndSelectIDS(IDS);		// LOCK/UNLOCK multiple commands
	
	ret = IDS_I2C_WriteTwoBytesToRegister(CURRENT_IDS, GPIO_I2C_ADDRESS, GPIO_CMD_CONFIG, GPIO_CONFIG_PORT0, GPIO_CONFIG_PORT1);
	if (ret == 0) ret = IDS_I2C_WriteWordToRegister(CURRENT_IDS, GPIO_I2C_ADDRESS, GPIO_CMD_OUTPUT, 0x0000);
	
	IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	return ret;
}



// @@@ Read a GPIO device
uint16_t ReadGPIO(int IDS)
{
	return IDS_I2C_ReadWordFromRegister(IDS, GPIO_I2C_ADDRESS, GPIO_CMD_INPUT);
}



// @@@ Read a GPIO device OUTPUT
uint16_t ReadGPIOOutput(int IDS)
{
	return IDS_I2C_ReadWordFromRegister(IDS, GPIO_I2C_ADDRESS, GPIO_CMD_OUTPUT);
}



// @@@ Read a SINGLE bit from GPIO (returned as 0 or 1)
uint8_t IDS_GPIO_ReadBit(int IDS, IDS_GPIO_Desc GPIOBit)
{
	uint16_t bits = ReadGPIO(IDS);
	return (uint8_t)((bits & (IDS_GPIO_DescToBitMask(GPIOBit))) == 0 ? 0 : 1);
}



// @@@ Extract the bit mask from GPIO descriptor
uint16_t IDS_GPIO_DescToBitMask(IDS_GPIO_Desc GPIODesc)
{
	return (uint16_t)(GPIODesc & 0x00FFFF);
}



// @@@ Set a bit OR bits (in same port)
void IDS_GPIO_SetBits(int IDS, IDS_GPIO_Desc GPIODesc)
{
	int ret;
	
	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);		// if IDS specified, LOCK/UNLOCK multiple commands
	
	uint16_t bits = ReadGPIOOutput(CURRENT_IDS);
	uint16_t mask = IDS_GPIO_DescToBitMask(GPIODesc);
	bits |= mask;
	ret = IDS_I2C_WriteWordToRegister(CURRENT_IDS, GPIO_I2C_ADDRESS, GPIO_CMD_OUTPUT, bits);
	
	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
}



// @@@ Clear a bit OR bits (in same port)
void IDS_GPIO_ClearBits(int IDS, IDS_GPIO_Desc GPIODesc)
{
	int ret;
	
	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);		// if IDS specified, LOCK/UNLOCK multiple commands
	
	uint16_t bits = ReadGPIOOutput(CURRENT_IDS);
	uint16_t mask = ~(IDS_GPIO_DescToBitMask(GPIODesc));
	bits &= mask;
	ret = IDS_I2C_WriteWordToRegister(CURRENT_IDS, GPIO_I2C_ADDRESS, GPIO_CMD_OUTPUT, bits);
	
	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
}



// @@@ Get Bit Descriptor by name
int IDS_GPIO_GetBitDesc(char *Name)
{
	// General GPIO
	if (strncmp(Name, "I_BROKEN_BAG", 99) == 0)
		return GPIO_I_BROKEN_BAG;		
	else if (strncmp(Name, "I_SUPPLY_PRSNT_L", 99) == 0)
		return GPIO_I_SUPPLY_PRSNT_L;		
	else if (strncmp(Name, "O_INK_VALVE_ON", 99) == 0)
		return GPIO_O_INK_VALVE_ON;		
	else if (strncmp(Name, "O_AIR_VALVE_ON", 99) == 0)
		return GPIO_O_AIR_VALVE_ON;		
	else if (strncmp(Name, "O_PUMP_ENABLE", 99) == 0)
		return GPIO_O_PUMP_ENABLE;		
	else if (strncmp(Name, "I_AIR_PRESS_LOW", 99) == 0)
		return GPIO_I_AIR_PRESS_LOW;		
	else if (strncmp(Name, "O_HEART_BEAT", 99) == 0)
		return GPIO_O_HEART_BEAT;
	else if (strncmp(Name, "O_AIR_VALVE_HOLD", 99) == 0)
		return GPIO_O_AIR_VALVE_HOLD;
	else if (strncmp(Name, "O_INK_VALVE_HOLD", 99) == 0)
		return GPIO_O_INK_VALVE_HOLD;
	// invalid
	else if (strncmp(Name, "INVALID", 99) == 0)
		return GPIO_INVALID;	
	// bit combinations
	else if (strncmp(Name, "COMBO_INK_BOTH", 99) == 0)
		return COMBO_INK_BOTH;
	else if (strncmp(Name, "COMBO_AIR_BOTH", 99) == 0)
		return COMBO_AIR_BOTH;
	else if (strncmp(Name, "COMBO_AIR_PUMP_ALL", 99) == 0)
		return COMBO_AIR_PUMP_ALL;
	else if (strncmp(Name, "COMBO_INK_AIR_PUMP_ALL", 99) == 0)
		return COMBO_INK_AIR_PUMP_ALL;
	return 0;
}



// @@@ Return 0/1 if IDS cartridge is present
int IDS_GPIO_Present(int IDS)
{
	return (IDS_GPIO_ReadBit(IDS, GPIO_I_SUPPLY_PRSNT_L) == 0 ? 1 : 0);		// (invert)
}



// @@@ Get all bits (intput AND output) for IDS
uint16_t IDS_GPIO_GetAllBits(int IDS)
{
	uint16_t mask = IDS_GPIO_DescToBitMask(COMBO_ALL_GEN_BITS);
	
	if (IDS >= 0) IDS_LockAccessAndSelectIDS(IDS);		// if IDS specified, LOCK/UNLOCK multiple commands
	
	uint16_t bits = ReadGPIO(CURRENT_IDS);
	bits |= ReadGPIOOutput(CURRENT_IDS);
	bits &= mask;
	
	if (IDS >= 0) IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	
	return bits;
}



int LED_Desc(int LED)
{
	if (LED < 0 || LED > 2) return -1;
	return (GPIO_O_LED_R << LED);
}

void IDS_LED_On(int IDS, int LED)
{
	int desc = LED_Desc(LED);
	if (desc < 0) return;
	BlinkMask[IDS] &= ~(desc);
	IDS_GPIO_SetBits(IDS, desc);
}

void IDS_LED_Off(int IDS, int LED)
{
	int desc = LED_Desc(LED);
	if (desc < 0) return;
	BlinkMask[IDS] &= ~(desc);
	IDS_GPIO_ClearBits(IDS, desc);
}
extern void IDS_LED_AllOff(void)
{
	int ids,led;
	for (ids=0; ids<2; ids++)
		for (led=0; led<3; led++)
			IDS_LED_Off(ids, led);
}

void IDS_LED_Blink(int IDS, int LED)
{
	int desc = LED_Desc(LED);
	if (desc < 0) return;
	BlinkMask[IDS] |= desc;
}



// @@@ background thread that blinks LEDs
void *_blink_thread(void *arg)
{
	int IDS;

	while (1)
	{
		// invert blink state
		BlinkOn = !BlinkOn;	

		// if blinking, change LEDs
		for (IDS=0; IDS<2; IDS++)
			if (BlinkMask[IDS] != 0)
			{
				if (BlinkOn)
					IDS_GPIO_SetBits(IDS, BlinkMask[IDS]);
				else
					IDS_GPIO_ClearBits(IDS, BlinkMask[IDS]);
			}
		
		// set delay depending on state
		useconds_t usec = (LED_OFF_MS * 1000);
		if (BlinkOn) usec = (LED_ON_MS * 1000);
		usleep(usec);
	}
}

int BlinkInit(void)
{
	// start blink thread
	int ret = pthread_create(&BlinkThread, NULL, _blink_thread, NULL);
	return (ret != 0 ? -1 : 0);		// return 0 or -1
}



void ResetMCU(void)
{
	// reset the MCU; note that this line is only connected for IDS 0
	IDS_LockAccessAndSelectIDS(0);		// LOCK/UNLOCK multiple commands
	
	// configure Reset pin as Output; Pull low; configure Reset pin as Input (default)
	IDS_I2C_WriteTwoBytesToRegister(CURRENT_IDS, GPIO_I2C_ADDRESS, GPIO_CMD_CONFIG, GPIO_CONFIG_PORT0, GPIO_RESET_CONFIG_PORT1);
	IDS_GPIO_ClearBits(CURRENT_IDS, GPIO_O_RESET);
	usleep(1000000);	// 1 sec
	IDS_I2C_WriteTwoBytesToRegister(CURRENT_IDS, GPIO_I2C_ADDRESS, GPIO_CMD_CONFIG, GPIO_CONFIG_PORT0, GPIO_CONFIG_PORT1);
	
	IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK	
}




// #####################################################################
// # ADC/DAC (includes PILS/Pressure Monitoring)
// #####################################################################

#define DAC_PSI_OFF 0.1325
#define DAC_CNTS_MAX ((float)0xFF)
#define DAC_CNTS_PER_V (DAC_CNTS_MAX/5.0)
#define DAC_PSI_PER_V 1.396

#define ADC_CONFIG_REG 0x01
#define ADC_CONFIG_PILS_HIGH 0x40
#define ADC_CONFIG_PRESSURE_HIGH 0x50
#define ADC_CONFIG_ONEPRESSURE_HIGH 0x51
#define ADC_CONFIG_TRIGGER_ONEPRESSURE_HIGH 0xD1
#define ADC_CONFIG_BUSY_BIT 0x80
#define ADC_CONFIG_LOW 0x03
#define ADC_CONVERSION_REG 0x00

#define ADC_PSI_OFF (-0.3222)
#define ADC_POS_CNTS_MAX ((float)0x7FF)
#define ADC_CNT_PER_V (ADC_POS_CNTS_MAX/6.144)
#define ADC_PSI_PER_V 1.6111

#define MONITOR_OFF 0
#define MONITOR_PILS 1
#define MONITOR_PRESSURE 2
int MonitorMode[2] = {MONITOR_OFF, MONITOR_OFF};
int LastActiveMonitorMode[2] = {MONITOR_OFF, MONITOR_OFF};		// used to detect mode changes
static pthread_mutex_t MonitorMutex;

#define PILS_MOVING_LEN 50
float MovingPILS[2][PILS_MOVING_LEN];
int NextPILSIdx[2];

float LastAvgPILS[2] = {0.0, 0.0};		// last read of moving-averaged PILS voltage (V)
float LastPressure[2] = {0.0, 0.0};		// last read of Pressure (psi)

pthread_t MonitorThread;



// @@@ Convert 8-bit unsigned into 2 bytes, shifted left 4 bits
void DACSplitAndShift8Bit(uint8_t Value, uint8_t *MSB, uint8_t *LSB)
{
	*MSB = (Value >> 4) & 0x0F;
	*LSB = (Value & 0x0F) << 4;
}



// @@@ Convert 2 byte, shifted, 12-bit into 16-bit signed
int16_t ADCFromShifted12Bit(uint8_t MSB, uint8_t LSB)
{
	int16_t value = (((uint16_t)MSB << 4) | ((uint16_t)LSB >> 4));
	int16_t extend = ((MSB & 0x80) != 0 ? 0xF000 : 0x0000);
	value |= extend;
	return value;
}



// @@@ Set pressure setpoint
int IDS_DAC_SetSetpointPSI(int IDS, float PSI)
{
	int ret;
	float fvalue;
	uint8_t value;
	uint8_t msb;
	uint8_t lsb;
	// DAC only works for values >= DAC_PSI_OFF
	if (PSI < DAC_PSI_OFF) PSI = DAC_PSI_OFF;
	// convert from PSI to DAC value
	fvalue = ((PSI - DAC_PSI_OFF) / DAC_PSI_PER_V) * DAC_CNTS_PER_V;
	// 8-bit (unsigned) DAC
	value = (uint8_t)round(fvalue);
	if (value > DAC_CNTS_MAX) 
		value = DAC_CNTS_MAX;
	// convert to write to DAC
	DACSplitAndShift8Bit(value, &msb, &lsb);
	ret = IDS_I2C_WriteTwoBytes(IDS, DAC_I2C_ADDRESS, msb, lsb);
	return ret;
}



// @@@ Read from ADC, converting to volts
// @@@ NOTE: error not returned
float ADCGetVolts(int IDS)
{
	int ret = 0;
	uint8_t bytes[2];
	int16_t ival;
	float fval;
	// read raw bytes from ADC
	bytes[0] = bytes[1] = 0;
	ret = IDS_I2C_ReadTwoBytesFromRegister(IDS, ADC_I2C_ADDRESS, ADC_CONVERSION_REG, bytes);
	if (ret < 0) ids_callback(IDS, LEVEL_ERR, "IDS_I2C_ReadTwoBytesFromRegister returned an error");
	// convert to a number
	ival = ADCFromShifted12Bit(bytes[0], bytes[1]);
	// if negative, make 0
	if (ival < 0) ival = 0;
	// conver ADC value to 
	fval = (float)ival / ADC_CNT_PER_V;
	return fval;
}

// @@@ Read from ADC, converting to psi
// @@@ NOTE: error not returned
float ADCGetPressurePSI(int IDS)
{
	float fval = ADCGetVolts(IDS);
	// conver volts to PSI
	fval = fval * ADC_PSI_PER_V + ADC_PSI_OFF;
	return fval;
}



// @@@ Initialize the moving value buffer
void ClearMovingBuffer(int IDS)
{
	int i;
	for (i=0; i<PILS_MOVING_LEN; i++) MovingPILS[IDS][i] = 0.0;		// (0.0 is invalid)
	NextPILSIdx[IDS] = 0;
}

// @@@ Calc a new PILS value and return the moving average OR 0.0 if no average yet
void CalcAvgPILS(int IDS, float new_value)
{
	int i;
	float f, accum;
	
	// if this value is invalid, return now
	if (new_value <= 0.0) return;		// (NOTE: does not change Last value)
	
	// set value and increment (circular) index
	i = NextPILSIdx[IDS];
	MovingPILS[IDS][i] = new_value;
	NextPILSIdx[IDS] = (i + 1) % PILS_MOVING_LEN;
	
	// now calculate a new moving average
	accum = 0;
	for (i=0; i<PILS_MOVING_LEN; i++)
	{
		f = MovingPILS[IDS][i];
		if (f <= 0.0)	// need a full buffer to calc
		{
			LastAvgPILS[IDS] = 0.0;	// last value is unknown
			return;
		}
		accum += f;
	}
	LastAvgPILS[IDS] = (accum / PILS_MOVING_LEN);	// set new average
}



// @@@ Write config bytes to ADC config register
int ConfigADC(int IDS, uint8_t high, uint8_t low)
{
	return IDS_I2C_WriteTwoBytesToRegister(IDS, ADC_I2C_ADDRESS, ADC_CONFIG_REG, high, low);
}
// @@@ Config for continuous PIL read
int ConfigADCForPILS(int IDS)
{
	return ConfigADC(IDS, ADC_CONFIG_PILS_HIGH, ADC_CONFIG_LOW);
}
// @@@ Config for continuous Pressure read
int ConfigADCForPressure(int IDS)
{
	return ConfigADC(IDS, ADC_CONFIG_PRESSURE_HIGH, ADC_CONFIG_LOW);
}
// @@@ Config for a triggered Pressure read
int ConfigADCForOnePressure(int IDS)
{
	return ConfigADC(IDS, ADC_CONFIG_ONEPRESSURE_HIGH, ADC_CONFIG_LOW);
}
// @@@ Trigger a (Pressure) read
int TriggerADCForOnePressure(int IDS)
{
	return ConfigADC(IDS, ADC_CONFIG_TRIGGER_ONEPRESSURE_HIGH, ADC_CONFIG_LOW);
}



// @@@ Wait until ADC indicates it is not busy converting (used in triggered read)
int WaitForNotBusy(int IDS)
{
	int i;
	uint8_t high;
	for (i=0; i<10; i++)
	{
		high = IDS_I2C_ReadByteFromRegister(IDS, ADC_I2C_ADDRESS, ADC_CONFIG_REG);
		if ((high & ADC_CONFIG_BUSY_BIT) != 0)
			return 1;
		usleep(5000);		// (wait 5 ms)							
	}
	return 0;
}



// @@@ background thread that monitors PILS and/or Pressure
void *_monitor_thread(void *arg)
{
	int pressure_count[2] = {0, 0};
	int IDS;
	float fval;
	int monitor_mode, last_active_monitor_mode;

	while (1)
	{
		// sleep for PILS interval
		usleep(1000000 / READ_PER_SEC);

		// check each IDS separately
		for (IDS = 0; IDS < 2; IDS++)
		{	
			// in LOCKED section check and change monitor modes
			{
				pthread_mutex_lock(&MonitorMutex);
				monitor_mode = MonitorMode[IDS];
				last_active_monitor_mode = LastActiveMonitorMode[IDS];
				// remember last ACTIVE monitor mode
				if (monitor_mode != MONITOR_OFF) LastActiveMonitorMode[IDS] = monitor_mode;
				pthread_mutex_unlock(&MonitorMutex);
			}
			// if mode changed, re-configure AND skip one reading
			if (monitor_mode != last_active_monitor_mode)	
			{
				if (monitor_mode == MONITOR_PRESSURE)
					ConfigADCForPressure(IDS);
				else if (monitor_mode == MONITOR_PILS)
					ConfigADCForPILS(IDS);
				continue;			
			}
			
			// NOTE: if mode is OFF, nothing will happen

			if (monitor_mode == MONITOR_PRESSURE)
			{
				// @@@ pressure mode @@@

				// get pressure measurement and store
				fval = ADCGetPressurePSI(IDS);
				LastPressure[IDS] = fval;
			}
			else if (monitor_mode == MONITOR_PILS)
			{
				// @@@ PILS mode @@@
				
				// get/store PILS measurement AND cooked PILS
				fval = ADCGetVolts(IDS);
				CalcAvgPILS(IDS, fval);
				
				// check pressure count if needed
				pressure_count[IDS]++;
				if (pressure_count[IDS] >= (int)(READ_PER_SEC * PRESSURE_SEC_PER_READ))
				{
					pressure_count[IDS] = 0;
					// config for SINGLE pressure value read
					ConfigADCForOnePressure(IDS);
					WaitForNotBusy(IDS);
					// trigger pressure read
					TriggerADCForOnePressure(IDS);
					if (WaitForNotBusy(IDS))	// if ready...
					{
						fval = ADCGetPressurePSI(IDS);
						LastPressure[IDS] = fval;
					}
					// revert to PILS reading
					ConfigADCForPILS(IDS);
				}
			}	
		}
	}
}



int MonitorInit(void)
{
	// init mutex for changing mode
    InitRecursiveMutex(&MonitorMutex);
	// start monitor thread
	int ret = pthread_create(&MonitorThread, NULL, _monitor_thread, NULL);
	return (ret != 0 ? -1 : 0);		// return 0 or -1
}



void SetMonitorMode(int IDS, int Mode)
{
	pthread_mutex_lock(&MonitorMutex);
	MonitorMode[IDS] = Mode;
	pthread_mutex_unlock(&MonitorMutex);
}


// @@@ Clear the PILS moving buffer
void IDS_ClearPILSBuffer(int IDS)
{
	ClearMovingBuffer(IDS);
}


// @@@ Switch monitor OFF
void IDS_MonitorOff(int IDS)
{
	SetMonitorMode(IDS, MONITOR_OFF);
}
// @@@ Switch monitor ON (NOTE: only works if mode is OFF)
void IDS_MonitorOn(int IDS)
{
	pthread_mutex_lock(&MonitorMutex);
	if (MonitorMode[IDS] == MONITOR_OFF) MonitorMode[IDS] = LastActiveMonitorMode[IDS];
	pthread_mutex_unlock(&MonitorMutex);
}


// @@@ Switch to PILS mode (PILS readings taken fast, with periodic Pressure reading)
void IDS_MonitorPILS(int IDS)
{
	ClearMovingBuffer(IDS);		// reset the moving values buffer for this IDS
	SetMonitorMode(IDS, MONITOR_PILS);
}
// @@@ Get the most recent PILS value (volts)
float IDS_LastAvgPILS(int IDS)
{
	return LastAvgPILS[IDS];
}


// @@@ Switch to Pressure mode (Pressure readings taken fast)
void IDS_MonitorPressure(int IDS)
{
	SetMonitorMode(IDS, MONITOR_PRESSURE);
}
// @@@ Get the most recent Pressure value (psi)
float IDS_LastPressure(int IDS)
{
	return LastPressure[IDS];
}




// #####################################################################
// # RTC
// #####################################################################

#define RTC_SEC_REG 0x00
#define RTC_MIN_REG 0x01
#define RTC_HR_REG 0x02
#define RTC_DAY_REG 0x03
#define RTC_DOM_REG 0x04
#define RTC_MON_REG 0x05
#define RTC_YR_REG 0x06
#define RTC_CONFIG_REG 0x07
#define RTC_CAL_REG 0x08

#define RTC_VBATEN 0x08

#define BCD_MASK8 0xFF
#define BCD_MASK7 0x7F
#define BCD_MASK6 0x3F
#define BCD_MASK5 0x1F
#define BCD_MASK3 0x07

#define RTC_OSC_ON_MASK 0x80

struct tm _tm;			// re-used each time a time structure is returned
time_t _time_t;			// (re-used with _tm)
#define TIME_STRING_MAX 30
char _time_string[TIME_STRING_MAX];	// re-used each time a time string is returned


uint8_t DecToBCD(int Dec)
{
	return (uint8_t)(((Dec / 10) << 4) | (Dec % 10));
}


int BCDToDec(uint8_t BCD, uint8_t Mask)
{
	BCD &= Mask;	// mask bits of interest
	return (BCD >> 4) * 10 + (BCD & 0x0F);
}


int RTCInit(void)
{
	int ret = 0;	
	IDS_LockAccessAndSelectIDS(0);			// LOCK/UNLOCK multiple commands
	
	// read the RTC Config registers
	ret = IDS_I2C_ReadByteFromRegister(0, RTC_I2C_ADDRESS, RTC_CONFIG_REG);
		
	IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	return ret;
}


int IDS_RTC_SetRTCFromSystemClock(void)
{
	int ret = 0;	
	time_t now;
	struct tm* tdetail;

	IDS_LockAccessAndSelectIDS(0);			// LOCK/UNLOCK multiple commands
	
	// get current time and convert to local
	ret = time(&now);
	if (ret >= 0) ret = 0;		// time returns -1 on error or a positive value
	if (ret == 0) tdetail = gmtime(&now);
	
	// NOTE: store GMT in RTC so no DST issues
	
	// disable oscillator (write 0 to seconds register)
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_SEC_REG, 0);
	
	// write date/time in reverse so seconds (with oscillator enable) is last
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_YR_REG, DecToBCD(tdetail->tm_year - 100)); 	// tm_year is years since 1900	
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_MON_REG, DecToBCD(tdetail->tm_mon + 1)); 	// tm_mon is 0-11
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_DOM_REG, DecToBCD(tdetail->tm_mday));
	// NOTE: set VBATEN when setting the day, to enable battery backup
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_DAY_REG, (RTC_VBATEN | DecToBCD(tdetail->tm_wday + 1))); 	// tm_wday is 0-6	
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_HR_REG, DecToBCD(tdetail->tm_hour));		// 24-hour format is default
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_MIN_REG, DecToBCD(tdetail->tm_min));
	if (ret == 0) ret = IDS_I2C_WriteByteToRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_SEC_REG, DecToBCD(tdetail->tm_sec) | RTC_OSC_ON_MASK);

	IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	return ret;
}


struct tm* IDS_RTC_GetRTCTime(void)
{
	int i;
	int sec;
	
	IDS_LockAccessAndSelectIDS(0);			// LOCK/UNLOCK multiple commands
	
	// don't read time if it is about to turn over
	for(i=0; i<6; i++)
	{
		sec = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_SEC_REG), BCD_MASK7);
		if (sec != 59) break;		// don't read in 59th second
		usleep(500000);	// .5 sec
	}	
	_tm.tm_sec = sec;
	_tm.tm_min = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_MIN_REG), BCD_MASK7);
	_tm.tm_hour = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_HR_REG), BCD_MASK6);
	_tm.tm_mday = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_DOM_REG), BCD_MASK6);
	_tm.tm_mon = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_MON_REG), BCD_MASK5) - 1;
	_tm.tm_year = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_YR_REG), BCD_MASK8) + 100;
	_tm.tm_wday = BCDToDec(IDS_I2C_ReadByteFromRegister(CURRENT_IDS, RTC_I2C_ADDRESS, RTC_DAY_REG), BCD_MASK3) - 1;	

	IDS_UnlockAccessAndDeSelectBothIDS();	// UNLOCK
	
	// convert RTC to GMT, then write back to _tm in local time
	_time_t = timegm(&_tm);
	localtime_r(&_time_t, &_tm);
	
	return &_tm;
}


char* IDS_RTC_GetRTCTimeString(void)
{
	// get time from RTC, and do a gross validation
	IDS_RTC_GetRTCTime();
	if (_tm.tm_year < (2018-1900))
		return NULL;
		
	// return time formatted for 'date -s'
	strftime(_time_string, TIME_STRING_MAX, "%Y-%m-%d %H:%M:%S", &_tm);
	return _time_string;
}


struct tm* IDS_RTC_SetSystemClockFromRTC(void)
{
	int ret;
	struct timeval tv;
	
	// get time from RTC, and do a gross validation
	IDS_RTC_GetRTCTime();
	if (_tm.tm_year < (2018-1900))
		return NULL;
	
	// set the time using global time_t
	tv.tv_sec = _time_t;
	tv.tv_usec = 0;
	ret = settimeofday(&tv, NULL);
	if (ret < 0)
	{
		if (errno == EPERM)
			printf("ERROR: setting system clock requires root privileges!\n");
		return NULL;		
	}
	
	return &_tm;
}

