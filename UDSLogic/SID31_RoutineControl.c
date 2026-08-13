#include "SID31_RoutineControl.h"
#include "service_cfg.h"
#include "uds_service.h"
#include "MyFlash.h"
#include "Store.h"
#include "uds_tp.h"
#include "bl_config.h"
#include "CRC32.h"
typedef enum __UDS_ROUTINE_CTRL_TYPE__
{
    UDS_ROUTINE_CTRL_NONE = 0,
    UDS_ROUTINE_CTRL_START = 0x01,
    UDS_ROUTINE_CTRL_STOP = 0x02,
    UDS_ROUTINE_CTRL_REQUEST_RESULT = 0x03
} uds_routine_ctrl_type;

static bool_t g_start_ok = TRUE;  // ← 新增：记录 start 执行结果
// ==================== RID START 处理函数 ====================

static void start_FF00(const uint8_t* msg_buf, uint16_t msg_dlc)
{
	g_start_ok = FALSE;  // ← 新增：预设失败
    uint32_t erase_addr = ((uint32_t)msg_buf[4] << 24) |
                          ((uint32_t)msg_buf[5] << 16) |
                          ((uint32_t)msg_buf[6] << 8)  |
                          (uint32_t)msg_buf[7];
    uint32_t erase_size = ((uint32_t)msg_buf[8] << 24) |
                          ((uint32_t)msg_buf[9] << 16) |
                          ((uint32_t)msg_buf[10] << 8) |
                          (uint32_t)msg_buf[11];

    if (erase_addr >= 0x08000000 && erase_addr < 0x08010000)
    {
        uint32_t end_addr = erase_addr + erase_size;
        if (end_addr > 0x08010000)
            end_addr = 0x08010000;

        uint32_t page_start = erase_addr & 0xFFFFFC00;
        uint32_t page_end = (end_addr + 0x3FF) & 0xFFFFFC00;

        for (uint32_t page = page_start; page < page_end; page += 0x400)
            MyFlash_ErasePage(page);
				
		/* 1、擦除APP Valid 标志位 */
		MyFlash_ErasePage(APP_VALID_FLAG_ADDR);

			g_start_ok = TRUE;  // ← 新增：执行成功才置 TRUE
    }
}

static void start_0202(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    extern uint32_t g_download_addr;
    extern uint32_t g_download_size;

    g_start_ok = FALSE;  // 预设失败，校验通过后再置 TRUE

    // 实际 APP 数据长度 = 总下载大小 - 末尾 4 字节 CRC
    uint32_t app_data_size = g_download_size - 4;

    // 读取期望 CRC（位于下载数据末尾）
    uint32_t expected_crc = MyFlash_ReadWord(g_download_addr + app_data_size);

    // 软件 CRC-32 计算（查表法，多项式 0x04C11DB7）
    uint32_t word_count = app_data_size / 4;
    uint32_t calculated_crc = CRC32_Calculate((const uint32_t *)(g_download_addr), word_count);

    // CRC 校验通过才写入 APP 有效标志
    if (calculated_crc == expected_crc)
    {
        MyFlash_ProgramWord(APP_VALID_FLAG_ADDR, APP_VALID_MAGIC);
        g_start_ok = TRUE;
    }
}

static void start_0203(const uint8_t* msg_buf, uint16_t msg_dlc)
{
	g_start_ok = TRUE;             // ← 新增
    MyFlash_ErasePage(0x0800FC00);
    Store_Init();
}

static void start_FF01(const uint8_t* msg_buf, uint16_t msg_dlc)
{
}

static void start_DFE0(const uint8_t* msg_buf, uint16_t msg_dlc)
{
}

static void start_0204(const uint8_t* msg_buf, uint16_t msg_dlc)
{
}

static void start_DD02(const uint8_t* msg_buf, uint16_t msg_dlc)   // ← 新增
{
	g_start_ok = TRUE;             // ← 新增
    MyFlash_ErasePage(0x0800FC00);
    Store_Init();
}
// ==================== RID STOP 处理函数 ====================

static void stop_NONE(void) { }

// ==================== RID 注册表 ====================

