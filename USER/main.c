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
 ALIENTEK NANO STM32开发板 扩展实验2
 ATK-ESP8266 WIFI模块测试实验(STA模式)-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int keep_num=0,full_flag=0;
float rate=1;
u8 smg_wei=0;//数码管位选
u8 smg_duan=0;//数码管段选
u16 t=0;

// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F, .,全灭
u8 smg_num[]= {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x01,0x00};
u8 num=0;//数值
u16 led_t=0;//led统计时间
u16 keep_t1=0;//dht11采样时间
u16 keep_t2=499;//dht11采样时间
u8 temperature;//温度值
u8 humidity;//湿度值

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
 	
  HAL_Init();                    	//初始化HAL库    
  Stm32_Clock_Init(RCC_PLL_MUL9); //设置时钟,72M
  delay_init(72);                 //初始化延时函数
	uart_init(115200);				//初始化串口115200
  LED_Init();                     //初始化LED 
	LED_SMG_Init();  //数码管初始化	
	KEY_Init();						//初始化按键
	USART2_Init(115200);            //初始化串口2波特率为115200  
  mem_init();                     //内存管理初始化
	Echo_Init();//超声波
	BEEP_Init();                    //初始化蜂鸣器端口
	GPIO_Init();          //初始化人体感应输入
	EXTI_Init();					//初始化外部中断
	OLED_Init();  //OLED显示屏初始化
	TIM1_PWM_Init(1999,719);			//PWM频率=72000000/(1999+1)/(719+1)=50hz 
	LED6=1;
	while(DHT11_Init());//温度传感器
	TIM2_Init(50,7199);//2ms定时显示
	TIM4_Init(999,7199);//
	atk_8266_test();	            //进入ATK_ESP8266测试 

}



//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	
	  if(htim==(&TIM2_Handler))
    {
				if(echo_dist<5)			
				{
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); //BEEP引脚拉低，响， 等同BEEP=0;
					LED_Write_Data(0xda,3);//数码管显�	    
					LED_Write_Data(0x8e,4);//数码管显�
					LED_Write_Data(0x7c,5);//数码管显�
					LED_Write_Data(0x1c,6);//数码管显�
					LED_Write_Data(0x1c,7);//数码管显�
					LED_Write_Data(0x00,7);//数码管显�
					LED_Refresh();//刷新显示
					
					if(keep_t1==0)
					{
						OLED_ShowString(38,6," FULL      ",16);//湿度单位
						keep_t1=200;
					}
					keep_t1--;
				} 
		
		
				else//显示温度
				{
					if(keep_t1>0)
					{
						keep_t1--;
						if(keep_t1==0)
							OLED_ShowString(38,6,"           ",16);//消除
					}
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET); //BEEP引脚拉高，不响， 等同BEEP=1;
					LED_Write_Data(smg_num[temperature/10],2);
					LED_Write_Data(smg_num[temperature%10],3);
					LED_Write_Data(smg_num[humidity/10],6);
					LED_Write_Data(smg_num[humidity%10],7);
					LED_Write_Data(0x00,7);
					LED_Refresh();//刷新显示
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
		//FULL及温度显示
		
	}
		
	  if(htim==(&TIM4_Handler))
    {
//        dht11_t++;
//			if(dht11_t==500)//DHT11 1S采样
//			{ 
				DHT11_Read_Data(&temperature,&humidity);//读取温湿度值		
				//LED6亮表示正常读取温湿度（虽然我不知道有什么用23333）
				LED6=0;	
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);	//给一个高电平触发 20us
        delay_us(100);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);			
			  SysTick->LOAD = 0x134F80;
        SysTick->VAL = 0;
        flag_echo_come = 1;

        while(!((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0))//都为1则不循环
        {
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4))     //可以接受到回声信号
            {
                flag_echo_come = 1;
                break;
            }
        }
        if(flag_echo_come==1)
        {
            SysTick->LOAD = 0x23FFFF;
            SysTick->VAL = 0;
            while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)|| ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0))  //  有信号||两个都为1，则循环
            {
            }
            time_a = SysTick->VAL;
            echo_dist = (0x23FFFF - time_a)/4235.6; //单位是cm
            SysTick->VAL = 0;
        }

			}

}




