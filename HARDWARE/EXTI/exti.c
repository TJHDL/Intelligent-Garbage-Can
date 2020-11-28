#include "exti.h"
#include "delay.h"
#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//外部中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/6/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
	  //__HAL_RCC_AFIO_CLK_ENABLE();                //开启AFIO时钟
    
    GPIO_Initure.Pin=GPIO_PIN_13;                //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING_FALLING;      //上升/下降沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //中断线8-PA8
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,1,0);       //抢占优先级为1，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //使能中断线13
    
}


void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);		//调用中断处理公用函数
}



//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(10);      	//消抖
    switch(GPIO_Pin)
    {
        case GPIO_PIN_13:
				LED6=!LED6;	//控制LED6暗亮
            break;	
    }
}
