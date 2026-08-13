/******************************************************************************
* 文件名称: SID34_RequestDownload.c
* 内容摘要: 请求下载
* 创建者の: 孔佳伟
* 个人主页: https://gitee.com/thin-wind/jump
* 修改记录: 
******************************************************************************/

#include "SID34_RequestDownload.h"
#include "service_cfg.h"
#include "uds_service.h"
#include "bl_config.h"
#include "MyFlash.h"

// 36 服务数据传输报文总大小
#define TOTAL_LEN_36	(1024 + 2)
#define ERASE_START  0x08004000
#define ERASE_END    0x0800F400   // 不含 BL_FLAG_ADDR(0x0800F400)

// 下载参数全局变量（供 36/37 服务使用）
uint32_t g_download_addr = 0x08004000;
uint32_t g_download_size = 0;
uint32_t g_download_offset = 0;



/******************************************************************************
* 函数名称: bool_t service_34_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 检查 34 服务数据长度是否合法
* 输入参数: uint16_t msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: TRUE: 合法; FALSE: 非法
* 其它说明: 无
******************************************************************************/
bool_t service_34_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
{
	bool_t ret = FALSE;

	(void)msg_buf;
	if (msg_dlc >= 10)
		ret = TRUE;

	return ret;
}


/******************************************************************************
* 函数名称: void service_34_RequestDownload(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 34 服务 - 请求下载
* 输入参数: uint8_t*    msg_buf         --数据首地址
     　　　　uint8_t     msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: 无
* 其它说明: 无
******************************************************************************/
void service_34_RequestDownload(const uint8_t* msg_buf, uint16_t msg_dlc)
{
	uint8_t rsp_buf[8];
	uint32_t page;
	for (page = ERASE_START; page < ERASE_END; page += 0x400) {
    	MyFlash_ErasePage(page);
	}

	// 1. 解析地址 (ALFID = 0x44: 4字节地址 + 4字节长度)
	g_download_addr = ((uint32_t)msg_buf[3] << 24) |
	                  ((uint32_t)msg_buf[4] << 16) |
	                  ((uint32_t)msg_buf[5] << 8)  |
	                  (uint32_t)msg_buf[6];
	g_download_size = ((uint32_t)msg_buf[7] << 24) |
	                  ((uint32_t)msg_buf[8] << 16) |
	                  ((uint32_t)msg_buf[9] << 8)  |
	                  (uint32_t)msg_buf[10];
	g_download_offset = 0;

    // 2. 【关键修改】先做地址合法性检查，不通过就直接返回 NRC，绝不动笔写 Flash
    if ((g_download_addr < APP_ADDR) || ((g_download_addr + g_download_size) > (APP_ADDR + APP_SIZE)))
    {
        // 地址超出 APP 范围，返回 NRC 0x31 (RequestOutOfRange)
        uds_negative_rsp(SID_34, 0x31);
        return;  // 立即退出，绝对不执行下面的正响应
    }

 	// 3. 地址校验通过，才发送肯定响应
	rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_34);
	rsp_buf[1] = 0x20;
	rsp_buf[2] = (uint8_t)(TOTAL_LEN_36 >> 8);
	rsp_buf[3] = (uint8_t)(TOTAL_LEN_36 >> 0);
	uds_positive_rsp(rsp_buf, 4);	
}


/****************EOF****************/
