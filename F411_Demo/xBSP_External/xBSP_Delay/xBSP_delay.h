#ifndef __DELAY_H
#define __DELAY_H 			   
#include "main.h"  
	 
void XK_delay_init(void);
void XK_delay_ms(uint32_t nms);
void XK_delay_us(uint32_t nus);
void XK_delay_xms(uint32_t nms);
void delay_ms(uint32_t tim);
void delay_us(uint32_t tim);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t us);

#endif



