/***************************************************************
 * �ļ���: xBSP_IIC.c
 * �汾��: v1.0
 * ����ʱ��: 2024-07-23
 * ��������: Xiang Likang
 * �ʼ���ַ: 3265103350@qq.com
 * 
 * ����:
 * ���ļ�ʵ����IIC�ӿڵĳ�ʼ�������ݶ�д�Ȼ������ܣ�֧�ֱ�׼IICͨ��Э�顣
 * ��Ҫ�ṩ��IIC����ʼ��ֹͣ�źŲ������ֽڷ�������ա�ACK/NACKӦ����Ⱥ�����
 * ��Щ����������Ƕ��ʽϵͳ����Ҫͨ��IIC�������ⲿ�豸ͨ�ŵĳ�����
 * 
 * ��������:
 * v1.0 2024-07-23
 *   - �����`IIC_Init`��������ʼ��IIC��SCL��SDA���š�
 *   - ʵ����`IIC_Start`��`IIC_Stop`��������������IIC����ʼ��ֹͣ�źš�
 *   - ������`IIC_Wait_Ack`��`IIC_Ack`��`IIC_NAck`���������ڴ���ACK/NACKӦ��
 *   - ʵ�����ֽڵķ��ͺͽ��պ���`IIC_Send_Byte`��`IIC_Read_Byte`��
 ***************************************************************/
 

#include "xBSP_IIC/xBSP_iic.h"
#include "xBSP_Delay/xBSP_delay.h"

/**
 * @brief  ��ʼ��IIC�ӿ�
 * @param  NULL
 * @return NULL
 * @note   ����SCL��SDA����Ϊ���ģʽ������ʼ��Ϊ�ߵ�ƽ��
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();   // ʹ��GPIOBʱ��

    // ���� SCL ����Ϊ���ģʽ
    GPIO_Initure.Pin = SCL_Pin;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_Initure);

    // ���� SDA ����Ϊ���ģʽ
    GPIO_Initure.Pin = SDA_Pin;
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_Initure);

    // ��ʼ��Ϊ�ߵ�ƽ��ȡ��Ƭѡ��
    IIC_SDA(1);
    IIC_SCL(1);
}

/**
 * @brief  ����IIC��ʼ�ź�
 * @param  NULL
 * @return NULL
 * @note   ��ʱ����Ϊ�ߵ�ƽʱ�������ߴӸߵ�ƽ����Ϊ�͵�ƽ����ʾIIC��ʼ�źš�
 */
void IIC_Start(void)
{
    SDA_OUT();     // SDA������Ϊ���
    IIC_SDA(1);	  
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);    // START: ��CLKΪ�ߵ�ƽʱ�������ߴӸ߱�Ϊ��
    delay_us(4);
    IIC_SCL(0);    // ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**
 * @brief  ����IICֹͣ�ź�
 * @param  NULL
 * @return NULL
 * @note   ��ʱ����Ϊ�ߵ�ƽʱ�������ߴӵ͵�ƽ����Ϊ�ߵ�ƽ����ʾIICֹͣ�źš�
 */
void IIC_Stop(void)
{
    SDA_OUT();    // SDA������Ϊ���
    IIC_SCL(0);
    IIC_SDA(0);   // STOP: ��CLKΪ�ߵ�ƽʱ�������ߴӵͱ�Ϊ��
    delay_us(4);
    IIC_SCL(1); 
    IIC_SDA(1);   // ����I2C���߽����ź�
    delay_us(4);							   	
}

/**
 * @brief  �ȴ�Ӧ���źŵ���
 * @param  NULL
 * @return 1 - ����Ӧ��ʧ��
 *         0 - ����Ӧ��ɹ�
 * @note   ��IIC�����ϵȴ��ӻ���ACKӦ���źţ�����ȴ���ʱ�򷵻�ʧ�ܡ�
 */
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN();      // SDA������Ϊ����  
    delay_us(1);	   
    IIC_SCL(1); 
    delay_us(1);	 
    while(READ_SDA())
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0); // ʱ�����0
    return 0;  
}

/**
 * @brief  ����ACKӦ��
 * @param  NULL
 * @return NULL
 */
void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}

/**
 * @brief  ������ACKӦ��
 * @param  NULL
 * @return NULL
 */
void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}

/**
 * @brief  IIC����һ���ֽ�
 * @param  txd - Ҫ���͵��ֽ�
 * @return NULL
 */
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
    SDA_OUT(); 	    
    IIC_SCL(0); // ����ʱ�ӿ�ʼ���ݴ���
    for(t = 0; t < 8; t++)
    {              
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1; 	  
        delay_us(2);   // ��TEA5767��������ʱ���Ǳ����
        IIC_SCL(1);
        delay_us(2); 
        IIC_SCL(0);	
        delay_us(2);
    }	 
}

/**
 * @brief  ��1���ֽ�
 * @param  ack - ack=1ʱ������ACK��ack=0ʱ������nACK
 * @return ��ȡ�����ֽ�
 */
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); // SDA������Ϊ����
    for(i = 0; i < 8; i++)
    {
        IIC_SCL(0); 
        delay_us(2);
        IIC_SCL(1);
        receive <<= 1;
        if(READ_SDA()) receive++;   
        delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck(); // ����nACK
    else
        IIC_Ack();  // ����ACK
    return receive;
}


