#ifndef __CRC32_H__
#define __CRC32_H__

#include "stm32f10x.h"

uint32_t CRC32_Calculate(const uint32_t pData[], uint32_t wordCount);

#endif
