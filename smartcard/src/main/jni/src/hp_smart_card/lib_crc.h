#ifndef __INC_LIB_CRC_H__
#define __INC_LIB_CRC_H__

#include "hp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t lib_crc_ccitt_calc(const uint8_t* start, const uint8_t* end);
uint16_t lib_crc_ccitt_init(void);
uint16_t lib_crc_ccitt_update(uint16_t crc, uint8_t c);
uint16_t lib_crc_ccitt_final(uint16_t crc);

#ifdef __cplusplus
}
#endif
#endif /* __INC_LIB_CRC_H__ */
