#include "bootloard.h"
upgrade_info_t g_upgrade_info;
static upgrade_info_t init_version = {
  0x00000000,
  1,
  0,
  {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
};

void ReadUpgradeInfo(void)
{
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

void CheckVersion(void)
{
  ReadUpgradeInfo();
  if(g_upgrade_info.upgrade_flag == 0xffffffff)
  {
    WriteUpgradeInfo(&init_version);
    ReadUpgradeInfo();
  }
}


void ClearUpgradeFlag(void)
{
    HAL_FLASH_Unlock();

    // 擦除 Upgrade Info 整个页面
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;

    eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = UPGRADE_INFO_ADDR;     // upgrade info 页
    eraseInit.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return;
    }

    // 擦除后重新写入 upgrade_info（flag = 0）
    upgrade_info_t info;
    info.upgrade_flag = 0x00000000;   // ✔ 代表不升级
    info.h_version     = g_upgrade_info.h_version;
    info.l_version     = g_upgrade_info.l_version;
    info.reserved[0]  = 0xFFFFFFFF;
    info.reserved[1]  = 0xFFFFFFFF;
    info.reserved[2]  = 0xFFFFFFFF;
    info.reserved[3]  = 0xFFFFFFFF;

    // 写回结构体
    uint32_t *p = (uint32_t *)&info;
    uint32_t addr = UPGRADE_INFO_ADDR;

    for (int i = 0; i < sizeof(info) / 4; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, p[i]);
        addr += 4;
    }

    HAL_FLASH_Lock();
}



void goto_application(void)
{
  void (*app_reset_handler)(void) = (void (*)(void))(*((volatile uint32_t*)(ETX_APP_START_ADDRESS + 4U)));

  __set_MSP(*(volatile uint32_t*) ETX_APP_START_ADDRESS);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	app_reset_handler();
}