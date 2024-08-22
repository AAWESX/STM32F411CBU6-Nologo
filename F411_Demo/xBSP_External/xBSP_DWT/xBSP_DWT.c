/***************************************************************
 * �ļ���: xBSP_DWT.c
 * �汾��: v1.1
 * ����ʱ��: 2023-08-25
 * ��������: Xiang Likang
 * �ʼ���ַ: 3265103350@qq.com
 * 
 * ����:
 * ����һ������DWT��Data Watchpoint and Trace��ģ��ʵ�ָ߾���ʱ���������������ģ�顣
 * ͨ��DWT�����������Ծ�ȷ��������ε�ִ��ʱ�䡣��ģ��֧�ֶ��ͨ����ʱ�������
 * ���ṩ�˼����õĽӿ�����ȡʱ������
 * 
 * ��������:
 * v1.0 2024-08-20
 *   - ʵ����DWTģ��ĳ�ʼ�����ܣ�ͨ��`xBSP_DWT_Init`����ʹ��DWT���貢����CYCCNT��������
 *   - �ṩ��`xBSP_DWT_GetCount`�������ڼ�������ʱ���֮���ʱ��
 *   - ʵ���˶�ͨ����ʱ��������ܣ�ͨ��`xBSP_DWT_Get_Tick`����֧���趨��㡢�����յ�����������Ȳ�����
 ***************************************************************/
 
 
 
 
 
#include "xBSP_DWT/xBSP_DWT.h"


/*
*   �� �� ��    		xBSP_DWT_Init
*   �� �� �� �� ����ʼ��DWT
*   �� �� �� �� ����	
*   �� �� �� �� ����
*/

void xBSP_DWT_Init(void)
{
/* 
*  1.��ʹ��DWT����,���ں˵��ԼĴ���DEM_CR��λ24����,д1ʹ�ܡ�
*  2.ʹ��CYCCNT�Ĵ���֮ǰ,����0.
*  3.ʹ��CYCCNT�Ĵ���,��DWT_CTRL��λ0���ƣ�д1ʹ�ܡ�
*/
  CoreDebug->DEMCR |= 1<<24;// ʹ��DWT����
  DWT->CYCCNT       = 0;    // ����CYCCNT
  DWT->CTRL        |= 1<<0; // ʹ��DWT����
}



/*
*   �� �� ��      xBSP_DWT_GetCount
*   �� �� �� �� ������DWT����ʱ��
*   �� �� �� �� ��startTick DWT��ʼʱ��(��ȥ��ʱ��)  stopTick DWTֹͣʱ��(���ڵ�ʱ��)
*   �� �� �� �� ��tick ����DWTʵ��ʱ��ֵ�ļ��㲢����ʱ��ֵ
*/
uint32_t xBSP_DWT_GetCount(uint32_t startTick,uint32_t stopTick)
{
    uint32_t tick; // ����ʱ���ֵ

    if(startTick < stopTick)
    {
        tick = (0xffffffff - startTick) + stopTick + 1; 
    }
    else
    {
        tick = stopTick - startTick;
    }
    return tick;
}  

/* DWTͨ���� */
#define DWT_CHANNEL_WIDTH 16
/* DWTͨ�� ������ */
uint32_t  DWT_CountBuf[DWT_CHANNEL_WIDTH] = {0};


/******************************************************************************
*�������ƣ� uint32_t xBSP_DWT_Get_Tick(uint8_t chx , uint8_t stu)
*���ܣ�     ���õ��õļ������ϵͳʱ�Ӽ�����100MƵ�ʶ�Ӧ1�����100000000������
              ʾ����
              xBSP_DWT_Get_Tick(0,0);
              HAL_Delay(10);
              time = xBSP_DWT_Get_Tick(0,2);
              time �����������ε��õ�ϵͳʱ�Ӽ�����
							����HAL_Delay(10)����ִ���������ϵͳʱ�Ӽ��� 
 �βΣ�    mode:    0 �� �趨������㣬����0 ��
                   1 �� �����յ㣬������㵽�յ�ò���ʱ��,���ʱ�䲻�޸�
                   2 �� �������/�ص㣬ÿ�ε��ò����ϴε��õ����ε��õ�ʱ��
           chx:    ����ͨ����0~9���� ֧�ֶ�·ͬʱ����
*����:		  ��
*******************************************************************************/

uint32_t xBSP_DWT_Get_Tick(uint8_t chx , uint8_t stu)
{
   uint32_t  tick  = 0 ; 
   static    uint8_t  initFlag = 0;
	
	 //  �״ε��ã� ��ɳ�ʼ��
   if(initFlag == 0)
	 {
	    initFlag = 1;
		  //  DWT ������ʼ��
			CoreDebug->DEMCR |= 1<<24 ;  // ʹ��DWT����
			DWT->CYCCNT       = 0     ;  // ����CYCCNT
			DWT->CTRL        |= 1<<0  ;  // ʹ�ܼ���
	 }	
	 
	 if(chx >= DWT_CHANNEL_WIDTH)  return 0;  //  ����ͨ������ֱ�ӷ���0
	 
	 switch(stu)
	 {
		 // �趨���
		 case 0:  
							DWT_CountBuf[chx]  = DWT->CYCCNT ;  // �洢��ʼֵ
			        break;
		 
		 //  �趨�ص㣬���ز���ʱ��
		 case 1:
			        tick  = xBSP_DWT_GetCount(DWT_CountBuf[chx],DWT->CYCCNT);
		          break;
		 //  ��������
		 case 2:    
			        tick  = xBSP_DWT_GetCount(DWT_CountBuf[chx],DWT->CYCCNT);
		          DWT_CountBuf[chx]  = DWT->CYCCNT ;  
			        break;
	 }
	 return tick;
}



