#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"

// ADC GPIO 宏定义
#define RHEOSTAT_ADC_GPIO_PORT              GPIOB
#define RHEOSTAT_ADC_GPIO_PIN               GPIO_PIN_0
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
    
// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC1
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL                ADC_CHANNEL_8

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()       __DMA2_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_CHANNEL            DMA_CHANNEL_0
#define RHEOSTAT_ADC_DMA_STREAM             DMA2_Stream0



void Rheostat_Init(void);

#endif /* __BSP_ADC_H */



