#include "User_main.h"

/* 标准库 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* HAL库 */
#include "usbd_cdc_if.h"
#include "tim.h"
#include "rtc.h"
#include "adc.h"

/* 用户外设驱动 */
#include "xDRV_W25Qxx/xDRV_w25Qxx.h"
#include "xDRV_Led/xDRV_led.h"
#include "xDRV_Key/xDRV_key.h"


/* 用户软件驱动 */
#include "Config_Data/ws_config.h"
// #include "xBSP_Usart/xBSP_usart.h"
#include "xBSP_DWT/xBSP_DWT.h"
#include "xBSP_Delay/xBSP_delay.h"
#include "xBSP_IIC/xBSP_iic.h"
#include "xBSP_Adc/xBSP_adc.h"
#include "Sys_Bit_Control\Bit_Control.h"


/* 用户算法 */
#include "str_control.h"

/* 外部句柄 */


int WS_OS_Task_Key(void * date);
int WS_OS_Task_UART_USB(void * date);
int WS_OS_Task_RTC(void * date);
int WS_OS_Task_ADC(void * date);
/*
 * @brife  外设初始化
 * @param  NULL
 * @return NULL
 */
void Main_Init(void)
{
	xBSP_DWT_Init();   // 初始化DWT精准时间监测
	USB_printf("USB STM32 OPEN");
	W25QXX_Init();			// 初始化外部存储
	WS_System_Config_Init();// 系统信息
	
	ADC_DMA_Start(&hadc1,3,1);
	HAL_TIM_Base_Start(&htim3);

}


/*
 * @brife  任务主循环
 * @param  NULL
 * @return NULL
 */
void Main_Loop(void)
{
	
	WS_OS_Init_Task();       /* 初始化任务管理器 */
	WS_OS_Create_Task(1,WS_OS_Task_Runled,10);  /* 运行灯 */
	WS_OS_Create_Task(2,WS_OS_Task_Key,20);	 	  /* 按键 */
	WS_OS_Create_Task(3,WS_OS_Task_UART_USB,30);/* USB 虚拟串口 */
	WS_OS_Create_Task(4,WS_OS_Task_RTC,40);			/* RTC实时时钟 */
	WS_OS_Create_Task(5,WS_OS_Task_ADC,150);		/* ADC采集 */
	
	while(1)
	{ 
	 	WS_OS_Run_Task();  /* 运行函数 */
	} 
}

int contCnt = 0;
int WS_OS_Task_Key(void * date)
{
	static uint8_t key_value = 0;
	key_value = xDRV_Key_scanf(0);
	switch(key_value)
	{
		case 1:
			// Uart_printf(&huart6,"key open");
			USB_printf("USB STM32 OPEN\r\n");
			break;
		default:
			break;
	}
	return 1;
}


uint8_t USB_buffer[256];//接收缓冲,最大USB_REC_LEN个字节.末字节为换行符

char *start_ptr;

int number = 0;
char buf[24];
int WS_OS_Task_UART_USB(void * date)
{
	if(USB_RX_STA != 0)
	{
		USB_RX_STA = 0;
		strncpy((char *)USB_buffer,(char *)USB_RX_BUF,sizeof(USB_buffer));
		
		/* 分割法解码 */
		start_ptr = strstr((char *)USB_buffer,"Data:");
		if(start_ptr != NULL)
		{
			number = My_atoi(start_ptr + sizeof("Data:") - 1);
			USB_printf("USB Data RX: %d\r\n",number);
		}
		
		/*  分段法解码 */
		if(strncmp("P_PID", (char *)USB_buffer, 5) == 0)   
    {
        Extract_Nth_Field((char *)USB_buffer , 2, ',' , buf);
        number = My_atoi(buf);
				USB_printf("USB Data P: %d\r\n",number);
        Extract_Nth_Field((char *)USB_buffer , 3, ',' , buf);
        number = My_atoi(buf);
				USB_printf("USB Data I: %d\r\n",number);
        Extract_Nth_Field((char *)USB_buffer , 4, ',' , buf);
        number = My_atoi(buf);
			
				USB_printf("USB Data D: %d\r\n",number);
    }
		
		
		
		memset(USB_RX_BUF,0,sizeof(USB_RX_BUF));//USB串口数据寄存器清0
		memset(USB_buffer,0,sizeof(USB_buffer));//备份数据清0
	}
	return 10;
}

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sData;

int WS_OS_Task_RTC(void * date)
{
	HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sData,RTC_FORMAT_BIN);
	
	//USB_printf("20%02d,%02d,%02d\r\n",sData.Year,sData.Month,sData.Date);
	//USB_printf("AM,PM:%d\r\n",sTime.TimeFormat);
	//USB_printf("%02dh,%02dmin,%02ds,%03dms\r\n",sTime.Hours,sTime.Minutes,sTime.Seconds,(255 + 1 - sTime.SubSeconds) * 1000 / (255 + 1));
	return 1000;
}


int WS_OS_Task_ADC(void * date)
{
	if(ADC_Samplink_STA_A == ADC1_SPL_STA_FLAG)
	{
		
		ADC_Samplink_STA_A = 0;
		USB_printf("ADC1_SPL_Buffer_A: %d , %d , %d \r\n",ADC1_SPL_Buffer_A[0],ADC1_SPL_Buffer_A[1],ADC1_SPL_Buffer_A[2]);
	
	}
	
	if(ADC_Samplink_STA_B == ADC1_SPL_STA_FLAG)
	{
		
		ADC_Samplink_STA_B = 0;
		USB_printf("ADC1_SPL_Buffer_B: %d , %d , %d \r\n",ADC1_SPL_Buffer_B[0],ADC1_SPL_Buffer_B[1],ADC1_SPL_Buffer_B[2]);
	}
	
	return 10;
}



