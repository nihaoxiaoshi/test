#ifndef _SID3C_ROUTER_H_
#define _SID3C_ROUTER_H_

#include <stdint.h>
#include "uds_type.h"

bool_t service_3C_check_len(const uint8_t* msg_buf, uint16_t msg_dlc);
void service_3C_Router(const uint8_t* msg_buf, uint16_t msg_dlc);

#endif