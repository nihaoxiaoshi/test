#ifndef __STORE_H__
#define __STORE_H__

#define STORE_START_ADDRESS   0x0800FC00
#define STORE_NUM             512

#include "stm32f10x.h"
#include "MyFlash.h"

extern uint16_t Store_Data[];

void Store_Init(void);
void Store_Save(void);
void Store_Clear(void);

#endif
