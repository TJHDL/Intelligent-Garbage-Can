#include "timer.h"
#include "usart2.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//��ʱ���ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/26
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM4_Handler;      //��ʱ�����
TIM_HandleTypeDef TIM3_Handler;      //��ʱ����� 
TIM_HandleTypeDef TIM2_Handler;      //��ʱ�����

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!

void TIM4_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;                          //ͨ�ö�ʱ��4
    TIM4_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM4_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
		TIM4_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;//ʹ���Զ�����
    HAL_TIM_Base_Init(&TIM4_Handler);
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //ʹ�ܶ�ʱ��4�Ͷ�ʱ��4�����жϣ�TIM_IT_UPDATE   
}


void TIM3_Init(u16 arr,u16 psc)
{  
	__HAL_RCC_TIM3_CLK_ENABLE(); 
	
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
		TIM3_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;//ʹ���Զ�����
    HAL_TIM_Base_Init(&TIM3_Handler);
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE 
	
	HAL_NVIC_SetPriority(TIM3_IRQn,0,1);    //�����ж����ȼ�����ռ���ȼ�0�������ȼ�1
	HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   

  
}

void TIM2_Init(u16 arr,u16 psc)
{  
    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��2
    TIM2_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM2_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
		TIM2_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;//ʹ���Զ�����
    HAL_TIM_Base_Init(&TIM2_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //ʹ�ܶ�ʱ��2�Ͷ�ʱ��2�����жϣ�TIM_IT_UPDATE   
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	
	if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM4_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM3�ж�   
	}
	
	if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,0,1);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}
	
	if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM2ʱ��
		HAL_NVIC_SetPriority(TIM2_IRQn,0,2);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�2
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����ITM2�ж�   
	}
	
}



//��ʱ��3�жϷ�����

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM4_Handler);
}

void TIM3_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&TIM3_Handler,TIM_FLAG_UPDATE)!=RESET)    //�����ж�
    {
        __HAL_TIM_CLEAR_IT(&TIM3_Handler,TIM_IT_UPDATE);            //����ж�
        USART2_RX_STA|=1<<15;	                                    //��ǽ������
        __HAL_TIM_DISABLE(&TIM3_Handler);                           //�رն�ʱ��7 
    }	
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM2_Handler);
}

