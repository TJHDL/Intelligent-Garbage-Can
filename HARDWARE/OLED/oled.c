//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : Evk123
//  生成日期   : 2014-0101
//  最近修改   :
//  功能描述   : 0.69寸OLED 接口演示例程(STM32F103ZE系列IIC)
//              说明:
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PD6（SCL）
//              SDA   接PD7（SDA）
//              ----------------------------------------------------------------
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////�

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"
#include "myiic.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
/**********************************************








/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);			//write command
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Command);
    IIC_Wait_Ack();
    IIC_Stop();
}

/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x40);			//write data
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();
    IIC_Stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
    if(cmd)
    {

        Write_IIC_Data(dat);

    }
    else {
        Write_IIC_Command(dat);

    }


}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
    unsigned char m,n;
    for(m=0; m<8; m++)
    {
        OLED_WR_Byte(0xb0+m,0);		//page0-page1
        OLED_WR_Byte(0x00,0);		//low column start address
        OLED_WR_Byte(0x10,0);		//high column start address
        for(n=0; n<128; n++)
        {
            OLED_WR_Byte(fill_Data,1);
        }
    }
}


/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
    unsigned int m;
    for(; Del_50ms>0; Del_50ms--)
        for(m=6245; m>0; m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
    unsigned char j;
    while(Del_1ms--)
    {
        for(j=0; j<123; j++);
    }
}

//坐标设置

void OLED_Set_Pos(unsigned char x, unsigned char y)
{   OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD);
}
//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址
        for(n=0; n<128; n++)OLED_WR_Byte(0,OLED_DATA);
    } //更新显示
}
void OLED_On(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址
        for(n=0; n<128; n++)OLED_WR_Byte(1,OLED_DATA);
    } //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 16/12
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{
    unsigned char c=0,i=0;
    c=chr-' ';//得到偏移后的值
    if(x>Max_Column-1) {
        x=0;
        y=y+2;
    }
    if(Char_Size ==16)
    {
        OLED_Set_Pos(x,y);
        for(i=0; i<8; i++)
            OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
        OLED_Set_Pos(x,y+1);
        for(i=0; i<8; i++)
            OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
    }
    else {
        OLED_Set_Pos(x,y);
        for(i=0; i<6; i++)
            OLED_WR_Byte(F6x8[c][i],OLED_DATA);

    }
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}
//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{
    u8 t,temp;
    u8 enshow=0;
    for(t=0; t<len; t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
                continue;
            } else enshow=1;

        }
        OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2);
    }
}
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {   OLED_ShowChar(x,y,chr[j],Char_Size);
        x+=8;
        if(x>120) {
            x=0;
            y+=2;
        }
        j++;
    }
}
//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{
    u8 t,adder=0;
    OLED_Set_Pos(x,y);
    for(t=0; t<16; t++)
    {
        OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
        adder+=1;
    }
    OLED_Set_Pos(x,y+1);
    for(t=0; t<16; t++)
    {
        OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
        adder+=1;
    }
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{
    unsigned int j=0;
    unsigned char x,y;

    if(y1%8==0) y=y1/8;
    else y=y1/8+1;
    for(y=y0; y<y1; y++)
    {
        OLED_Set_Pos(x0,y);
        for(x=x0; x<x1; x++)
        {
            OLED_WR_Byte(BMP[j++],OLED_DATA);
        }
    }
}

//初始化SSD1306
void OLED_Init(void)
{


//    GPIO_InitTypeDef  GPIO_Initure;
//    __HAL_RCC_GPIOC_CLK_ENABLE();           	//开启GPIOC时钟
//    __HAL_RCC_GPIOD_CLK_ENABLE();           	//开启GPIOD时钟

//    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //使能PC,D,G端口时钟

//    GPIO_Initure.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_3|GPIO_PIN_8;	 //PD3,PD6推挽输出
//    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
//    GPIO_Initure.Speed = GPIO_SPEED_HIGH;//速度50MHz
//		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
//    //GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD3,6
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_11|GPIO_PIN_3|GPIO_PIN_8,GPIO_PIN_SET);
//    //GPIO_SetBits(GPIOD,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_3|GPIO_PIN_8);	//PD3,PD6 输出高

//#if OLED_MODE==1

//    GPIO_Initure.Pin =0xFF; //PC0~7 OUT推挽输出
//		HAL_GPIO_Init(GPIOC,&GPIO_Initure);
//    //GPIO_Init(GPIOC, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOC,0xFF,GPIO_PIN_SET);   
//		//GPIO_SetBits(GPIOC,0xFF); //PC0~7输出高

//    GPIO_Initure.Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //PG13,14,15 OUT推挽输出
//		HAL_GPIO_Init(GPIOC,&GPIO_Initure);
//		//GPIO_Init(GPIOG, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15,GPIO_PIN_SET);   
//	  //GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PG13,14,15 OUT  输出高

//#else
//    GPIO_Initure.Pin = GPIO_PIN_10|GPIO_PIN_11;				 //PC0,1 OUT推挽输出
//		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
//    //GPIO_Init(GPIOC, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10|GPIO_PIN_11,GPIO_PIN_SET);
//		//GPIO_SetBits(GPIOC,GPIO_PIN_0|GPIO_PIN_1);						 //PC0,1 OUT  输出高

//    GPIO_Initure.Pin = GPIO_PIN_15;				 //PG15 OUT推挽输出	  RST
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
//		//GPIO_Init(GPIOG, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
//		//GPIO_SetBits(GPIOG,GPIO_PIN_15);						 //PG15 OUT  输出高


//#endif

//		IIC_Init();
//    delay_ms(200);

//    OLED_WR_Byte(0xAE,OLED_CMD);//--display off
//    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
//    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
//    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address
//    OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
//    OLED_WR_Byte(0x81,OLED_CMD); // contract control
//    OLED_WR_Byte(0xFF,OLED_CMD);//--128
//    OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap
//    OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
//    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
//    OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
//    OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
//    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
//    OLED_WR_Byte(0x00,OLED_CMD);//

//    OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
//    OLED_WR_Byte(0x80,OLED_CMD);//

//    OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
//    OLED_WR_Byte(0x05,OLED_CMD);//

//    OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
//    OLED_WR_Byte(0xF1,OLED_CMD);//

//    OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
//    OLED_WR_Byte(0x12,OLED_CMD);//

//    OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
//    OLED_WR_Byte(0x30,OLED_CMD);//

//    OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
//    OLED_WR_Byte(0x14,OLED_CMD);//

//    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel

	IIC_Init();
	delay_ms(500);								//初始化之前的延时很重要！
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0x00,OLED_CMD);//设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);//设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);//设置起始行地址,集映射RAM显示起始行(0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//设置对比度控制寄存器
	OLED_WR_Byte(0xCF,OLED_CMD);//设置SEG输出电流亮度
	OLED_WR_Byte(0xA1,OLED_CMD);//段重定义设置,bit0:0,0->0;1,0->127; 0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//设置正常显示(设置显示方式;bit0:1,反相显示;0,正常显示	)
	OLED_WR_Byte(0xA8,OLED_CMD);//设置驱动路数 设置多路复用比(1比64)
	OLED_WR_Byte(0x3F,OLED_CMD);//1/64 duty(默认0X3F(1/64))
	OLED_WR_Byte(0xD3,OLED_CMD);//设置显示偏移位移映射RAM计数器(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xD5,OLED_CMD);//设置显示时钟分频比/振荡器频率
	OLED_WR_Byte(0x80,OLED_CMD);//设置分频比，设置时钟为100帧/秒
	OLED_WR_Byte(0xD9,OLED_CMD);//设置预充电周期
	OLED_WR_Byte(0xF1,OLED_CMD);//设置预充15个时钟，放电1个时钟([3:0],PHASE 1;[7:4],PHASE 2;)
	OLED_WR_Byte(0xDA,OLED_CMD);//设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD);//[5:4]配置
	OLED_WR_Byte(0xDB,OLED_CMD);//设置VCOMH 电压倍率
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM 释放电压([6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;)
	OLED_WR_Byte(0x20,OLED_CMD);//设置页面寻址模式(0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0x8D,OLED_CMD);//设置充电泵启用/禁用
	OLED_WR_Byte(0x14,OLED_CMD);//设置(0x10禁用,0x14启用)
	OLED_WR_Byte(0xA4,OLED_CMD);// 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏) (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// 设置显示方式;bit0:1,反相显示;0,正常显示 (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//开启显示	
	OLED_Clear();
	OLED_ShowCHinese(18,0,2);//智
	OLED_ShowCHinese(36,0,3);//能
	OLED_ShowCHinese(54,0,4);//垃
	OLED_ShowCHinese(72,0,5);//圾
	OLED_ShowCHinese(90,0,6);//桶
	OLED_ShowCHinese(38,3,28);//温度单位
	OLED_ShowString(93,3,"RH",16);//湿度单位
}



