/*
 * Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.
 *
 * THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.
 *
 * HP Company Confidential
 * © Copyright 2019 HP Development Company, L.P.
 * Made in U.S.A.
 */


#include "common.h"
/***********************************************************
 *  Utility Functions
 ***********************************************************/

int ids_check(char *function, IDSResult_t result)
{
    if(result == IDS_OK) return 0;
    LOGE("%s() failed: %s\n", function, ids_get_error_description(result));
    return -1;
}

// strings adapted from print_head_driver_ifc.h
char *ph_state_string[] = {
    "Powered ON",
    "Powered OFF",
    "Present",
    "Not Present",
    "Invalid"
};
#define STATE_COUNT 5
char *ph_state_description(int state) {
    if(STATE_COUNT != sizeof(ph_state_string)/sizeof(char*)) {
        LOGE("STATE_COUNT != sizeof(ph_state_string)/sizeof(char*)");
        return "";
    }
    if (state < 0 || state >= STATE_COUNT) return "??";
    return ph_state_string[state];
}

// strings adapted from print_head_driver_ifc.h
char *ph_error_string[] = {
    "NO_ERROR",
    "ACUMEN_RDWR",
    "REG_RDWR",
    "VDD_POWON_FAIL",
    "VDD_UV",
    "VDD_OV",
    "VPP_POWON_FAIL",
    "VPP_ADJ_FAIL",
    "VPP_VALIDATION",
    "INKSHORT",
    "DIE_RDWR",
    "OVTEMP",
    "OVSPEED",
    "CONTINUITY",
    "INVALID_STATE_FOR_INPUT",
    "TEMP_SHUTDOWN",
    "PULSEWIDTH_VALIDITY",
    "VOLTAGE_VALIDITY",
    "GPIO_RDWR",
    "VPP_POWOF_FAIL",
    "VPP_UV",
    "IO_RDWR",
    "DRP_PROC",
    "VPP_GOOD",
    "VPP_GOODSTABLE",
    "VPP_SHUNTDISABLE",
    "VDD_VPP_SHORT",
    "VPP_NOT_POWOF",
    "EC_PW_SET",
    "EC_SPIT",
    "EC_VOLTSET",
    "EC_TEMP_TIMEOUT",
    "EC_INVALID_STATE",
    "EC_TEMP_FAULT",
    "PURGE_INVALID_STATE",
    "PURGE_INVALID_PH",
    "PURGE_SPIT",
    "PURGE_TEMP_FAULT",
    "PURGE_INVALID_SLOT",
    "INVALID",
    "TEMP_VALIDITY",
    "VPP_AVG",
    "VPP_NOT_CONVERGING",
    "TTOE_SHFPRESENT",
    "IDS_OVRNOTSET",
    "IDS_NOTPAIRED",
    "IDS_NOHSHAKE",
    "IDS_SUPPLY",
    "IDS_MSGNOTVALID",
    "TTOEPURGE_IDSNOHSHAKE",
    "ALTERED",
    "OUTOFINK",
    "PAIRINGDONE_NOTUPDATED",
    "PAIRINGINFO_READ",
    "DIEID",
    "DIECONTINUITY",
    "IDSEXPIRED",
    "PARTIALINKSHORT",
};
#define ERROR_COUNT 58

char *ph_error_description(int error) {
    if(ERROR_COUNT != sizeof(ph_error_string)/sizeof(char*)) {
        LOGE("STATE_COUNT != sizeof(ph_state_string)/sizeof(char*)");
        return "??";
    }
    if (error < 0 || error >= ERROR_COUNT) return "??";
    return ph_error_string[error];
}

int pd_check_ph(char *function, PDResult_t result, int PenIdx) {
    if(result == PD_OK) return 0;

    if (PenIdx < 0) {
        LOGE("ERROR: %s() failed: %s\n", function, pd_get_error_description(result));
    } else {
        // attempt to get print head status to include in error log
        PrintHeadStatus status;
        status.print_head_state = -1;
        status.print_head_error = -1;
        pd_get_print_head_status(PD_INSTANCE, PenIdx, &status);

        LOGE("ERROR: %s() failed: %s  PH state: %s  PH error: %d %s\n", function,
            pd_get_error_description(result),
            ph_state_description(status.print_head_state),
            status.print_head_error,
            ph_error_description(status.print_head_error));
    }
    return -1;
}
int pd_check(char *function, PDResult_t result) {
    return pd_check_ph(function, result, -1);
}

#define TIME_STRING_MAX 20
char _time_string[TIME_STRING_MAX];

// returns pointer to structure filled with local time information
struct tm* get_time() {
    int ret;
    time_t now;

    while (true) {
        // get current time and convert to local time
        ret = time(&now);
        if (ret > 0) return localtime(&now);
    }
}

// returns pointer to time string
char *time_string() {
    struct tm* tdetail = get_time();
    strftime(_time_string, TIME_STRING_MAX, "%H:%M:%S", tdetail);
    return _time_string;
}

