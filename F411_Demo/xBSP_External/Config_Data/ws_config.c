/***********************************************************
 * �ļ�����: ws_config.c
 * �汾��: v1.0
 * ����ʱ��: 2024-08-20
 * ��������: Xiang Likang
 * �ʼ���ַ: 3265103350@qq.com
 * 
 * ����:
 * ���ļ��������ϵͳ�������ݵı������ȡ���ṩϵͳ������Ϣ�ṹ��Ĵ�ȡ������
 * ��ģ��ͨ�������ػ��Ƴ�ʼ��������Ϣ�������������ݷ����仯ʱ�Զ����档
 * ������״��ϵ磬���ʼ��Ĭ�����ã�����¼����������
 * 
 * ��������:
 * v1.0 2024-08-20
 *   - ʵ����ϵͳ������Ϣ�ĳ�ʼ���������ع��ܡ�
 *   - �ṩ�˱������ȡϵͳ������Ϣ�Ľӿڡ�
 *   - �����˿�������ͳ�ƹ��ܡ�
 ************************************************************/

#include "ws_config.h"

// �����ر�־
char configInfiInitIfg = 0;

// ϵͳ������Ϣ����
WS_ConfigInfoTypeDef systemConfigInfo;

/*
 * �� �� ��: WS_Config_System_Save_Timer
 * �� �� �� ��: �����������ݽṹ��
 * ��    ��: data - δʹ�ò���
 * ��    ��: ��
 */
int WS_Config_System_Save_Timer(void *data)
{
    data = data;
    WS_Config_Write_Struct_Callback((uint8_t *)&systemConfigInfo, sizeof(systemConfigInfo));
    return 0;
}

/*
 * �� �� ��: WS_Config_System_Save
 * �� �� �� ��: ����������������
 * ��    ��: ��
 * ��    ��: ��
 */
void WS_Config_System_Save(void)
{
    WS_Config_System_Save_Timer(0);
    // CreateTimer(WS_Config_System_Save_Timer, 100);
}

/*
 * �� �� ��: WS_System_Config_Init
 * �� �� �� ��: ��ȡ�������ݳ�ʼ��������ǵ�һ���ϵ�����Ĭ������
 * ��    ��: ��
 * ��    ��: ��
 */
void WS_System_Config_Init(void)
{
    // ��������Ϣ
    configInfiInitIfg = 1; // ��ʼ����־

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
        systemConfigInfo.powerOnCount++; // ��¼��������
        WS_Config_System_Save();         // ������Ϣ
        // Uart_printf(&huart6,"Open count : %d\r\n",systemConfigInfo.powerOnCount);
    }
}

/*
 * �� �� ��: WS_Config_Read_Struct_Callback
 * �� �� �� ��: ��ȡϵͳ������Ϣ���麯��������ʵ��Ӧ����ʵ��
 * ��    ��: date - ������Ϣ����ָ��
 *          len  - ������Ϣ����
 * ��    ��: ��
 */
__weak void WS_Config_Read_Struct_Callback(uint8_t *date, uint32_t len)
{
    // �麯�����û�����ʵ��Ӧ����ʵ��
}

/*
 * �� �� ��: WS_Config_Write_Struct_Callback
 * �� �� �� ��: д��ϵͳ������Ϣ���麯��������ʵ��Ӧ����ʵ��
 * ��    ��: date - ������Ϣ����ָ��
 *          len  - ������Ϣ����
 * ��    ��: ��
 */
__weak void WS_Config_Write_Struct_Callback(uint8_t *date, uint32_t len)
{
    // �麯�����û�����ʵ��Ӧ����ʵ��
}
