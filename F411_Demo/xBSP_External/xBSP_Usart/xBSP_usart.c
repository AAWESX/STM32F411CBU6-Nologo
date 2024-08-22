/***************************************************************
 * �ļ���: xBSP_Usart.c
 * �汾��: v1.1
 * ����ʱ��: 2024-08-20
 * ��������: Xiang Likun
 * �ʼ���ַ: xianglikun@example.com
 * 
 * ����:
 * ���ļ���Ҫ���𴮿�ͨ�ŵĳ�ʼ�������ݴ����������ڵ�DMA���ա����͡��͸�ʽ��������ܡ�
 * ��֧�ֶ��ִ��ڵ�DMAͨ�ţ��������Դ�ӡ���ܡ�ѭ�����͵�����Ϣ����ʽ������ַ����Լ���ָ�����ݵĽ����ȡ�
 * 
 * ��������:
 * v1.0 (2024-06-24):
 *   - ��ɻ������ڳ�ʼ�����ܣ�֧��USART1��USART6��DMA�������жϴ���
 *   - ʵ��Uart_printf������֧�ָ�ʽ�����������
 *   - ��ӵ�����Ϣѭ�����͹��ܣ���֧�ֶ�ͨ�����ս�����
 * 
 * v1.1 (2024-08-20):
 *   - �Ż�������Ϣ��ѭ�����ͻ��ƣ���߲�������������
 *   - �޸����DMA����ʱ�Ļ�����������⡣
 *   - ��Ӷ�HAL_UART_RxCpltCallback�ص��ĸĽ���ʵ�ִ������ݽ��պ���Զ��崦��
 *   - ���Clear_Uart_Buffer������������ս��ջ�������
 *   - ���ӶԸ������ݽ�����Parse_Data������֧�����������������ַ�����������ȡ��
 ***************************************************************/

#include "xBSP_Usart/xBSP_usart.h"

#include "usart.h"

/* ��׼ͷ�ļ� */
#include <string.h>
#include <stdarg.h>		  
#include <stdio.h>
#include <stdlib.h>


#if  DEBUG_EN

////�ض���fputc���� 
int fputc(int ch, FILE *f)
{  
		HAL_UART_Transmit(&huart1,(uint8_t *)ch,sizeof(ch),10);
//	while((USART1->SR&0X40) == 0); //ѭ������,ֱ���������   
//    USART1->DR = (uint8_t) ch;
	return ch;
}



//   ��ʽ���ɱ�����ַ�������
char  formatBuf[256];
//   ��ʽ���ɱ�����ַ���
char *XK_Format(char *p,...)
{
    va_list ap;
		va_start(ap,p);
		vsprintf(formatBuf,p,ap);
		va_end(ap);
	  return formatBuf;
}


//   ��ʽ���ɱ�����ַ�������
#define   DEBUG_LOOP_LEN 10
char      debugBuf[DEBUG_LOOP_LEN][128];
uint16_t  loopCount   = 0 ;
uint16_t  loopTxCount = 0 ;
//   ��ʽ������ĵ��Ժ���
void XK_printf(char *p,...)   // �� printf  һ��һ���Ĺ��� 
{
	  /*  ��ʽ������  */
    va_list ap;
		va_start(ap,p);
		vsprintf(debugBuf[loopCount],p,ap);
		va_end(ap);
	  if(++loopCount == DEBUG_LOOP_LEN ) loopCount = 0;	
//  �������	
#ifdef DEBUG_EN    
	  if(huart1.gState == HAL_UART_STATE_READY) 
		{
		  XK_Debug_Loop_Transmit(); 
		}			
#endif	 
}

//   ѭ�����͵�����Ϣ
void XK_Debug_Loop_Transmit(void)
{
    if(loopTxCount != loopCount)
		{
		    HAL_UART_Transmit_IT(&huart1,(uint8_t *)debugBuf[loopTxCount],strlen(debugBuf[loopTxCount])); //  ��Ϣ����ӿ�
		    
			  if(++ loopTxCount == DEBUG_LOOP_LEN)
					loopTxCount = 0;
		}
}