static uds_rtctrl_t rtctrl_list[RTCTRL_NUM] = {
    {RID_ERASE_MEMORY,    start_FF00, stop_NONE, UDS_RT_ST_IDLE},
    {RID_CHECK_INTEGRITY, start_0202, stop_NONE, UDS_RT_ST_IDLE},
    {RID_UPDATE_COND,     start_0203, stop_NONE, UDS_RT_ST_IDLE},
    {RID_CONSISTENCY,     start_FF01, stop_NONE, UDS_RT_ST_IDLE},
    {RID_SLAVE_MODE,      start_DFE0, stop_NONE, UDS_RT_ST_IDLE},
    {RID_DIGITAL_SIG,     start_0204, stop_NONE, UDS_RT_ST_IDLE},
	{RID_TEST_DD02,       start_DD02, stop_NONE, UDS_RT_ST_IDLE},
};

// ==================== 辅助函数 ====================

static void rsp_or_skip(uint8_t* data, uint16_t len)
{
    if (N_TATYPE_FUNCTIONAL != g_tatype)
        uds_positive_rsp(data, len);
}

// ==================== 长度检查 ====================

bool_t service_31_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    if (msg_dlc >= 4)
    {
        uint16_t rid = ((uint16_t)msg_buf[2]) << 8 | msg_buf[3];
        if (rid == RID_ERASE_MEMORY)
            return (msg_dlc >= 12);
        return TRUE;
    }
    return FALSE;
}

// ==================== 主处理函数 ====================

void service_31_RoutineControl(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    uint8_t subfunction = UDS_GET_SUB_FUNCTION(msg_buf[1]);
    uint16_t rid = ((uint16_t)msg_buf[2]) << 8 | msg_buf[3];
    uint16_t rid_n;
    bool_t find_rid = FALSE;
    uint8_t rsp_buf[8];

    for (rid_n = 0; rid_n < RTCTRL_NUM; rid_n++)
    {
        if (rtctrl_list[rid_n].rid == rid)
        {
            find_rid = TRUE;
            break;
        }
    }

    rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_31);
    rsp_buf[1] = msg_buf[1];
    rsp_buf[2] = msg_buf[2];
    rsp_buf[3] = msg_buf[3];

    switch (subfunction)
    {
    case UDS_ROUTINE_CTRL_START:
        if (!find_rid)
        {
            uds_negative_rsp(SID_31, NRC_REQUEST_OUT_OF_RANGE);
            break;
        }

        rtctrl_list[rid_n].start_routine(msg_buf, msg_dlc);
        
		if (!g_start_ok)                // ← 新增：检查执行结果
    	{
        	uds_negative_rsp(SID_31, NRC_REQUEST_OUT_OF_RANGE);
        	break;
    	}
        rsp_buf[4] = 0x00;
        rsp_or_skip(rsp_buf, 5);
        break;

    case UDS_ROUTINE_CTRL_STOP:
        if (!find_rid)
        {
            uds_negative_rsp(SID_31, NRC_REQUEST_OUT_OF_RANGE);
            break;
        }
        if (rtctrl_list[rid_n].rtst == UDS_RT_ST_IDLE)
        {
            uds_negative_rsp(SID_31, NRC_REQUEST_SEQUENCE_ERROR);
            break;
        }
        rtctrl_list[rid_n].stop_routine();
        rtctrl_list[rid_n].rtst = UDS_RT_ST_IDLE;
        rsp_or_skip(rsp_buf, 4);
        break;

    case UDS_ROUTINE_CTRL_REQUEST_RESULT:
        if (!find_rid)
        {
            uds_negative_rsp(SID_31, NRC_REQUEST_OUT_OF_RANGE);
            break;
        }
        if (rtctrl_list[rid_n].rtst == UDS_RT_ST_IDLE)
        {
            uds_negative_rsp(SID_31, NRC_REQUEST_SEQUENCE_ERROR);
            break;
        }
        rsp_buf[4] = (uint8_t)rtctrl_list[rid_n].rtst;
        rsp_or_skip(rsp_buf, 5);
        break;

    default:
        uds_negative_rsp(SID_31, NRC_SUBFUNCTION_NOT_SUPPORTED);
        break;
    }
}