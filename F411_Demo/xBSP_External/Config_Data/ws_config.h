



#ifndef _WS_CONFIG_H__
#define _WS_CONFIG_H__


#include "main.h"



//    配置参数控制结构体
typedef struct  __WS_ConfigInfoTypeDef
{
	uint32_t           powerOnCount  ;            //  记录开机次数  
	int                timer         ;
	
		
	uint16_t           initFlag   ;               //  初始化标志
	

} WS_ConfigInfoTypeDef;

//      系统配置信息   
extern  WS_ConfigInfoTypeDef   systemConfigInfo    ;


void WS_System_Config_Init(void);
void WS_Config_System_Save(void);	

//  存储、读取的虚函数
__weak  void  WS_Config_Read_Struct_Callback (uint8_t *  date  ,  uint32_t len) ;
__weak  void  WS_Config_Write_Struct_Callback(uint8_t *  date  ,  uint32_t len);


#endif


