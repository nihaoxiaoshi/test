#ifndef __MYCAN_H__
#define __MYCAN_H__

#include "stm32f10x.h"                  // Device header
#include <string.h>
#define  NULL_PTR    ((void *)0)

void MyCAN_Init(void);
void MyCAN_Transmit(CanTxMsg *TxMessage);
uint8_t MyCAN_ReceiveFlag(void);
void MyCAN_Receive(CanRxMsg *RxMessage);
void InitCanTxMsg(CanTxMsg *msg, uint32_t stdId, uint8_t *pData, uint8_t dlc);






#endif 



