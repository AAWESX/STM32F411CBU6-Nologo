#ifndef _ADC_H_
#define _ADC_H_

#include "main.h"

#include "adc.h"

#define ADC1_Control 1 /* ADC1 DMAʹ��λ */
#define ADC2_Control 0 /* ADC2 DMAʹ��λ */
#define ADC3_Control 0 /* ADC3 DMAʹ��λ */

#define ADC_Double_Control 1 /* ˫����ģʽ DMAʹ��λ  */

#define ADC_NCCM_Control 1   /* �������ɼ�ģʽ���� 0Ϊ����ģʽ��1������ģʽ*/


#if ADC1_Control 
extern ADC_HandleTypeDef hadc1;
#endif

#if ADC2_Control 
extern ADC_HandleTypeDef hadc2;
#endif

#if ADC3_Control 
extern ADC_HandleTypeDef hadc3;
#endif

extern uint16_t ADC_Samplink_STA_A;

#if ADC_Double_Control
extern uint16_t ADC_Samplink_STA_B;
#endif

#if ADC1_Control 
#define ADC1_MAX_SAMPLING_LENGTH  9  // ����������������������
#define  ADC1_SPL_STA_FLAG (0x01<<0)				/*DMA ����״̬*/
extern uint16_t ADC1_SPL_Buffer_A[ADC1_MAX_SAMPLING_LENGTH];	/* �������� */
	#if ADC_Double_Control
	extern uint16_t ADC1_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH];	/* �������� */
	#endif
#endif



#if ADC2_Control 
#define ADC2_MAX_SAMPLING_LENGTH  9  // ����������������������
#define  ADC2_SPL_STA_FLAG (0x01<<1)				/*DMA ����״̬*/
extern uint16_t ADC2_SPL_Buffer[ADC2_MAX_SAMPLING_LENGTH];	/* �������� */
#endif


#if ADC3_Control 
#define ADC3_MAX_SAMPLING_LENGTH  9  // ����������������������
#define  ADC3_SPL_STA_FLAG (0x01<<2)				/*DMA ����״̬*/
extern uint16_t ADC3_SPL_Buffer[ADC3_MAX_SAMPLING_LENGTH];	/* �������� */
#endif



void ADC_DMA_Start(ADC_HandleTypeDef *hadc,uint16_t Sampling_Length,uint16_t Sampling_Cnt);
uint16_t ADC_get_Value(void);



#endif

