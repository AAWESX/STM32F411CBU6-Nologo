/***************************************************************
 * 文件名: xBSP_DWT.c
 * 版本号: v1.1
 * 更新时间: 2023-08-25
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 这是一个利用DWT（Data Watchpoint and Trace）模块实现高精度时间测量的驱动程序模块。
 * 通过DWT计数器，可以精确测量代码段的执行时间。该模块支持多个通道的时间测量，
 * 并提供了简单易用的接口来获取时间间隔。
 * 
 * 更新内容:
 * v1.0 2024-08-20
 *   - 实现了DWT模块的初始化功能，通过`xBSP_DWT_Init`函数使能DWT外设并启动CYCCNT计数器。
 *   - 提供了`xBSP_DWT_GetCount`函数用于计算两个时间戳之间的时间差。
 *   - 实现了多通道的时间测量功能，通过`xBSP_DWT_Get_Tick`函数支持设定起点、测量终点和连续测量等操作。
 ***************************************************************/
 
 
 
 
 
#include "xBSP_DWT/xBSP_DWT.h"


/*
*   函 数 名    		xBSP_DWT_Init
*   函 数 功 能 ：初始化DWT
*   函 数 入 口 ：无	
*   函 数 返 回 ：无
*/

void xBSP_DWT_Init(void)
{
/* 
*  1.先使能DWT外设,由内核调试寄存器DEM_CR的位24控制,写1使能。
*  2.使能CYCCNT寄存器之前,先清0.
*  3.使能CYCCNT寄存器,由DWT_CTRL的位0控制，写1使能。
*/
  CoreDebug->DEMCR |= 1<<24;// 使能DWT外设
  DWT->CYCCNT       = 0;    // 清零CYCCNT
  DWT->CTRL        |= 1<<0; // 使能DWT计数
}



/*
*   函 数 名      xBSP_DWT_GetCount
*   函 数 功 能 ：利用DWT计算时间
*   函 数 入 口 ：startTick DWT起始时间(过去的时间)  stopTick DWT停止时间(现在的时间)
*   函 数 返 回 ：tick 利用DWT实现时间值的计算并返回时间值
*/
uint32_t xBSP_DWT_GetCount(uint32_t startTick,uint32_t stopTick)
{
    uint32_t tick; // 返回时间的值

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

/* DWT通道数 */
#define DWT_CHANNEL_WIDTH 16
/* DWT通道 缓存区 */
uint32_t  DWT_CountBuf[DWT_CHANNEL_WIDTH] = {0};


/******************************************************************************
*函数名称： uint32_t xBSP_DWT_Get_Tick(uint8_t chx , uint8_t stu)
*功能：     利用调用的间隔测量系统时钟计数，100M频率对应1秒计数100000000个脉冲
              示范：
              xBSP_DWT_Get_Tick(0,0);
              HAL_Delay(10);
              time = xBSP_DWT_Get_Tick(0,2);
              time 变量存贮两次调用的系统时钟计数，
							测量HAL_Delay(10)函数执行完所需的系统时钟计数 
 形参：    mode:    0 ： 设定测量起点，返回0 ；
                   1 ： 测量终点，返回起点到终点得测量时间,起点时间不修改
                   2 ： 测量起点/重点，每次调用测量上次调用到本次调用的时间
           chx:    测量通道（0~9）， 支持多路同时测量
*返回:		  无
*******************************************************************************/

uint32_t xBSP_DWT_Get_Tick(uint8_t chx , uint8_t stu)
{
   uint32_t  tick  = 0 ; 
   static    uint8_t  initFlag = 0;
	
	 //  首次调用， 完成初始化
   if(initFlag == 0)
	 {
	    initFlag = 1;
		  //  DWT 计数初始化
			CoreDebug->DEMCR |= 1<<24 ;  // 使能DWT外设
			DWT->CYCCNT       = 0     ;  // 清零CYCCNT
			DWT->CTRL        |= 1<<0  ;  // 使能计数
	 }	
	 
	 if(chx >= DWT_CHANNEL_WIDTH)  return 0;  //  超过通道数，直接返回0
	 
	 switch(stu)
	 {
		 // 设定起点
		 case 0:  
							DWT_CountBuf[chx]  = DWT->CYCCNT ;  // 存储起始值
			        break;
		 
		 //  设定重点，返回测量时间
		 case 1:
			        tick  = xBSP_DWT_GetCount(DWT_CountBuf[chx],DWT->CYCCNT);
		          break;
		 //  连续测量
		 case 2:    
			        tick  = xBSP_DWT_GetCount(DWT_CountBuf[chx],DWT->CYCCNT);
		          DWT_CountBuf[chx]  = DWT->CYCCNT ;  
			        break;
	 }
	 return tick;
}



