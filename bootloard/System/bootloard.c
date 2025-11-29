#include "bootloard.h"
upgrade_info_t g_upgrade_info;
static upgrade_info_t init_version = {
  0x00000000,
  1,
  0,
  {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
};
uint8_t version[2] = {0};
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

void CheckVersion(void)
{
  // 检查版本信息是否有效
  ReadUpgradeInfo();
  // 如果版本信息无效，则初始化为初始版本
  if(g_upgrade_info.upgrade_flag == 0xffffffff)
  {
    WriteUpgradeInfo(&init_version);
    ReadUpgradeInfo();
  }
  version[0] = (uint8_t)g_upgrade_info.h_version;
  version[1] = (uint8_t)g_upgrade_info.l_version;
}

//清除升级标志并保留版本信息
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
    info.upgrade_flag = 0x00000000;   // 代表不升级
    info.h_version     = (uint32_t)version[0];
    info.l_version     = (uint32_t)version[1];
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
    ReadUpgradeInfo();
}


HAL_StatusTypeDef write_data_to_flash_app(uint8_t *data, uint32_t data_len, uint8_t is_first_block)
{
  static uint32_t application_write_idx = 0; //当前写入的位置
  HAL_StatusTypeDef state;
  do
  {
    //如果是第一次写入，擦除Application区flash
    if(is_first_block)  
    {
      LOG_INFO("Erasing the Flash memory.....");

      FLASH_EraseInitTypeDef EraseInitStruct;
      uint32_t SectorError;

      EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.PageAddress = ETX_APP_START_ADDRESS;
      EraseInitStruct.NbPages = APP_PAGES;                    //根据实际规划页数修改

      state = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
      if(state!= HAL_OK)  {LOG_ERROR("Erase Error");  break;}

      application_write_idx = 0;                              //重置写入位置
    }

    //写入flash数据
    for(uint32_t i = 0; i < data_len / 2; i++)
    {
      uint16_t halfword_data = data[i * 2] | (data[i * 2 + 1] << 8);
      //以半字为单位写入flash
      state = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                               (ETX_APP_START_ADDRESS + application_write_idx),
                               halfword_data);
      if(state == HAL_OK)
      {
        application_write_idx += 2;
      }
      else
      {
        LOG_ERROR("Flash Write Error");
        break;
      }
    }
    if(state != HAL_OK) break;
  } while (0);
  return state;
}

void FirmwareUpdate(void)
{
  uint8_t x = 'x';
  uint8_t y = 'y';
  uint8_t v = 'v';
  uint32_t current_app_size = 0;
  uint8_t block[MAX_BLOCK_SIZE] = {0};
  uint8_t app_kb[2] = {0};
  uint32_t app_size = 0;
  HAL_StatusTypeDef state;
  do
  {
    HAL_UART_Transmit(&huart2, &v, 1, 1000);
    state = HAL_UART_Receive(&huart2, version, 2, 5000);      //接收版本信息
    if(state != HAL_OK) {LOG_ERROR("version receive timeout!!!"); break;}
    LOG_INFO("Update Version: %d.%d", version[0], version[1]);

    HAL_UART_Transmit(&huart2, &x, 1, 1000);
    state = HAL_UART_Receive(&huart2, app_kb, 2, 5000);      //接收应用程序大小单位为kb 
    if(state != HAL_OK) {LOG_ERROR("app_kb receive timeout!!!"); break;}
    LOG_INFO("The application size is %d KB", app_kb[0] * 256 + app_kb[1]);
    app_size = 1024 * (app_kb[0] * 256 + app_kb[1]);
    //解锁flash
    state = HAL_FLASH_Unlock();
    if(state != HAL_OK) {LOG_ERROR("flash unlock error!!!"); break;}
    while(1)
    {
      HAL_UART_Transmit(&huart2, &y, 1, 1000);
      state = HAL_UART_Receive(&huart2, block, MAX_BLOCK_SIZE, 5000);   //接收1kb数据
      if(state != HAL_OK) {LOG_ERROR("block[%d] receive timeout!!!", (current_app_size / MAX_BLOCK_SIZE) + 1); break;}
      current_app_size += MAX_BLOCK_SIZE;

      state = write_data_to_flash_app(block, MAX_BLOCK_SIZE, (current_app_size <= MAX_BLOCK_SIZE));
      LOG_INFO("Received Block[%d]", current_app_size / MAX_BLOCK_SIZE);
      if(state != HAL_OK) {LOG_ERROR("Received Block[%d] write error!!!", (current_app_size / MAX_BLOCK_SIZE) + 1); break;}
      memset(block, 0, MAX_BLOCK_SIZE);

      if(current_app_size >= app_size)
      {
        LOG_INFO("Received all Application data");
        state = HAL_FLASH_Lock();     //锁定flash
        if(state != HAL_OK) {LOG_ERROR("Lock Error"); break;}
        break;
      }
    }
    
  } while (0);
  if(state != HAL_OK)
  {
    LOG_ERROR("Update Failed!!!!!!!");
    state = HAL_FLASH_Lock();     //锁定flash
    if(state != HAL_OK) {LOG_ERROR("Lock Error");}
    while(1);
  }

}

void goto_application(void)
{
		uint32_t app_start_addr = *(volatile uint32_t*)ETX_APP_START_ADDRESS;
    uint32_t app_reset_handler_addr = *(volatile uint32_t*)(ETX_APP_START_ADDRESS + 4U);
    void (*app_reset_handler)(void) = (void (*)(void))app_reset_handler_addr;

    // 关中断
    __disable_irq();

    // 关闭SysTick
    SysTick->CTRL = 0;

    // 清除所有中断pending位
    for (int i = 0; i < 8; i++)
        NVIC->ICER[i] = 0xFFFFFFFF;
    for (int i = 0; i < 8; i++)
        NVIC->ICPR[i] = 0xFFFFFFFF;

    // 重映射 Vector Table 到 App
    SCB->VTOR = ETX_APP_START_ADDRESS;

    // 设置 MSP
    __set_MSP(app_start_addr);

    // 开中断
    __enable_irq();

    // 跳到 App reset handler
    app_reset_handler();
}