/***********************************************************
 *  Commands
 ***********************************************************/

// Initialize libraries and specific IDS/PD instances
int InitSystem() {
    IDSResult_t ids_r;
    PDResult_t pd_r;
    IdsSysInfo_t ids_sys_info;

    LOGI("v%s\n", VERSION);

    ids_r = ids_lib_init();
    if (ids_check("ids_lib_init", ids_r)) return -1;
    ids_r = ids_init(IDS_INSTANCE);
    if (ids_check("ids_init", ids_r)) return -1;
    ids_r = ids_info(IDS_INSTANCE, &ids_sys_info);
    if (ids_check("ids_info", ids_r)) return -1;
    LOGD("IDS FW = %d.%d\n", ids_sys_info.fw_major_rev, ids_sys_info.fw_minor_rev);

    PDSystemStatus pd_system_status;

    pd_r = pd_lib_init();
    if (pd_check("pd_lib_init", pd_r)) return -1;
    pd_r = pd_init(PD_INSTANCE);
    if (pd_check("pd_init", pd_r)) return -1;
    pd_r = pd_get_system_status(PD_INSTANCE, &pd_system_status);
    if (pd_check("pd_get_system_status", pd_r)) return -1;
    LOGD("PD FW = %d.%d\n", pd_system_status.fw_rev_major, pd_system_status.fw_rev_minor);
    return 0;
}


// Shutdown libraries and specific IDS/PD instances
int ShutdownSystem() {
    IDSResult_t ids_r;
    PDResult_t pd_r;

    ids_r = ids_shutdown(IDS_INSTANCE);
    if (ids_check("ids_shutdown", ids_r)) return -1;
    ids_r = ids_lib_shutdown();
    if (ids_check("ids_lib_shutdown", ids_r)) return -1;
    pd_r = pd_shutdown(PD_INSTANCE);
    if (pd_check("pd_shutdown", pd_r)) return -1;
    pd_r = pd_lib_shutdown();
    if (pd_check("pd_lib_shutdown", pd_r)) return -1;
    LOGI("Shutdown complete\n");
    return 0;
}


// Set platform info and date
int SetInfo() {
    IDSResult_t ids_r;
    PDResult_t pd_r;

    PlatformInfo_t platform_info;
    strncpy(platform_info.model, PLATFORM_MODEL, sizeof(platform_info.model));
    platform_info.mfg_year = PLATFORM_YEAR;
    platform_info.mfg_woy = PLATFORM_WOY;
    platform_info.mfg_country = PLATFORM_COUNTRY;
    platform_info.mfg_rev_major = PLATFORM_REV_MAJOR;
    platform_info.mfg_rev_minor = PLATFORM_REV_MINOR;
    platform_info.orientation = PLATFORM_ORIENTATION;      // used by PD only

    ids_r = ids_set_platform_info(IDS_INSTANCE, &platform_info);
    if (ids_check("ids_set_platform_info", ids_r)) return -1;
    pd_r = pd_set_platform_info(PD_INSTANCE, &platform_info);
    if (pd_check("pd_set_platform_info", pd_r)) return -1;

    struct tm* tdetail = get_time();

    ids_r = ids_set_date(IDS_INSTANCE, (1900 + tdetail->tm_year), tdetail->tm_mon, tdetail->tm_mday);
    if (ids_check("ids_set_date", ids_r)) return -1;
    pd_r = pd_set_date(PD_INSTANCE, (1900 + tdetail->tm_year), tdetail->tm_mon, tdetail->tm_mday);
    if (pd_check("pd_set_date", pd_r)) return -1;
    return 0;
}


// Delete pairing and reset sequence
int DeletePairing() {
    IDSResult_t ids_r;
    PDResult_t pd_r;

    // delete pairing and reset sequence
    ids_r = ids_pairing_delete(IDS_INSTANCE);
    if (ids_check("ids_pairing_delete", ids_r)) return -1;
    ids_r = ids_terminate_session(IDS_INSTANCE);
    if (ids_check("ids_terminate_session", ids_r)) return -1;
    pd_r = pd_pairing_delete(PD_INSTANCE);
    if (pd_check("pd_pairing_delete", pd_r)) return -1;
    pd_r = pd_terminate_session(PD_INSTANCE);
    if (pd_check("pd_terminate_session", pd_r)) return -1;
    LOGI("Pairing deleted\n");
    return 0;
}


