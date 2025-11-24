#include "my_log.h"
/**
  ******************************************************************************
  * @file    my_log.c
  * @brief   自定义日志系统实现
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "my_log.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
// UART句柄指针
static UART_HandleTypeDef *log_huart = NULL;

// 运行时日志级别(默认为编译时配置)
static log_level_t current_log_level = LOG_LEVEL_CURRENT;

// 日志级别字符串
static const char* log_level_strings[] = {
    "DEBUG",
    "INFO", 
    "WARN",
    "ERROR"
};

/* Private function prototypes -----------------------------------------------*/
static void log_output_string(const char *str);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  初始化日志系统
  * @param  huart: 用于日志输出的UART句柄指针
  */
void log_init(UART_HandleTypeDef *huart)
{
    log_huart = huart;
    current_log_level = LOG_LEVEL_CURRENT;
    
    LOG_INFO("Log system initialized, level: %d", current_log_level);
}

/**
  * @brief  设置运行时日志级别
  * @param  level: 新的日志级别
  */
void log_set_level(log_level_t level)
{
    current_log_level = level;
    LOG_INFO("Log level changed to: %d", level);
}

/**
  * @brief  获取当前日志级别
  * @retval 当前日志级别
  */
log_level_t log_get_level(void)
{
    return current_log_level;
}

/**
  * @brief  日志实现核心函数
  * @param  level: 日志级别
  * @param  file:  源文件名
  * @param  line:  行号
  * @param  fmt:   格式字符串
  * @param  ...:   可变参数
  */
void log_impl(log_level_t level, const char* file, int line, const char* fmt, ...)
{
    // 检查级别是否应该输出
    if (level < current_log_level) {
        return;
    }
    
    // 检查UART是否初始化
    if (log_huart == NULL) {
        return;
    }
    
    // 格式化缓冲区
    char log_buffer[LOG_MAX_LINE_LEN];
    int pos = 0;
    
    // 3. 添加日志级别
    pos += snprintf(log_buffer + pos, sizeof(log_buffer) - pos, "[%s]", log_level_strings[level]);
    
    // 4. 添加文件名和行号 (只显示文件名，不显示路径)
    const char *filename = strrchr(file, '/');
    if (filename == NULL) {
        filename = strrchr(file, '\\');
    }
    if (filename != NULL) {
        filename++; // 跳过路径分隔符
    } else {
        filename = file;
    }
    pos += snprintf(log_buffer + pos, sizeof(log_buffer) - pos, "[%s:%d]", filename, line);
    
    // 5. 添加用户消息
    va_list args;
    va_start(args, fmt);
    pos += vsnprintf(log_buffer + pos, sizeof(log_buffer) - pos, fmt, args);
    va_end(args);
    
    // 6. 添加颜色重置和换行
    snprintf(log_buffer + pos, sizeof(log_buffer) - pos, "\r\n");
    
    // 7. 输出日志
    log_output_string(log_buffer);
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  输出字符串到UART
  * @param  str: 要输出的字符串
  */
static void log_output_string(const char *str)
{
    if (log_huart == NULL || str == NULL) {
        return;
    }
    
    // 使用HAL_UART_Transmit以阻塞方式发送
    // 对于实时性要求高的系统，可以考虑使用DMA方式
    HAL_UART_Transmit(log_huart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}


