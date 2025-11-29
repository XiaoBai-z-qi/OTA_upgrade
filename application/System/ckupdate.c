#include "ckupdate.h"
#include "key_driver.h"
#include "my_log.h"
upgrade_info_t g_upgrade_info;
uint8_t config[4];		//0,1----version 2,3----size-kb

void ckUpdateInit(void)
{
    KEY_Register(KEY1_GPIO_Port, KEY1_Pin, ckUpdateFunc, NULL);
    KEY_Register(KEY2_GPIO_Port, KEY2_Pin, UpdateFunc, NULL);
}


void ckUpdateFunc(void)
{
    uint8_t temp = 0x80;
    __HAL_UART_FLUSH_DRREGISTER(&huart2);
    __HAL_UART_CLEAR_OREFLAG(&huart2);

    LOG_INFO("key1 down......");

    HAL_UART_Receive_IT(&huart2, config, 4);
    HAL_UART_Transmit(&huart2, &temp, 1, 1000);
}

void UpdateFunc(void)
{
		LOG_INFO("key2 down......");
    ReadUpgradeInfo();
    if(((uint32_t)config[0] > g_upgrade_info.h_version) || ((uint32_t)config[1] > g_upgrade_info.l_version))
    {
			LOG_INFO("uodata");
        g_upgrade_info.upgrade_flag = 0xaaaaaaaa;
        g_upgrade_info.h_version = (uint32_t)config[0];
        g_upgrade_info.l_version = (uint32_t)config[1];
        g_upgrade_info.kb[0] = (uint32_t)config[2];
        g_upgrade_info.kb[1] = (uint32_t)config[3];
        WriteUpgradeInfo(&g_upgrade_info);

        goto_bootloader();
    }
}

void ReadUpgradeInfo(void)
{
  // 读取升级和版本信息
  memcpy(&g_upgrade_info, (void*)UPGRADE_INFO_ADDR, sizeof(upgrade_info_t));
}

void WriteUpgradeInfo(upgrade_info_t *info)
{
    HAL_FLASH_Unlock();

    // 1. 擦除 1KB 页
    FLASH_EraseInitTypeDef erase;
    uint32_t PageError;

    erase.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = UPGRADE_INFO_ADDR;
    erase.NbPages     = 1;

    HAL_FLASHEx_Erase(&erase, &PageError);

    // 2. 写入结构体
    uint32_t *data = (uint32_t *)info;
    uint32_t addr  = UPGRADE_INFO_ADDR;

    for (int i = 0; i < sizeof(upgrade_info_t) / 4; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data[i]);
        addr += 4;
    }

    HAL_FLASH_Lock();
}

void goto_bootloader(void)
{
    LOG_INFO("ready to bootloader");
    NVIC_SystemReset();  // 执行系统复位
    // void (*boot_rest_handler)(void) = (void (*)(void))(*((volatile uint32_t*)(ETX_BOOT_START_ADDRESS + 4U)));

    // __set_MSP(*(volatile uint32_t*) ETX_BOOT_START_ADDRESS);
    // boot_rest_handler();

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
			LOG_INFO("version:%d.%d  bin:%dkb", config[0], config[1], (config[2] << 8) | (config[3]));
    }
}

