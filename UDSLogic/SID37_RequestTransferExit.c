/******************************************************************************
* 文件名称: SID37_RequestTransferExit.c
* 内容摘要: 请求退出传输（37服务）
* ...
******************************************************************************/

#include "SID37_RequestTransferExit.h"
#include "service_cfg.h"
#include "uds_service.h"
#include "bl_config.h"          // 新增：定义 APP_VALID_FLAG_ADDR, APP_VALID_MAGIC
#include "stm32f10x.h"          // 新增：提供 NVIC_SystemReset() 函数
#include "MyFlash.h"

extern uint32_t g_download_offset;
extern uint32_t g_download_size;

/******************************************************************************
* 函数名称: bool_t service_37_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 检查 37 服务数据长度是否合法
* 输入参数: uint16_t msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: TRUE: 合法; FALSE: 非法
* 其它说明: 无
******************************************************************************/
bool_t service_37_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    bool_t ret = FALSE;

    (void)msg_buf;
    ret = TRUE;

    return ret;
}

/******************************************************************************
* 函数名称: service_37_RequestTransferExit
* 功能说明: 37服务 - 请求退出传输，完成APP下载后写入有效标志并复位
******************************************************************************/
void service_37_RequestTransferExit(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    uint8_t rsp_buf[8];

    (void)msg_buf;
    (void)msg_dlc;

    // 1. 验证接收到的数据长度是否与预期一致
    if (g_download_offset != g_download_size)
    {
        uds_negative_rsp(SID_37, NRC_GENERAL_PROGRAMMING_FAILURE);
        return;
    }

    // 2. 【Step 5 核心修改】写入 APP 有效标志
    //    注意：如果标志存储在 Flash 中，不能直接指针赋值，需使用 Flash 写入函数。
    //    若使用备份寄存器或 SRAM（不掉电区域），可如下直接操作。
    //*(volatile uint32_t *)APP_VALID_FLAG_ADDR = APP_VALID_MAGIC;
	// MyFlash_ProgramHalfWord(APP_VALID_FLAG_ADDR,     (uint16_t)(APP_VALID_MAGIC & 0xFFFF));
	// MyFlash_ProgramHalfWord(APP_VALID_FLAG_ADDR + 2, (uint16_t)((APP_VALID_MAGIC >> 16) & 0xFFFF));
    // 3. 发送肯定响应（告诉上位机下载完成）
    rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_37);
    uds_positive_rsp(rsp_buf, 1);

    // 4. 【可选】短暂延时，确保 CAN 报文发送完成（避免复位后发送中断）
    //    若 CAN 发送采用 DMA 或硬件缓存，可省略；若担心丢包，建议加一个短延时。
    //    例如：for (uint32_t i = 0; i < 100000; i++);  // 约几毫秒，需实测
	/* 等待 CAN 发送完成 */
	for (volatile uint32_t i = 0; i < 10000; i++);
    // 5. 执行软复位，跳转到新下载的 APP
    //NVIC_SystemReset();

    // 注意：复位后不会再执行此函数后续代码
}