
/******************************************************************************
* ͷ�ļ��趨
******************************************************************************/
#include <p18f4580.h>																//��Ƭ��ͷ�ļ�											
#include <delays.h>																	//��ʱ����ͷ�ļ�
#include "CAN.h"
//About Set ID***************************************************
unsigned char ID_H = 0;															//ID�Ÿ�λ�������������ɲ��뿪�ظ�3λ����
unsigned char ID_M = 0;															//ID����λ�����ֵ�·�����ͣ��������趨
unsigned char ID_L = 0;															//ID�ŵ�λ��������ͬ�����·�壬�ɲ��뿪�ص�5λ����
unsigned char REG_SIDH     =0;
unsigned char REG_SIDL     =0;
unsigned char Error_Send=0;
//About Set ID***************************************************
/******************************************************************************
* �������ܣ�	CAN��Ϣ����
* ��        ����	��
* ��  ��  ֵ��  	��			
* ��        ע��	����ǰд���ݳ�����TXB0DLC���Խ�ʡ����ʱ��
******************************************************************************/
void can_send(void)
{
	unsigned int Time_Out=5000;
	unsigned char Times_Flash=30;
	PIR3bits.TXB0IF=0;
	TXB0CONbits.TXREQ=1;														//������,TXREQ=1
	Delay1TCY();
	while(PIR3bits.TXB0IF==0)	
	{
		Delay1KTCYx(1);																//��ʱ1ms
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
			break;																				//��ʱ�������ͳ�ʱ
		}						
	}
	PIR3bits.TXB0IF=0;
}
/******************************************************************************
* �������ܣ�	CAN���߳�ʼ��
* ��        ����	��
* ��  ��  ֵ��  	��			
* ��        ע��	��
******************************************************************************/
void initialize_can(void)
{
	oLED				= 0;
	oLED_CTR		= 0;																	//LED������Ϊ���
	LATA				= 0xFF;
	TRISA				= 0xFF;															//ID_L��ӦΪ����
	LATE				= 0x07;
	TRISE				= 0x07;															//ID_H ��Ϊ����	
	Error_Send	= 0;

	TRISB=(TRISB|0X08)&0XFB;												//����CANRX/RB3Ϊ���룬CANTX/RB2Ϊ���	
																									//����CAN�Ĳ�����Ϊ125K����Fosc=4Mʱ��Tbit=8us������BRP=01h
																									//��TQ=[2*��1+BRP��]/Fosc=2*��1+1��/4=1us��
																									//NOMINAL BIT RATE=8TQ��SJW=1��Sync_Seg=1TQ,Prog _Seg=1TQ,Phase_Seg1=3TQ,Phase_Seg2=3TQ
	CANCON=0X80;																	//�������CAN����ģʽREQOP=100	
	while(CANSTAT&0X80==0){;}											//�ȴ�����CAN����ģʽOPMODE=100
	BRGCON1=0X01;																	//����SJW��BRP��SJW=1TQ��BRP=01H
	BRGCON2=0X90;																	//����Phase_Seg1=3TQ��Prog _Seg=1TQ
	BRGCON3=0X42;																	//����Phase_Seg2=3TQ

//��������0
	TXB0CON=0X03;																	//�������ȼ�Ϊ������ȼ���TXPRI=11
//����CAN_ID��V1.3-spc********************************
	//ID_H			= PORTE&0x07;
	//ID_M			= 1;
	//ID_L				= PORTA&0x1F;
	ID_H			= 0;
	ID_M			= 0;
	ID_L				= 5;
	REG_SIDH	= ( ID_H<<5 ) + ( ID_M<<2 ) + ( ID_L>>3 );
	REG_SIDL	= ID_L<<5;
	
//���÷���ID�����͵�ʱ�����������
	TXB0SIDH=REG_SIDH;
	TXB0SIDL=REG_SIDL;
	TXB0EIDH=0;				
	TXB0EIDL=0;							
//�����˲���0
	RXF0SIDH=REG_SIDH;
	RXF0SIDL=REG_SIDL;
	RXF0EIDH=0;				
	RXF0EIDL=0;		
//�����˲���1
	RXF1SIDH=REG_SIDH;
	RXF1SIDL=REG_SIDL;
	RXF1EIDH=0;					
	RXF1EIDL=0;
//��������λ
	RXM0SIDH=0xFF;
	RXM0SIDL=0xFF;																	//ֻ���ձ�׼ID����						
	RXM0EIDH=0xFF;																	//��չID			
	RXM0EIDL=0xFF;	
//���÷�������	
	TXB0DLC=0X08;																	//�������ݳ���Ϊ8���ֽ�
	TXB0D0=0;																			//д���ͻ�����������������
	TXB0D1=0;
	TXB0D2=0;
	TXB0D3=0;
	TXB0D4=0;
	TXB0D5=0;
	TXB0D6=0;
	TXB0D7=0;
//���ý�������
	RXB0CON=0X20;																	//�������ձ�׼��ʶ������Ч��Ϣ��FILHIT0=0��ʾRXB0����filter0
//ʹCAN������������ģʽģʽ
	CANCON=0X00;																	//0x00,��������ģʽ
	while(CANSTAT&0XE0!=0){;}

//��ʼ��CAN���ж�
	PIR3=0X00;																			//�������жϱ�־
	PIE3=0X01;																			//ʹ�ܽ��ջ�����0�Ľ����ж�
	IPR3=0X01;																			//���ջ�����0�Ľ����ж�Ϊ������ȼ�
	RXB0CONbits.RXFUL=0;														//�򿪽��ջ���������������Ϣ
}
