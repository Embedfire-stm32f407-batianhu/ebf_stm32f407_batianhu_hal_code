#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

//串口波特率
#define DEBUG_USART_BAUDRATE                    115200

//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART2
#define DEBUG_USART_CLK_ENABLE()                __USART2_CLK_ENABLE();

#define RCC_PERIPHCLK_UARTx                     RCC_PERIPHCLK_USART2
#define RCC_UARTxCLKSOURCE_SYSCLK               RCC_USART2CLKSOURCE_SYSCLK

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_3
#define DEBUG_USART_RX_AF                       GPIO_AF7_USART2

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_2
#define DEBUG_USART_TX_AF                       GPIO_AF7_USART2

#define DEBUG_USART_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART_IRQ                 		    USART2_IRQn
/************************************************************/

void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef UartHandle;
#endif /* __USART1_H */
