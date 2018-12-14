#include "./rcc/bsp_clkconfig.h"
#include "stm32f4xx_hal_rcc.h"

/*
 * 使用HSE时，设置系统时钟的步骤
 * 1、开启HSE ，并等待 HSE 稳定
 * 2、设置 AHB、APB2、APB1的预分频因子
 * 3、设置PLL的时钟来源
 *    设置VCO输入时钟 分频因子        m
 *    设置VCO输出时钟 倍频因子        n
 *    设置PLLCLK时钟分频因子          p
 *    设置OTG FS,SDIO,RNG时钟分频因子 q
 * 4、开启PLL，并等待PLL稳定
 * 5、把PLLCK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 */

/*
 * m: VCO输入时钟 分频因子，取值2~63
 * n: VCO输出时钟 倍频因子，取值50~432
 * p: PLLCLK时钟分频因子  ，取值2，4，6，8
 * q: OTG FS,SDIO,RNG时钟分频因子，取值4~15
 * 函数调用举例，使用HSE设置时钟
 * SYSCLK=HCLK=180MHz,PCLK2=HCLK/2=84MHz,PCLK1=HCLK/4=42MHz
 * HSE_SetSysClock(25, 336, 2, 7);
 * HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法
 */
void HSE_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q)	
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}



/*
 * 使用HSE时，设置系统时钟的步骤
 * 1、开启HSE ，并等待 HSE 稳定
 * 2、设置 AHB、APB2、APB1的预分频因子
 * 3、设置PLL的时钟来源
 *    设置VCO输入时钟 分频因子        m
 *    设置VCO输出时钟 倍频因子        n
 *    设置PLLCLK时钟分频因子          p
 *    设置OTG FS,SDIO,RNG时钟分频因子 q
 * 4、开启PLL，并等待PLL稳定
 * 5、把PLLCK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 */

/*
 * m: VCO输入时钟 分频因子，取值2~63
 * n: VCO输出时钟 倍频因子，取值50~432
 * p: PLLCLK时钟分频因子  ，取值2，4，6，8
 * q: OTG FS,SDIO,RNG时钟分频因子，取值4~15
 * 函数调用举例，使用HSE设置时钟
 * SYSCLK=HCLK=180MHz,PCLK2=HCLK/2=84MHz,PCLK1=HCLK/4=42MHz
 * HSE_SetSysClock(16, 336, 2, 7);
 * HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法
 */
void HSI_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q)	
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}






