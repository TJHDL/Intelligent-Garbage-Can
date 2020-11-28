#include "sys.h"
#include "usart2.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h" 
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����2��ʼ������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2018/6/26
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved  
////////////////////////////////////////////////////////////////////////////////// 


UART_HandleTypeDef UART2_Handler; //UART2���

//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	u8 res;
	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
        HAL_UART_Receive(&UART2_Handler,&res,1,1000); 
		if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//�����Խ�������
			{
                __HAL_TIM_SET_COUNTER(&TIM3_Handler,0);	//���������			
				if(USART2_RX_STA==0) 				//ʹ�ܶ�ʱ��3���ж� 
				{
                    __HAL_TIM_ENABLE(&TIM3_Handler); //ʹ�ܶ�ʱ��3
				}
				USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else 
			{
				USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
    }         													 
}   
//��ʼ��IO ����2
//bound:������ 
void USART2_Init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_Initure;
	       
    //UART2 ��ʼ������
    __HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
    __HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
    GPIO_Initure.Pull=GPIO_PULLUP;			//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA2

	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //��������
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA3

	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART2
    
    __HAL_UART_ENABLE_IT(&UART2_Handler,UART_IT_RXNE);  //���������ж�
	HAL_NVIC_EnableIRQ(USART2_IRQn);				    //ʹ��USART2�ж�ͨ��
	HAL_NVIC_SetPriority(USART2_IRQn,0,2);			    //��2�����ȼ�0,2,������ȼ� 
 
	TIM3_Init(99,7199);	                //10ms�ж�һ��
    __HAL_TIM_DISABLE(&TIM3_Handler);   //�رն�ʱ��3 
	USART2_RX_STA=0;				    //���� 
}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
		while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
		USART2->DR=USART2_TX_BUF[j];  
	}
}
