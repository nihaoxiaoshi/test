/******************************************************************************
* 文件名称: SID27_SecurityAccess.c
* 内容摘要: 安全访问
* 创建者の: 孔佳伟
* 个人主页: https://gitee.com/thin-wind/jump
* 修改记录: 
******************************************************************************/

#include "SID27_SecurityAccess.h"
#include "service_cfg.h"
#include "uds_service.h"
//#include "stm32mp1xx_hal.h"
#include "systick.h"
#include <stdlib.h>

#define UNLOCKKEY					0x00000000
#define UNLOCKSEED					0x00000000
#define UNDEFINESEED				0xFFFFFFFF
#define SEEDMASK					0x80000000
#define SHIFTBIT					1
#define ALGORITHMASK				0x42303131

#define UDS_SEED_LENGTH                   (0x10)
#define UDS_REQUEST_SEED                  (0x01)
#define UDS_SEND_KEY                      (0x02)
#define UDS_FAS_MAX_TIMES                 (0x02)  /* failed security access */

#define USE_FIXED_SEED
void uds_compute_key(const uint8_t* seed, uint8_t* key, uint8_t subfunction);
static uint8_t req_seed = 0;				// 接收到请求种子标志
static uint8_t org_seed_buf[UDS_SEED_LENGTH];
static uint8_t req_subfunc = 0;   // 记录当前请求种子的子功能（0x01 或 0x11）

// 当前安全访问等级
static uds_sa_lv curr_sa = UDS_SA_NON;

static const uint8_t fixed_seed[UDS_SEED_LENGTH] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

// 安全访问种子匹配错误次数
uint8_t uds_fsa_cnt = 0;

/******************************************************************************
* 函数名称: void set_current_sa_lv(uds_sa_lv level)
* 功能说明: 设置当前安全访问等级
* 输入参数: uds_sa_lv level        	--安全访问等级
* 输出参数: 无
* 函数返回: 无
* 其它说明: 无
******************************************************************************/
void set_current_sa_lv(uds_sa_lv level)
{
	curr_sa = level;
}

/* 种子转密钥算法接口，可用于DLL/测试验证 */
void uds_compute_key(const uint8_t* seed, uint8_t* key, uint8_t subfunction);

/******************************************************************************
* 函数名称: uds_session_t get_current_sa_lv(void)
* 功能说明: 获取当前安全访问等级
* 输入参数: 无
* 输出参数: 无
* 函数返回: 当前安全访问等级
* 其它说明: 无
******************************************************************************/
uds_sa_lv get_current_sa_lv(void)
{
    return curr_sa;
}


/******************************************************************************
* 函数名称: static uint8_t rand_u8 (void)
* 功能说明: 获取随机数
* 输入参数: 无
* 输出参数: 无
* 函数返回: 8 位随机数
* 其它说明: 无
******************************************************************************/
static uint8_t rand_u8(void)
{
	static uint32_t cnt = 666;
	srand(GetSysRunTime() + cnt);
	cnt++;

	return (rand() % 0xFF);
}


/******************************************************************************
* 函数名称: static uint32_t seedTOKey(uint32_t seed)
* 功能说明: 安全访问算法
* 输入参数: uint32_t seed		--种子
* 输出参数: 无
* 函数返回: key 值
* 其它说明: 该算法需根据实际需求而定
******************************************************************************/
static uint32_t seedTOKey(uint32_t seed)
{
	return (~seed);
}

#if 0
/******************************************************************************
* 函数名称: int uds_security_access(uint8_t* key_buf, uint8_t* seed_buf)
* 功能说明: 比较自己根据种子 seed 计算的 key 值与接收到的 key 值是否一致
* 输入参数: uint8_t* key_buf		--接收到的 key
    　　　　uint8_t* seed_buf		--种子
* 输出参数: 无
* 函数返回: 0: 一致; -1: 不一致
* 其它说明: 无
******************************************************************************/
int uds_security_access(uint8_t* key_buf, uint8_t* seed_buf)
{
	uint32_t key = 0;
	uint32_t seed = 0;
    
	key = (key_buf[0] << 24) |  (key_buf[1] << 16) |  (key_buf[2] << 8) |  key_buf[3];
	seed = (seed_buf[0] << 24) |  (seed_buf[1] << 16) |  (seed_buf[2] << 8) |  seed_buf[3];

	if (key == seedTOKey(seed))
	    return 0;
	else
	    return -1;
}
#endif
/******************************************************************************
* 函数名称: int uds_security_access(uint8_t* key_buf, uint8_t* seed_buf)
* 功能说明: 比较自己根据种子 seed 计算的 key 值与接收到的 key 值是否一致 (16字节版本)
* 输入参数: uint8_t* key_buf		--接收到的key (16字节)
*           uint8_t* seed_buf		--本地生成的种子 (16字节)
* 输出参数: 无
* 函数返回: 0: 一致; -1: 不一致
* 其它说明: 算法为 Key = ~Seed (按位取反)
******************************************************************************/

