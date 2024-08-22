/***************************************************************
 * �ļ���: xBSP_delay.c
 * �汾��: v1.0
 * ����ʱ��: 2024-01-10
 * ��������: Xiang Likang
 * �ʼ���ַ: 3265103350@qq.com
 
 * ����:
 * ����һ��ʵ����ʱ���ܵ���������ģ�飬֧��΢��ͺ��뼶�����ʱ������
 * ģ��֧��FreeRTOS��������µ���ʱ������ͬʱҲ�ṩ���ڷ�OS�����µ���ʱ���ܡ�
 * ��ģ����������Ҫ��ȷʱ����Ƶ�Ƕ��ʽϵͳӦ�á�
 
 * ��������:
 * v1.0 2024-08-20
 *   - ʵ���˻�����΢��ͺ�����ʱ����������delay_us��delay_ms�ȡ�
 *   - ��������FreeRTOS��������ʱ��֧�ֺ�������XK_delay_us��XK_delay_ms��
 *   - ����˻���ѭ����������ʱ�����������ڸ���ϵͳʱ�����á�
***************************************************************/

#include "xBSP_Delay/xBSP_delay.h"

#define SYSTEM_SUPPORT_OS 0  // ����ϵͳ�ļ����Ƿ�֧��OS

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"  // FreeRTOSʹ��
#include "task.h"

static uint8_t fac_us = 0;  // us��ʱ������
static uint16_t fac_ms = 0; // ms��ʱ������, ��ucos��, ����ÿ�����ĵ�ms��

extern void xPortSysTickHandler(void);

/**
 * @brief ��ʼ���ӳٺ�����
 * @description ��ʼ���ӳٺ�������SYSTICK��ʱ��Ƶ�ʸ�ΪAHB��Ƶ�ʣ�ȷ���ӳٺ�����FreeRTOS������������
 */
void XK_delay_init()
{
    uint32_t reload;
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  // ѡ���ⲿʱ��HCLK
    fac_us = SystemCoreClock / 1000000;                   // �����Ƿ�ʹ��OS, fac_us����Ҫʹ��
    reload = SystemCoreClock / 1000000;                   // ÿ���ӵļ������� ��λΪM
    reload *= 1000000 / configTICK_RATE_HZ;               // ����configTICK_RATE_HZ�趨���ʱ��
                                                          // reloadΪ24λ�Ĵ���, ���ֵ:16777216, ��72M��, Լ��0.233s����
    fac_ms = 1000 / configTICK_RATE_HZ;                   // ����OS������ʱ�����ٵ�λ

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // ����SYSTICK�ж�
    SysTick->LOAD = reload;                     // ÿ1/configTICK_RATE_HZ���ж�һ��
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   // ����SYSTICK
}

/**
 * @brief ��ʱָ��΢������
 * @param nus Ҫ��ʱ��΢������
 * @description ʹ��SysTickʵ�ֵ�΢�뼶��ʱ��������FreeRTOS������Ȼ�����
 */
void XK_delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;  // LOAD��ֵ
    ticks = nus * fac_us;             // ��Ҫ�Ľ�����
    told = SysTick->VAL;              // �ս���ʱ�ļ�����ֵ
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;  // ����ע��һ��SYSTICK��һ���ݼ��ļ�����
            else
                tcnt += reload - tnow + told;
            told = tnow;
            if (tcnt >= ticks)
                break;  // ʱ�䳬��/����Ҫ�ӳٵ�ʱ��, ���˳�
        }
    }
}

/**
 * @brief ��ʱָ����������
 * @param nms Ҫ��ʱ�ĺ�������
 * @description ��FreeRTOS������Ȼ����£�����ʹ���������������ʱ���ܡ�
 */
void XK_delay_ms(uint32_t nms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)  // ϵͳ�Ѿ�����
    {
        if (nms >= fac_ms)  // ��ʱ��ʱ�����OS������ʱ������
        {
            vTaskDelay(nms / fac_ms);  // FreeRTOS��ʱ
        }
        nms %= fac_ms;  // OS�Ѿ��޷��ṩ��ôС����ʱ��, ������ͨ��ʽ��ʱ
    }
    XK_delay_us((uint32_t)(nms * 1000));  // ��ͨ��ʽ��ʱ
}

/**
 * @brief ��ʱָ������������������������ȡ�
 * @param nms Ҫ��ʱ�ĺ�������
 * @description ʹ��ѭ����ʱ�����ᴥ��������ȣ������ڷ�FreeRTOS������
 */
void delay_xms(uint32_t nms)
{
    uint32_t i;
    for (i = 0; i < nms; i++)
        XK_delay_us(1000);
}
#endif

#define SYSTEM_CLOCK_FREQ 100000000UL  // ����ϵͳʱ������Ϊ100MHz

#define MS_DELAY_LOOP_COUNT_MS (SYSTEM_CLOCK_FREQ / 1250UL)     // ���Թ���ѭ������
#define MS_DELAY_LOOP_COUNT_US (SYSTEM_CLOCK_FREQ / 1250000UL)  // ���Թ���ѭ������

/**
 * @brief ��ʱָ����������
 * @param tim Ҫ��ʱ�ĺ�������
 * @description �򵥵���ʱʵ�֣��������ڲ���ϵͳ��
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
 * @brief ��ʱָ��΢������
 * @param tim Ҫ��ʱ��΢������
 * @description �򵥵���ʱʵ�֣��������ڲ���ϵͳ��
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
 * @brief ��ʱָ��΢����������ȷ����
 * @param us Ҫ��ʱ��΢������
 * @description ʹ��NOPָ����о�ȷ����ʱ��������ʱ�����еĲ�����
 */
void Delay_us(uint32_t us)
{
    uint32_t count = (us * (SystemCoreClock / 1000000)) / 3;
    while (count--)
    {
        __NOP();  // �ղ������ķ�һ��ʱ������
    }
}

/**
 * @brief ��ʱָ��������������ȷ����
 * @param ms Ҫ��ʱ�ĺ�������
 * @description ʹ��΢����ʱ����ʵ�ֺ�����ʱ��
 */
void Delay_ms(uint32_t ms)
{
    while (ms--)
    {
        Delay_us(1000);  // ÿ����ʱ1����
    }
}
