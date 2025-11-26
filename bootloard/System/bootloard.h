#ifndef __BOOTLOARD_H__
#define __BOOTLOARD_H__
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "my_log.h"
#define MAX_BLOCK_SIZE          ( 1024 )  
#define UPGRADE_INFO_ADDR       0x08003C00        //升级信息存储地址
#define ETX_APP_START_ADDRESS   0x08004400 
#define APP_PAGES               47

extern UART_HandleTypeDef huart2;


typedef struct{
  uint32_t upgrade_flag;   // 0xAAAAAAAA = 需要升级, 0x00000000 = 未升级	0xffffffff为系统第一次进入
  uint32_t h_version;       // 主版本号
  uint32_t l_version;       // 次版本号
  uint32_t reserved[4];    // 预留给以后使用
}upgrade_info_t;

void CheckVersion(void);
void ReadUpgradeInfo(void);
void WriteUpgradeInfo(upgrade_info_t *info);
void ClearUpgradeFlag(void);
void FirmwareUpdate(void);
void goto_application(void);
#endif

