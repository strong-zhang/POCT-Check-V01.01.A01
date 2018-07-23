#ifndef __CAN_H
#define __CAN_H

#define oLED LATCbits.LATC1
#define oLED_CTR TRISCbits.TRISC1

extern unsigned char ID_H;
extern unsigned char ID_M;
extern unsigned char ID_L;
extern unsigned char REG_SIDH;
extern unsigned char REG_SIDL;
extern unsigned char Error_Send;

void initialize_can(void);															//CAN总线初始化
void can_send(void);																//CAN消息发送
#endif
