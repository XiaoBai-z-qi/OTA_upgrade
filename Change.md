## [1.0.0] - 2025-11-25
### Added

- 添加 bootloader 升级信息结构体 upgrade_info_t
- 添加 读写版本接口 ReadUpgradeInfo / WriteUpgradeInfo
- 添加版本号自动初始化功能（首次启动）
- 添加 Flash 地址布局定义（Bootloader / Upgrade Info / Application）
  - Bootloader：0x08000000 ~ 0x08003FFF（16 KB）
  - 升级信息区：0x08004000 ~ 0x080043FF（1 KB）
    - upgrade_flag：0x08004000
    - h_version：0x08004004
    - l_version：0x08004008
    - reserved：0x0800400C ~ 0x0800401F (保留，将来可能用的上)
  - 应用程序区域：0x08004400 ~ 0x0800FFFF（剩余 47 KB）



## 2025-11-25 - 11-26

### Added

- 添加==bootloard.c/.h==文件

- 添加goto_application函数 ---- 跳转至Application应用程序

- bootloard整体框架搭好

  