#include "MyCAN.h"

void MyCAN_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* CAN基础配置 */
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_Prescaler = 48;		//波特率 = 36M / 48 / (1 + 2 + 3) = 125K
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_Init(CAN1, &CAN_InitStructure);
	
	/* CAN过滤器配置 */
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

void MyCAN_Transmit(CanTxMsg *TxMessage)
{
	uint32_t Timeout = 0;
	uint8_t TransmitMailbox;
	
	TransmitMailbox = CAN_Transmit(CAN1, TxMessage);
	
	while (CAN_TransmitStatus(CAN1, TransmitMailbox) != CAN_TxStatus_Ok)
	{
		Timeout ++;
		if (Timeout > 100000)
		{
			break;
		}
	}
}

uint8_t MyCAN_ReceiveFlag(void)
{
	if (CAN_MessagePending(CAN1, CAN_FIFO0) > 0)
	{
		return 1;
	}
	return 0;
}

/* C语言不支持函数多返回值，函数使用输出参数，相当于函数返回多个值 */
void MyCAN_Receive(CanRxMsg *RxMessage)
{
	CAN_Receive(CAN1, CAN_FIFO0, RxMessage);
}

void InitCanTxMsg(CanTxMsg *msg, uint32_t stdId, uint8_t *pData, uint8_t dlc) 
{
    msg->StdId = stdId;
    msg->ExtId = 0;      //不使用扩展ID
    msg->IDE = CAN_Id_Standard;
    msg->RTR = CAN_RTR_Data;
    msg->DLC = (dlc > 8) ? 8 : dlc; // 限制DLC不超过8
    if (pData != NULL_PTR) 
	{
        memcpy(msg->Data, pData, msg->DLC);
    }
}


