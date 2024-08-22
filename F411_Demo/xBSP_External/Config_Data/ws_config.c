/***********************************************************
 * 文件名称: ws_config.c
 * 版本号: v1.0
 * 更新时间: 2024-08-20
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 本文件用于完成系统配置数据的保存与读取，提供系统配置信息结构体的存取操作。
 * 该模块通过懒加载机制初始化配置信息，并在配置数据发生变化时自动保存。
 * 如果是首次上电，会初始化默认配置，并记录开机次数。
 * 
 * 更新内容:
 * v1.0 2024-08-20
 *   - 实现了系统配置信息的初始化与懒加载功能。
 *   - 提供了保存与读取系统配置信息的接口。
 *   - 增加了开机次数统计功能。
 ************************************************************/

#include "ws_config.h"

// 懒加载标志
char configInfiInitIfg = 0;

// 系统配置信息定义
WS_ConfigInfoTypeDef systemConfigInfo;

/*
 * 函 数 名: WS_Config_System_Save_Timer
 * 函 数 功 能: 保存配置数据结构体
 * 输    入: data - 未使用参数
 * 返    回: 无
 */
int WS_Config_System_Save_Timer(void *data)
{
    data = data;
    WS_Config_Write_Struct_Callback((uint8_t *)&systemConfigInfo, sizeof(systemConfigInfo));
    return 0;
}

/*
 * 函 数 名: WS_Config_System_Save
 * 函 数 功 能: 立即保存配置数据
 * 输    入: 无
 * 返    回: 无
 */
void WS_Config_System_Save(void)
{
    WS_Config_System_Save_Timer(0);
    // CreateTimer(WS_Config_System_Save_Timer, 100);
}

/*
 * 函 数 名: WS_System_Config_Init
 * 函 数 功 能: 读取配置数据初始化，如果是第一次上电设置默认数据
 * 输    入: 无
 * 返    回: 无
 */
void WS_System_Config_Init(void)
{
    // 读配置信息
    configInfiInitIfg = 1; // 初始化标志

    WS_Config_Read_Struct_Callback((uint8_t *)&systemConfigInfo, sizeof(systemConfigInfo));

    if (systemConfigInfo.initFlag != 'w')
    {
        systemConfigInfo.initFlag = 'w';
        systemConfigInfo.powerOnCount = 1;
        systemConfigInfo.timer = 300;

        // WS_Debug("First power on\r\n");
        WS_Config_System_Save_Timer((void *)1);
    }
    else
    {
        systemConfigInfo.powerOnCount++; // 记录开机次数
        WS_Config_System_Save();         // 保存信息
        // Uart_printf(&huart6,"Open count : %d\r\n",systemConfigInfo.powerOnCount);
    }
}

/*
 * 函 数 名: WS_Config_Read_Struct_Callback
 * 函 数 功 能: 读取系统配置信息的虚函数，需在实际应用中实现
 * 输    入: date - 配置信息数据指针
 *          len  - 配置信息长度
 * 返    回: 无
 */
__weak void WS_Config_Read_Struct_Callback(uint8_t *date, uint32_t len)
{
    // 虚函数，用户需在实际应用中实现
}

/*
 * 函 数 名: WS_Config_Write_Struct_Callback
 * 函 数 功 能: 写入系统配置信息的虚函数，需在实际应用中实现
 * 输    入: date - 配置信息数据指针
 *          len  - 配置信息长度
 * 返    回: 无
 */
__weak void WS_Config_Write_Struct_Callback(uint8_t *date, uint32_t len)
{
    // 虚函数，用户需在实际应用中实现
}
