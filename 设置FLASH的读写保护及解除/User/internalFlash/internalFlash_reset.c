/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   恢复选项字节为默认值范例(解除读、写保护)
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./internalFlash/internalFlash_reset.h" 

FLASH_OBProgramInitTypeDef OBInit;

/**
  * @brief  InternalFlash_Reset,恢复内部FLASH的默认配置
  * @param  None
  * @retval None
  */
HAL_StatusTypeDef InternalFlash_Reset(void)
{

	/* 使能访问选项字节寄存器 */
	HAL_FLASH_OB_Unlock();

	FLASH_INFO("\r\n");
	FLASH_INFO("正在准备恢复的条件，请耐心等待...");
	//选项字节全部恢复默认值
	OBInit.OptionType = OPTIONBYTE_WRP|OPTIONBYTE_RDP|OPTIONBYTE_USER|OPTIONBYTE_BOR;
						
	OBInit.WRPSector  = OB_WRP_SECTOR_All;
	OBInit.RDPLevel  = OB_RDP_LEVEL_0;
	OBInit.USERConfig  = OB_IWDG_SW|OB_STOP_NO_RST|OB_STDBY_NO_RST;
//					             OB_DUAL_BOOT_DISABLE;
	OBInit.BORLevel = OB_BOR_OFF;
//	OBInit.BootAddr0 = OB_BOOTADDR_ITCM_FLASH;
//	OBInit.BootAddr1 = OB_BOOTADDR_SYSTEM;
	HAL_FLASHEx_OBProgram(&OBInit);

	if (HAL_FLASH_OB_Launch() != HAL_OK)
	{
		FLASH_ERROR("对选项字节编程出错，恢复失败");
		return HAL_ERROR;
	}
	FLASH_INFO("恢复选项字节默认值成功！");
	//禁止访问
	HAL_FLASH_OB_Lock();
			
	return HAL_OK;
}




#define FLASH_WRP_SECTORS   (OB_WRP_SECTOR_0|OB_WRP_SECTOR_1) 
__IO uint32_t SectorsWRPStatus = 0xFFF;

/**
  * @brief  WriteProtect_Test,普通的写保护配置
  * @param  运行本函数后会给扇区FLASH_WRP_SECTORS进行写保护，再重复一次会进行解写保护
  * @retval None
  */
void WriteProtect_Test(void)
{
	/* 获取扇区的写保护状态 */
	HAL_FLASHEx_OBGetConfig(&OBInit);
	SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

	if (SectorsWRPStatus == 0x00)
	{
		/* 扇区已被写保护，执行解保护过程*/

		/* 使能访问OPTCR寄存器 */
		HAL_FLASH_OB_Unlock();

		HAL_FLASH_Unlock();
		/* 设置对应的nWRP位，解除写保护 */
		OBInit.OptionType = OPTIONBYTE_WRP;
		OBInit.WRPState   = OB_WRPSTATE_DISABLE;
		OBInit.WRPSector  = FLASH_WRP_SECTORS;
		HAL_FLASHEx_OBProgram(&OBInit);
		/* 开始对选项字节进行编程 */  
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
			FLASH_ERROR("对选项字节编程出错，解除写保护失败");
			while (1)
			{
			}
		}
		/* 禁止访问OPTCR寄存器 */
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
		/* 获取扇区的写保护状态 */
		HAL_FLASHEx_OBGetConfig(&OBInit);
		SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

		/* 检查是否配置成功 */
		if (SectorsWRPStatus == FLASH_WRP_SECTORS)
		{
			FLASH_INFO("解除写保护成功！");
		}
		else
		{
			FLASH_ERROR("未解除写保护！");
		}
	}
	else
	{ /* 若扇区未被写保护，开启写保护配置 */

		/* 使能访问OPTCR寄存器 */
		HAL_FLASH_OB_Unlock();

		HAL_FLASH_Unlock();
		/*使能 FLASH_WRP_SECTORS 扇区写保护 */
		OBInit.OptionType = OPTIONBYTE_WRP;
		OBInit.WRPState   = OB_WRPSTATE_ENABLE;
		OBInit.WRPSector  = FLASH_WRP_SECTORS;
		HAL_FLASHEx_OBProgram(&OBInit); 

		/* 开始对选项字节进行编程 */  
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
			FLASH_ERROR("对选项字节编程出错，解除写保护失败");
			while (1)
			{
			}
		}

		/* 禁止访问OPTCR寄存器 */
		HAL_FLASH_OB_Lock();

		HAL_FLASH_Lock();

		/* 获取扇区的写保护状态 */
		HAL_FLASHEx_OBGetConfig(&OBInit);
		SectorsWRPStatus = OBInit.WRPSector & FLASH_WRP_SECTORS;

		/* 检查是否配置成功 */
		if (SectorsWRPStatus == 0x00)
		{
			FLASH_INFO("设置写保护成功！");
		}
		else
		{
			FLASH_ERROR("设置写保护失败！");
		}
	}
}


