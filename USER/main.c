#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart2.h"
#include "string.h"	 
#include "key.h"
#include "usart.h"
#include "common.h"
#include "timer.h"
#include "smg.h"
#include "dht11.h"
#include "beep.h"
#include "gpio.h"
#include "exti.h"
#include "pwm.h"
#include "myiic.h"
#include "oled.h"
/************************************************
 ALIENTEK NANO STM32������ ��չʵ��2
 ATK-ESP8266 WIFIģ�����ʵ��(STAģʽ)-HAL��汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int keep_num=0,full_flag=0;
float rate=1;
u8 smg_wei=0;//�����λѡ
u8 smg_duan=0;//����ܶ�ѡ
u16 t=0;

// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,ȫ��
u8 smg_num[]= {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00};
u8 num=0;//��ֵ
u16 led_t=0;//ledͳ��ʱ��
u16 keep_t1=0;//dht11����ʱ��
u16 keep_t2=499;//dht11����ʱ��
u8 temperature;//�¶�ֵ
u8 humidity;//ʪ��ֵ

int flag_echo_come = 1;
int time_a=0;
int echo_dist=0;
int test=0;
int a;
u8* p;
char buf1[5];
char buf2[5];

int main(void)
{
 	
  HAL_Init();                    	//��ʼ��HAL��    
  Stm32_Clock_Init(RCC_PLL_MUL9); //����ʱ��,72M
  delay_init(72);                 //��ʼ����ʱ����
	uart_init(115200);				//��ʼ������115200
  LED_Init();                     //��ʼ��LED 
	LED_SMG_Init();  //����ܳ�ʼ��	
	KEY_Init();						//��ʼ������
	USART2_Init(115200);            //��ʼ������2������Ϊ115200  
  mem_init();                     //�ڴ�����ʼ��
	Echo_Init();//������
	BEEP_Init();                    //��ʼ���������˿�
	GPIO_Init();          //��ʼ�������Ӧ����
	EXTI_Init();					//��ʼ���ⲿ�ж�
	OLED_Init();  //OLED��ʾ����ʼ��
	TIM1_PWM_Init(1999,719);			//PWMƵ��=72000000/(1999+1)/(719+1)=50hz 
	LED6=1;
	while(DHT11_Init());//�¶ȴ�����
	TIM2_Init(50,7199);//2ms��ʱ��ʾ
	TIM4_Init(999,7199);//
	atk_8266_test();	            //����ATK_ESP8266���� 

}



//�ص���������ʱ���жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	
	  if(htim==(&TIM2_Handler))
    {
				if(echo_dist<5)			
				{
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP�������ͣ��죬 ��ͬBEEP=0;
					LED_Write_Data(0xda,3);//��������	    
					LED_Write_Data(0x8e,4);//��������
					LED_Write_Data(0x7c,5);//��������
					LED_Write_Data(0x1c,6);//��������
					LED_Write_Data(0x1c,7);//��������
					LED_Write_Data(0x00,7);//��������
					LED_Refresh();//ˢ����ʾ
					
					if(keep_t1==0)
					{
						OLED_ShowString(38,6," FULL      ",16);//ʪ�ȵ�λ
						keep_t1=200;
					}
					keep_t1--;
				} 
		
		
				else//��ʾ�¶�
				{
					if(keep_t1>0)
					{
						keep_t1--;
						if(keep_t1==0)
							OLED_ShowString(38,6,"           ",16);//����
					}
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET); //BEEP�������ߣ����죬 ��ͬBEEP=1;
					LED_Write_Data(smg_num[temperature/10],2);
					LED_Write_Data(smg_num[temperature%10],3);
					LED_Write_Data(smg_num[humidity/10],6);
					LED_Write_Data(smg_num[humidity%10],7);
					LED_Write_Data(0x00,7);
					LED_Refresh();//ˢ����ʾ
					//OLED_ShowNum(18,3,temperature,2,16);
					//OLED_ShowNum(72,3,humidity,2,16);		
					keep_t2++;
					if(keep_t2==1000)
					{
						OLED_ShowNum(18,3,temperature,2,16);
						OLED_ShowNum(72,3,humidity,2,16);
						keep_t2=0;
					}
					
				}
		//FULL���¶���ʾ
		
	}
		
	  if(htim==(&TIM4_Handler))
    {
//        dht11_t++;
//			if(dht11_t==500)//DHT11 1S����
//			{ 
				DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��ֵ		
				//LED6����ʾ������ȡ��ʪ�ȣ���Ȼ�Ҳ�֪����ʲô��23333��
				LED6=0;	
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);	//��һ���ߵ�ƽ���� 20us
        delay_us(100);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);			
			  SysTick->LOAD = 0x134F80;
        SysTick->VAL = 0;
        flag_echo_come = 1;

        while(!((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0))//��Ϊ1��ѭ��
        {
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4))     //���Խ��ܵ������ź�
            {
                flag_echo_come = 1;
                break;
            }
        }
        if(flag_echo_come==1)
        {
            SysTick->LOAD = 0x23FFFF;
            SysTick->VAL = 0;
            while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)|| ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0))  //  ���ź�||������Ϊ1����ѭ��
            {
            }
            time_a = SysTick->VAL;
            echo_dist = (0x23FFFF - time_a)/4235.6; //��λ��cm
            SysTick->VAL = 0;
        }

			}

}




