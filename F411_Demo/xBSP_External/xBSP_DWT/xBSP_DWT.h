#ifndef _BSP_DWT_H
#define _BSP_DWT_H

#include "main.h"

void xBSP_DWT_Init(void);    //��ʼ��DWT
uint32_t getDWT_CountDC(uint32_t startTick,uint32_t stopTick);//DWTʱ���������
uint32_t xBSP_DWT_Get_Tick(uint8_t chx , uint8_t stu);

#endif
