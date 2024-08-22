/***************************************************************
 * 文件名: xBSP_adc.c
 * 版本号: 1.0
 * 更新时间: 2024-08-22
 * 更新人物: Xiang Likang
 * 邮件地址: 3265103350@qq.com
 * 
 * 描述:
 * 这个文件包含了与ADC（模数转换器）相关的功能实现，包括初始化ADC、启动DMA采样和处理ADC转换完成的回调函数。
 * 如果使用双缓冲模式（ADC_Double_Control 宏定义），则可以处理双缓冲的数据传输和回调。
 * 
 * 更新内容:
 * 1.0V 2024-08-22
 *  - 添加了双缓冲模式下的DMA启动和回调处理函数。
 *  - 实现了ADC转换完成的处理函数，根据不同的ADC实例（ADC1、ADC2、ADC3）处理采样数据。
 * 待更新
 * __HAL_DMA_GET_FLAG(hadc->DMA_Handle, DMA_FLAG_TCIF0_4);// 全传输完成中断标志
 * __HAL_DMA_CLEAR_FLAG(hadc->DMA_Handle, DMA_FLAG_TCIF0_4);// 清除全传输完成中断标志
 ***************************************************************/


#include "xBSP_Adc/xBSP_adc.h"
#include "usbd_cdc_if.h"

uint16_t ADC_Samplink_STA_A = 0;

#if ADC_Double_Control
	uint16_t ADC_Samplink_STA_B = 0;
#endif

#if ADC1_Control 
uint16_t ADC1_SPL_Length; 	/* 采样长度 */
uint16_t ADC1_SPL_Cnt;      /* 采样次数 */
uint16_t ADC1_SPL_Buffer_A[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* 采样数据_A */
	#if ADC_Double_Control
	uint16_t ADC1_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* 采样数据_B */
	
	#endif
#endif


#if ADC2_Control 
uint16_t ADC2_SPL_Length; 	/* 采样长度 */
uint16_t ADC2_SPL_Cnt; /* 采样次数 */
uint16_t ADC2_SPL_Buffer[ADC2_MAX_SAMPLING_LENGTH] = {0};	/* 采样数据_A */
	#if ADC_Double_Control
	uint16_t ADC2_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* 采样数据_B */
	#endif
#endif

#if ADC3_Control 
uint16_t ADC3_SPL_Length; 	/* 采样长度 */
uint16_t ADC3_SPL_Cnt; /* 采样次数 */
uint16_t ADC3_SPL_Buffer[ADC3_MAX_SAMPLING_LENGTH] = {0};	/* 采样数据_A */
	#if ADC_Double_Control
	uint16_t ADC3_SPL_Buffer_B[ADC1_MAX_SAMPLING_LENGTH] = {0};	/* 采样数据_B */
	#endif
#endif


//启动函数，需要在main中调用一次
void ADC_DMA_Start(ADC_HandleTypeDef *hadc,uint16_t Sampling_Length,uint16_t Sampling_Cnt)
{
#if ADC1_Control 
	if(hadc->Instance == ADC1)
	{
		ADC1_SPL_Length = Sampling_Length;
		ADC1_SPL_Cnt = Sampling_Cnt; 		
		
#if ADC_Double_Control
		// 配置 DMA 双缓冲模式
		HAL_DMAEx_MultiBufferStart((&hadc1)->DMA_Handle, 
															 (uint32_t)&ADC1->DR, 
															 (uint32_t)ADC1_SPL_Buffer_A, 
															 (uint32_t)ADC1_SPL_Buffer_B, 
																ADC1_SPL_Length * ADC1_SPL_Cnt);
		
		__HAL_DMA_ENABLE_IT((&hadc1)->DMA_Handle,DMA_IT_HT); // 启用半传输完成中断
#endif
		__HAL_DMA_ENABLE_IT((&hadc1)->DMA_Handle,DMA_IT_TC);  //启用全传输完成中断	
		// 启动DMA
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_SPL_Buffer_A, ADC1_SPL_Length * ADC1_SPL_Cnt);


	}
#endif
}


#if ADC_Double_Control

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{	
	
#if ADC1_Control 
    if (hadc->Instance == ADC1) 
		{
        ADC_Samplink_STA_A |= ADC1_SPL_STA_FLAG;
		}
#endif
		
#if ADC2_Control 
		if (hadc->Instance == ADC2) 
    {
        ADC_Samplink_STA_A |= ADC2_SPL_STA_FLAG;
    }
#endif
		
#if ADC3_Control 
		if (hadc->Instance == ADC3) 
    {
        ADC_Samplink_STA_A |= ADC3_SPL_STA_FLAG;
    }
#endif
		
}


void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) 
{	
	
#if ADC1_Control 
		if (hadc->Instance == ADC1) 
    {
        ADC_Samplink_STA_B |= ADC1_SPL_STA_FLAG;
    }
#endif
		
#if ADC2_Control 
		if (hadc->Instance == ADC2) 
    {
        ADC_Samplink_STA_B |= ADC2_SPL_STA_FLAG;
		}
#endif
		
#if ADC3_Control 
		if (hadc->Instance == ADC3) 
    {
        ADC_Samplink_STA_B |= ADC3_SPL_STA_FLAG;
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
    //开启ADC1
		HAL_ADC_Start(&hadc1);
    //等待ADC转换完成，超时为100ms
    HAL_ADC_PollForConversion(&hadc1,100);
    //判断ADC是否转换成功
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC)){
         //读取值
       return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}
