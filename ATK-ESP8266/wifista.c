#include "common.h"
#include "stdlib.h"
#include "led.h"
#include "usart2.h"
#include "string.h"	 
#include "key.h"
#include "usart.h"
#include "myiic.h"
#include "oled.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#define ang_0 50     //0°
#define ang_dry 100  //45°
#define ang_wet 150  //90°
#define ang_rec 200  //135°
#define ang_har 250  //180°

const u8* wifi_ipaddress="192.168.0.102"; 	//ip地址

u16 garbage_pwmval(char garbage,int flag)  //4种垃圾对应4种不同的pwm占空比,并设定对应的占空比
{
	u16 pwmval;
	switch(garbage){
		case 'B':pwmval = ang_0; break;  //不可回收垃圾
		case 'G':pwmval = ang_dry; break;  //干垃圾
		case 'S':pwmval = ang_wet; break;  //湿垃圾
		case 'K':pwmval = ang_rec; break;  //可回收垃圾
		case 'Y':pwmval = ang_har; break;  //有害垃圾
		default:pwmval=ang_0;break;
	}
	
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) != flag)
	{
		pwmval=ang_0;
		garbage='Z';
		flag=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);
	}
	
	return pwmval;
}




//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 netpro=0;	//网络模式
u8 atk_8266_wifista_test(void)
{
  u16 pwmval=ang_0;   
  char garbage='Z';                  //从WIFI模块接收到的垃圾种类
	int flag=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);
	
	u16 len=0;
	u8 old_constate=0;
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	p=mymalloc(32);							  //申请32字节内存
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50); //设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);	  //DHCP服务器关闭(仅AP模式有效) 
	
	delay_ms(1000); //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));	//连接目标路由器,并且获得IP
	

PRESTA:
  netpro=0;
	netpro|=atk_8266_netpro_sel();	//STA模式选择工作模式
	if(netpro&0X02)//UDP模式
	{
			/*  printf("\r\nUDP模式\r\n");
		    printf("\r\nKEY_UP:返回\r\n");
		    printf("请输入UDP服务器IP地址(端口默认使用:8086):\r\n"); */
		    USART_RX_STA=0;
		/*	while(1)   //IP输入
			{
				if(USART_RX_STA&0x8000)
				{					   
					len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
					USART_RX_BUF[len]=0;
					printf("IP地址为:%s\r\n",USART_RX_BUF);
					USART_RX_STA=0;
					break;
				}
				
				key=KEY_Scan(0);
			    if(key==WKUP_PRES)goto PRESTA;
				
				delay_ms(100);
				LED0=!LED0;
				
			}*/
			//sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",USART_RX_BUF,(u8*)portnum);    //配置目标UDP服务器
			sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",wifi_ipaddress,(u8*)portnum);    //配置目标UDP服务器
			delay_ms(200);
		    atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //单链接模式
			delay_ms(200);
			while(atk_8266_send_cmd(p,"OK",500));
		
	}
	
	else //TCP模式
	{
		if(netpro&0X01) //TCP Client 透传模式测试
		{
	        printf("\r\nTCP客户端\r\n");
			printf("\r\nKEY_UP:返回\r\n");
			printf("请输入TCP服务器IP地址(端口默认使用:8086):\r\n"); //IP输入
		    USART_RX_STA=0;
			while(1)   
			{
				if(USART_RX_STA&0x8000)
				{					   
					len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
					USART_RX_BUF[len]=0;
					printf("IP地址为:%s\r\n",USART_RX_BUF);
					USART_RX_STA=0;
					break;
				}
				
				key=KEY_Scan(0);
				if(key==WKUP_PRES)goto PRESTA;
				
				delay_ms(100);
				LED0=!LED0;	
			}
			
			printf("正在连接服务器,请稍等...\r\n");
			
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);//0：单连接，1：多连接
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",USART_RX_BUF,(u8*)portnum);//配置目标TCP服务器
			while(atk_8266_send_cmd(p,"OK",200))
			{
				    printf("\r\nKEY_UP:返回\r\n");
				    printf("连接TCP服务器失败\r\n");//连接失败
				
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			
		}
		else //TCP Server
		{
			
			    printf("\r\nTCP服务器\r\n");
			    printf("正在配置ATK-ESP模块,请稍等...\r\n");
				atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
				sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);  //开启Server模式(0，关闭；1，打开)，端口号为portnum
				atk_8266_send_cmd(p,"OK",50);    
		}
	}
	

	printf("WK_UP:退出测试  KEY0:发送数据\r\n");
	atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
	sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
	printf("%s\r\n",p);//显示IP地址和端口
	printf("状态：%s\r\n",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro]);//连接状态
	USART2_RX_STA=0;
	

	
	while(1)
	{
		

		key=KEY_Scan(0);
		if(key==WKUP_PRES)//WK_UP 退出测试		 
		{ 
			printf("退出测试\r\n");
			res=0;					
			atk_8266_quit_trans();	//退出透传
			atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
			break;												 
		}
		
		else if(key==KEY0_PRES)	//KEY0 发送数据 
		{
		
			if((netpro==3)||(netpro==2)) //UDP
			{
				
				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				printf("\r\n%s",p);
				atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
				timex=100;
			
			}
			else if((netpro==1))   //TCP Client
			{
				atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传      						
				sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				printf("\r\n%s",p);
				u2_printf("%s",p);
				timex=100;
			}
			else    //TCP Server
			{
				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				printf("%s\r\n",p);
				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
				timex=100;
			}
		}else;
	
		if(timex)timex--;
		t++;
		delay_ms(10);
		
		
		if(USART2_RX_STA&0X8000)		//接收到一次数据了
		{ 
			rlen=USART2_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART2_RX_BUF[rlen]=0;		//添加结束符 
			printf("\r\n收到%d字节,内容如下\r\n",rlen);
			printf("%s\r\n",USART2_RX_BUF);	//发送到串口  
			
			
			if(USART2_RX_BUF[rlen-1]=='B')
			{
					LED_Init();                     //初始化LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);//LED1亮
			}
			if(USART2_RX_BUF[rlen-1]=='G')
			{
					LED_Init();                     //初始化LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);//LED2亮
			}
			if(USART2_RX_BUF[rlen-1]=='S')
			{
					LED_Init();                     //初始化LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);//LED3亮
			}
			if(USART2_RX_BUF[rlen-1]=='K')
			{
					LED_Init();                     //初始化LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);//LED4亮
			}
			if(USART2_RX_BUF[rlen-1]=='Y')
			{
					LED_Init();                     //初始化LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);//LED5亮
			}
			USART2_RX_STA=0;
			if(constate!='+')t=1000;	//状态为还未连接,立即更新连接状态
			else t=0;                   //状态为已经连接了,10秒后再检查
		}  
		
		/////////////////////////PWM///////////////////////
		garbage=USART2_RX_BUF[rlen-1];
		
		pwmval=garbage_pwmval(garbage,flag);
    TIM_SetTIM1Compare1(pwmval);		//修改比较值，修改占空比	  
		/////////////////////////PWM///////////////////////
		
		if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
		{

			constate=atk_8266_consta_check();//得到连接状态
			if(constate!=old_constate)
			{
				old_constate = constate;
				if(constate=='+') printf("连接成功\r\n");//连接状态
				else             printf("连接失败\r\n");

			}
			USART2_RX_STA=0;

			t=0;
		}
		if((t%20)==0)LED0=!LED0;
	}
	myfree(p);		//释放内存 
	return res;		
} 



