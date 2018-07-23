
/******************************************************************************
* 头文件设定
******************************************************************************/
#include <p18f4580.h>																//单片机头文件											
#include <delays.h>																	//延时函数头文件
#include "CAN.h"
//About Set ID***************************************************
unsigned char ID_H = 0;															//ID号高位，区分整机，由拨码开关高3位设置
unsigned char ID_M = 0;															//ID号中位，区分电路板类型，程序中设定
unsigned char ID_L = 0;															//ID号低位，区分相同种类电路板，由拨码开关低5位设置
unsigned char REG_SIDH     =0;
unsigned char REG_SIDL     =0;
unsigned char Error_Send=0;
//About Set ID***************************************************
/******************************************************************************
* 函数功能：	CAN消息发送
* 参        数：	无
* 返  回  值：  	无			
* 备        注：	调用前写数据长度至TXB0DLC可以节省发送时间
******************************************************************************/
void can_send(void)
{
	unsigned int Time_Out=5000;
	unsigned char Times_Flash=30;
	PIR3bits.TXB0IF=0;
	TXB0CONbits.TXREQ=1;														//请求发送,TXREQ=1
	Delay1TCY();
	while(PIR3bits.TXB0IF==0)	
	{
		Delay1KTCYx(1);																//延时1ms
		Time_Out--;
		if(Time_Out==0)																//5000*1ms=5s
		{						
			TXB0CONbits.TXREQ=0;
			Error_Send++;
			if(Error_Send>=127)
			{
				Error_Send=127;
			}
			while(Times_Flash--)
			{
				oLED=~oLED;
				Delay10KTCYx(25);
				oLED=~oLED;
				Delay10KTCYx(25);
			}			
			break;																				//延时到，发送超时
		}						
	}
	PIR3bits.TXB0IF=0;
}
/******************************************************************************
* 函数功能：	CAN总线初始化
* 参        数：	无
* 返  回  值：  	无			
* 备        注：	无
******************************************************************************/
void initialize_can(void)
{
	oLED				= 0;
	oLED_CTR		= 0;																	//LED口设置为输出
	LATA				= 0xFF;
	TRISA				= 0xFF;															//ID_L对应为输入
	LATE				= 0x07;
	TRISE				= 0x07;															//ID_H 口为输入	
	Error_Send	= 0;

	TRISB=(TRISB|0X08)&0XFB;												//设置CANRX/RB3为输入，CANTX/RB2为输出	
																									//设置CAN的波特率为125K，在Fosc=4M时，Tbit=8us，假设BRP=01h
																									//则TQ=[2*（1+BRP）]/Fosc=2*（1+1）/4=1us。
																									//NOMINAL BIT RATE=8TQ，SJW=1，Sync_Seg=1TQ,Prog _Seg=1TQ,Phase_Seg1=3TQ,Phase_Seg2=3TQ
	CANCON=0X80;																	//请求进入CAN配置模式REQOP=100	
	while(CANSTAT&0X80==0){;}											//等待进入CAN配置模式OPMODE=100
	BRGCON1=0X01;																	//设置SJW和BRP，SJW=1TQ，BRP=01H
	BRGCON2=0X90;																	//设置Phase_Seg1=3TQ和Prog _Seg=1TQ
	BRGCON3=0X42;																	//设置Phase_Seg2=3TQ

//设置邮箱0
	TXB0CON=0X03;																	//发送优先级为最高优先级，TXPRI=11
//设置CAN_ID，V1.3-spc********************************
	//ID_H			= PORTE&0x07;
	//ID_M			= 1;
	//ID_L				= PORTA&0x1F;
	ID_H			= 0;
	ID_M			= 0;
	ID_L				= 5;
	REG_SIDH	= ( ID_H<<5 ) + ( ID_M<<2 ) + ( ID_L>>3 );
	REG_SIDL	= ID_L<<5;
	
//设置发送ID，发送的时候会重新设置
	TXB0SIDH=REG_SIDH;
	TXB0SIDL=REG_SIDL;
	TXB0EIDH=0;				
	TXB0EIDL=0;							
//设置滤波器0
	RXF0SIDH=REG_SIDH;
	RXF0SIDL=REG_SIDL;
	RXF0EIDH=0;				
	RXF0EIDL=0;		
//设置滤波器1
	RXF1SIDH=REG_SIDH;
	RXF1SIDL=REG_SIDL;
	RXF1EIDH=0;					
	RXF1EIDL=0;
//设置屏蔽位
	RXM0SIDH=0xFF;
	RXM0SIDL=0xFF;																	//只接收标准ID数据						
	RXM0EIDH=0xFF;																	//扩展ID			
	RXM0EIDL=0xFF;	
//设置发送邮箱	
	TXB0DLC=0X08;																	//设置数据长度为8个字节
	TXB0D0=0;																			//写发送缓冲器数据区的数据
	TXB0D1=0;
	TXB0D2=0;
	TXB0D3=0;
	TXB0D4=0;
	TXB0D5=0;
	TXB0D6=0;
	TXB0D7=0;
//设置接收邮箱
	RXB0CON=0X20;																	//仅仅接收标准标识符的有效信息，FILHIT0=0表示RXB0采用filter0
//使CAN进入正常工作模式模式
	CANCON=0X00;																	//0x00,正常操作模式
	while(CANSTAT&0XE0!=0){;}

//初始化CAN的中断
	PIR3=0X00;																			//清所有中断标志
	PIE3=0X01;																			//使能接收缓冲器0的接收中断
	IPR3=0X01;																			//接收缓冲器0的接收中断为最高优先级
	RXB0CONbits.RXFUL=0;														//打开接收缓冲器来接收新信息
}
