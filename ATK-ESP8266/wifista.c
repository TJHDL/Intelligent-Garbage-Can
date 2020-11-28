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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#define ang_0 50     //0��
#define ang_dry 100  //45��
#define ang_wet 150  //90��
#define ang_rec 200  //135��
#define ang_har 250  //180��

const u8* wifi_ipaddress="192.168.0.102"; 	//ip��ַ

u16 garbage_pwmval(char garbage,int flag)  //4��������Ӧ4�ֲ�ͬ��pwmռ�ձ�,���趨��Ӧ��ռ�ձ�
{
	u16 pwmval;
	switch(garbage){
		case 'B':pwmval = ang_0; break;  //���ɻ�������
		case 'G':pwmval = ang_dry; break;  //������
		case 'S':pwmval = ang_wet; break;  //ʪ����
		case 'K':pwmval = ang_rec; break;  //�ɻ�������
		case 'Y':pwmval = ang_har; break;  //�к�����
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




//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 netpro=0;	//����ģʽ
u8 atk_8266_wifista_test(void)
{
  u16 pwmval=ang_0;   
  char garbage='Z';                  //��WIFIģ����յ�����������
	int flag=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);
	
	u16 len=0;
	u8 old_constate=0;
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	p=mymalloc(32);							  //����32�ֽ��ڴ�
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50); //����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);	  //DHCP�������ر�(��APģʽ��Ч) 
	
	delay_ms(1000); //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));	//����Ŀ��·����,���һ��IP
	

PRESTA:
  netpro=0;
	netpro|=atk_8266_netpro_sel();	//STAģʽѡ����ģʽ
	if(netpro&0X02)//UDPģʽ
	{
			/*  printf("\r\nUDPģʽ\r\n");
		    printf("\r\nKEY_UP:����\r\n");
		    printf("������UDP������IP��ַ(�˿�Ĭ��ʹ��:8086):\r\n"); */
		    USART_RX_STA=0;
		/*	while(1)   //IP����
			{
				if(USART_RX_STA&0x8000)
				{					   
					len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
					USART_RX_BUF[len]=0;
					printf("IP��ַΪ:%s\r\n",USART_RX_BUF);
					USART_RX_STA=0;
					break;
				}
				
				key=KEY_Scan(0);
			    if(key==WKUP_PRES)goto PRESTA;
				
				delay_ms(100);
				LED0=!LED0;
				
			}*/
			//sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",USART_RX_BUF,(u8*)portnum);    //����Ŀ��UDP������
			sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",wifi_ipaddress,(u8*)portnum);    //����Ŀ��UDP������
			delay_ms(200);
		    atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //������ģʽ
			delay_ms(200);
			while(atk_8266_send_cmd(p,"OK",500));
		
	}
	
	else //TCPģʽ
	{
		if(netpro&0X01) //TCP Client ͸��ģʽ����
		{
	        printf("\r\nTCP�ͻ���\r\n");
			printf("\r\nKEY_UP:����\r\n");
			printf("������TCP������IP��ַ(�˿�Ĭ��ʹ��:8086):\r\n"); //IP����
		    USART_RX_STA=0;
			while(1)   
			{
				if(USART_RX_STA&0x8000)
				{					   
					len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
					USART_RX_BUF[len]=0;
					printf("IP��ַΪ:%s\r\n",USART_RX_BUF);
					USART_RX_STA=0;
					break;
				}
				
				key=KEY_Scan(0);
				if(key==WKUP_PRES)goto PRESTA;
				
				delay_ms(100);
				LED0=!LED0;	
			}
			
			printf("�������ӷ�����,���Ե�...\r\n");
			
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);//0�������ӣ�1��������
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",USART_RX_BUF,(u8*)portnum);//����Ŀ��TCP������
			while(atk_8266_send_cmd(p,"OK",200))
			{
				    printf("\r\nKEY_UP:����\r\n");
				    printf("����TCP������ʧ��\r\n");//����ʧ��
				
					key=KEY_Scan(0);
					if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
		}
		else //TCP Server
		{
			
			    printf("\r\nTCP������\r\n");
			    printf("��������ATK-ESPģ��,���Ե�...\r\n");
				atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
				sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);  //����Serverģʽ(0���رգ�1����)���˿ں�Ϊportnum
				atk_8266_send_cmd(p,"OK",50);    
		}
	}
	

	printf("WK_UP:�˳�����  KEY0:��������\r\n");
	atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
	sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
	printf("%s\r\n",p);//��ʾIP��ַ�Ͷ˿�
	printf("״̬��%s\r\n",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro]);//����״̬
	USART2_RX_STA=0;
	

	
	while(1)
	{
		

		key=KEY_Scan(0);
		if(key==WKUP_PRES)//WK_UP �˳�����		 
		{ 
			printf("�˳�����\r\n");
			res=0;					
			atk_8266_quit_trans();	//�˳�͸��
			atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //�ر�͸��ģʽ
			break;												 
		}
		
		else if(key==KEY0_PRES)	//KEY0 �������� 
		{
		
			if((netpro==3)||(netpro==2)) //UDP
			{
				
				sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
				printf("\r\n%s",p);
				atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //����ָ�����ȵ�����
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
				timex=100;
			
			}
			else if((netpro==1))   //TCP Client
			{
				atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��      						
				sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
				printf("\r\n%s",p);
				u2_printf("%s",p);
				timex=100;
			}
			else    //TCP Server
			{
				sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
				printf("%s\r\n",p);
				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //����ָ�����ȵ�����
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
				timex=100;
			}
		}else;
	
		if(timex)timex--;
		t++;
		delay_ms(10);
		
		
		if(USART2_RX_STA&0X8000)		//���յ�һ��������
		{ 
			rlen=USART2_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
			USART2_RX_BUF[rlen]=0;		//��ӽ����� 
			printf("\r\n�յ�%d�ֽ�,��������\r\n",rlen);
			printf("%s\r\n",USART2_RX_BUF);	//���͵�����  
			
			
			if(USART2_RX_BUF[rlen-1]=='B')
			{
					LED_Init();                     //��ʼ��LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);//LED1��
			}
			if(USART2_RX_BUF[rlen-1]=='G')
			{
					LED_Init();                     //��ʼ��LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);//LED2��
			}
			if(USART2_RX_BUF[rlen-1]=='S')
			{
					LED_Init();                     //��ʼ��LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);//LED3��
			}
			if(USART2_RX_BUF[rlen-1]=='K')
			{
					LED_Init();                     //��ʼ��LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);//LED4��
			}
			if(USART2_RX_BUF[rlen-1]=='Y')
			{
					LED_Init();                     //��ʼ��LED
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);//LED5��
			}
			USART2_RX_STA=0;
			if(constate!='+')t=1000;	//״̬Ϊ��δ����,������������״̬
			else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
		}  
		
		/////////////////////////PWM///////////////////////
		garbage=USART2_RX_BUF[rlen-1];
		
		pwmval=garbage_pwmval(garbage,flag);
    TIM_SetTIM1Compare1(pwmval);		//�޸ıȽ�ֵ���޸�ռ�ձ�	  
		/////////////////////////PWM///////////////////////
		
		if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
		{

			constate=atk_8266_consta_check();//�õ�����״̬
			if(constate!=old_constate)
			{
				old_constate = constate;
				if(constate=='+') printf("���ӳɹ�\r\n");//����״̬
				else             printf("����ʧ��\r\n");

			}
			USART2_RX_STA=0;

			t=0;
		}
		if((t%20)==0)LED0=!LED0;
	}
	myfree(p);		//�ͷ��ڴ� 
	return res;		
} 



