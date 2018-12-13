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
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./i2c/bsp_i2c.h"
#include "./camera/bsp_ov2640.h"

/*简单任务管理*/
uint32_t Task_Delay[NumOfTask]={0};
uint8_t dispBuf[100];
uint8_t fps=0;

extern uint16_t img_width, img_height;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
//  	/*图像大小，修改这两个值即可改变图像的大小*/
//	//要求为4的倍数
//	img_width=852;
//	img_height =480;
  
	OV2640_IDTypeDef OV2640_Camera_ID;	
  /* 系统时钟初始化成216 MHz */
  SystemClock_Config();
  /* LED 端口初始化 */
  LED_GPIO_Config();	
  /*初始化USART1*/
  DEBUG_USART_Config(); 
	
	//LCD 初始化
	ILI9806G_Init ();         
  
  LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */

 //其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9806G_GramScan ( 5 );
	
  CAMERA_DEBUG("STM32F407 DCMI 驱动OV2640例程");
  	//初始化 I2C
	I2CMaster_Init(); 
  
  OV2640_HW_Init();
 

	HAL_Delay(100);


	/* 读取摄像头芯片ID，确定摄像头正常连接 */
	OV2640_ReadID(&OV2640_Camera_ID);

	if(OV2640_Camera_ID.PIDH  == 0x26)
  {
   CAMERA_DEBUG("%x%x",OV2640_Camera_ID.PIDH ,OV2640_Camera_ID.PIDL); 
  }
  else
  {
    LCD_SetTextColor(RED);
    ILI9806G_DispString_EN(10,10,"Can not detect OV2640 module,please check the connection!");
    CAMERA_DEBUG("没有检测到OV2640摄像头，请重新检查连接。");

    while(1);  
  }
  
  OV2640_UXGAConfig();
   
	OV2640_Init();  

  	//重置
  fps =0;
	Task_Delay[0]=1000;

 //扫描模式，横屏
  ILI9806G_GramScan(5);
  LCD_SetFont(&Font16x32);
  LCD_SetColors(RED,BLACK);

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  ILI9806G_DispStringLine_EN(LINE(0),"BH 4.8 inch LCD + OV2640");

  /*DMA会把数据传输到液晶屏，开窗后数据按窗口排列 */
  ILI9806G_OpenWindow(0,0,img_width,img_height);	

  while(1)
	{
		//使用串口输出帧率
		if(Task_Delay[0]==0)
		{					
			/*输出帧率*/
			CAMERA_DEBUG("\r\n帧率:%.1f/s \r\n", (double)fps/5.0);
			//重置
			fps =0;						
			Task_Delay[0]=5000; //此值每1ms会减1，减到0才可以重新进来这里
		}		
	}	
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
 void SystemClock_Config(void)
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
