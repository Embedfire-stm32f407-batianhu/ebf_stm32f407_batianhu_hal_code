#include "./adc/bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[2]={0};
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle1;
ADC_HandleTypeDef ADC_Handle2;
ADC_ChannelConfTypeDef ADC_Config;

static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使能 GPIO 时钟
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();    
    // 配置 IO
    GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);
    
    // 使能 GPIO 时钟
    RL_ADC_GPIO_CLK_ENABLE();    
    // 配置 IO
    GPIO_InitStructure.Pin = RL_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
    HAL_GPIO_Init(RL_ADC_GPIO_PORT, &GPIO_InitStructure);

}

static void Rheostat_ADC_Mode_Config(void)
{
    ADC_MultiModeTypeDef   mode;
    // ------------------DMA Init 结构体参数 初始化--------------------------
    // ADC1使用DMA2，数据流0，通道0，这个是手册固定死的
    // 开启DMA时钟
    RHEOSTAT_ADC_DMA_CLK_ENABLE();
    // 数据传输通道
    DMA_Init_Handle.Instance = RHEOSTAT_ADC_DMA_STREAM;
    // 数据传输方向为外设到存储器	
    DMA_Init_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;	
    // 外设寄存器只有一个，地址不用递增
    DMA_Init_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    // 存储器地址固定
    DMA_Init_Handle.Init.MemInc = DMA_MINC_ENABLE; 
    // // 外设数据大小为半字，即两个字节 
    DMA_Init_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; 
    //	存储器数据大小也为半字，跟外设数据大小相同
    DMA_Init_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	
    // 循环传输模式
    DMA_Init_Handle.Init.Mode = DMA_CIRCULAR;
    // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
    DMA_Init_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    // 禁止DMA FIFO	，使用直连模式
    DMA_Init_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;  
    // FIFO 大小，FIFO模式禁止时，这个不用配置	
    DMA_Init_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    DMA_Init_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
    DMA_Init_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;  
    // 选择 DMA 通道，通道存在于流中
    DMA_Init_Handle.Init.Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
    //初始化DMA流，流相当于一个大的管道，管道里面有很多通道
    HAL_DMA_Init(&DMA_Init_Handle);
    

    // 开启ADC时钟
    RHEOSTAT_ADC1_CLK_ENABLE();
    RL_ADC2_CLK_ENABLE();
    // -------------------ADC1 Init 结构体 参数 初始化------------------------
    // ADC1
    ADC_Handle1.Instance = RHEOSTAT_ADC1;
    // 时钟为fpclk 4分频	
    ADC_Handle1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC 分辨率
    ADC_Handle1.Init.Resolution = ADC_RESOLUTION_12B;
    // 禁止扫描模式，多通道采集才需要	
    ADC_Handle1.Init.ScanConvMode = DISABLE; 
    // 连续转换	
    ADC_Handle1.Init.ContinuousConvMode = ENABLE;
    // 非连续转换	
    ADC_Handle1.Init.DiscontinuousConvMode = DISABLE;
    // 非连续转换个数
    ADC_Handle1.Init.NbrOfDiscConversion   = 0;
    //禁止外部边沿触发    
    ADC_Handle1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //使用软件触发，外部触发不用配置，注释掉即可
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //数据右对齐	
    ADC_Handle1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //转换通道个数
    ADC_Handle1.Init.NbrOfConversion = 1;
    //使能连续转换请求
    ADC_Handle1.Init.DMAContinuousRequests = ENABLE;
    //转换完成标志
    ADC_Handle1.Init.EOCSelection          = DISABLE;    
    // 初始化ADC	                          
    HAL_ADC_Init(&ADC_Handle1);
    //---------------------------------------------------------------------------
    // 配置 ADC1 通道13转换顺序为1，第一个转换，采样时间为3个时钟周期
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;
    ADC_Config.Rank         = 1;    
    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;// 采样时间间隔	
    ADC_Config.Offset       = 0;
    HAL_ADC_ConfigChannel(&ADC_Handle1, &ADC_Config);
    
    // -------------------ADC2 Init 结构体 参数 初始化------------------------
    // ADC2
    ADC_Handle2.Instance = RL_ADC2;
    // 时钟为fpclk 4分频	
    ADC_Handle2.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC 分辨率
    ADC_Handle2.Init.Resolution = ADC_RESOLUTION_12B;
    // 禁止扫描模式，多通道采集才需要	
    ADC_Handle1.Init.ScanConvMode = DISABLE; 
    // 连续转换	
    ADC_Handle2.Init.ContinuousConvMode = ENABLE;
    // 非连续转换	
    ADC_Handle2.Init.DiscontinuousConvMode = DISABLE;
    // 非连续转换个数
    ADC_Handle2.Init.NbrOfDiscConversion   = 0;
    //禁止外部边沿触发    
    ADC_Handle2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //使用软件触发，外部触发不用配置，注释掉即可
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //数据右对齐	
    ADC_Handle2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //转换通道个数
    ADC_Handle2.Init.NbrOfConversion = 1;
    //使能连续转换请求
    ADC_Handle2.Init.DMAContinuousRequests = ENABLE;
    //转换完成标志
    ADC_Handle2.Init.EOCSelection          = DISABLE;    
    // 初始化ADC	                          
    HAL_ADC_Init(&ADC_Handle2);
    // 配置 ADC2 通道4转换顺序为1，第一个转换，采样时间为3个时钟周期
    ADC_Config.Channel      = RL_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES; // 采样时间间隔	
    ADC_Config.Offset       = 0;
    HAL_ADC_ConfigChannel(&ADC_Handle2, &ADC_Config);
    

    /*配置双重AD采样*/
    mode.Mode = ADC_DUALMODE_REGSIMULT;
    mode.DMAAccessMode = ADC_DMAACCESSMODE_1;
    mode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
    
    HAL_ADCEx_MultiModeConfigChannel(&ADC_Handle1, &mode);
    
    HAL_ADC_Start(&ADC_Handle2);
    
    __HAL_LINKDMA(&ADC_Handle1, DMA_Handle, DMA_Init_Handle);
    __HAL_LINKDMA(&ADC_Handle2, DMA_Handle, DMA_Init_Handle);   
    HAL_ADCEx_MultiModeStart_DMA(&ADC_Handle1, (uint32_t *)ADC_ConvertedValue,2);
}



void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}