//  �����ַ����ڵ���ָ�������ַ���
//  p_stepString : �������ַ���
//  index        : ��Ҫ���ҵ��ַ����±�,1~XXXX;
//  p_RxBuf      ; ������ҵ����ַ���
char XK_Find_Index_String(char *p_stepString , int index, char *p_RxBuf)
{
	   int n = 0      ;     //  ��¼�����ַ���
	   int strnum = 0 ;     //  ��¼�����ַ�����
	   p_RxBuf[0]  = 0;     //  ���
     while(n < STEP_STRING_MAX_LEN && *p_stepString != 0) 
		 {  
				if((*p_stepString  == STOP_CHAR) ||  n == 0)
				{
				   strnum ++;
					 if(strnum == index) //  �ҵ�ƥ���±�
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

uint16_t  UART_Receive_STA = 0;/* ����״̬��־ */
uint16_t  UART_Receive_Len = 0;/* �������ݳ��� */

#if USART_DMA1
	uint8_t uart1_Data[uart1Buffer_Len]; 	/* ���ݻ����� */
#endif

#if USART_DMA2
	uint8_t uart2_Data[uart2Buffer_Len];	/* ���ݻ����� */
#endif

#if USART_DMA3
	uint8_t uart3_Data[uart3Buffer_Len];	/* ���ݻ����� */
#endif

#if USART_DMA4
	uint8_t uart4_Data[uart4Buffer_Len];	/* ���ݻ����� */
#endif

#if USART_DMA5
	uint8_t uart5_Data[uart5Buffer_Len];	/* ���ݻ����� */
#endif

#if USART_DMA6
	uint8_t uart6_Data[uart6Buffer_Len];	/* ���ݻ����� */
#endif

/* *******************************************************************
 * @name  void Uart_init(UART_HandleTypeDef *huart)
 * @brief ��ʼ�������ж��Լ�DMA
 * @param  *huart ��ʼ����Ӧ���ھ��
 * @return NULL
 * @date 2024-06-24
 * *******************************************************************/
void Uart_init(UART_HandleTypeDef *huart)
{
	
#if USART_DMA1
	if (huart->Instance == USART1)
	{
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);	/* ����1�жϳ�ʼ�� ��� */
		HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_Data, sizeof(uart1_Data));/* ����1 DMA���ճ�ʼ�� ��� */
	}
#endif
	
#if USART_DMA2
	if (huart->Instance == USART2)
	{
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);	/* ����2�жϳ�ʼ�� ��� */
		HAL_UART_Receive_DMA(&huart2, (uint8_t *)uart2_Data, sizeof(uart2_Data));/* ����2 DMA���ճ�ʼ�� ��� */
	}
#endif

#if USART_DMA3
	if (huart->Instance == USART3)
	{
		__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);	/* ����3�жϳ�ʼ�� ��� */
		HAL_UART_Receive_DMA(&huart3, (uint8_t *)uart3_Data, sizeof(uart3_Data));/* ����3 DMA���ճ�ʼ�� ��� */
	}
#endif

#if USART_DMA4
	if (huart->Instance == USART4)
	{
		__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);	/* ����4�жϳ�ʼ�� ��� */
		HAL_UART_Receive_DMA(&huart4, (uint8_t *)uart4_Data, sizeof(uart4_Data));/* ����4 DMA���ճ�ʼ�� ��� */
	}
#endif

#if USART_DMA5
	if (huart->Instance == USART5)
	{
		__HAL_UART_ENABLE_IT(&huart5,UART_IT_IDLE);	/* ����5�жϳ�ʼ�� ��� */
			HAL_UART_Receive_DMA(&huart5, (uint8_t *)uart5_Data, sizeof(uart5_Data));/* ����5 DMA���ճ�ʼ�� ��� */
	}
#endif

#if USART_DMA6
	if (huart->Instance == USART6)
	{
		__HAL_UART_ENABLE_IT(&huart6,UART_IT_IDLE);	/* ����6�жϳ�ʼ�� ��� */
			HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart6_Data, sizeof(uart6_Data));/* ����6 DMA���ճ�ʼ�� ��� */
	}
#endif

}




/* *******************************************************************
 * @name  void User_Printf(UART_HandleTypeDef *huart,char *p,...)
 * @brief  ʹ��ָ���Ĵ��ڸ�ʽ�����
 * @param  *huart ������������ݵĴ��ھ��
					*p, ...����Ҫ���������
 * @return��NULL
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
 * @brief DMA�������ݻص�
 * @param   *huart ��ʼ����Ӧ���ھ��
 * @return NULL
 * @date 2024-06-24
 * *******************************************************************/
