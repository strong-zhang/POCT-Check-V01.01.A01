
/******************************************************************************
* ͷ�ļ��趨
******************************************************************************/
#include <p18f4580.h>															//��Ƭ��ͷ�ļ�											
#include <delays.h>																//��ʱ����ͷ�ļ�
#include "mydefine.h"															//�Զ���ͷ�ļ�
#include "Can.h"																		//CAN���
/******************************************************************************
* �������ܣ�	�߼��жϺ���
* ��        ����	��
* ��  ��  ֵ��  	��			
* ��        ע��	
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
		PIR3bits.RXB0IF=0;															//������жϱ�־
		status.received=1;															//��λ���ձ�־
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
		if ( status.received )															//���յ���Ϣ
		{
			status.received	= 0;
			oLED					=~oLED;
			TXB0D1				=RXB0D0;											//��2�ֽڷ��ؽ��յ�������
			switch (RXB0D0)
			{
				case 0x12:																	//��ѯ
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
					Buz_Times			= RXB0D1;									//���ش���				
					Buz_On_Time	= RXB0D2;									//��ʱ��
					Buz_Off_Time	= RXB0D3;									//�ر�ʱ��

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
				case 0x21:																	//������ƿ���
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
				case 0xE2:																	//CAN״̬
					TXB0D0=1;
					TXB0D2=Error_Send;		//00
					TXB0D3=COMSTAT;		//00
					TXB0D4=TXB0CON;			//03
					TXB0D5=TXB1CON;			//02
					TXB0D6=PIR3;					//0x80
					TXB0DLC=7;	
					break;
				case 0xF2:																	//��ѯ�汾״̬
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
					TXB0D0=0x10;														//ָ����Ч
					TXB0DLC=2;
					break;
			}
			can_send();
			oLED=~oLED;
			RXB0CONbits.RXFUL=0;												//�򿪽��ջ���������������Ϣ
		}
	}
}

/******************************************************************************
* �������ܣ�	ϵͳ��ʼ������
* ��        ����	��
* ��  ��  ֵ��  	��			
* ��        ע��	
******************************************************************************/
void Initialize_Register(void)
{
	WDTCON	= 0;																		//=1,���Ź�ʹ��;=0,��ֹ���Ź�
	INTCON		= 0x00;																//��ֹ�����ж�
	ADCON1	= 0x0F;																//����AD������Ϊ����IO��
	CMCON		= 0x07;																//�رձȽ�������RD����Ϊ����I/O	

	LATB			= 0xFF;
	TRISB			= 0xFF;	
	LATC			= 0xCD;
	TRISC			= 0x4D;
//	ECCP1CON= 0;																		//ECCPģ��رգ�PORTD�ڸ�4λ��P1A���Ϊ��ͨI/O
	LATD			= 0xF7;
	TRISD			= 0xE3;
	oBUZ	= BUZ_OFF;
	status.received	= 0;																//������ձ�־λ
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
