/***************************************************************
 * 文件名: xBSP_Usart.c
 * 版本号: v1.1
 * 更新时间: 2024-08-20
 * 更新人物: Xiang Likun
 * 邮件地址: xianglikun@example.com
 * 
 * 描述:
 * 该文件主要负责串口通信的初始化和数据处理，包括串口的DMA接收、发送、和格式化输出功能。
 * 它支持多种串口的DMA通信，包含调试打印功能、循环发送调试信息、格式化输出字符串以及对指定数据的解析等。
 * 
 * 更新内容:
 * v1.0 (2024-06-24):
 *   - 完成基本串口初始化功能，支持USART1至USART6的DMA接收与中断处理。
 *   - 实现Uart_printf函数，支持格式化数据输出。
 *   - 添加调试信息循环发送功能，并支持多通道接收解析。
 * 
 * v1.1 (2024-08-20):
 *   - 优化调试信息的循环发送机制，提高并发处理能力。
 *   - 修复多次DMA接收时的缓冲区溢出问题。
 *   - 添加对HAL_UART_RxCpltCallback回调的改进，实现串口数据接收后的自定义处理。
 *   - 添加Clear_Uart_Buffer函数，用于清空接收缓冲区。
 *   - 增加对复杂数据解析的Parse_Data函数，支持整数、浮点数和字符串的数据提取。
 ***************************************************************/

#include "xBSP_Usart/xBSP_usart.h"

#include "usart.h"

/* 标准头文件 */
#include <string.h>
#include <stdarg.h>		  
#include <stdio.h>
#include <stdlib.h>


#if  DEBUG_EN

////重定义fputc函数 
int fputc(int ch, FILE *f)
{  
		HAL_UART_Transmit(&huart1,(uint8_t *)ch,sizeof(ch),10);
//	while((USART1->SR&0X40) == 0); //循环发送,直到发送完毕   
//    USART1->DR = (uint8_t) ch;
	return ch;
}



//   格式化可变变量字符串缓存
char  formatBuf[256];
//   格式化可变变量字符串
char *XK_Format(char *p,...)
{
    va_list ap;
		va_start(ap,p);
		vsprintf(formatBuf,p,ap);
		va_end(ap);
	  return formatBuf;
}


//   格式化可变变量字符串缓存
#define   DEBUG_LOOP_LEN 10
char      debugBuf[DEBUG_LOOP_LEN][128];
uint16_t  loopCount   = 0 ;
uint16_t  loopTxCount = 0 ;
//   格式化输出的调试函数
void XK_printf(char *p,...)   // 与 printf  一摸一样的功能 
{
	  /*  格式化数据  */
    va_list ap;
		va_start(ap,p);
		vsprintf(debugBuf[loopCount],p,ap);
		va_end(ap);
	  if(++loopCount == DEBUG_LOOP_LEN ) loopCount = 0;	
//  编译控制	
#ifdef DEBUG_EN    
	  if(huart1.gState == HAL_UART_STATE_READY) 
		{
		  XK_Debug_Loop_Transmit(); 
		}			
#endif	 
}

//   循环发送调试信息
void XK_Debug_Loop_Transmit(void)
{
    if(loopTxCount != loopCount)
		{
		    HAL_UART_Transmit_IT(&huart1,(uint8_t *)debugBuf[loopTxCount],strlen(debugBuf[loopTxCount])); //  信息输出接口
		    
			  if(++ loopTxCount == DEBUG_LOOP_LEN)
					loopTxCount = 0;
		}
}


//  查找字符串内的在指定数字字符串
//  p_stepString : 被查找字符串
//  index        : 需要查找的字符串下标,1~XXXX;
//  p_RxBuf      ; 缓存查找到的字符串
char XK_Find_Index_String(char *p_stepString , int index, char *p_RxBuf)
{
	   int n = 0      ;     //  记录查找字符数
	   int strnum = 0 ;     //  记录查找字符串数
	   p_RxBuf[0]  = 0;     //  清空
     while(n < STEP_STRING_MAX_LEN && *p_stepString != 0) 
		 {  
				if((*p_stepString  == STOP_CHAR) ||  n == 0)
				{
				   strnum ++;
					 if(strnum == index) //  找到匹配下标
						{
							 if(n > 0) p_stepString ++;
							 while(*p_stepString != 0 && *p_stepString != STOP_CHAR)
							 {
							   *p_RxBuf = *p_stepString;
								  p_stepString ++        ;
								  p_RxBuf ++;
							 }
							 *p_RxBuf = 0;
							 return XK_Status_True;
						}
				}
			  n ++           ;
				p_stepString ++;
		 }
	 *p_RxBuf = 0; 
	 return XK_Status_False;	 
}

