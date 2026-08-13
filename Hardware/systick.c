#include "systick.h"

static uint64_t g_sysRunTime = 0; //系统运行时间(ms)

uint64_t GetSysRunTime(void)
{
	return g_sysRunTime;
}

/* 1ms 触发一次中断 */
void SysTick_Handler(void)
{
	g_sysRunTime++;  //系统运行时间自增1
}
