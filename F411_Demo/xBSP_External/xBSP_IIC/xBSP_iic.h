#ifndef _MYIIC_H
#define _MYIIC_H
#include "main.h"


// 定义 SDA 引脚的端口和引脚号
#define SDA_GPIO_Port GPIOC
#define SDA_Pin GPIO_PIN_7
#define SCL_GPIO_Port GPIOC
#define SCL_Pin GPIO_PIN_6

// 配置 SDA 为输入模式
#define SDA_IN()  { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = SDA_Pin; \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct); \
}

// 配置 SDA 为输出模式
#define SDA_OUT() { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = SDA_Pin; \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; \
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct); \
}

// 设置 SCL 和 SDA 的状态
#define IIC_SCL(state) HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define IIC_SDA(state) HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)

// 读取 SDA 的状态
#define READ_SDA() HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin)

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