/**
  * @brief  OptionByte_Info,打印选项字节的配置信息
  * @param  None
  * @retval None
  */
void OptionByte_Info(void)
{
	uint32_t temp;
	uint8_t sector_num;
	
	HAL_FLASHEx_OBGetConfig(&OBInit);	
	
	//读保护级别
	FLASH_INFO("\r\n");
	FLASH_INFO("-------------读保护级别-------------");
	if(OBInit.RDPLevel==OB_RDP_LEVEL_0)
	{
		FLASH_INFO("STM32内部FLASH读保护级别为LEVEL0");	
	}
	else 
	{
		//返回值不是LEVEL_0时，可能为LEVEL1或LEVEL2，
		//但在LEVEL2时，连RAM调试都不能运行，即本程序都无法下载到芯片，
		//在LEVEL2时芯片无法再解锁
		FLASH_INFO("内部FLASH读保护级别为LEVEL1");
	}
	
	FLASH_DEBUG("\r\n");
	FLASH_DEBUG("-------------写保的护扇区-------------");
	if(OBInit.WRPSector == OB_WRP_SECTOR_All)
	{
		FLASH_DEBUG("默认值：无读写保护");
	}
	else
	{
		temp = OBInit.WRPSector >>16;
		for(sector_num=0;sector_num<12;sector_num++)
		{
			if(!((temp>>sector_num)&0x001))
			{
				FLASH_DEBUG("sector%d已写保护",sector_num);
			}
		}
		
	}


	
	FLASH_DEBUG("\r\n");	
	FLASH_DEBUG("-------------用户选项字节USER------------");
	temp = OBInit.USERConfig;
	
	if(temp&0x01)
		FLASH_DEBUG("默认值：软件独立看门狗");
	else
		FLASH_DEBUG("硬件独立看门狗");
	
	if(temp&(0x01<<1))
		FLASH_DEBUG("默认值：进入停止模式时不产生复位");
	else
		FLASH_DEBUG("进入停止模式时产生复位");

	if(temp&(0x01<<2))	
		FLASH_DEBUG("默认值：进入待机模式时不产生复位");
	else
		FLASH_DEBUG("进入待机模式时产生复位");
	
	FLASH_DEBUG("\r\n");	
	FLASH_DEBUG("-------------复位级别BOR_LEV------------");
//	temp = FLASH_OB_GetBOR();
  
	switch(OBInit.BORLevel)
	{
		case OB_BOR_LEVEL3:
				FLASH_DEBUG("复位阈值电压为 2.70 V 到 3.60 V");
				break;
		
		case OB_BOR_LEVEL2:
				FLASH_DEBUG("复位阈值电压为 2.40 V 到 2.70 V");
				break;
		
		case OB_BOR_LEVEL1:
				FLASH_DEBUG("复位阈值电压为 2.10 V 到 2.40 V");
				break;
			
		case OB_BOR_OFF:
				FLASH_DEBUG("默认值：复位阈值电压为 1.80 V 到 2.10 V");
				break;	
	}			
				
				

}	











