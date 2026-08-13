#include "usart.h"

uint8_t Serial_RxData = 0;
uint8_t Serial_RxFlag = 0;

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];
char Rx_Packet[100];


void USARTx_Conf(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    //注意时钟使能的外设是在APB1还是APB2上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    USART_DeInit(USART1);//将USART1寄存器重置为默认值

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//串口发送和接收模式
    USART_Init(USART1, &USART_InitStructure);//串口初始化

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);//使能串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//串口中断使能
}

void USARTx_SendByte(USART_TypeDef *pUSARTx, uint8_t data)
{
    USART_SendData(pUSARTx, data);
    //当我们发送一帧数据时，一定要等待TXE置1，才能发送下一帧数据，不然会覆盖TDR寄存器上一帧的数据
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

void USARTx_SendArray(USART_TypeDef *pUSARTx, uint8_t *Array,uint16_t Length)
{
    uint16_t i;
    for(i = 0; i < Length; i++)
    {
        USARTx_SendByte(pUSARTx,Array[i]);
    }
}

void USARTx_SendStr(USART_TypeDef *pUSARTx, char *str)
{
    uint8_t i = 0;
    while(*(str+i) != '\0')
    {
        USARTx_SendByte(pUSARTx, *(str+i));
        i++;
    }
}

/* 计算x的y次方 */
uint32_t Serial_Pow(uint32_t x,uint32_t y)
{
    uint32_t Result = 1;
    while(y--)
    {
        Result *= x;
    }
    return Result;
}

void USARTx_SendNumber(USART_TypeDef *pUSARTx,uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++)
    {
        USARTx_SendByte(pUSARTx, Number / Serial_Pow(10,Length-i-1) % 10 + '0');
    }
}

/* printf串口重定向 */
int fputc(int ch, FILE *f)
{
    USARTx_SendByte(USART1, ch);
    return ch;
}

/* 采用可变参数列表 */
void USARTx_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	USARTx_SendStr(USART1, String);
}

uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

void Serial_SendPacket(void)
{
	USARTx_SendByte(USART1,0xFF);
	USARTx_SendArray(USART1,Serial_TxPacket,4);
	USARTx_SendByte(USART1,0xFE);
}

#if 0
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
		Serial_RxData = USART_ReceiveData(USART1);

		if(RxState == 0)
		{
			if(Serial_RxData == 0xFF)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if(RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = Serial_RxData;
			pRxPacket++;
			if(pRxPacket >= 4)
			{
				RxState = 2;
			}
		}
		else if(RxState == 2)
		{
			if(Serial_RxData == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
#endif

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
		Serial_RxData = USART_ReceiveData(USART1);

		if(RxState == 0)
		{
			if(Serial_RxData == '@' && Serial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if(RxState == 1)
		{
			if(Serial_RxData == '\r')
			{
				RxState = 2;
			}
			else
			{
				Rx_Packet[pRxPacket] = Serial_RxData;
				pRxPacket++;
			}
		}
		else if(RxState == 2)
		{
			if(Serial_RxData == '\n')
			{
				RxState = 0;
				Rx_Packet[pRxPacket] = '\0';
				Serial_RxFlag = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}


