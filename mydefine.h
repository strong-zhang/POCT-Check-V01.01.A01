#ifndef __MYDEFINE_H
#define __MYDEFINE_H

unsigned char Version_Str[]="V01.01";								//�汾��

//define bits
#define LASER_EXP			1														//�м�������Ϊ1
#define DOOR_CLOSE		0														//�Ź�״̬Ϊ0
#define BUZ_ON				0														//��������
#define BUZ_OFF				1
#define LASER_ON			1														//�����
#define LASER_OFF			0					

//�����ź�***********************************************
#define iSGNL_BAR		PORTDbits.RD1								//IN1�����嵽λ
#define iSGNL_CVT		PORTDbits.RD0								//IN2��������ȫ
#define iSGNL_DOOR		PORTCbits.RC2									//IN4������

#define oBAR				LATCbits.LATC4								//STP2����Ϊ�Լ�����⼤�⿪��
#define oCVT				LATDbits.LATD3								//DIR1����Ϊ���׼�⼤�⿪��
#define oBUZ				LATCbits.LATC5								//DIR2�����Ʒ�����
			
char Buz_Times			= 0;															//���ش���
char Buz_On_Time	= 3;															//��ʱ�䣬0.1�뱶��
char Buz_Off_Time	= 3;															//�ر�ʱ�䣬0.1�뱶��
char tmp_v = 0;
struct																
{
	unsigned received:1;			
	unsigned bar:1;	
	unsigned cvt:1;	
	unsigned :5;
}status;
union
{
	int D16;
	unsigned char D8[2];
}Parameter;

//��������
void Initialize_Register(void);												//ϵͳ��ʼ������

#endif