#endif

uint16_t  UART_Receive_STA = 0;/* 接收状态标志 */
uint16_t  UART_Receive_Len = 0;/* 接收数据长度 */

#if USART_DMA1
	uint8_t uart1_Data[uart1Buffer_Len]; 	/* 数据缓存区 */
#endif

#if USART_DMA2
	uint8_t uart2_Data[uart2Buffer_Len];	/* 数据缓存区 */
#endif

#if USART_DMA3
	uint8_t uart3_Data[uart3Buffer_Len];	/* 数据缓存区 */
#endif

#if USART_DMA4
	uint8_t uart4_Data[uart4Buffer_Len];	/* 数据缓存区 */
#endif

#if USART_DMA5
	uint8_t uart5_Data[uart5Buffer_Len];	/* 数据缓存区 */
#endif

#if USART_DMA6
	uint8_t uart6_Data[uart6Buffer_Len];	/* 数据缓存区 */
#endif

/* *******************************************************************
 * @name  void Uart_init(UART_HandleTypeDef *huart)
 * @brief 初始化串口中断以及DMA
 * @param  *huart 初始化对应串口句柄
 * @return NULL
 * @date 2024-06-24
 * *******************************************************************/
void Uart_init(UART_HandleTypeDef *huart)
{
	
#if USART_DMA1
	if (huart->Instance == USART1)
	{
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);	/* 串口1中断初始化 完成 */
		HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_Data, sizeof(uart1_Data));/* 串口1 DMA接收初始化 完成 */
	}
#endif
	
#if USART_DMA2
	if (huart->Instance == USART2)
	{
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);	/* 串口2中断初始化 完成 */
		HAL_UART_Receive_DMA(&huart2, (uint8_t *)uart2_Data, sizeof(uart2_Data));/* 串口2 DMA接收初始化 完成 */
	}
#endif

#if USART_DMA3
	if (huart->Instance == USART3)
	{
		__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);	/* 串口3中断初始化 完成 */
		HAL_UART_Receive_DMA(&huart3, (uint8_t *)uart3_Data, sizeof(uart3_Data));/* 串口3 DMA接收初始化 完成 */
	}
#endif

#if USART_DMA4
	if (huart->Instance == USART4)
	{
		__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);	/* 串口4中断初始化 完成 */
		HAL_UART_Receive_DMA(&huart4, (uint8_t *)uart4_Data, sizeof(uart4_Data));/* 串口4 DMA接收初始化 完成 */
	}
#endif

#if USART_DMA5
	if (huart->Instance == USART5)
	{
		__HAL_UART_ENABLE_IT(&huart5,UART_IT_IDLE);	/* 串口5中断初始化 完成 */
			HAL_UART_Receive_DMA(&huart5, (uint8_t *)uart5_Data, sizeof(uart5_Data));/* 串口5 DMA接收初始化 完成 */
	}
#endif

#if USART_DMA6
	if (huart->Instance == USART6)
	{
		__HAL_UART_ENABLE_IT(&huart6,UART_IT_IDLE);	/* 串口6中断初始化 完成 */
			HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart6_Data, sizeof(uart6_Data));/* 串口6 DMA接收初始化 完成 */
	}
#endif

}




/* *******************************************************************
 * @name  void User_Printf(UART_HandleTypeDef *huart,char *p,...)
 * @brief  使用指定的串口格式化输出
 * @param  *huart ：用来输出数据的串口句柄
					*p, ...：需要输出的数据
 * @return：NULL
 * @date 2024-06-24
 * *******************************************************************/
void Uart_printf(UART_HandleTypeDef *huart,char *fmt, ...)
{
	static uint8_t USART_TX_BUF[128] = {0};
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART_TX_BUF, fmt, ap);
  va_end(ap);
	
	
	
#if USART_DMA1
	if (huart->Instance == USART1)
	{
		HAL_UART_Transmit(&huart1, USART_TX_BUF, strlen((const char *)USART_TX_BUF),100);
	}
#endif

#if USART_DMA2
	if (huart->Instance == USART2)
	{
		HAL_UART_Transmit(&huart2, USART_TX_BUF, strlen((const char *)USART_TX_BUF),100);
	}
