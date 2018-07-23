
/******************************************************************************
* 头文件设定
******************************************************************************/
#include <p18f4580.h>															//单片机头文件											
#include <delays.h>																//延时函数头文件
#include "mydefine.h"															//自定义头文件
#include "Can.h"																		//CAN相关
/******************************************************************************
* 函数功能：	高级中断函数
* 参        数：	无
* 返  回  值：  	无			
* 备        注：	
******************************************************************************/

#pragma	interrupt	high_isr	
#pragma	code high_ISR=0x08
void high_ISR()
{
	_asm
	goto high_isr	
	_endasm
}
#pragma	code

void high_isr()
{
	if(PIR3bits.RXB0IF)
	{
		PIR3bits.RXB0IF=0;															//清接收中断标志
		status.received=1;															//置位接收标志
	}	
}

void main(void)
{
	Initialize_Register();
	
	INTCON=0xc0;

	while(1)
	{
		if ( ( iSGNL_DOOR == !DOOR_CLOSE ) || ( iSGNL_BAR == !LASER_EXP ) || ( iSGNL_CVT == !LASER_EXP ) )
		{
			oLED = 0;
		}
		else
		{
			oLED = 1;
		}
		if ( status.received )															//接收到消息
		{
			status.received	= 0;
			oLED					=~oLED;
			TXB0D1				=RXB0D0;											//第2字节返回接收到的命令
			switch (RXB0D0)
			{
				case 0x12:																	//查询
					oBAR		=	LASER_ON;
					oCVT		=	LASER_ON;
					Delay1KTCYx(10);	
					TXB0D0	= 1;
					if ( iSGNL_DOOR == DOOR_CLOSE )
					{
						TXB0D2		= 1;
					}
					else
					{
						TXB0D2		= 0;
					}
					if ( iSGNL_BAR == LASER_EXP )
					{
						TXB0D3		= 1;
					}
					else
					{
						TXB0D3		= 0;
					}
					if ( iSGNL_CVT == LASER_EXP )
					{
						TXB0D4		= 1;
					}
					else
					{
						TXB0D4		= 0;
					}
					if ( !status.bar )
					{
						oBAR		=LASER_OFF;
					}
					if ( !status.cvt )
					{
						oCVT		=LASER_OFF;
					}
					TXB0DLC	= 5;
					break;	
				case 0x11:
					Buz_Times			= RXB0D1;									//开关次数				
					Buz_On_Time	= RXB0D2;									//打开时间
					Buz_Off_Time	= RXB0D3;									//关闭时间

					for ( ; Buz_Times > 0; Buz_Times-- )
					{
						oBUZ	= BUZ_ON;
						for ( tmp_v = 0; tmp_v < Buz_On_Time; tmp_v++)
						{
							Delay10KTCYx ( 10 );
						}
						oBUZ	= BUZ_OFF;
						for ( tmp_v = 0; tmp_v < Buz_Off_Time; tmp_v++)
						{
							Delay10KTCYx ( 10 );
						}
					}

					TXB0D0			= 1;
					TXB0DLC		= 2;
					break;				
				case 0x21:																	//激光控制开关
					TXB0D0	=	1;
					if ( RXB0D1 == 1 )
					{
						status.bar	= 1;
						oBAR			=	LASER_ON;
					}
					else
					{
						status.bar	= 0;
						oBAR			=	LASER_OFF;
					}
					if ( RXB0D2 == 1 )
					{
						status.cvt	= 1;
						oCVT			=	LASER_ON;
					}
					else
					{
						status.cvt	= 0;
						oCVT			=	LASER_OFF;
					}
					TXB0DLC	=	2;	
					break;
				case 0xE2:																	//CAN状态
					TXB0D0=1;
					TXB0D2=Error_Send;		//00
					TXB0D3=COMSTAT;		//00
					TXB0D4=TXB0CON;			//03
					TXB0D5=TXB1CON;			//02
					TXB0D6=PIR3;					//0x80
					TXB0DLC=7;	
					break;
				case 0xF2:																	//查询版本状态
					TXB0D0=1;	
					TXB0D2=Version_Str[0];
					TXB0D3=Version_Str[1];
					TXB0D4=Version_Str[2];
					TXB0D5=Version_Str[3];
					TXB0D6=Version_Str[4];
					TXB0D7=Version_Str[5];
					TXB0DLC=8;	
					break;
				default:
					TXB0D0=0x10;														//指令无效
					TXB0DLC=2;
					break;
			}
			can_send();
			oLED=~oLED;
			RXB0CONbits.RXFUL=0;												//打开接收缓冲器来接收新信息
		}
	}
}

/******************************************************************************
* 函数功能：	系统初始化函数
* 参        数：	无
* 返  回  值：  	无			
* 备        注：	
******************************************************************************/
void Initialize_Register(void)
{
	WDTCON	= 0;																		//=1,看门狗使能;=0,禁止看门狗
	INTCON		= 0x00;																//禁止所有中断
	ADCON1	= 0x0F;																//所有AD均配置为数字IO口
	CMCON		= 0x07;																//关闭比较器，将RD口作为数字I/O	

	LATB			= 0xFF;
	TRISB			= 0xFF;	
	LATC			= 0xCD;
	TRISC			= 0x4D;
//	ECCP1CON= 0;																		//ECCP模块关闭，PORTD口高4位除P1A外均为普通I/O
	LATD			= 0xF7;
	TRISD			= 0xE3;
	oBUZ	= BUZ_OFF;
	status.received	= 0;																//定义接收标志位
	status.bar			= 0;	
	status.cvt			= 0;	
	initialize_can();
	Delay10KTCYx(30);
	oLED=1;
	Delay10KTCYx(30);
	oLED=0;
	Delay10KTCYx(30);
	oLED=1;
	Delay10KTCYx(30);
	oLED=0;
	Delay10KTCYx(30);
	oLED=1;
}
