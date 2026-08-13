/*
 * Timer.c
 *
 *  Created on: Apr 24, 2024
 *      Author: 86131
 */
#include "Timer.h"

void Timer_Init(void)
{
    /* 配置通用定时器2的时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 配置为内部时钟模式 */
    TIM_InternalClockConfig(TIM2);

    /* 定时器初始化配置，根据系统时钟频率的不同来设置 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//定时器时钟分频，这里不分频，直接使用总线时钟频率
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;//自动重装载值   1000 ticks = 1ms
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;//预分频系数    72MHz / 72 = 1MHz
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//重复计数器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /* 手动清除更新中断标志位 ，避免刚初始化完就进中断函数*/
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    /* 使能中断输出信号 */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//使能更新中断

    /* 中断分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* 中断控制器配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能定时器 当产生更新时，就会触发中断 */
    TIM_Cmd(TIM2, ENABLE);
}

/* 定时器2中断函数
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
     
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除中断更新标志位 
    }
}
*/
