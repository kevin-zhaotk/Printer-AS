/*
 * Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.
 *
 * THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.
 *
 * HP Company Confidential
 * © Copyright 2019 HP Development Company, L.P.
 * Made in U.S.A.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ids_ifc.h"
#include "print_head_driver_ifc.h"

/***********************************************************
 *  Release Version
 ***********************************************************/
#define VERSION "1.1"


/***********************************************************
 *  Customization
 *
 *  Settings for basic customization.
 ***********************************************************/

// Pen and Supply to use (only one pen and one supply are used in example code)
#define PEN_IDX 0
#define SUPPLY_IDX 0

// Instance values match dev kit defaults
#define PD_INSTANCE     1
#define IDS_INSTANCE    2

// PD and IDS identifiers (used for pairing and secure ink messaging)
#define PD_ID   10
#define IDS_ID  20

// Platform Information provided to IDS and PD, used for First Install and MRU
#define PLATFORM_MODEL          "TestPlatform"
#define PLATFORM_YEAR           2018
#define PLATFORM_WOY            50
#define PLATFORM_COUNTRY        1
#define PLATFORM_REV_MAJOR      1
#define PLATFORM_REV_MINOR      2
#define PLATFORM_ORIENTATION    0

// IDS Stall Insert Count provided to IDS, used for First Install and MRU
#define IDS_STALL_INSERT        5

// Buffer sizes
#define PAYLOAD_BUFFER_SIZE 1000
#define BUFFER_SIZE 100



/***********************************************************
 *  Common Functions
 ***********************************************************/

extern int ids_check(char *function, IDSResult_t result);
extern char *ph_state_string[];
extern char *ph_state_description(int state);
extern char *ph_error_string[];
extern char *ph_error_description(int error);
extern int pd_check(char *function, PDResult_t result);
extern int pd_check_ph(char *function, PDResult_t result, int PenIdx);
extern char _time_string[];
extern struct tm* get_time();
extern char *time_string();

extern int InitSystem();
extern int ShutdownSystem();
extern int SetInfo();
extern int DeletePairing();
extern int DoPairing(int SupplyIdx, int PenIdx);
extern int DoOverrides(int SupplyIdx, int PenIdx);
extern float GetInkWeight(int PenIdx);
extern int GetAndProcessInkUse(int PenIdx, int SupplyIdx);




