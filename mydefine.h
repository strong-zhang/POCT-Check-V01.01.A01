#ifndef __MYDEFINE_H
#define __MYDEFINE_H

unsigned char Version_Str[]="V01.01";								//版本号

//define bits
#define LASER_EXP			1														//有激光照射为1
#define DOOR_CLOSE		0														//门关状态为0
#define BUZ_ON				0														//蜂鸣器打开
#define BUZ_OFF				1
#define LASER_ON			1														//激光打开
#define LASER_OFF			0					

//开关信号***********************************************
#define iSGNL_BAR		PORTDbits.RD1								//IN1，条插到位
#define iSGNL_CVT		PORTDbits.RD0								//IN2，套脱完全
#define iSGNL_DOOR		PORTCbits.RC2									//IN4，翻门

#define oBAR				LATCbits.LATC4								//STP2，改为试剂条检测激光开关
#define oCVT				LATDbits.LATD3								//DIR1，改为磁套检测激光开关
#define oBUZ				LATCbits.LATC5								//DIR2，控制蜂鸣器
			
char Buz_Times			= 0;															//开关次数
char Buz_On_Time	= 3;															//打开时间，0.1秒倍数
char Buz_Off_Time	= 3;															//关闭时间，0.1秒倍数
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

//函数声明
void Initialize_Register(void);												//系统初始化函数

#endif
