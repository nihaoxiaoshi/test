/*
 * key.h
 *
 *  Created on: Apr 23, 2024
 *      Author: 86131
 */

#ifndef HARDWARE_KEY_KEY_H_
#define HARDWARE_KEY_KEY_H_

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif /* HARDWARE_KEY_KEY_H_ */
