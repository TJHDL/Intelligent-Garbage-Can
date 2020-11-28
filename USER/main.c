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
 ALIENTEK NANO STM32¿ª·¢°å À©Õ¹ÊµÑé2
 ATK-ESP8266 WIFIÄ£¿é²âÊÔÊµÑé(STAÄ£Ê½)-HAL¿â°æ±¾
 ¼¼ÊõÖ§³Ö£ºwww.openedv.com
 ÌÔ±¦µêÆÌ£ºhttp://eboard.taobao.com 
 ¹Ø×¢Î¢ĞÅ¹«ÖÚÆ½Ì¨Î¢ĞÅºÅ£º"ÕıµãÔ­×Ó"£¬Ãâ·Ñ»ñÈ¡STM32×ÊÁÏ¡£
 ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾  
 ×÷Õß£ºÕıµãÔ­×Ó @ALIENTEK
************************************************/

int keep_num=0,full_flag=0;
float rate=1;
u8 smg_wei=0;//ÊıÂë¹ÜÎ»Ñ¡
u8 smg_duan=0;//ÊıÂë¹Ü¶ÎÑ¡
u16 t=0;

// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,È«Ãğ
u8 smg_num[]= {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00};
u8 num=0;//ÊıÖµ
u16 led_t=0;//ledÍ³¼ÆÊ±¼ä
u16 keep_t1=0;//dht11²ÉÑùÊ±¼ä
u16 keep_t2=499;//dht11²ÉÑùÊ±¼ä
u8 temperature;//ÎÂ¶ÈÖµ
u8 humidity;//Êª¶ÈÖµ

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
 	
  HAL_Init();                    	//³õÊ¼»¯HAL¿â    
  Stm32_Clock_Init(RCC_PLL_MUL9); //ÉèÖÃÊ±ÖÓ,72M
  delay_init(72);                 //³õÊ¼»¯ÑÓÊ±º¯Êı
	uart_init(115200);				//³õÊ¼»¯´®¿Ú115200
  LED_Init();                     //³õÊ¼»¯LED 
	LED_SMG_Init();  //ÊıÂë¹Ü³õÊ¼»¯	
	KEY_Init();						//³õÊ¼»¯°´¼ü
	USART2_Init(115200);            //³õÊ¼»¯´®¿Ú2²¨ÌØÂÊÎª115200  
  mem_init();                     //ÄÚ´æ¹ÜÀí³õÊ¼»¯
	Echo_Init();//³¬Éù²¨
	BEEP_Init();                    //³õÊ¼»¯·äÃùÆ÷¶Ë¿Ú
	GPIO_Init();          //³õÊ¼»¯ÈËÌå¸ĞÓ¦ÊäÈë
	EXTI_Init();					//³õÊ¼»¯Íâ²¿ÖĞ¶Ï
	OLED_Init();  //OLEDÏÔÊ¾ÆÁ³õÊ¼»¯
	TIM1_PWM_Init(1999,719);			//PWMÆµÂÊ=72000000/(1999+1)/(719+1)=50hz 
	LED6=1;
	while(DHT11_Init());//ÎÂ¶È´«¸ĞÆ÷
	TIM2_Init(50,7199);//2ms¶¨Ê±ÏÔÊ¾
	TIM4_Init(999,7199);//
	atk_8266_test();	            //½øÈëATK_ESP8266²âÊÔ 

}



//»Øµ÷º¯Êı£¬¶¨Ê±Æ÷ÖĞ¶Ï·şÎñº¯Êıµ÷ÓÃ
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	
	  if(htim==(&TIM2_Handler))
    {
				if(echo_dist<5)			
				{
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEPÒı½ÅÀ­µÍ£¬Ïì£¬ µÈÍ¬BEEP=0;
					LED_Write_Data(0xda,3);//ÊıÂë¹ÜÏÔÊ	    
					LED_Write_Data(0x8e,4);//ÊıÂë¹ÜÏÔÊ
					LED_Write_Data(0x7c,5);//ÊıÂë¹ÜÏÔÊ
					LED_Write_Data(0x1c,6);//ÊıÂë¹ÜÏÔÊ
					LED_Write_Data(0x1c,7);//ÊıÂë¹ÜÏÔÊ
					LED_Write_Data(0x00,7);//ÊıÂë¹ÜÏÔÊ
					LED_Refresh();//Ë¢ĞÂÏÔÊ¾
					
					if(keep_t1==0)
					{
						OLED_ShowString(38,6," FULL      ",16);//Êª¶Èµ¥Î»
						keep_t1=200;
					}
					keep_t1--;
				} 
		
		
				else//ÏÔÊ¾ÎÂ¶È
				{
					if(keep_t1>0)
					{
						keep_t1--;
						if(keep_t1==0)
							OLED_ShowString(38,6,"           ",16);//Ïû³ı
					}
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET); //BEEPÒı½ÅÀ­¸ß£¬²»Ïì£¬ µÈÍ¬BEEP=1;
					LED_Write_Data(smg_num[temperature/10],2);
					LED_Write_Data(smg_num[temperature%10],3);
					LED_Write_Data(smg_num[humidity/10],6);
					LED_Write_Data(smg_num[humidity%10],7);
					LED_Write_Data(0x00,7);
					LED_Refresh();//Ë¢ĞÂÏÔÊ¾
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
		//FULL¼°ÎÂ¶ÈÏÔÊ¾
		
	}
		
	  if(htim==(&TIM4_Handler))
    {
//        dht11_t++;
//			if(dht11_t==500)//DHT11 1S²ÉÑù
//			{ 
				DHT11_Read_Data(&temperature,&humidity);//¶ÁÈ¡ÎÂÊª¶ÈÖµ		
				//LED6ÁÁ±íÊ¾Õı³£¶ÁÈ¡ÎÂÊª¶È£¨ËäÈ»ÎÒ²»ÖªµÀÓĞÊ²Ã´ÓÃ23333£©
				LED6=0;	
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);	//¸øÒ»¸ö¸ßµçÆ½´¥·¢ 20us
        delay_us(100);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);			
			  SysTick->LOAD = 0x134F80;
        SysTick->VAL = 0;
        flag_echo_come = 1;

        while(!((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0))//¶¼Îª1Ôò²»Ñ­»·
        {
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4))     //¿ÉÒÔ½ÓÊÜµ½»ØÉùĞÅºÅ
            {
                flag_echo_come = 1;
                break;
            }
        }
        if(flag_echo_come==1)
        {
            SysTick->LOAD = 0x23FFFF;
            SysTick->VAL = 0;
            while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)|| ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0))  //  ÓĞĞÅºÅ||Á½¸ö¶¼Îª1£¬ÔòÑ­»·
            {
            }
            time_a = SysTick->VAL;
            echo_dist = (0x23FFFF - time_a)/4235.6; //µ¥Î»ÊÇcm
            SysTick->VAL = 0;
        }

			}

}




