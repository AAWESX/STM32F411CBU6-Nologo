

#include "xBSP_Adc/xBSP_adc.h"
#include "usbd_cdc_if.h"

uint16_t ADC_Samplink_STA_A = 0;

#if ADC_Double_Control
	uint16_t ADC_Samplink_STA_B = 0;
#endif

#if ADC1_Control 
uint16_t ADC1_SPL_Length; 	/* �������� */
uint16_t ADC1_SPL_Cnt;      /* �������� */
uint16_t ADC1_SPL_Buffer_A[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* ��������_A */
	#if ADC_Double_Control
	uint16_t ADC1_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* ��������_B */
	
	#endif
#endif


#if ADC2_Control 
uint16_t ADC2_SPL_Length; 	/* �������� */
uint16_t ADC2_SPL_Cnt; /* �������� */
uint16_t ADC2_SPL_Buffer[ADC2_MAX_SAMPLING_LENGTH] = {0};	/* ��������_A */
	#if ADC_Double_Control
	uint16_t ADC2_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* ��������_B */
	#endif
#endif

#if ADC3_Control 
uint16_t ADC3_SPL_Length; 	/* �������� */
uint16_t ADC3_SPL_Cnt; /* �������� */
uint16_t ADC3_SPL_Buffer[ADC3_MAX_SAMPLING_LENGTH] = {0};	/* ��������_A */
	#if ADC_Double_Control
	uint16_t ADC3_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* ��������_B */
	#endif
#endif


//������������Ҫ��main�е���һ��
void ADC_DMA_Start(ADC_HandleTypeDef *hadc,uint16_t Sampling_Length,uint16_t Sampling_Cnt)
{
#if ADC1_Control 
	if(hadc->Instance == ADC1)
	{
		ADC1_SPL_Length = Sampling_Length;
		ADC1_SPL_Cnt = Sampling_Cnt; 		
		
#if ADC_Double_Control
		// ���� DMA ˫����ģʽ
		HAL_DMAEx_MultiBufferStart((&hadc1)->DMA_Handle, 
															 (uint32_t)&ADC1->DR, 
															 (uint32_t)ADC1_SPL_Buffer_A, 
															 (uint32_t)ADC1_SPL_Buffer_B, 
																ADC1_SPL_Length * ADC1_SPL_Cnt);
		
		__HAL_DMA_ENABLE_IT((&hadc1)->DMA_Handle,DMA_IT_HT); // ���ð봫������ж�
#endif
		__HAL_DMA_ENABLE_IT((&hadc1)->DMA_Handle,DMA_IT_TC);  //����ȫ��������ж�	
		// ����DMA
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_SPL_Buffer_A, ADC1_SPL_Length * ADC1_SPL_Cnt);


	}
#endif
}


#if ADC_Double_Control

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{	
#if ADC1_Control 
    // Buffer1 �Ѿ�������ɣ����� Buffer1 ������

	
//		uint32_t flag_status = __HAL_DMA_GET_FLAG(hadc->DMA_Handle, DMA_FLAG_TCIF0_4);
//		USB_printf("TCIF0_4 Flag Status: %lu\r\n", flag_status);
	
    // �жϵ�ǰ����ʹ�õĻ�����ָ��
    if (hadc->Instance == ADC1) 
   {
		 			// ���ȫ��������жϱ�־
//			__HAL_DMA_CLEAR_FLAG(hadc->DMA_Handle, DMA_FLAG_TCIF0_4);
        // ��ǰ������ָ��Ϊ0����ʾ Buffer2 �Ѿ�������ɣ�DMA �л��� Buffer1
        //USB_printf("ADC1_SPL_Buffer_A: %d , %d , %d \r\n", ADC1_SPL_Buffer_A[0], ADC1_SPL_Buffer_A[1], ADC1_SPL_Buffer_A[2]);
    
        ADC_Samplink_STA_A |= ADC1_SPL_STA_FLAG;
			

		}
		
#endif
}


void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) 
{	
#if ADC1_Control 
    // Buffer2 �Ѿ�������ɣ����� Buffer2 ������
    // ����Ƿ��ǰ봫������ж�
	
//	uint32_t flag_status = __HAL_DMA_GET_FLAG(hadc->DMA_Handle, DMA_FLAG_HTIF0_4);
//	USB_printf("HTIF0_4 Flag Status: %lu\r\n", flag_status);


	
		if (hadc->Instance == ADC1) 
    {
			// ����봫������жϱ�־
//			 __HAL_DMA_CLEAR_FLAG(hadc->DMA_Handle, DMA_FLAG_HTIF0_4);
        
       
        // ��ǰ������ָ�벻Ϊ0����ʾ Buffer1 �Ѿ�������ɣ�DMA �л��� Buffer2
        //USB_printf("ADC1_SPL_Buffer_B: %d , %d , %d \r\n", ADC1_SPL_Buffer_B[0], ADC1_SPL_Buffer_B[1], ADC1_SPL_Buffer_B[2]);
        
        ADC_Samplink_STA_B |= ADC1_SPL_STA_FLAG;
			

			
    }
		
#endif
}


#else

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
#if ADC1_Control 
    if (hadc->Instance == ADC1) 
    {
#if ADC_NCCM_Control
			HAL_ADC_Stop_DMA(&hadc1);
#endif
			USB_printf("ADC Data: %d , %d , %d \r\n",ADC1_SPL_Buffer_A[0],ADC1_SPL_Buffer_A[1],ADC1_SPL_Buffer_A[2]);
			ADC_Samplink_STA_A |= ADC1_SPL_STA_FLAG;
    }
#endif

#if ADC2_Control 
    if (hadc->Instance == ADC2) 
    {
#if ADC_NCCM_Control
			HAL_ADC_Stop_DMA(&hadc2);
#endif
			USB_printf("ADC Data: %d , %d , %d \r\n",ADC2_SPL_Buffer_A[0],ADC2_SPL_Buffer_A[1],ADC2_SPL_Buffer_A[2]);
			ADC_Samplink_STA_A |= ADC2_SPL_STA_FLAG;
    }
#endif

#if ADC3_Control 
    if (hadc->Instance == ADC3) 
    {
#if ADC_NCCM_Control
			HAL_ADC_Stop_DMA(&hadc3);
#endif
			
			USB_printf("ADC Data: %d , %d , %d \r\n",ADC3_SPL_Buffer_A[0],ADC3_SPL_Buffer_A[1],ADC3_SPL_Buffer_A[2]);
			ADC_Samplink_STA_A |= ADC3_SPL_STA_FLAG;
    }
#endif
}

#endif


uint16_t ADC_get_Value(void)
{
    //����ADC1
		HAL_ADC_Start(&hadc1);
    //�ȴ�ADCת����ɣ���ʱΪ100ms
    HAL_ADC_PollForConversion(&hadc1,100);
    //�ж�ADC�Ƿ�ת���ɹ�
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC)){
         //��ȡֵ
       return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}
