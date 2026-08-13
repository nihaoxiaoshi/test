/*
 * key.c
 *
 *  Created on: Apr 23, 2024
 *      Author: 86131
 */
#include "key.h"

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //PA0
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//œ¬¿≠ ‰»Î
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
        Delay_ms(20);
        KeyNum = 1;
    }
    return KeyNum;
}

