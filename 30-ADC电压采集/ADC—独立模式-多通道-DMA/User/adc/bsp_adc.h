#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"

#define RHEOSTAT_NOFCHANEL      3

/*=====================通道1 IO======================*/
// PC3 通过调帽接电位器
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT1             GPIOB
#define RHEOSTAT_ADC_GPIO_PIN1              GPIO_PIN_0
#define RHEOSTAT_ADC_GPIO_CLK1_ENABLE()     __GPIOB_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL1               ADC_CHANNEL_8
/*=====================通道2 IO ======================*/
// PA4 通过调帽接光敏电阻
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT2             GPIOB
#define RHEOSTAT_ADC_GPIO_PIN2              GPIO_PIN_1
#define RHEOSTAT_ADC_GPIO_CLK2_ENABLE()     __GPIOB_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL2               ADC_CHANNEL_9
/*=====================通道3 IO ======================*/
// PA6 悬空，可用杜邦线接3V3或者GND来实验
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT3             GPIOA
#define RHEOSTAT_ADC_GPIO_PIN3              GPIO_PIN_6
#define RHEOSTAT_ADC_GPIO_CLK3_ENABLE()     __GPIOA_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL3               ADC_CHANNEL_6
   
// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC1
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()       __DMA2_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_CHANNEL            DMA_CHANNEL_0
#define RHEOSTAT_ADC_DMA_STREAM             DMA2_Stream0



void Rheostat_Init(void);

#endif /* __BSP_ADC_H */