#if 0
int uds_security_access(uint8_t* key_buf, uint8_t* seed_buf)
{
    uint16_t i;
    for (i = 0; i < UDS_SEED_LENGTH; i++) 
    {
        // 计算 seed_buf[i] 按位取反的结果，并与收到的 key_buf[i] 比较
        // 注意：~ 运算符优先级高于比较，加括号明确，并强转为 uint8_t 防止符号位扩展
        if (key_buf[i] != (uint8_t)(~seed_buf[i])) 
        {
            return -1; // 只要有一个字节不匹配，密钥无效
        }
    }
    return 0; // 全部匹配，解锁成功
}
#endif

/******************************************************************************
* 函数名称: bool_t service_27_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 检查 27 服务数据长度是否合法
* 输入参数: uint16_t msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: TRUE: 合法; FALSE: 非法
* 其它说明: 无
******************************************************************************/
bool_t service_27_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    bool_t ret = FALSE;
    uint8_t subfunction;
    subfunction = UDS_GET_SUB_FUNCTION(msg_buf[1]);

    // 请求种子：0x01 或 0x11，长度应为 2
    if ((subfunction == 0x01 || subfunction == 0x11) && msg_dlc == 2)
    {
        ret = TRUE;
    }
    // 发送密钥：0x02 或 0x12，长度应为 2 + UDS_SEED_LENGTH
    else if ((subfunction == 0x02 || subfunction == 0x12) && msg_dlc == (2 + UDS_SEED_LENGTH))
    {
        ret = TRUE;
    }

    return ret;
}

#if 0
/******************************************************************************
* 函数名称: void service_27_SecurityAccess(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 27 服务 - 安全访问
* 输入参数: uint8_t*    msg_buf         --数据首地址
    　　　　uint8_t     msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: 无
* 其它说明: 无
******************************************************************************/
void service_27_SecurityAccess(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    uint8_t subfunction;
	uint8_t rsp_buf[UDS_SEED_LENGTH + 3];//16 + 3 = 19 字节，原先只能存8字节
	uint16_t i;

	subfunction = UDS_GET_SUB_FUNCTION(msg_buf[1]);

	switch (subfunction)
	{
		case UDS_REQUEST_SEED:	// 请求种子
		{
			// 锁定时间要求不能因模块断电被清零，这里暂未实现掉电保存的功能
			if (uds_timer_chk(UDS_TIMER_FSA) > 0)
			{
				uds_negative_rsp(SID_27, NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED);
				break;
			}
			req_seed = 1;
			rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_27);
			rsp_buf[1] = subfunction;
			for (i = 0; i < UDS_SEED_LENGTH; i++) 
			{
				// ECU 在已经解锁的情况下，如果再次收到请求种子，则返回种子 0x00000000
				if (curr_sa == UDS_SA_LV1)
					org_seed_buf[i] = 0;
				else
				    org_seed_buf[i] = rand_u8();
				rsp_buf[2+i] = org_seed_buf[i];
			}
			uds_positive_rsp (rsp_buf, UDS_SEED_LENGTH+2);
			break;
		}
		case UDS_SEND_KEY:		// 发送密钥
		{
			// 在发送秘钥前必须先请求种子
			if (req_seed == 0)
			{
				uds_negative_rsp(SID_27, NRC_REQUEST_SEQUENCE_ERROR);
				break;
			}
			req_seed = 0;

			// 判断发送过来的密钥和自己计算的密钥是否一致
			if (!uds_security_access((uint8_t *)&msg_buf[2], org_seed_buf))
			{
				rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_27);
				rsp_buf[1] = subfunction;
				uds_positive_rsp (rsp_buf,2);
				set_current_sa_lv(UDS_SA_LV1);
			}
			else
			{
				uds_fsa_cnt++;
				if (uds_fsa_cnt >= UDS_FAS_MAX_TIMES) 
				{
					// 密钥尝试次数超过限值
					uds_timer_start (UDS_TIMER_FSA); // 锁定时间要求不能因模块断电被清零，这里暂未实现掉电保存的功能
					uds_negative_rsp (SID_27, NRC_EXCEEDED_NUMBER_OF_ATTEMPTS);
				} 
				else 
				{
					// 密钥无效
					uds_negative_rsp (SID_27, NRC_INVALID_KEY);
				}
			}
			break;
		}
		default:
			uds_negative_rsp (SID_27, NRC_SUBFUNCTION_NOT_SUPPORTED);
			break;
	}
}
#endif

