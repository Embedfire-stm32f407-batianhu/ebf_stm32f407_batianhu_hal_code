/**
  ******************************************************************************
  * @file    bsp_touchpad.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   电容按键应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F767 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./TouchPad/bsp_touchpad.h"
#include "./usart/bsp_debug_usart.h"
#include "stm32f4xx_hal.h"

//定时器最大计数值
#define TPAD_ARR_MAX_VAL 	0XFFFF	

//保存没按下时定时器计数值
__IO uint16_t tpad_default_val=0;

/***********************************
 *
 * 定时器输入捕获配置
 *
 ***********************************/	
 TIM_HandleTypeDef TIM_Handle;
static void TIMx_CHx_Cap_Init(uint32_t arr,uint16_t psc)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 
    TIM_IC_InitTypeDef sConfigIC;
    //使能TIM时钟
    TPAD_TIM_CLK_ENABLE(); 	
    //使能通道引脚时钟	
    TPAD_TIM_GPIO_CLK_ENABLE(); 	
    //端口配置
    GPIO_InitStructure.Pin = TPAD_TIM_CH_PIN;	
    //复用功能
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = TPAD_TIM_AF;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    //不带上下拉 
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
    //初始化TIM
    //设定计数器自动重装值
    TIM_Handle.Instance = TPAD_TIMx;
    TIM_Handle.Init.Prescaler = psc;
    TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM_Handle.Init.RepetitionCounter = 0;
    TIM_Handle.Init.Period = arr;
    TIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM_Handle);
	//上升沿触发
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	// 输入捕获选择
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	//配置输入分频,不分频
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	//配置输入滤波器 不滤波
    sConfigIC.ICFilter = 0;
	//初始化捕获通道
    HAL_TIM_IC_ConfigChannel(&TIM_Handle, &sConfigIC, TPAD_TIM_Channel_X);
    //启动TIM 
    HAL_TIM_IC_Start(&TIM_Handle, TPAD_TIM_Channel_X);
}

/****************************************
 *
 * 为电容按键放电
 * 清除定时器标志及计数
 *
 *****************************************/
static void TPAD_Reset(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 
    //配置引脚为普通推挽输出
    GPIO_InitStructure.Pin = TPAD_TIM_CH_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;	
    GPIO_InitStructure.Pull = GPIO_PULLDOWN; 
    HAL_GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);

    //输出低电平,放电
    HAL_GPIO_WritePin ( TPAD_TIM_CH_PORT, TPAD_TIM_CH_PIN ,GPIO_PIN_RESET);						 
    //保持一小段时间低电平，保证放电完全
    HAL_Delay(5);

    //清除更新标志
    __HAL_TIM_CLEAR_FLAG(&TIM_Handle,TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG(&TIM_Handle,TIM_FLAG_UPDATE);
    //计数器归0
    __HAL_TIM_SET_COUNTER(&TIM_Handle,0);
    //引脚配置为复用功能，不上、下拉
    GPIO_InitStructure.Pin = TPAD_TIM_CH_PIN; 
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = TPAD_TIM_AF;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; 
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TPAD_TIM_CH_PORT,&GPIO_InitStructure);
}

/****************************************************
 *
 * 得到定时器捕获值
 * 如果超时,则直接返回定时器的计数值.
 *
 *****************************************************/
static uint16_t TPAD_Get_Val(void)
{		
  /* 先放电完全，并复位计数器 */	
	TPAD_Reset();
	//等待捕获上升沿
	while(__HAL_TIM_GET_FLAG(&TIM_Handle,TIM_FLAG_CC1) == RESET )
	{
		//超时了,直接返回CNT的值
		if(__HAL_TIM_GET_COUNTER( &TIM_Handle)>TPAD_ARR_MAX_VAL-500)
			return __HAL_TIM_GET_COUNTER( &TIM_Handle);
	};
	/* 捕获到上升沿后输出TIMx_CCRx寄存器值 */
	return HAL_TIM_ReadCapturedValue(&TIM_Handle, TIM_CHANNEL_1);  
} 	

/****************************************************
 *
 * 读取n次,取最大值
 * n：连续获取的次数
 * 返回值：n次读数里面读到的最大读数值
 * 
 *****************************************************/
static uint16_t TPAD_Get_MaxVal(uint8_t n)
{
	uint16_t temp=0;
	uint16_t res=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//得到一次值
		if(temp>res)res=temp;
	};
	return res;
}  

/********************************************************
*
* 初始化触摸按键
* 获得空载的时候触摸按键的取值.
* 返回值:0,初始化成功;1,初始化失败
*
*********************************************************/
uint8_t TPAD_Init(void)
{
	uint16_t buf[10];
	uint32_t temp=0;
	uint8_t j,i;
	
	//设定定时器预分频器目标时钟为：9MHz(216Mhz/24)
	TIMx_CHx_Cap_Init(TPAD_ARR_MAX_VAL,24-1);
	for(i=0;i<10;i++)//连续读取10次
	{				 
		buf[i]=TPAD_Get_Val();
		HAL_Delay(10);	    
	}				    
	for(i=0;i<9;i++)//排序
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	//取中间的6个数据进行平均
	for(i=2;i<8;i++)
	{
	  temp+=buf[i];
	}
	
	tpad_default_val=temp/6;	
	/* printf打印函数调试使用，用来确定阈值TPAD_GATE_VAL，在应用工程中应注释掉 */
	printf("tpad_default_val:%d\r\n",tpad_default_val);	
	
	//初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常!
	if(tpad_default_val>TPAD_ARR_MAX_VAL/2)
	{
		return 1;
	}
	
	return 0;		     	    					   
}
 

/*******************************************************************************
*
* 扫描触摸按键
* mode:0,不支持连续触发(按下一次必须松开才能按下一次);1,支持连续触发(可以一直按下)
* 返回值:0,没有按下;1,有按下;			
*
*******************************************************************************/
//阈值：捕获时间必须大于(tpad_default_val + TPAD_GATE_VAL),才认为是有效触摸.
#define TPAD_GATE_VAL 	100	

uint8_t TPAD_Scan(uint8_t mode)
{
	//0,可以开始检测;>0,还不能开始检测	
	static uint8_t keyen=0;
	//扫描结果
	uint8_t res=0;
	//默认采样次数为3次
	uint8_t sample=3;	
  //捕获值	
	uint16_t rval;
	
	if(mode)
	{
		//支持连按的时候，设置采样次数为6次
		sample=6;	
		//支持连按	
		keyen=0;	  
	}	
	/* 获取当前捕获值(返回 sample 次扫描的最大值) */
	rval=TPAD_Get_MaxVal(sample); 	
	/* printf打印函数调试使用，用来确定阈值TPAD_GATE_VAL，在应用工程中应注释掉 */
	//printf("scan_rval=%d\n",rval);
	
	//大于tpad_default_val+TPAD_GATE_VAL,且小于10倍tpad_default_val,则有效
	if(rval>(tpad_default_val+TPAD_GATE_VAL)&&rval<(10*tpad_default_val))
	{			
    //keyen==0,有效 		
		if(keyen==0)
		{
			res=1;		 
		}			
		keyen=3;				//至少要再过3次之后才能按键有效   
	}
	
	if(keyen)
	{
		keyen--;		
	}		
	return res;
}	 
/*********************************************END OF FILE**********************/
