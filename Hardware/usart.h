#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];
extern char Rx_Packet[];
extern uint8_t Serial_RxFlag;
	
void USARTx_Conf(void);
void USARTx_SendByte(USART_TypeDef *pUSARTx, uint8_t data);
void USARTx_SendArray(USART_TypeDef *pUSARTx, uint8_t *Array,uint16_t Length);
void USARTx_SendStr(USART_TypeDef *pUSARTx, char *str);
void USARTx_SendNumber(USART_TypeDef *pUSARTx,uint32_t Number, uint8_t Length);
void USARTx_Printf(char *format, ...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);
void Serial_SendPacket(void);

#endif

