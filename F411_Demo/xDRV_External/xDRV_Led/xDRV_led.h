#ifndef _LED_UESR_H_
#define _LED_UESR_H_

#include "main.h"

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define  WS_OS_LED  1 /* 0��δ����WSOS ����LED 1������WSOS����LED */

typedef enum
{
	Pin_LOW = 0U,
	Pin_HIGH
}LED_GPIO_STAT;

typedef enum
{
	Pin_0 = 0,
	Pin_1,
	Pin_2,
	Pin_3,
	Pin_4,
	Pin_5,
	Pin_6,
	Pin_7,
	Pin_8,
	Pin_9,
	Pin_10,
	Pin_11,
	Pin_12,
	Pin_13,
	Pin_14,
	Pin_15
}LED_Pin_Switch;

void LED_Control(GPIO_TypeDef *GPIOx,LED_Pin_Switch LED_Pin, LED_GPIO_STAT  GpioStat);
void LED_Toggle(GPIO_TypeDef *GPIOx,LED_Pin_Switch LED_Pin);



#if WS_OS_LED 
//   ���еƹرպ���
int  WS_Borad_LED_RUN_Close(void *date);
//  ���е���������
void WS_Borad_LED_RUN_Open_Time(int time);
//  ���е���������
void WS_Borad_LED_RUN(int time);

int  WS_OS_Task_Runled(void * date);
#endif

#endif


