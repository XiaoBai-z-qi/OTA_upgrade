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



## 2025-11-26

### Added

- 将最小堆栈大小设置为0x800
- 创建应用程序
- 添加==serialota.py文件==实现串口升级
- 修改接收版本信息不准确的==bug==
- 添加升级完成跳转应用程序逻辑



## 2025-11-27

### Added

- 增加esp8266工程进行无线ota调试
- 删除application工程（暂时）
- 更改bin文件夹为appbin并修改内容
- esp8266工程里实现联网与获取电脑服务器stm32的版本号和指定固件的块
- appbin文件夹修改
  - 添加==config.json== ==app.bin== ==crc.bin==文件

- esp8266工程
  - 添加ArduinoJson库并使用
  - 新建==ota.cpp== ==ota.h==文件
  - 添加获取json里版本号和固件大小函数

## 2025-11-28

### Added

- esp8266工程
  - 加上返回stm32版本号和固件大小的逻辑



## 2025-11-29

### Added

- application工程

- 按键接收版本号和固件大小

- ### ==修改bootloard跳转application不能进入中断的bug== 

- 完成检查更新和确定更新的逻辑

- ### ==目前完成获取服务器版本号和固件大小写入flash，确定更新后软件复位到bootloard==

 
