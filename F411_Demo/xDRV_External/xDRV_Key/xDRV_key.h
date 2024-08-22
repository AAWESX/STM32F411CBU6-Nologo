#ifndef _DRV_KEY_H_
#define _DRV_KEY_H_

#include "main.h"

#ifdef INC_FREERTOS_H
	#define delay_ms osDelay
#endif
#ifndef INC_FREERTOS_H
	#define delay_ms HAL_Delay
#endif

#define KEY_1  HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)
//#define KEY_2  HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_7)
//#define KEY_3  HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_8)
//#define KEY_4  HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_9)
//#define KEY_5  HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

typedef enum 
{
	KEY_UNPRESS = 0,
	KEY_PRESS_1,
	KEY_PRESS_2,
	KEY_PRESS_3,
	KEY_PRESS_4,
	KEY_PRESS_5,
	KEY_PRESS_6,
	KEY_PRESS_7,
	KEY_PRESS_8
}KEY_PRESS_DATA;

uint8_t xDRV_Key_scanf(uint8_t mode);



#endif



