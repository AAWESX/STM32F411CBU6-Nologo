/***************************************************************
 * 文件名: xBSP_IIC.c
 * 版本号: v1.0
 * 更新时间: 2024-07-23
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 本文件实现了IIC接口的初始化、数据读写等基本功能，支持标准IIC通信协议。
 * 主要提供了IIC的起始、停止信号产生、字节发送与接收、ACK/NACK应答处理等函数。
 * 这些功能适用于嵌入式系统中需要通过IIC总线与外部设备通信的场景。
 * 
 * 更新内容:
 * v1.0 2024-07-23
 *   - 添加了`IIC_Init`函数，初始化IIC的SCL和SDA引脚。
 *   - 实现了`IIC_Start`和`IIC_Stop`函数，用于生成IIC的起始和停止信号。
 *   - 增加了`IIC_Wait_Ack`、`IIC_Ack`、`IIC_NAck`函数，用于处理ACK/NACK应答。
 *   - 实现了字节的发送和接收函数`IIC_Send_Byte`和`IIC_Read_Byte`。
 ***************************************************************/
 

#include "xBSP_IIC/xBSP_iic.h"
#include "xBSP_Delay/xBSP_delay.h"

/**
 * @brief  初始化IIC接口
 * @param  NULL
 * @return NULL
 * @note   配置SCL和SDA引脚为输出模式，并初始化为高电平。
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();   // 使能GPIOB时钟

    // 配置 SCL 引脚为输出模式
    GPIO_Initure.Pin = SCL_Pin;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_Initure);

    // 配置 SDA 引脚为输出模式
    GPIO_Initure.Pin = SDA_Pin;
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_Initure);

    // 初始化为高电平（取消片选）
    IIC_SDA(1);
    IIC_SCL(1);
}

/**
 * @brief  产生IIC起始信号
 * @param  NULL
 * @return NULL
 * @note   当时钟线为高电平时，数据线从高电平跳变为低电平，表示IIC起始信号。
 */
void IIC_Start(void)
{
    SDA_OUT();     // SDA线配置为输出
    IIC_SDA(1);	  
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);    // START: 当CLK为高电平时，数据线从高变为低
    delay_us(4);
    IIC_SCL(0);    // 钳住I2C总线，准备发送或接收数据 
}

/**
 * @brief  产生IIC停止信号
 * @param  NULL
 * @return NULL
 * @note   当时钟线为高电平时，数据线从低电平跳变为高电平，表示IIC停止信号。
 */
void IIC_Stop(void)
{
    SDA_OUT();    // SDA线配置为输出
    IIC_SCL(0);
    IIC_SDA(0);   // STOP: 当CLK为高电平时，数据线从低变为高
    delay_us(4);
    IIC_SCL(1); 
    IIC_SDA(1);   // 发送I2C总线结束信号
    delay_us(4);							   	
}

/**
 * @brief  等待应答信号到来
 * @param  NULL
 * @return 1 - 接收应答失败
 *         0 - 接收应答成功
 * @note   在IIC总线上等待从机的ACK应答信号，如果等待超时则返回失败。
 */
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN();      // SDA线配置为输入  
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
    IIC_SCL(0); // 时钟输出0
    return 0;  
}

/**
 * @brief  产生ACK应答
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
 * @brief  不产生ACK应答
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
 * @brief  IIC发送一个字节
 * @param  txd - 要发送的字节
 * @return NULL
 */
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
    SDA_OUT(); 	    
    IIC_SCL(0); // 拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {              
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1; 	  
        delay_us(2);   // 对TEA5767这三个延时都是必须的
        IIC_SCL(1);
        delay_us(2); 
        IIC_SCL(0);	
        delay_us(2);
    }	 
}

/**
 * @brief  读1个字节
 * @param  ack - ack=1时，发送ACK，ack=0时，发送nACK
 * @return 读取到的字节
 */
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); // SDA线配置为输入
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
        IIC_NAck(); // 发送nACK
    else
        IIC_Ack();  // 发送ACK
    return receive;
}


