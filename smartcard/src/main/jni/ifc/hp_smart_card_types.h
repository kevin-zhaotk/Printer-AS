/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
//
// File:        hp_smart_card_types.h
//
//-----------------------------------------------------------------------------
//
// Description: common defines and typedefs for HP smart card library
//
//*****************************************************************************

#ifndef _HP_SMART_CARD_TYPES_H_
#define _HP_SMART_CARD_TYPES_H_

#include "hp_types.h"
#include "hp_smart_card_config.h"

//-----------------------------------------------------------------------------
// Smart card partition type identifiers
//-----------------------------------------------------------------------------
typedef enum
{
    C_MODE_RW = 0,
    C_MODE_RW_OR,
    C_MODE_RO,
    C_MODE_RW2RO,
} HP_SMART_CARD_part_mode_t;

//-----------------------------------------------------------------------------
// smart card tag identifiers
//-----------------------------------------------------------------------------
typedef enum
{
    HP_SMART_CARD_BLANK_TAG = 0x00,
} HP_SMART_CARD_tag_t;

//-----------------------------------------------------------------------------
// Common smart card fields.
//-----------------------------------------------------------------------------
typedef enum
{
    // Common field ids
    HP_SMART_CARD_NOT_A_FIELD               = 0,

    // Header fields are common
    HP_SMART_CARD_CHIP_TAG                  = 1,
    HP_SMART_CARD_RESERVED_1                = 2,
    HP_SMART_CARD_FAMILY_ID                 = 3,
    HP_SMART_CARD_APPROVED_HP_OEM_BIT       = 4,
    HP_SMART_CARD_OEM_ID                    = 5,
    HP_SMART_CARD_ADDRESS_POSITION          = 6,
    HP_SMART_CARD_TEMPLATE_VERSION_MAJOR    = 7,
    HP_SMART_CARD_TEMPLATE_VERSION_MINOR    = 8,
//    HP_SMART_CARD_TAG_ENCRYPTION_MASK     = 9,
    HP_SMART_CARD_MEMORY_PARTITION_REV      = 10,

    /* append product specifc smart card fields here */
    HP_SMART_CARD_product_fields,
} HP_SMART_CARD_field_t;


//-----------------------------------------------------------------------------
// SMART_CARD field description
//-----------------------------------------------------------------------------
typedef struct
{
    HP_SMART_CARD_field_t   field_id;       // tagged field identifier (see above)
    uint8_t                 start_byte;     // start byte of field within tag
    uint8_t                 start_bit;      // start bit of field within start byte
    uint8_t                 size;           // field size in number of bits
} HP_SMART_CARD_field_desc_t;


//-----------------------------------------------------------------------------
// SMART_CARD tag description
//-----------------------------------------------------------------------------
typedef struct
{
    HP_SMART_CARD_tag_t                 tag_id;
    int                                 size;
    int                                 num_fields;
    const HP_SMART_CARD_field_desc_t    *field_desc;
    HP_SMART_CARD_part_mode_t           access_mode;
} HP_SMART_CARD_tag_desc_t;


typedef struct
{
    int                             chip_tag;
    int                             family_id;
    int                             num_tag_fields;
    int                             last_field;
    const HP_SMART_CARD_tag_desc_t  *tag;
} HP_SMART_CARD_device_info_t;

#ifndef INCLUDE_SMART_CARD_HEADER_FIELD_DATA
extern const HP_SMART_CARD_field_desc_t     hp_smart_card_header_fields[];
extern const HP_SMART_CARD_device_info_t    hp_smart_card_device[];
#else
//-----------------------------------------------------------------------------
// Smart card header layout - common to all CHIP_TAG 2 smart card devices
//-----------------------------------------------------------------------------
const HP_SMART_CARD_field_desc_t hp_smart_card_header_fields[] =
{
    { HP_SMART_CARD_CHIP_TAG,               0, 7, 4 },
    { HP_SMART_CARD_RESERVED_1,             0, 3, 1 },
    { HP_SMART_CARD_FAMILY_ID,              0, 2, 5 },
    { HP_SMART_CARD_APPROVED_HP_OEM_BIT,    1, 5, 1 },
    { HP_SMART_CARD_OEM_ID,                 1, 4, 5 },
    { HP_SMART_CARD_ADDRESS_POSITION,       2, 7, 8 },
    { HP_SMART_CARD_TEMPLATE_VERSION_MAJOR, 3, 7, 4 },
    { HP_SMART_CARD_TEMPLATE_VERSION_MINOR, 3, 3, 4 },
//    {HP_SMART_CARD_TAG_ENCRYPTION_MASK,      4, 7,16},
    { HP_SMART_CARD_MEMORY_PARTITION_REV,   6, 7, 4 },
};
#define N_HP_SMART_CARD_HEADER_BYTES        8
#define N_HP_SMART_CARD_HEADER_FIELDS       (sizeof(hp_smart_card_header_fields) / sizeof(HP_SMART_CARD_field_desc_t))
#endif  // EXTERNAL_REFERENCES_ONLY
#endif  /* _HP_SMART_CARD_TYPES_H_ */
