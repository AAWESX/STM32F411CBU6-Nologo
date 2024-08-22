#ifndef __CORE_H__
#define __CORE_H__

#include "main.h"

#define DEBUG_EN 0 /* �жϷ������� */

#if DEBUG_EN

#define STOP_CHAR ','
#define STEP_STRING_MAX_LEN 1024

#define  XK_Status_True		    ( 0 )
#define  XK_Status_False	    ( 1 )

/* ��ʽ���ɱ�����ַ��� */  
char *XK_Format(char *p,...);
/* ������� */
void XK_printf(char *p,...);
/* ѭ�����͵�����Ϣ */
void XK_Debug_Loop_Transmit(void);
/* ���ַ��� */
char WS_Find_Index_String(char *p_stepString , int index, char *p_RxBuf);

#endif

/**************************************DMA����*************************************************/
#define USART_DMA1	0		/* ���ô���1 			  0:���ô���1 */
#define USART_DMA2	0		/* ���ô���2 			  0:���ô���2 */
#define USART_DMA3	0		/* ���ô���3 			  0:���ô���3 */
#define USART_DMA4	0		/* ���ô���4 			  0:���ô���4 */
#define USART_DMA5	0		/* ���ô���5 			  0:���ô���5 */
#define USART_DMA6	0		/* ���ô���6 			  0:���ô���6 */
/**************************************DMA����*************************************************/



/**************************************��������************************************************/

extern uint16_t  UART_Receive_STA; /* ����״̬��־ */
extern uint16_t  UART_Receive_Len; /* �������ݳ��� */


#if USART_DMA1
	#define uart1Buffer_Len 200 				 	/* ���ݻ��泤�� */
	extern uint8_t uart1_Data[uart1Buffer_Len]; 	/* ���ݻ����� */
	extern UART_HandleTypeDef huart1;		 	/* ���ھ�� */
	#define UART1_RX_STA_FLAG (0x0001<<0)	/* ���ձ�־ */
#endif

#if USART_DMA2
	#define uart2Buffer_Len 200 					/* ���ݻ��泤�� */
	extern uint8_t uart2_Data[uart2Buffer_Len];	/* ���ݻ����� */
	extern UART_HandleTypeDef huart2;			/* ���ھ�� */
	#define UART2_RX_STA_FLAG (0x0001<<1)	/* ���ձ�־ */
#endif

#if USART_DMA3
	#define uart3Buffer_Len 128 					/* ���ݻ��泤�� */
	extern uint8_t uart3_Data[uart3Buffer_Len];	/* ���ݻ����� */
	extern UART_HandleTypeDef huart3; 		/* ���ھ�� */
	#define UART3_RX_STA_FLAG (0x0001<<2)	/* ���ձ�־ */
#endif

#if USART_DMA4
	#define uart4Buffer_Len 128 					/* ���ݻ��泤�� */
	extern uint8_t uart4_Data[uart4Buffer_Len];	/* ���ݻ����� */
	extern UART_HandleTypeDef huart4;			/* ���ھ�� */
	#define UART4_RX_STA_FLAG (0x0001<<3)	/* ���ձ�־ */
#endif

#if USART_DMA5
	#define uart5Buffer_Len 128 					/* ���ݻ��泤�� */
	extern uint8_t uart5_Data[uart5Buffer_Len];	/* ���ݻ����� */
	extern UART_HandleTypeDef huart5;			/* ���ھ�� */
	#define UART5_RX_STA_FLAG (0x0001<<4)	/* ���ձ�־ */
#endif

#if USART_DMA6
	#define uart6Buffer_Len 128 					/* ���ݻ��泤�� */
	extern uint8_t uart6_Data[uart6Buffer_Len];	/* ���ݻ����� */
	extern UART_HandleTypeDef huart6;			/* ���ھ�� */
	#define UART6_RX_STA_FLAG (0x0001<<5)	/* ���ձ�־ */
#endif

/**************************************��������************************************************/

/*****************************************API**************************************************/
void Uart_init(UART_HandleTypeDef *huart);  /* �жϳ�ʼ�� */
void Uart_printf(UART_HandleTypeDef *huart,char *fmt, ...);/* ��ʽ������ */
void Clear_Uart_Buffer(UART_HandleTypeDef* huart);
void Uart_Receive(UART_HandleTypeDef *huart);  /* �����жϻص� ���Ŵ��ڻص��� */
/*****************************************API**************************************************/

#define SUCCESS 0
#define FAILURE 1


/*
 * @brife ����֧�ֽ������������� 
 */
typedef enum {
    INT_TYPE,
    FLOAT_TYPE,
    STRING_TYPE
} DataType;

/*
 * @brife ���������ṹ
 */
typedef struct {
    const char* label; /* ���ݱ�ǩ */
    DataType type;		 /* ������������ */
    void* address;     /* ���ݴ�ŵ�ַ */
		int max_length;  // ���� type Ϊ STRING_TYPE ʱʹ��
} ParseItem;


/* ���ݽ������� */
int Parse_Data(const char* data, int num_items, ParseItem items[]);
void Split_parsing(char* In_Data,char* label,int* Returns_Data,unsigned short Data_long);


#endif



