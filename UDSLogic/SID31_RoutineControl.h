#ifndef _SID31_ROUTINE_CONTROL_H_
#define _SID31_ROUTINE_CONTROL_H_

#include <stdint.h>
#include "uds_type.h"

#define RID_ERASE_MEMORY    0xFF00
#define RID_CHECK_INTEGRITY 0x0202
#define RID_UPDATE_COND     0x0203
#define RID_CONSISTENCY     0xFF01
#define RID_SLAVE_MODE      0xDFE0
#define RID_DIGITAL_SIG     0x0204
#define RID_TEST_DD02       0xDD02
#define RTCTRL_NUM          7

typedef enum {
    UDS_RT_ST_IDLE = 0,
    UDS_RT_ST_RUNNING,
    UDS_RT_ST_COMPLETED,
    UDS_RT_ST_ERROR
} uds_rt_status_t;

typedef struct {
    uint16_t rid;
    void (*start_routine)(const uint8_t* msg_buf, uint16_t msg_dlc);
    void (*stop_routine)(void);
    uint8_t rtst;
} uds_rtctrl_t;

void service_31_RoutineControl(const uint8_t* msg_buf, uint16_t msg_dlc);
bool_t service_31_check_len(const uint8_t* msg_buf, uint16_t msg_dlc);

#endif