/* USER CODE BEGIN Header */
/******************************************************************************
 *  文件名称        : main.c
 *  作者            : 青山
 *  创建日期        : 2025-11-25
 *  最后修改日期    : 
 *  文件描述        : bootloader引导加载程序
 *
 *  ----------------------------------------------------------------------------
 *  修改记录:
 *  日期           作者        版本号     修改内容
 *  2025-11-25    青山         1.0.0      初始创建
 *
 ******************************************************************************/
/******************************************************************************
 *  Bootloader / 应用程序 地址布局说明
 *
 *  Flash 地址分布（基于 64KB Flash 的 STM32F103C8T6）：
 *
 *  ┌───────────────────────────────────────────────────────────────┐
 *  │  Bootloader 区域（16 KB）                                     │
 *  │  地址：0x08000000 ~ 0x08003FFF                                │
 *  └───────────────────────────────────────────────────────────────┘
 *
 *  ┌───────────────────────────────────────────────────────────────┐
 *  │  升级标志区 + 版本号（1 KB）                                    │
 *  │  地址：0x08004000 ~ 0x080043FF                                │
 *  │  - upgrade_flag (需要升级则写入 0x01)                          │
 *  │  - firmware_version (如 0x00010000 → V1.0.0)                  │
 *  └───────────────────────────────────────────────────────────────┘
 *
 *  ┌───────────────────────────────────────────────────────────────┐
 *  │  应用程序区域（约 47 KB）                                       │
 *  │  地址：0x08004400 ~ 0x0800FFFF                                 │
 *  └───────────────────────────────────────────────────────────────┘
 *
 *  Bootloader 跳转逻辑：
 *  1. 上电 → Bootloader 启动
 *  2. 读取 0x08004000 的 upgrade_flag
 *  3. 如果 upgrade_flag == 1：
 *       → 进入 OTA/ESP8266 升级流程
 *     否则：
 *       → 跳转到应用程序 (0x08004400)
 *
 ******************************************************************************/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "my_log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UPGRADE_INFO_ADDR       0x08003C00        //升级信息存储地址
#define ETX_APP_START_ADDRESS   0x08004400        //application 起始地址
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef struct{
  uint32_t upgrade_flag;   // 0xAAAAAAAA = 需要升级, 0xFFFFFFFF = 未升级
  uint32_t h_version;       // 主版本号
  uint32_t l_version;       // 次版本号
  uint32_t reserved[4];    // 预留给以后使用
}upgrade_info_t;

upgrade_info_t g_upgrade_info;
upgrade_info_t init_version = {
  0x00000000,
  1,
  0,
  {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ReadUpgradeInfo(void);
void WriteUpgradeInfo(upgrade_info_t *info);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	log_init(&huart1);
  ReadUpgradeInfo();
  if(g_upgrade_info.upgrade_flag == 0xffffffff)
  {
    WriteUpgradeInfo(&init_version);
    ReadUpgradeInfo();
  }
  LOG_INFO("Version:%d.%d", g_upgrade_info.h_version, g_upgrade_info.l_version);

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  while (1)
  {
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