// Pair IDS to both slots of PD Pen
int DoPairing(int SupplyIdx, int PenIdx) {
    IDSResult_t ids_r;
    PDResult_t pd_r;
    int step;
    uint8_t payload[PAYLOAD_BUFFER_SIZE];
    int32_t payload_size;
    int32_t status;
    int32_t slotbits;

    // before pairing, reset the sequence in case previous pairing had failed
    if (ids_terminate_session(IDS_INSTANCE) != IDS_OK ||
        pd_terminate_session(PD_INSTANCE) != PD_OK) {
            LOGE("ERROR: Cannot terminate pairing sessions\n");
            return -1;
    }

    // step through the pairing steps
    payload_size = 0;
    for (step=1; step<=3; step++) {
        ids_r = ids_pairing(IDS_INSTANCE, step, PD_ID, SupplyIdx, payload, payload_size, &status, payload, &payload_size, PAYLOAD_BUFFER_SIZE);
        if(ids_r != IDS_OK) {
            LOGE("ERROR: ids_pairing() step %d failed: %s\n", step, ids_get_error_description(ids_r));
            return -1;
        }
        if (status != KEY_NEGOTIATION_SESSION_OK && status != KEY_NEGOTIATION_SESSION_COMPLETE) {
            LOGE("ERROR: ids_pairing() step %d failed, status %d\n", step, status);
            return -1;
        }

        slotbits = (PenIdx == 1 ? 0x0C : 0x03);    // pair both columns of one pen
        pd_r = pd_pairing(PD_INSTANCE, step, slotbits, IDS_ID, payload, payload_size, &status, payload, &payload_size, PAYLOAD_BUFFER_SIZE);
        if(pd_r != PD_OK) {
            LOGE("ERROR: pd_pairing() step %d failed: %s\n", step, pd_get_error_description(pd_r));
            return -1;
        }
        if (status != KEY_NEGOTIATION_SESSION_OK && status != KEY_NEGOTIATION_SESSION_COMPLETE) {
            LOGE("ERROR: pd_pairing() step %d failed, status %d\n", step, status);
            return -1;
        }
    }
    LOGI("Pairing COMPLETE\n");
    return 0;
}


// Overrides from Supply to Pen (both slots)
int DoOverrides(int SupplyIdx, int PenIdx) {
    IDSResult_t ids_r;
    PDResult_t pd_r;
    int slot;
    uint8_t payload[PAYLOAD_BUFFER_SIZE];
    int32_t payload_size;
    int32_t status;

    ids_r = ids_get_overrides(IDS_INSTANCE, PD_ID, SupplyIdx, &status, payload, &payload_size, PAYLOAD_BUFFER_SIZE);
    if (ids_check("ids_get_overrides", ids_r)) return -1;
    for (slot=0; slot<=1; slot++) {
        pd_r = pd_set_secure_overrides(PD_INSTANCE, PenIdx, slot, payload, payload_size, &status);
        if (pd_check("pd_set_secure_overrides", pd_r)) return -1;
        if (status != 0) {
            LOGE("ERROR: pd_set_secure_overrides() slot %d failed, status %d\n", slot, status);
            return -1;
        }
    }
    LOGI("Overrides COMPLETE\n");
    return 0;
}


// NON-SECURE ink use weight (for PILS algorithm); poll both slots
// NOTE: only poll when pen is On (print_head_state from pd_get_system_status call)
float GetInkWeight(int PenIdx) {
    PDResult_t pd_r;
    float total_weight = 0;
    float slot_weight;
    int slot;

    for (slot=0; slot<=1; slot++) {
        pd_r = pd_get_used_ink_weight(PD_INSTANCE, PenIdx, slot, &slot_weight);
        if (pd_check_ph("pd_get_used_ink_weight", pd_r, PenIdx)) return -1.0;
        total_weight += slot_weight;
    }
    return total_weight;
}


// SECURE ink use
// NOTE: only poll when pen is On (print_head_state from pd_get_system_status call)
int GetAndProcessInkUse(int PenIdx, int SupplyIdx) {
    IDSResult_t ids_r;
    PDResult_t pd_r;
    uint8_t payload[PAYLOAD_BUFFER_SIZE];
    int32_t payload_size;
    int32_t status;
    int32_t status2;
    int slot;

    // poll both slots
    for (slot=0; slot<=1; slot++) {
        // get ink use payload from PD
        pd_r = pd_ink_use(PD_INSTANCE, PenIdx, slot, &status, payload, &payload_size, PAYLOAD_BUFFER_SIZE);
        if (pd_check_ph("pd_ink_use", pd_r, PenIdx)) exit(-1);
        if (status != 0) {
            LOGE("ERROR: pd_ink_use() slot %d failed, status %d\n", slot, status);
            return -1;
        }
        // pass payload to IDS and get status payload
        ids_r = ids_ink_use(IDS_INSTANCE, PD_ID, SupplyIdx, payload, payload_size, &status, &status2, payload, &payload_size, PAYLOAD_BUFFER_SIZE);
        if (ids_check("ids_ink_use", ids_r)) exit(-1);
        if (status != 0) {
            LOGE("ERROR: ids_ink_use() from slot %d failed, status %d\n", slot, status);
            return -1;
        }
        // pass status payload back to PD
        pd_r = pd_supply_status(PD_INSTANCE, PenIdx, slot, payload, payload_size, &status);
        if (pd_check_ph("pd_supply_status", pd_r, PenIdx)) exit(-1);
        if (status != 0) {
            LOGE("ERROR: pd_supply_status() slot %d failed, status %d\n", slot, status);
            return -1;
        }
    }
    return 0;
}


