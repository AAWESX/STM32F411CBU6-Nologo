#ifndef __CORE_H__
#define __CORE_H__

#include "main.h"

#define DEBUG_EN 0 /* 中断发送数据 */

#if DEBUG_EN

#define STOP_CHAR ','
#define STEP_STRING_MAX_LEN 1024

#define  XK_Status_True		    ( 0 )
#define  XK_Status_False	    ( 1 )

/* 格式化可变变量字符串 */  
char *XK_Format(char *p,...);
/* 调试输出 */
void XK_printf(char *p,...);
/* 循环发送调试信息 */
void XK_Debug_Loop_Transmit(void);
/* 查字符串 */
char WS_Find_Index_String(char *p_stepString , int index, char *p_RxBuf);

#endif

/**************************************DMA启动*************************************************/
#define USART_DMA1	0		/* 启用串口1 			  0:禁用串口1 */
#define USART_DMA2	0		/* 启用串口2 			  0:禁用串口2 */
#define USART_DMA3	0		/* 启用串口3 			  0:禁用串口3 */
#define USART_DMA4	0		/* 启用串口4 			  0:禁用串口4 */
#define USART_DMA5	0		/* 启用串口5 			  0:禁用串口5 */
#define USART_DMA6	0		/* 启用串口6 			  0:禁用串口6 */
/**************************************DMA启动*************************************************/



/**************************************接收数据************************************************/

extern uint16_t  UART_Receive_STA; /* 接收状态标志 */
extern uint16_t  UART_Receive_Len; /* 接收数据长度 */


#if USART_DMA1
	#define uart1Buffer_Len 200 				 	/* 数据缓存长度 */
	extern uint8_t uart1_Data[uart1Buffer_Len]; 	/* 数据缓存区 */
	extern UART_HandleTypeDef huart1;		 	/* 串口句柄 */
	#define UART1_RX_STA_FLAG (0x0001<<0)	/* 接收标志 */
#endif

#if USART_DMA2
	#define uart2Buffer_Len 200 					/* 数据缓存长度 */
	extern uint8_t uart2_Data[uart2Buffer_Len];	/* 数据缓存区 */
	extern UART_HandleTypeDef huart2;			/* 串口句柄 */
	#define UART2_RX_STA_FLAG (0x0001<<1)	/* 接收标志 */
#endif

#if USART_DMA3
	#define uart3Buffer_Len 128 					/* 数据缓存长度 */
	extern uint8_t uart3_Data[uart3Buffer_Len];	/* 数据缓存区 */
	extern UART_HandleTypeDef huart3; 		/* 串口句柄 */
	#define UART3_RX_STA_FLAG (0x0001<<2)	/* 接收标志 */
#endif

#if USART_DMA4
	#define uart4Buffer_Len 128 					/* 数据缓存长度 */
	extern uint8_t uart4_Data[uart4Buffer_Len];	/* 数据缓存区 */
	extern UART_HandleTypeDef huart4;			/* 串口句柄 */
	#define UART4_RX_STA_FLAG (0x0001<<3)	/* 接收标志 */
#endif

#if USART_DMA5
	#define uart5Buffer_Len 128 					/* 数据缓存长度 */
	extern uint8_t uart5_Data[uart5Buffer_Len];	/* 数据缓存区 */
	extern UART_HandleTypeDef huart5;			/* 串口句柄 */
	#define UART5_RX_STA_FLAG (0x0001<<4)	/* 接收标志 */
#endif

#if USART_DMA6
	#define uart6Buffer_Len 128 					/* 数据缓存长度 */
	extern uint8_t uart6_Data[uart6Buffer_Len];	/* 数据缓存区 */
	extern UART_HandleTypeDef huart6;			/* 串口句柄 */
	#define UART6_RX_STA_FLAG (0x0001<<5)	/* 接收标志 */
#endif

/**************************************接收数据************************************************/

/*****************************************API**************************************************/
void Uart_init(UART_HandleTypeDef *huart);  /* 中断初始化 */
void Uart_printf(UART_HandleTypeDef *huart,char *fmt, ...);/* 格式化发送 */
void Clear_Uart_Buffer(UART_HandleTypeDef* huart);
void Uart_Receive(UART_HandleTypeDef *huart);  /* 接收中断回调 ，放串口回调中 */
/*****************************************API**************************************************/

#define SUCCESS 0
#define FAILURE 1


/*
 * @brife 定义支持解析的数据类型 
 */
typedef enum {
    INT_TYPE,
    FLOAT_TYPE,
    STRING_TYPE
} DataType;

/*
 * @brife 定义解析项结构
 */
typedef struct {
    const char* label; /* 数据标签 */
    DataType type;		 /* 解析数据类型 */
    void* address;     /* 数据存放地址 */
		int max_length;  // 仅在 type 为 STRING_TYPE 时使用
} ParseItem;


/* 数据解析函数 */
int Parse_Data(const char* data, int num_items, ParseItem items[]);
void Split_parsing(char* In_Data,char* label,int* Returns_Data,unsigned short Data_long);


#endif



