/***************************************************************
 * 文件名: xDRV_Key.c
 * 版本号: v1.0
 * 更新时间: 2024-01-10
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 这是一个用于按键扫描的驱动程序。通过检测按键的状态，判断按键的按下和释放动作，并返回相应的按键标识符。支持单次扫描模式和连续扫描模式，适用于嵌入式系统中的按键输入处理。
 * 
 * 更新内容:
 * v1.0 2023-11-22
 *   - 实现了`xDRV_Key_scanf`函数，用于按键状态的检测和识别。
 *   - 优化了按键去抖动处理，通过短暂延时减少误触发的可能性。
 *   - 预留了对其他按键的支持接口（KEY_2、KEY_3、KEY_4、KEY_5）。
 *   - 修复了在连续模式下可能导致按键状态无法正确恢复的问题。
 ***************************************************************/



#include "xDRV_Key/xDRV_key.h"

uint8_t xDRV_Key_scanf(uint8_t mode)           
{
  static uint8_t KLL = 0;
	
	if(mode == 1)
		KLL = 1;
	if( KLL == 0 &&(KEY_1 == 0 /*|| KEY_2 == 0 || KEY_3 == 0|| KEY_4 == 0 || KEY_5 == 1*/)){
		HAL_Delay(1);
		KLL = 1;
		if(KEY_1 == 0)
			return KEY_PRESS_1;
//		else if(KEY_2 == 0)
//			return KEY_PRESS_2;
//		else if(KEY_3 == 0)
//			return KEY_PRESS_3;
//		else if(KEY_4 == 0)
//			return KEY_PRESS_4;
//		else if(KEY_5 == 1)
//			return KEY_PRESS_5;	
	}
	
	else if(KLL == 1 && KEY_1 == 1 /* && KEY_2 == 1 && KEY_3 == 1 && KEY_4 == 1 && KEY_5 == 0*/){
		KLL = 0;
	}
	return KEY_UNPRESS;
}

