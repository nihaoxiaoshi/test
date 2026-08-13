#include "usart.h"
#include "MyCAN.h"
#include "uds_port.h"
#include "Timer.h"
#include "systick.h"
#include "Store.h"
#include "bl_config.h"
#include "stm32f10x.h"
typedef void (*pFunction)(void);  /* 函数指针类型 */

CanRxMsg RxMsg;

/* 检查APP是否有效（检查APP末尾的magic字）*/
static uint8_t check_app_valid(void)
{
    volatile uint32_t *app_valid = (volatile uint32_t *)APP_VALID_FLAG_ADDR;
    return (*app_valid == APP_VALID_MAGIC) ? 1 : 0;
}

/* 检查是否需要进入Bootloader模式 */
static uint8_t check_bl_mode(void)
{
    /* 方式1：检查RAM标志（APP在复位前写入） */
    volatile uint32_t *bl_flag = (volatile uint32_t *)BL_FLAG_ADDR;
    if (*bl_flag == BL_FLAG_VALUE)
    {
		//*bl_flag = 0;  /* 清除bootloader等待更新标志位，如果是存放在Flash中，使用擦除接口擦除为0xFF */
		MyFlash_ErasePage(BL_FLAG_ADDR);
		return 1;      /* 进入Bootloader */
    }
    
    /* 方式2：检查GPIO按键（如PA0按下时进入Bootloader） */
    /* if (GPIO按键按下) return 1; */
    
    return 0;  /* 默认跳转到APP */
}

/* 跳转到APP */
static void jump_to_app(void)
{
    uint32_t app_stack;
    pFunction app_entry;
    
    /* 关闭全局中断 */
    __disable_irq();


    
    
    /* 关闭所有外设中断（STM32F10x SPL v3.5.0 无 NVIC_DeInit，直接操作寄存器） */
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;
    
    /* 关闭SysTick */
    SysTick->CTRL = 0;
    
    /* 关闭所有外设时钟（可选，由APP自己初始化） */
    RCC_DeInit();
    
    /* 获取APP的栈顶指针（向量表第1个字） */
    app_stack = *(volatile uint32_t *)APP_ADDR;
    
    /* 获取APP的复位中断入口（向量表第2个字） */
    app_entry = (pFunction)(*(volatile uint32_t *)(APP_ADDR + 4));
    
    /* 设置主栈指针 */
    __set_MSP(app_stack);
    
    /* 跳转到APP */
    app_entry();
    
    /* 不会返回到这里 */
}

void uds_compute_key(const uint8_t* seed, uint8_t* key, uint8_t subfunction);
void BootJumpToApplication(void);
int main(void)
{
	uint8_t seed[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};
	uint8_t Key_seed[16] = {0};

    /* 第一步：检查是否需要进入Bootloader模式 */
    if (!check_bl_mode())
    {
        /* 不需要进入Bootloader，检查APP有效性 */
        if (check_app_valid())
        {
            /* APP有效，跳转 */
            BootJumpToApplication();
        }
        /* APP无效，继续执行Bootloader */
    }
    
    /* 第二步：Bootloader初始化 */
    USARTx_Conf();
    MyCAN_Init();
    Timer_Init();
    uds_init();
    Store_Init();
    uds_compute_key(seed,Key_seed,0x01);
    while(1)
    {
        if(MyCAN_ReceiveFlag() == SET)
        {
            MyCAN_Receive(&RxMsg);
            uds_recv_frame(RxMsg.StdId, RxMsg.Data, RxMsg.DLC);
        }
    }
}

/* 1ms进一次定时中断 */
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        uds_1ms_task();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

typedef void(*pFunction)(void);
#define APPLICATION_ADDRESS    (uint32_t)0x08004000 
pFunction JumpToApplication; 
uint32_t JumpAddress;
void BootJumpToApplication(void)
{
	__disable_irq();
	USART_DeInit(USART1);
	CAN_DeInit(CAN1);

	/* Jump to user Application */
	JumpAddress = *(__IO uint32_t*)(APPLICATION_ADDRESS + 4); 
	JumpToApplication = (pFunction)JumpAddress;
	
	uint32_t stack_top = *(__IO uint32_t*)APPLICATION_ADDRESS;
	__set_MSP(stack_top);  
	
	JumpToApplication();

	while(1);
}