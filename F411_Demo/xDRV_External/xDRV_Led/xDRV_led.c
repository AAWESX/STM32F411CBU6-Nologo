/***************************************************************
 * �ļ���: xDRV_Led.c
 * �汾��: v1.1
 * ����ʱ��: 2024-08-20
 * ��������: Xiang Likang
 * �ʼ���ַ: 3265103350@qq.com
 * 
 * ����:
 * ����һ�����ڿ���LED����������ģ�顣֧�ֻ�����LED���ء�״̬�л��Լ���������˸���ܡ�
 * �����а����˶�WS OS����ϵͳ��֧�֣�������ָ��ʱ���ر�LED��
 * ���⣬�����������еƵĿ��ƺͶ�ʱ���ܣ���������ҪLEDָʾ��Ƕ��ʽϵͳ��
 * 
 * ��������:
 * v1.0 2023-11-22
 *   - �����`LED_Toggle`���������ڿ���LED��״̬�л�����������ȥ��������
 *   - �Ż���`LED_Control`������ʹ��֧�ָ�Ч��GPIO������
 *   - �޸��˿��ܵ���LED���ض�������޷���ȷ���ص�Ǳ�����⡣
 * v1.1 2024-8-20
 *   - ������WS OS����ϵͳ֧�ֵ����еƶ�ʱ�رչ��ܡ�
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
//   ���еƹرպ���
int  WS_Borad_LED_RUN_Close(void *date)
{
     HAL_GPIO_WritePin(LED_RUN_GPIO_Port,LED_RUN_Pin,GPIO_PIN_SET);
	   return 0;
}

//  ���е���������
void WS_Borad_LED_RUN_Open_Time(int time)
{
      HAL_GPIO_WritePin(LED_RUN_GPIO_Port,LED_RUN_Pin,GPIO_PIN_RESET);
	    WS_OS_Create_Timer(WS_Borad_LED_RUN_Close,time) ;
}


//  ���е���������
void WS_Borad_LED_RUN(int time)
{
      HAL_GPIO_WritePin(LED_RUN_GPIO_Port,LED_RUN_Pin,GPIO_PIN_RESET);
	    WS_OS_Create_Timer(WS_Borad_LED_RUN_Close,time) ;
}

/* ���е� */
int  WS_OS_Task_Runled(void * date)
{
	  WS_Borad_LED_RUN_Open_Time(100);
    return  1000;
}
#endif



