#ifndef __MY_LOG_H__
#define __MY_LOG_H__
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"

typedef enum {
    LOG_LVL_DEBUG = 0,
    LOG_LVL_INFO,
    LOG_LVL_WARN, 
    LOG_LVL_ERROR,
    LOG_LVL_NONE    // 用于关闭所有日志
} log_level_t;

#define LOG_LEVEL_CURRENT          LOG_LVL_DEBUG


/**
  * @brief  是否启用颜色输出(如果终端支持)
  */
#define LOG_ENABLE_COLOR           1

/**
  * @brief  单条日志的最大长度(字节)
  */
#define LOG_MAX_LINE_LEN           256

/* Exported macro ------------------------------------------------------------*/
// 内部使用的核心日志函数声明
void log_impl(log_level_t level, const char* file, int line, const char* fmt, ...);

/**
  * @brief  用户调用的日志宏
  * @note   这些宏会自动捕获文件名和行号
  */
#if (LOG_LEVEL_CURRENT <= LOG_LVL_DEBUG)
#define LOG_DEBUG(fmt, ...)    log_impl(LOG_LVL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)    
#endif

#if (LOG_LEVEL_CURRENT <= LOG_LVL_INFO)  
#define LOG_INFO(fmt, ...)     log_impl(LOG_LVL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)     
#endif

#if (LOG_LEVEL_CURRENT <= LOG_LVL_WARN)
#define LOG_WARN(fmt, ...)     log_impl(LOG_LVL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else  
#define LOG_WARN(fmt, ...)     
#endif

#if (LOG_LEVEL_CURRENT <= LOG_LVL_ERROR)
#define LOG_ERROR(fmt, ...)    log_impl(LOG_LVL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)    
#endif

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  日志系统初始化
  * @param  huart: 用于日志输出的UART句柄指针
  */
void log_init(UART_HandleTypeDef *huart);

/**
  * @brief  设置运行时日志级别(可选功能)
  * @param  level: 新的日志级别
  */
void log_set_level(log_level_t level);

/**
  * @brief  获取当前日志级别
  */
log_level_t log_get_level(void);
#endif
