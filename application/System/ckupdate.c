#include "ckupdate.h"
#include "key_driver.h"
#include "my_log.h"

uint8_t config[4];		//0,1----version 2,3----size-kb

void ckUpdateInit(void)
{
    KEY_Register(KEY1_GPIO_Port, KEY1_Pin, ckUpdateFunc, NULL);

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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
			LOG_INFO("version:%d.%d  bin:%dkb", config[0], config[1], (config[2] << 8) | (config[3]));
    }
}

