/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

/*********************************
* hp_host_smart_card_priv.h
*********************************
*
*********************************/
#ifndef _HP_SMART_CARD_PRIVATE_H_
#define _HP_SMART_CARD_PRIVATE_H_

#include "hp_host_smart_card_ifc.h"
#include "carnac_types.h"

// Delay times for Host smart card.
// These are used to suspend host smart card on the i2c transport
// to prevent hogging the i2c bus.
#define HOST_SMART_CARD_SHORT_TIME_USEC         (1000)
#define HOST_SMART_CARD_MEDIUM_DELAY_USEC       (5000)
#define HOST_SMART_CARD_LONG_DELAY_USEC         (10000)

HP_HOST_SMART_CARD_return_t hw_smart_dev_echo(uint8_t* input,
                                              int input_length,
                                              smart_dev_t smart_dev);

HP_HOST_SMART_CARD_return_t hw_smart_dev_gen_sk(CARNAC_sk_id_t* sk_id,
                                                CARNAC_sk_t* sk,
                                                smart_dev_t smart_dev);

HP_HOST_SMART_CARD_return_t hw_smart_dev_gen_rand(uint32_t* rand_num,
                                                  smart_dev_t smart_dev);

HP_HOST_SMART_CARD_return_t hw_smart_dev_list_mkdb(CARNAC_mk_entry_t* db_entries,
                                                   int* entry_count,
                                                   smart_dev_t smart_dev);
#endif /*  _HP_SMART_CARD_PRIVATE_H_ */
