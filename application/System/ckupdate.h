#ifndef __CHUPDATE_H__
#define __CHUPDATE_H__
#include "main.h"
#include "usart.h"

#define ETX_BOOT_START_ADDRESS   0x08004400 
#define UPGRADE_INFO_ADDR       0x08003C00        //升级信息存储地址

typedef struct{
  uint32_t upgrade_flag;   // 0xAAAAAAAA = 需要升级, 0x00000000 = 未升级	0xffffffff为系统第一次进入
  uint32_t h_version;       // 主版本号
  uint32_t l_version;       // 次版本号
  uint32_t kb[2];    // 预留给以后使用
}upgrade_info_t;


void ckUpdateInit(void);
void ckUpdateFunc(void);
void UpdateFunc(void);
void ReadUpgradeInfo(void);
void WriteUpgradeInfo(upgrade_info_t *info);
void goto_bootloader(void);
#endif