void service_27_SecurityAccess(const uint8_t* msg_buf, uint16_t msg_dlc)
{
    uint8_t subfunction;
    uint8_t rsp_buf[UDS_SEED_LENGTH + 3];
    uint16_t i;

    subfunction = UDS_GET_SUB_FUNCTION(msg_buf[1]);

    switch (subfunction)
    {
        // ---------- 请求种子（0x01 和 0x11） ----------
        case 0x01:   // 原有 Level 1
        case 0x11:   // 新增 Level 11
        {
            if (uds_timer_chk(UDS_TIMER_FSA) > 0) {
                uds_negative_rsp(SID_27, NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED);
                break;
            }

            req_subfunc = subfunction;   // 记录当前请求的级别

            rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_27);
            rsp_buf[1] = subfunction;

            // 种子生成：如果当前已经解锁了对应的级别，则返回全0，否则返回随机数
            // 注：原代码只判断了 UDS_SA_LV1，这里我们判断具体级别
            if ((subfunction == 0x01 && curr_sa == UDS_SA_LV1) ||
                (subfunction == 0x11 && curr_sa == UDS_SA_LV11)) {
                // 已解锁该级别，种子全0
                for (i = 0; i < UDS_SEED_LENGTH; i++) {
                    org_seed_buf[i] = 0;
                }
            } else {
                // 生成随机种子
                for (i = 0; i < UDS_SEED_LENGTH; i++) {
                    org_seed_buf[i] = fixed_seed[i];
                }
            }

            // 将种子填入响应
            for (i = 0; i < UDS_SEED_LENGTH; i++) {
                rsp_buf[2 + i] = org_seed_buf[i];
            }
            uds_positive_rsp(rsp_buf, UDS_SEED_LENGTH + 2);
            break;
        }

        // ---------- 发送密钥（0x02 和 0x12） ----------
        case 0x02:   // 原有 Level 1 密钥
        case 0x12:   // 新增 Level 11 密钥
        {
            uint8_t expected_key[UDS_SEED_LENGTH];
            uint8_t match = 1;

            // 检查是否已请求过种子，且级别必须匹配
            if (req_subfunc == 0) {
                uds_negative_rsp(SID_27, NRC_REQUEST_SEQUENCE_ERROR);
                break;
            }
            // 检查级别匹配：0x02 必须对应 0x01，0x12 必须对应 0x11
            if ((subfunction == 0x02 && req_subfunc != 0x01) ||
                (subfunction == 0x12 && req_subfunc != 0x11)) {
                uds_negative_rsp(SID_27, NRC_REQUEST_SEQUENCE_ERROR);
                break;
            }
#if 0
            // 根据级别计算期望密钥
            if (req_subfunc == 0x01) {
                // Level 1 算法：全字节取反
                for (i = 0; i < UDS_SEED_LENGTH; i++) {
                    expected_key[i] = (uint8_t)(~org_seed_buf[i]);
                }
            } else if (req_subfunc == 0x11) {
                // Level 11 算法：前两个字节取反，后两个字节高四位置0，其余不变
                for (i = 0; i < UDS_SEED_LENGTH; i++) {
                    if (i < 2) {
                        expected_key[i] = (uint8_t)(~org_seed_buf[i]);
                    } else if (i >= 14) {  // 最后两个字节（索引14和15）
                        expected_key[i] = org_seed_buf[i] & 0x0F;   // 高四位清零
                    } else {
                        expected_key[i] = org_seed_buf[i];          // 保持不变
                    }
                }
            }
#endif

            uds_compute_key(fixed_seed, expected_key, req_subfunc);
            // 比较接收到的密钥（msg_buf[2] 开始）与期望密钥
            for (i = 0; i < UDS_SEED_LENGTH; i++) {
                if (msg_buf[2 + i] != expected_key[i]) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                // 密钥正确，解锁对应级别
                rsp_buf[0] = USD_GET_POSITIVE_RSP(SID_27);
                rsp_buf[1] = subfunction;
                uds_positive_rsp(rsp_buf, 2);

                if (subfunction == 0x02) {
                    set_current_sa_lv(UDS_SA_LV1);
                } else if (subfunction == 0x12) {
                    set_current_sa_lv(UDS_SA_LV11);
                }
                req_subfunc = 0;   // 清除请求标记
            } else {
                // 密钥错误
                uds_fsa_cnt++;
                if (uds_fsa_cnt >= UDS_FAS_MAX_TIMES) {
                    uds_timer_start(UDS_TIMER_FSA);
                    uds_negative_rsp(SID_27, NRC_EXCEEDED_NUMBER_OF_ATTEMPTS);
                } else {
                    uds_negative_rsp(SID_27, NRC_INVALID_KEY);
                }
                // 注意：此时 req_subfunc 未清除，但错误后应清除，防止重复使用
                req_subfunc = 0;
            }
            break;
        }

        default:
            uds_negative_rsp(SID_27, NRC_SUBFUNCTION_NOT_SUPPORTED);
            break;
    }
}

void uds_compute_key(const uint8_t* seed, uint8_t* key, uint8_t subfunction)
{
    uint16_t i;
    if (subfunction == 0x01) {
        for (i = 0; i < UDS_SEED_LENGTH; i++) {
            key[i] = (uint8_t)(~seed[i]);
        }
    } else if (subfunction == 0x11) {
        for (i = 0; i < UDS_SEED_LENGTH; i++) {
            if (i < 2)
                key[i] = (uint8_t)(~seed[i]);
            else if (i >= 14)
                key[i] = seed[i] & 0x0F;
            else
                key[i] = seed[i];
        }
    }
}
/****************EOF****************/
