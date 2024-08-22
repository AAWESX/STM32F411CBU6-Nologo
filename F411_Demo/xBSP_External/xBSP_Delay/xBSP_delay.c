/***************************************************************
 * 文件名: xBSP_delay.c
 * 版本号: v1.0
 * 更新时间: 2024-01-10
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 
 * 描述:
 * 这是一个实现延时功能的驱动程序模块，支持微秒和毫秒级别的延时操作。
 * 模块支持FreeRTOS任务调度下的延时操作，同时也提供了在非OS环境下的延时功能。
 * 该模块适用于需要精确时序控制的嵌入式系统应用。
 
 * 更新内容:
 * v1.0 2024-08-20
 *   - 实现了基本的微秒和毫秒延时函数，包括delay_us、delay_ms等。
 *   - 增加了在FreeRTOS环境下延时的支持函数，如XK_delay_us和XK_delay_ms。
 *   - 设计了基于循环计数的延时方法，适用于各种系统时钟配置。
***************************************************************/

#include "xBSP_Delay/xBSP_delay.h"

#define SYSTEM_SUPPORT_OS 0  // 定义系统文件夹是否支持OS

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"  // FreeRTOS使用
#include "task.h"

static uint8_t fac_us = 0;  // us延时倍乘数
static uint16_t fac_ms = 0; // ms延时倍乘数, 在ucos下, 代表每个节拍的ms数

extern void xPortSysTickHandler(void);

/**
 * @brief 初始化延迟函数。
 * @description 初始化延迟函数，将SYSTICK的时钟频率改为AHB的频率，确保延迟函数在FreeRTOS下正常工作。
 */
void XK_delay_init()
{
    uint32_t reload;
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  // 选择外部时钟HCLK
    fac_us = SystemCoreClock / 1000000;                   // 不论是否使用OS, fac_us都需要使用
    reload = SystemCoreClock / 1000000;                   // 每秒钟的计数次数 单位为M
    reload *= 1000000 / configTICK_RATE_HZ;               // 根据configTICK_RATE_HZ设定溢出时间
                                                          // reload为24位寄存器, 最大值:16777216, 在72M下, 约合0.233s左右
    fac_ms = 1000 / configTICK_RATE_HZ;                   // 代表OS可以延时的最少单位

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // 开启SYSTICK中断
    SysTick->LOAD = reload;                     // 每1/configTICK_RATE_HZ秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   // 开启SYSTICK
}

/**
 * @brief 延时指定微秒数。
 * @param nus 要延时的微秒数。
 * @description 使用SysTick实现的微秒级延时，适用于FreeRTOS任务调度环境。
 */
void XK_delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;  // LOAD的值
    ticks = nus * fac_us;             // 需要的节拍数
    told = SysTick->VAL;              // 刚进入时的计数器值
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;  // 这里注意一下SYSTICK是一个递减的计数器
            else
                tcnt += reload - tnow + told;
            told = tnow;
            if (tcnt >= ticks)
                break;  // 时间超过/等于要延迟的时间, 则退出
        }
    }
}

/**
 * @brief 延时指定毫秒数。
 * @param nms 要延时的毫秒数。
 * @description 在FreeRTOS任务调度环境下，优先使用任务调度器的延时功能。
 */
void XK_delay_ms(uint32_t nms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)  // 系统已经运行
    {
        if (nms >= fac_ms)  // 延时的时间大于OS的最少时间周期
        {
            vTaskDelay(nms / fac_ms);  // FreeRTOS延时
        }
        nms %= fac_ms;  // OS已经无法提供这么小的延时了, 采用普通方式延时
    }
    XK_delay_us((uint32_t)(nms * 1000));  // 普通方式延时
}

/**
 * @brief 延时指定毫秒数，不会引起任务调度。
 * @param nms 要延时的毫秒数。
 * @description 使用循环延时，不会触发任务调度，适用于非FreeRTOS环境。
 */
void delay_xms(uint32_t nms)
{
    uint32_t i;
    for (i = 0; i < nms; i++)
        XK_delay_us(1000);
}
#endif

#define SYSTEM_CLOCK_FREQ 100000000UL  // 假设系统时钟配置为100MHz

#define MS_DELAY_LOOP_COUNT_MS (SYSTEM_CLOCK_FREQ / 1250UL)     // 粗略估计循环次数
#define MS_DELAY_LOOP_COUNT_US (SYSTEM_CLOCK_FREQ / 1250000UL)  // 粗略估计循环次数

/**
 * @brief 延时指定毫秒数。
 * @param tim 要延时的毫秒数。
 * @description 简单的延时实现，不依赖于操作系统。
 */
void delay_ms(uint32_t tim)
{
    uint32_t i = 0;
    for (i = 0; i < tim; i++)
    {
        volatile uint32_t delayCount = MS_DELAY_LOOP_COUNT_MS;
        while (delayCount--)
            ;
    }
}

/**
 * @brief 延时指定微秒数。
 * @param tim 要延时的微秒数。
 * @description 简单的延时实现，不依赖于操作系统。
 */
void delay_us(uint32_t tim)
{
    uint32_t i = 0;
    for (i = 0; i < tim; i++)
    {
        volatile uint32_t delayCount = MS_DELAY_LOOP_COUNT_US;
        while (delayCount--)
            ;
    }
}

/**
 * @brief 延时指定微秒数（更精确）。
 * @param us 要延时的微秒数。
 * @description 使用NOP指令进行精确的延时，适用于时间敏感的操作。
 */
void Delay_us(uint32_t us)
{
    uint32_t count = (us * (SystemCoreClock / 1000000)) / 3;
    while (count--)
    {
        __NOP();  // 空操作，耗费一个时钟周期
    }
}

/**
 * @brief 延时指定毫秒数（更精确）。
 * @param ms 要延时的毫秒数。
 * @description 使用微秒延时函数实现毫秒延时。
 */
void Delay_ms(uint32_t ms)
{
    while (ms--)
    {
        Delay_us(1000);  // 每次延时1毫秒
    }
}
