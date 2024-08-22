#ifndef _MYIIC_H
#define _MYIIC_H
#include "main.h"


// ���� SDA ���ŵĶ˿ں����ź�
#define SDA_GPIO_Port GPIOC
#define SDA_Pin GPIO_PIN_7
#define SCL_GPIO_Port GPIOC
#define SCL_Pin GPIO_PIN_6

// ���� SDA Ϊ����ģʽ
#define SDA_IN()  { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = SDA_Pin; \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct); \
}

// ���� SDA Ϊ���ģʽ
#define SDA_OUT() { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = SDA_Pin; \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; \
    HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct); \
}

// ���� SCL �� SDA ��״̬
#define IIC_SCL(state) HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define IIC_SDA(state) HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)

// ��ȡ SDA ��״̬
#define READ_SDA() HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

