/***************************************************************
 * 文件名: xDRV_Led.c
 * 版本号: v1.1
 * 更新时间: 2024-08-20
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 这是一个用于控制LED的驱动程序模块。支持基础的LED开关、状态切换以及周期性闪烁功能。
 * 代码中包含了对WS OS操作系统的支持，用于在指定时间后关闭LED。
 * 此外，还包含了运行灯的控制和定时功能，适用于需要LED指示的嵌入式系统。
 * 
 * 更新内容:
 * v1.0 2023-11-22
 *   - 添加了`LED_Toggle`函数，用于控制LED的状态切换，并增加了去抖动处理。
 *   - 优化了`LED_Control`函数，使其支持高效的GPIO操作。
 *   - 修复了可能导致LED在特定情况下无法正确开关的潜在问题。
 * v1.1 2024-8-20
 *   - 增加了WS OS操作系统支持的运行灯定时关闭功能。
 ***************************************************************/



#include "xDRV_Led\xDRV_led.h"


static uint8_t LED_OFF_ON_Start = 0;

void LED_Control(GPIO_TypeDef *GPIOx,LED_Pin_Switch LED_Pin, LED_GPIO_STAT  GpioStat)
{
	if(GpioStat == Pin_HIGH)
	{
		GPIOx->BSRR = ((uint16_t)0x0001) << LED_Pin;
		 LED_OFF_ON_Start = 0;
	}
	else
	{
		GPIOx->BSRR = ((uint16_t)0x0001) << (16 + LED_Pin);
		LED_OFF_ON_Start = 1;
	}
}


	

void LED_Toggle(GPIO_TypeDef *GPIOx,LED_Pin_Switch LED_Pin)
{
	static __IO uint32_t systick_Led_bak = 0;
	
	if(uwTick - systick_Led_bak  > 100)
	{
		
		systick_Led_bak  = uwTick;
		if( LED_OFF_ON_Start == 1 )
		{
			LED_Control(GPIOx,LED_Pin,Pin_HIGH);
		}
		else
		{
			LED_Control(GPIOx,LED_Pin,Pin_LOW);
		}
	}
	
}



#if WS_OS_LED 
//   运行灯关闭函数
int  WS_Borad_LED_RUN_Close(void *date)
{
     HAL_GPIO_WritePin(LED_RUN_GPIO_Port,LED_RUN_Pin,GPIO_PIN_SET);
	   return 0;
}

//  运行灯驱动函数
void WS_Borad_LED_RUN_Open_Time(int time)
{
      HAL_GPIO_WritePin(LED_RUN_GPIO_Port,LED_RUN_Pin,GPIO_PIN_RESET);
	    WS_OS_Create_Timer(WS_Borad_LED_RUN_Close,time) ;
}


//  运行灯驱动函数
void WS_Borad_LED_RUN(int time)
{
      HAL_GPIO_WritePin(LED_RUN_GPIO_Port,LED_RUN_Pin,GPIO_PIN_RESET);
	    WS_OS_Create_Timer(WS_Borad_LED_RUN_Close,time) ;
}

/* 运行灯 */
int  WS_OS_Task_Runled(void * date)
{
	  WS_Borad_LED_RUN_Open_Time(100);
    return  1000;
}
#endif