void Uart_Receive(UART_HandleTypeDef *huart)
{
	uint32_t size;

	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)/* �ж��Ƿ� ����һ֡���� ��־λ */
	{
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_IDLE | UART_FLAG_RXNE);	// ��֡�жϱ�־ UART_FLAG_IDLE   ���ֽ��жϱ�־ UART_FLAG_RXNE
		
		//���Ĵ����������־
		size = huart->Instance->DR;		// �崮�����ݼĴ���
		size = huart->Instance->SR;		// �崮��״̬�Ĵ���
		
		HAL_UART_DMAStop(huart);/* ֹͣ ����3 DMA ����  */
		
		size = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);	//�����������֡����
		huart->RxXferCount = size;	//�浽���ڽ��սṹ���Ա��
		
		UART_Receive_Len = sizeof(uart6_Data);/* ��¼ ����6 �������� */
		
#if USART_DMA1
	if(huart->Instance == USART1)
	{
		UART_Receive_Len = sizeof(uart1_Data);/* ��¼ ����1 �������� */
		HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_Data, sizeof(uart1_Data));/* ���� ����1 DMA */
	}
#endif

#if USART_DMA2
	if (huart->Instance == USART2)
	{
		HAL_UART_Receive_DMA(&huart2, (uint8_t *)uart2_Data, sizeof(uart2_Data));/* ���� ����2 DMA */
	}
#endif

#if USART_DMA3
	if (huart->Instance == USART3)
	{
		HAL_UART_Receive_DMA(&huart3, (uint8_t *)uart3_Data, sizeof(uart3_Data));/* ���� ����3 DMA */
	}
#endif

#if USART_DMA4
	if (huart->Instance == USART4)
	{
		HAL_UART_Receive_DMA(&huart4, (uint8_t *)uart4_Data, sizeof(uart4_Data));/* ���� ����4 DMA */
	}
#endif

#if USART_DMA5
	if (huart->Instance == USART5)
	{
		HAL_UART_Receive_DMA(&huart5, (uint8_t *)uart5_Data, sizeof(uart5_Data));/* ���� ����5 DMA */
	}
#endif

#if USART_DMA6
	if (huart->Instance == USART6)
	{
		HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart6_Data, sizeof(uart6_Data));/* ���� ����6 DMA */
	}
#endif
	HAL_UART_RxCpltCallback(huart);
	
	}
	
	
	
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)		// ���ջص�����
{
	
#if USART_DMA1
	if(huart->Instance == USART1)
	{
		UART_Receive_STA = UART_Receive_STA | UART1_RX_STA_FLAG;/* ���ڽ��ձ�־ */
	}
#endif

#if USART_DMA2
	if(huart->Instance == USART2)
	{
		UART_Receive_STA = UART_Receive_STA | UART2_RX_STA_FLAG;/* ���ڽ��ձ�־ */
	}
#endif

#if USART_DMA3
	if(huart->Instance == USART3)
	{
		UART_Receive_STA = UART_Receive_STA | UART3_RX_STA_FLAG;/* ���ڽ��ձ�־ */
	}
#endif

#if USART_DMA4
	if(huart->Instance == USART4)
	{
		UART_Receive_STA = UART_Receive_STA | UART4_RX_STA_FLAG;/* ���ڽ��ձ�־ */
	}
#endif

#if USART_DMA5
	if(huart->Instance == USART5)
	{
		UART_Receive_STA = UART_Receive_STA | UART5_RX_STA_FLAG;/* ���ڽ��ձ�־ */
	}
#endif
	
#if USART_DMA6
	if(huart->Instance == USART6)
	{
		UART_Receive_STA = UART_Receive_STA | UART6_RX_STA_FLAG;/* ���ڽ��ձ�־ */
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
	buffer[sizeof(buffer) - 1] = '\0'; // ȷ���ַ����� '\0' ����

	
// ʹ�� strtok �ָ��ַ���
	char* token = strtok((char *)buffer, " ");
	while (token != NULL) {
			if (strncmp(token, label, Data_long) == 0) {
					*Returns_Data = atoi(token + Data_long);
					return; // �ҵ�������Ŀ���ǩ�󷵻�
			}
			token = strtok(NULL, " ");
	}
}



/*
 * @name  Parse_Data
 * @brife ���޲������ͣ��������ݺ���
 * @param data:����������
 * @param num_items: ���ݳ���
 * @param items: �����б�
 * @return SUCCESS:�ɹ� FAILURE:ʧ��
 * ʵ����
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
                            // ȷ���ַ������ᳬ��������
                            str_address[items[i].max_length - 1] = '\0';
                        } else {
                            return FAILURE;
                        }
                    }
                }
            }
        }
        // �ƶ�����һ����ǩ
        while (*ptr != ' ' && *ptr != '\0') {
            ptr++;
        }
        if (*ptr == ' ') {
            ptr++;
        }
    }
    return SUCCESS;
}





