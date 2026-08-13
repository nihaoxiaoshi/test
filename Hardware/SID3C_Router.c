#include "SID3C_Router.h"
#include "service_cfg.h"
#include "uds_service.h"
#include "uds_tp.h"

bool_t service_3C_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    bool_t ret = FALSE;
    (void)msg_buf;
    // ????: SID(1) + ????(1) + ??SID(1) + ??1???? = 4
    if (msg_dlc >= 4)
        ret = TRUE;
    return ret;
}

void service_3C_Router(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    uint8_t inner_buf[64];
    uint16_t inner_dlc;
    uint8_t inner_len;
    uint16_t i;

    if (msg_dlc < 3)
    {
        uds_negative_rsp(SID_3C, NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT);
        return;
    }

    inner_len = msg_buf[1];

    if (inner_len < 2 || (uint16_t)(2 + inner_len) > msg_dlc)
    {
        uds_negative_rsp(SID_3C, NRC_INVALID_MESSAGE_LENGTH_OR_FORMAT);
        return;
    }

    inner_dlc = inner_len;
    for (i = 0; i < inner_dlc && i < sizeof(inner_buf); i++)
        inner_buf[i] = msg_buf[2 + i];

    // ????? UDS ????
    uds_service_dispatch(inner_buf, inner_dlc);
}