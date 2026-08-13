#ifndef __MYFLASH_H__
#define __MYFLASH_H__

#include "stm32f10x.h"

uint32_t MyFlash_ReadWord(uint32_t Address);
uint16_t MyFlash_ReadHalfWord(uint32_t Address);
uint8_t MyFlash_ReadByte(uint32_t Address);
void MyFlash_EraseAllPages(void);
void MyFlash_ErasePage(uint32_t Address);
void MyFlash_ProgramWord(uint32_t Address, uint32_t Data);
void MyFlash_ProgramHalfWord(uint32_t Address, uint16_t Data);

#endif
