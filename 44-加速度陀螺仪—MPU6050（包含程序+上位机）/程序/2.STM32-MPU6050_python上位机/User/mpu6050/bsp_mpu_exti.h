#ifndef __MPU_EXTI_H
#define	__MPU_EXTI_H

#include "stm32f4xx.h"


#define MPU_INT_GPIO_PORT                GPIOF
#define MPU_INT_GPIO_CLK_ENABLE()        __GPIOF_CLK_ENABLE()
#define MPU_INT_GPIO_PIN                 GPIO_PIN_10
#define MPU_INT_EXTI_IRQ                 EXTI15_10_IRQn

#define MPU_IRQHandler                   EXTI15_10_IRQHandler

void EXTI_MPU_Config(void);

#endif /* __EXTI_H */