#endif

#if USART_DMA3
	if (huart->Instance == USART3)
	{
		HAL_UART_Transmit(&huart3, USART_TX_BUF, strlen((const char *)USART_TX_BUF),100);
	}
#endif

#if USART_DMA4
	if (huart->Instance == USART4)
	{
		HAL_UART_Transmit(&huart4, USART_TX_BUF, strlen((const char *)USART_TX_BUF),100);
	}
#endif

#if USART_DMA5
	if (huart->Instance == USART5)
	{
		HAL_UART_Transmit(&huart5, USART_TX_BUF, strlen((const char *)USART_TX_BUF),100);
	}
#endif

#if USART_DMA6
	if (huart->Instance == USART6)
	{
		HAL_UART_Transmit(&huart6, USART_TX_BUF, strlen((const char *)USART_TX_BUF),100);
	}
#endif

}



/* *******************************************************************
 * @name  void Uart_Receive(UART_HandleTypeDef *huart)
 * @brief DMA接收数据回调
 * @param   *huart 初始化对应串口句柄
 * @return NULL
 * @date 2024-06-24
 * *******************************************************************/
void Uart_Receive(UART_HandleTypeDef *huart)
{
	uint32_t size;

	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)/* 判断是否 接收一帧数据 标志位 */
	{
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_IDLE | UART_FLAG_RXNE);	// 清帧中断标志 UART_FLAG_IDLE   清字节中断标志 UART_FLAG_RXNE
		
		//读寄存器以清除标志
		size = huart->Instance->DR;		// 清串口数据寄存器
		size = huart->Instance->SR;		// 清串口状态寄存器
		
		HAL_UART_DMAStop(huart);/* 停止 串口3 DMA 接收  */
		
		size = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);	//计算接收数据帧长度
		huart->RxXferCount = size;	//存到串口接收结构体成员中
		
		UART_Receive_Len = sizeof(uart6_Data);/* 记录 串口6 接收数据 */
		
#if USART_DMA1
	if(huart->Instance == USART1)
	{
		UART_Receive_Len = sizeof(uart1_Data);/* 记录 串口1 接收数据 */
		HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_Data, sizeof(uart1_Data));/* 开启 串口1 DMA */
	}
#endif

#if USART_DMA2
	if (huart->Instance == USART2)
	{
		HAL_UART_Receive_DMA(&huart2, (uint8_t *)uart2_Data, sizeof(uart2_Data));/* 开启 串口2 DMA */
	}
#endif

#if USART_DMA3
	if (huart->Instance == USART3)
	{
		HAL_UART_Receive_DMA(&huart3, (uint8_t *)uart3_Data, sizeof(uart3_Data));/* 开启 串口3 DMA */
	}
#endif

#if USART_DMA4
	if (huart->Instance == USART4)
	{
		HAL_UART_Receive_DMA(&huart4, (uint8_t *)uart4_Data, sizeof(uart4_Data));/* 开启 串口4 DMA */
	}
#endif

#if USART_DMA5
	if (huart->Instance == USART5)
	{
		HAL_UART_Receive_DMA(&huart5, (uint8_t *)uart5_Data, sizeof(uart5_Data));/* 开启 串口5 DMA */
	}
#endif

#if USART_DMA6
	if (huart->Instance == USART6)
	{
		HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart6_Data, sizeof(uart6_Data));/* 开启 串口6 DMA */
	}
#endif
	HAL_UART_RxCpltCallback(huart);
	
	}
	
	
	
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)		// 接收回调函数
{
	
#if USART_DMA1
	if(huart->Instance == USART1)
	{
		UART_Receive_STA = UART_Receive_STA | UART1_RX_STA_FLAG;/* 串口接收标志 */
	}
#endif

#if USART_DMA2
	if(huart->Instance == USART2)
	{
		UART_Receive_STA = UART_Receive_STA | UART2_RX_STA_FLAG;/* 串口接收标志 */
	}
#endif

#if USART_DMA3
	if(huart->Instance == USART3)
	{
		UART_Receive_STA = UART_Receive_STA | UART3_RX_STA_FLAG;/* 串口接收标志 */
	}
#endif

#if USART_DMA4
	if(huart->Instance == USART4)
	{
		UART_Receive_STA = UART_Receive_STA | UART4_RX_STA_FLAG;/* 串口接收标志 */
	}
#endif

#if USART_DMA5
	if(huart->Instance == USART5)
	{
		UART_Receive_STA = UART_Receive_STA | UART5_RX_STA_FLAG;/* 串口接收标志 */
	}
#endif
	
#if USART_DMA6
	if(huart->Instance == USART6)
	{
		UART_Receive_STA = UART_Receive_STA | UART6_RX_STA_FLAG;/* 串口接收标志 */
	}
#endif
	
}



