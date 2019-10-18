/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   GPIO输出--使用固件库点亮LED灯
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
/* 相关宏定义，使用存储器到存储器传输必须使用DMA2 */
DMA_HandleTypeDef DMA_Handle;

#define DMA_STREAM               DMA2_Stream0
#define DMA_CHANNEL              DMA_CHANNEL_0
#define DMA_STREAM_CLOCK()       __DMA2_CLK_ENABLE()

#define BUFFER_SIZE              32

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
  const关键字将aSRC_Const_Buffer数组变量定义为常量类型 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
/* 定义DMA传输目标存储器 */
uint32_t aDST_Buffer[BUFFER_SIZE];
                                                                       
static void DMA_Config(void);                                    
static void Delay(__IO uint32_t nCount);
static void SystemClock_Config(void); 
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength); 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 定义存放比较结果变量 */
	uint8_t TransferStatus;
	/* 系统时钟初始化成168 MHz */
	SystemClock_Config();
	/* LED 端口初始化 */
	LED_GPIO_Config();
	/* 设置RGB彩色灯为紫色 */
	LED_PURPLE;  

	/* 简单延时函数 */
	Delay(0xFFFFFF);  

	/* DMA传输配置 */
	DMA_Config(); 
  
	/* 等待DMA传输完成 */
	while(__HAL_DMA_GET_FLAG(&DMA_Handle,DMA_FLAG_TCIF0_4)==DISABLE)
	{

	}   

	/* 比较源数据与传输后数据 */
	TransferStatus=Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);

	/* 判断源数据与传输后数据比较结果*/
	if(TransferStatus==0)  
	{
		/* 源数据与传输后数据不相等时RGB彩色灯显示红色 */
		LED_RED;
	}
	else
	{ 
		/* 源数据与传输后数据相等时RGB彩色灯显示蓝色 */
		LED_BLUE;
	}

	while (1)
	{		
	}
}

/* 简单的延时函数 */
static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

/**
  * DMA传输配置
  */
static void DMA_Config(void)
{
	HAL_StatusTypeDef DMA_status = HAL_ERROR; 
	/* 使能DMA时钟 */
	DMA_STREAM_CLOCK();

	DMA_Handle.Instance = DMA_STREAM;
	/* DMA数据流通道选择 */
	DMA_Handle.Init .Channel = DMA_CHANNEL;  
	/* 存储器到存储器模式 */
	DMA_Handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
	/* 使能自动递增功能 */
	DMA_Handle.Init.PeriphInc = DMA_PINC_ENABLE;
	/* 使能自动递增功能 */
	DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
	/* 源数据是字大小(32位) */
	DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	/* 目标数据也是字大小(32位) */
	DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	/* 一次传输模式，存储器到存储器模式不能使用循环传输 */
	DMA_Handle.Init.Mode = DMA_NORMAL;
	/* DMA数据流优先级为高 */
	DMA_Handle.Init.Priority = DMA_PRIORITY_HIGH;
	/* 禁用FIFO模式 */
	DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;     
	DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	/* 单次模式 */
	DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
	/* 单次模式 */
	DMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
	/* 完成DMA数据流参数配置 */
	HAL_DMA_Init(&DMA_Handle);

	DMA_status = HAL_DMA_Start(&DMA_Handle,(uint32_t)aSRC_Const_Buffer,(uint32_t)aDST_Buffer,BUFFER_SIZE);
	/* 判断DMA状态 */
	if (DMA_status != HAL_OK)
	{
		/* DMA出错就让程序运行下面循环：RGB彩色灯闪烁 */
		while (1)
		{      
			LED_RED;
			Delay(0xFFFFFF);
			LED_RGBOFF;
			Delay(0xFFFFFF);
		}
	} 
}

/**
  * 判断指定长度的两个数据源是否完全相等，
  * 如果完全相等返回1，只要其中一对数据不相等返回0
  */
uint8_t Buffercmp(const uint32_t* pBuffer, 
                  uint32_t* pBuffer1, uint16_t BufferLength)
{
	/* 数据长度递减 */
	while(BufferLength--)
	{
		/* 判断两个数据源是否对应相等 */
		if(*pBuffer != *pBuffer1)
		{
		  /* 对应数据源不相等马上退出函数，并返回0 */
		  return 0;
		}
		/* 递增两个数据源的地址指针 */
		pBuffer++;
		pBuffer1++;
	}
	/* 完成判断并且对应数据相对 */
	return 1;  
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
