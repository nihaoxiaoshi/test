/******************************************************************************
* 文件名称: SID2E_WriteDataByIdentifier.c
* 内容摘要: 根据标识符写入数据
* 创建者の: 孔佳伟
* 个人主页: https://gitee.com/thin-wind/jump
* 修改记录: 
******************************************************************************/


#include "SID2E_WriteDataByIdentifier.h"
#include "service_cfg.h"
#include "uds_service.h"
#include "flash.h"
#include "Store.h"

/******************************************************************************
* 函数名称: bool_t service_2E_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 检查 2E 服务数据长度是否合法
* 输入参数: uint16_t msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: TRUE: 合法; FALSE: 非法
* 其它说明: 无
******************************************************************************/
bool_t service_2E_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
{
	bool_t ret = FALSE;
	
	(void)msg_buf;
	if(msg_dlc >= 4)
		ret = TRUE;

	return ret;
}


/******************************************************************************
* 函数名称: void service_2E_WriteDataByIdentifier(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 2E 服务 - 根据标识符写入数据
* 输入参数: uint8_t*    msg_buf         --数据首地址
    　　　　uint8_t     msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: 无
* 其它说明: 无
******************************************************************************/
void service_2E_WriteDataByIdentifier(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    uint8_t rsp_buf[8];
	uint16_t did;

	did = ((uint16_t)msg_buf[1]) << 8;
	did |= msg_buf[2];

	switch(did)
	{
		case 0x1234:
		{
			uint16_t data_len = msg_dlc - 3;
			if (FlashDrv_WriteDID(did, &msg_buf[3], data_len) == FLASH_DRV_OK)
			{
				rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_2E);
				rsp_buf[1] = msg_buf[1];
				rsp_buf[2] = msg_buf[2];
				uds_positive_rsp(rsp_buf, 3);
			}
			else
			{
				uds_negative_rsp(SID_2E, NRC_GENERAL_PROGRAMMING_FAILURE);
			}
			break;
		}
#if 0
		/*硬编码写入字节长度9*/
		case 0xF0FA:
		{
			uint8_t i;
			uint16_t halfword;
			for (i = 0; i < 9; i += 2)
			{
				halfword = msg_buf[3 + i];
				if (i + 1 < 9)
					halfword |= (uint16_t)msg_buf[4 + i] << 8;
				else
					halfword |= 0xFF00;
				Store_Data[1 + i / 2] = halfword;
			}
			Store_Save();
			rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_2E);
			rsp_buf[1] = msg_buf[1];
			rsp_buf[2] = msg_buf[2];
			uds_positive_rsp(rsp_buf, 3);
			break;
		}
#endif
		case 0xF184:
		case 0xF0FA:
		{
			uint16_t data_len = msg_dlc - 3;            // 实际数据字节数
			uint16_t halfword_cnt = (data_len + 1) / 2;  // 需要写的 halfword 数量
			uint16_t max_cnt = STORE_NUM - 1;            // Store_Data[1..511]
			uint8_t i;

			if (halfword_cnt > max_cnt)
			{
				uds_negative_rsp(SID_2E, NRC_REQUEST_OUT_OF_RANGE);
				break;
			}

			for (i = 0; i < halfword_cnt; i++)
			{
				uint16_t halfword = msg_buf[3 + i * 2];
				if (i * 2 + 1 < data_len)
					halfword |= (uint16_t)msg_buf[4 + i * 2] << 8;
				else
					halfword |= 0xFF00;                  // 奇数长度时最后一字节 MSB 补 0xFF
				Store_Data[1 + i] = halfword;
			}
			Store_Save();
			rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_2E);
			rsp_buf[1] = msg_buf[1];
			rsp_buf[2] = msg_buf[2];
			uds_positive_rsp(rsp_buf, 3);
			break;
		}
		default:
			uds_negative_rsp(SID_2E, NRC_REQUEST_OUT_OF_RANGE);
			break;
	}
}


/****************EOF****************/