void Clear_Uart_Buffer(UART_HandleTypeDef* huart) {
	#if USART_DMA1
	if(huart->Instance == USART1)
	{
		memset(uart1_Data, 0, sizeof(uart1_Data));
	}
#endif

#if USART_DMA2
	if(huart->Instance == USART2)
	{
		memset(uart2_Data, 0, sizeof(uart2_Data));
	}
#endif

#if USART_DMA3
	if(huart->Instance == USART3)
	{
		memset(uart3_Data, 0, sizeof(uart3_Data));
	}
#endif

#if USART_DMA4
	if(huart->Instance == USART4)
	{
		memset(uart4_Data, 0, sizeof(uart4_Data));
	}
#endif

#if USART_DMA5
	if(huart->Instance == USART5)
	{
		memset(uart5_Data, 0, sizeof(uart5_Data));
	}
#endif
	
#if USART_DMA6
	if(huart->Instance == USART6)
	{
		memset(uart6_Data, 0, sizeof(uart6_Data));
	}
#endif
    
}


void Split_parsing(char* In_Data,char* label,int* Returns_Data,unsigned short Data_long)
{
	char buffer[sizeof(In_Data)+1];
	strncpy(buffer, In_Data, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0'; // 确保字符串以 '\0' 结束

	
// 使用 strtok 分割字符串
	char* token = strtok((char *)buffer, " ");
	while (token != NULL) {
			if (strncmp(token, label, Data_long) == 0) {
					*Returns_Data = atoi(token + Data_long);
					return; // 找到并解析目标标签后返回
			}
			token = strtok(NULL, " ");
	}
}



/*
 * @name  Parse_Data
 * @brife 不限参数类型，解析数据函数
 * @param data:待解析数据
 * @param num_items: 数据长度
 * @param items: 数据列表
 * @return SUCCESS:成功 FAILURE:失败
 * 实例：
 *	int P, I, D;
 *	float S;
 *
 *	ParseItem tlem[] = {
 *	  {"P",INT_TYPE,&P},
 *	  {"I",INT_TYPE,&I},
 *  	{"D",INT_TYPE,&D},
 *  	{"S",FLOAT_TYPE,&S},
 *	};
 *
 *  Parse_Data( (char*) uart1_Data,  sizeof(tlem) / sizeof(tlem[0]),  tlem);
 * 	LCD_printf(30,70,24 ,"P is:%d ",P);
 *	LCD_printf(30,100,24,"I is:%d ",I);
 *  LCD_printf(30,130,24,"D is:%d ",D);
 *  LCD_printf(30,160,24,"S is:%f ",S);
 */
int Parse_Data(const char* data, int num_items, ParseItem items[]) {
    char label[32];
    const char* ptr = data;
    while (*ptr != '\0') {
        if (sscanf(ptr, "%31[^:]:", label) == 1) {
            for (int i = 0; i < num_items; i++) {
                if (strcmp(label, items[i].label) == 0) {
                    if (items[i].type == INT_TYPE) {
                        int value;
                        if (sscanf(ptr, "%*[^:]:%d", &value) == 1) {
                            *(int*)(items[i].address) = value;
                        } else {
                            return FAILURE;
                        }
                    } else if (items[i].type == FLOAT_TYPE) {
                        float value;
                        if (sscanf(ptr, "%*[^:]:%f", &value) == 1) {
                            *(float*)(items[i].address) = value;
                        } else {
                            return FAILURE;
                        }
                    } else if (items[i].type == STRING_TYPE) {
                        char* str_address = (char*)(items[i].address);
                        if (sscanf(ptr, "%*[^:]:%s", str_address) == 1) {
                            // 确保字符串不会超出缓冲区
                            str_address[items[i].max_length - 1] = '\0';
                        } else {
                            return FAILURE;
                        }
                    }
                }
            }
        }
        // 移动到下一个标签
        while (*ptr != ' ' && *ptr != '\0') {
            ptr++;
        }
        if (*ptr == ' ') {
            ptr++;
        }
    }
    return SUCCESS;
}





