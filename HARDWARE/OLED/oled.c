//////////////////////////////////////////////////////////////////////////////////
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ÖĞ¾°Ô°µç×Ó
//µêÆÌµØÖ·£ºhttp://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  ÎÄ ¼ş Ãû   : main.c
//  °æ ±¾ ºÅ   : v2.0
//  ×÷    Õß   : Evk123
//  Éú³ÉÈÕÆÚ   : 2014-0101
//  ×î½üĞŞ¸Ä   :
//  ¹¦ÄÜÃèÊö   : 0.69´çOLED ½Ó¿ÚÑİÊ¾Àı³Ì(STM32F103ZEÏµÁĞIIC)
//              ËµÃ÷:
//              ----------------------------------------------------------------
//              GND   µçÔ´µØ
//              VCC   ½Ó5V»ò3.3vµçÔ´
//              SCL   ½ÓPD6£¨SCL£©
//              SDA   ½ÓPD7£¨SDA£©
//              ----------------------------------------------------------------
//Copyright(C) ÖĞ¾°Ô°µç×Ó2014/3/16
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////£

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"
#include "myiic.h"
//OLEDµÄÏÔ´æ
//´æ·Å¸ñÊ½ÈçÏÂ.
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

//×ø±êÉèÖÃ

void OLED_Set_Pos(unsigned char x, unsigned char y)
{   OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD);
}
//¿ªÆôOLEDÏÔÊ¾
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDCÃüÁî
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//¹Ø±ÕOLEDÏÔÊ¾
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDCÃüÁî
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
//ÇåÆÁº¯Êı,ÇåÍêÆÁ,Õû¸öÆÁÄ»ÊÇºÚÉ«µÄ!ºÍÃ»µãÁÁÒ»Ñù!!!
void OLED_Clear(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //ÉèÖÃÒ³µØÖ·£¨0~7£©
        OLED_WR_Byte (0x00,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞµÍµØÖ·
        OLED_WR_Byte (0x10,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞ¸ßµØÖ·
        for(n=0; n<128; n++)OLED_WR_Byte(0,OLED_DATA);
    } //¸üĞÂÏÔÊ¾
}
void OLED_On(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //ÉèÖÃÒ³µØÖ·£¨0~7£©
        OLED_WR_Byte (0x00,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞµÍµØÖ·
        OLED_WR_Byte (0x10,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞ¸ßµØÖ·
        for(n=0; n<128; n++)OLED_WR_Byte(1,OLED_DATA);
    } //¸üĞÂÏÔÊ¾
}
//ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û,°üÀ¨²¿·Ö×Ö·û
//x:0~127
//y:0~63
//mode:0,·´°×ÏÔÊ¾;1,Õı³£ÏÔÊ¾
//size:Ñ¡Ôñ×ÖÌå 16/12
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{
    unsigned char c=0,i=0;
    c=chr-' ';//µÃµ½Æ«ÒÆºóµÄÖµ
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
//m^nº¯Êı
u32 oled_pow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}
//ÏÔÊ¾2¸öÊı×Ö
//x,y :Æğµã×ø±ê
//len :Êı×ÖµÄÎ»Êı
//size:×ÖÌå´óĞ¡
//mode:Ä£Ê½	0,Ìî³äÄ£Ê½;1,µş¼ÓÄ£Ê½
//num:ÊıÖµ(0~4294967295);
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
//ÏÔÊ¾Ò»¸ö×Ö·ûºÅ´®
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
//ÏÔÊ¾ºº×Ö
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
/***********¹¦ÄÜÃèÊö£ºÏÔÊ¾ÏÔÊ¾BMPÍ¼Æ¬128¡Á64ÆğÊ¼µã×ø±ê(x,y),xµÄ·¶Î§0¡«127£¬yÎªÒ³µÄ·¶Î§0¡«7*****************/
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

//³õÊ¼»¯SSD1306
void OLED_Init(void)
{


//    GPIO_InitTypeDef  GPIO_Initure;
//    __HAL_RCC_GPIOC_CLK_ENABLE();           	//¿ªÆôGPIOCÊ±ÖÓ
//    __HAL_RCC_GPIOD_CLK_ENABLE();           	//¿ªÆôGPIODÊ±ÖÓ

//    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //Ê¹ÄÜPC,D,G¶Ë¿ÚÊ±ÖÓ

//    GPIO_Initure.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_3|GPIO_PIN_8;	 //PD3,PD6ÍÆÍìÊä³ö
//    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 		 //ÍÆÍìÊä³ö
//    GPIO_Initure.Speed = GPIO_SPEED_HIGH;//ËÙ¶È50MHz
//		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
//    //GPIO_Init(GPIOD, &GPIO_InitStructure);	  //³õÊ¼»¯GPIOD3,6
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_11|GPIO_PIN_3|GPIO_PIN_8,GPIO_PIN_SET);
//    //GPIO_SetBits(GPIOD,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_3|GPIO_PIN_8);	//PD3,PD6 Êä³ö¸ß

//#if OLED_MODE==1

//    GPIO_Initure.Pin =0xFF; //PC0~7 OUTÍÆÍìÊä³ö
//		HAL_GPIO_Init(GPIOC,&GPIO_Initure);
//    //GPIO_Init(GPIOC, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOC,0xFF,GPIO_PIN_SET);   
//		//GPIO_SetBits(GPIOC,0xFF); //PC0~7Êä³ö¸ß

//    GPIO_Initure.Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //PG13,14,15 OUTÍÆÍìÊä³ö
//		HAL_GPIO_Init(GPIOC,&GPIO_Initure);
//		//GPIO_Init(GPIOG, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15,GPIO_PIN_SET);   
//	  //GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PG13,14,15 OUT  Êä³ö¸ß

//#else
//    GPIO_Initure.Pin = GPIO_PIN_10|GPIO_PIN_11;				 //PC0,1 OUTÍÆÍìÊä³ö
//		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
//    //GPIO_Init(GPIOC, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10|GPIO_PIN_11,GPIO_PIN_SET);
//		//GPIO_SetBits(GPIOC,GPIO_PIN_0|GPIO_PIN_1);						 //PC0,1 OUT  Êä³ö¸ß

//    GPIO_Initure.Pin = GPIO_PIN_15;				 //PG15 OUTÍÆÍìÊä³ö	  RST
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
//		//GPIO_Init(GPIOG, &GPIO_InitStructure);
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
//		//GPIO_SetBits(GPIOG,GPIO_PIN_15);						 //PG15 OUT  Êä³ö¸ß


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
	delay_ms(500);								//³õÊ¼»¯Ö®Ç°µÄÑÓÊ±ºÜÖØÒª£¡
	OLED_WR_Byte(0xAE,OLED_CMD); //¹Ø±ÕÏÔÊ¾
	OLED_WR_Byte(0x00,OLED_CMD);//ÉèÖÃµÍÁĞµØÖ·
	OLED_WR_Byte(0x10,OLED_CMD);//ÉèÖÃ¸ßÁĞµØÖ·
	OLED_WR_Byte(0x40,OLED_CMD);//ÉèÖÃÆğÊ¼ĞĞµØÖ·,¼¯Ó³ÉäRAMÏÔÊ¾ÆğÊ¼ĞĞ(0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//ÉèÖÃ¶Ô±È¶È¿ØÖÆ¼Ä´æÆ÷
	OLED_WR_Byte(0xCF,OLED_CMD);//ÉèÖÃSEGÊä³öµçÁ÷ÁÁ¶È
	OLED_WR_Byte(0xA1,OLED_CMD);//¶ÎÖØ¶¨ÒåÉèÖÃ,bit0:0,0->0;1,0->127; 0xa0×óÓÒ·´ÖÃ 0xa1Õı³£
	OLED_WR_Byte(0xC8,OLED_CMD);//ÉèÖÃCOMÉ¨Ãè·½Ïò;bit3:0,ÆÕÍ¨Ä£Ê½;1,ÖØ¶¨ÒåÄ£Ê½ COM[N-1]->COM0;N:Çı¶¯Â·Êı   0xc0ÉÏÏÂ·´ÖÃ 0xc8Õı³£
	OLED_WR_Byte(0xA6,OLED_CMD);//ÉèÖÃÕı³£ÏÔÊ¾(ÉèÖÃÏÔÊ¾·½Ê½;bit0:1,·´ÏàÏÔÊ¾;0,Õı³£ÏÔÊ¾	)
	OLED_WR_Byte(0xA8,OLED_CMD);//ÉèÖÃÇı¶¯Â·Êı ÉèÖÃ¶àÂ·¸´ÓÃ±È(1±È64)
	OLED_WR_Byte(0x3F,OLED_CMD);//1/64 duty(Ä¬ÈÏ0X3F(1/64))
	OLED_WR_Byte(0xD3,OLED_CMD);//ÉèÖÃÏÔÊ¾Æ«ÒÆÎ»ÒÆÓ³ÉäRAM¼ÆÊıÆ÷(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xD5,OLED_CMD);//ÉèÖÃÏÔÊ¾Ê±ÖÓ·ÖÆµ±È/Õñµ´Æ÷ÆµÂÊ
	OLED_WR_Byte(0x80,OLED_CMD);//ÉèÖÃ·ÖÆµ±È£¬ÉèÖÃÊ±ÖÓÎª100Ö¡/Ãë
	OLED_WR_Byte(0xD9,OLED_CMD);//ÉèÖÃÔ¤³äµçÖÜÆÚ
	OLED_WR_Byte(0xF1,OLED_CMD);//ÉèÖÃÔ¤³ä15¸öÊ±ÖÓ£¬·Åµç1¸öÊ±ÖÓ([3:0],PHASE 1;[7:4],PHASE 2;)
	OLED_WR_Byte(0xDA,OLED_CMD);//ÉèÖÃCOMÓ²¼şÒı½ÅÅäÖÃ
	OLED_WR_Byte(0x12,OLED_CMD);//[5:4]ÅäÖÃ
	OLED_WR_Byte(0xDB,OLED_CMD);//ÉèÖÃVCOMH µçÑ¹±¶ÂÊ
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM ÊÍ·ÅµçÑ¹([6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;)
	OLED_WR_Byte(0x20,OLED_CMD);//ÉèÖÃÒ³ÃæÑ°Ö·Ä£Ê½(0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//[1:0],00£¬ÁĞµØÖ·Ä£Ê½;01£¬ĞĞµØÖ·Ä£Ê½;10,Ò³µØÖ·Ä£Ê½;Ä¬ÈÏ10;
	OLED_WR_Byte(0x8D,OLED_CMD);//ÉèÖÃ³äµç±ÃÆôÓÃ/½ûÓÃ
	OLED_WR_Byte(0x14,OLED_CMD);//ÉèÖÃ(0x10½ûÓÃ,0x14ÆôÓÃ)
	OLED_WR_Byte(0xA4,OLED_CMD);// È«¾ÖÏÔÊ¾¿ªÆô;bit0:1,¿ªÆô;0,¹Ø±Õ;(°×ÆÁ/ºÚÆÁ) (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// ÉèÖÃÏÔÊ¾·½Ê½;bit0:1,·´ÏàÏÔÊ¾;0,Õı³£ÏÔÊ¾ (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//¿ªÆôÏÔÊ¾	
	OLED_Clear();
	OLED_ShowCHinese(18,0,2);//ÖÇ
	OLED_ShowCHinese(36,0,3);//ÄÜ
	OLED_ShowCHinese(54,0,4);//À¬
	OLED_ShowCHinese(72,0,5);//»ø
	OLED_ShowCHinese(90,0,6);//Í°
	OLED_ShowCHinese(38,3,28);//ÎÂ¶Èµ¥Î»
	OLED_ShowString(93,3,"RH",16);//Êª¶Èµ¥Î»
}



