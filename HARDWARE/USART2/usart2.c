#include "sys.h"
#include "usart2.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h" 
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口2初始化代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2018/6/26
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved  
////////////////////////////////////////////////////////////////////////////////// 


UART_HandleTypeDef UART2_Handler; //UART2句柄

//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	u8 res;
	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        HAL_UART_Receive(&UART2_Handler,&res,1,1000); 
		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
			{
                __HAL_TIM_SET_COUNTER(&TIM3_Handler,0);	//计数器清空			
				if(USART2_RX_STA==0) 				//使能定时器3的中断 
				{
                    __HAL_TIM_ENABLE(&TIM3_Handler); //使能定时器3
				}
				USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART2_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
    }         													 
}   
//初始化IO 串口2
//bound:波特率 
void USART2_Init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_Initure;
	       
    //UART2 初始化设置
    __HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
    __HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
    GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA2

	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //浮空输入
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA3

	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART2
    
    __HAL_UART_ENABLE_IT(&UART2_Handler,UART_IT_RXNE);  //开启接收中断
	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //使能USART2中断通道
	HAL_NVIC_SetPriority(USART2_IRQn,0,2);			    //组2，优先级0,2,最高优先级 
 
	TIM3_Init(99,7199);	                //10ms中断一次
    __HAL_TIM_DISABLE(&TIM3_Handler);   //关闭定时器3 
	USART2_RX_STA=0;				    //清零 
}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
		while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
		USART2->DR=USART2_TX_BUF[j];  
	}
}
