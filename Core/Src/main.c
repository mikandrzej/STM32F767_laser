/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "global_defines.h"
#include "laser/laser_irq.h"
#include "laser/laser_frame_processor.h"
#include "laser/laser_font.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static uint16_t spi_data = 0xFF02u;
static uint16_t DAC1_data = 0;
static uint16_t DAC2_data = 0;
static uint16_t laser_r_data = 0;
static uint16_t laser_g_data = 0;
static uint16_t laser_b_data = 0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC_Init();
  MX_TIM8_Init();
  MX_SPI1_Init();
  __HAL_SPI_ENABLE(&hspi1);

  err_t status;
  
  status = laser_font_print_text("DUPA", -20000, -20000, 2, 255,255,255);
  if(ERR_OK == status)
  {
    (void)laser_frame_processor_enqueue();
  }

  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)&DAC1_data, 1u, DAC_ALIGN_12B_L);
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)&DAC2_data, 1u, DAC_ALIGN_12B_L);

  HAL_TIM_OC_Start_IT(&htim8, TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&htim8, TIM_CHANNEL_2);
  HAL_TIM_OC_Start_IT(&htim8, TIM_CHANNEL_3);
  HAL_TIM_OC_Start_IT(&htim8, TIM_CHANNEL_4);

  HAL_TIM_Base_Start(&htim8);

  while(1)
  {    

  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(TIM8 == htim->Instance)
  {
    if(GPIOC->IDR & (1 << 6u))
    {
      if(HAL_TIM_ACTIVE_CHANNEL_1 == htim->Channel)
      {
        laser_point_s *point = laser_irq_get_laser_point();
        if(NULL != point)
        {
          DAC1_data = (uint16_t)((point->x / 16) + 2048);
          DAC2_data = (uint16_t)((point->y / 16) + 2048);
          laser_r_data = point->r;
          laser_g_data = point->g;
          laser_b_data = point->b;
          HAL_GPIO_WritePin(TEST_PIN2_GPIO_Port, TEST_PIN2_Pin, 0);
          HAL_GPIO_WritePin(TEST_PIN2_GPIO_Port, TEST_PIN2_Pin, point->on);
        }
      }
    }
    else
    {
      if(HAL_TIM_ACTIVE_CHANNEL_1 == htim->Channel)
      {
        HAL_GPIO_WritePin(TEST_PIN1_GPIO_Port, TEST_PIN1_Pin, 1u);
        hspi1.Instance->DR = spi_data;
      }
      if(HAL_TIM_ACTIVE_CHANNEL_2 == htim->Channel)
      {
        HAL_GPIO_WritePin(TEST_PIN1_GPIO_Port, TEST_PIN1_Pin, 0u);
        HAL_GPIO_WritePin(TEST_PIN1_GPIO_Port, TEST_PIN1_Pin, 1u);
        hspi1.Instance->DR = spi_data;
      }
      else if(HAL_TIM_ACTIVE_CHANNEL_3 == htim->Channel)
      {
        HAL_GPIO_WritePin(TEST_PIN1_GPIO_Port, TEST_PIN1_Pin, 0u);
        HAL_GPIO_WritePin(TEST_PIN1_GPIO_Port, TEST_PIN1_Pin, 1u);
        hspi1.Instance->DR = spi_data;
      }
      else if(HAL_TIM_ACTIVE_CHANNEL_4 == htim->Channel)
      {
        HAL_GPIO_WritePin(TEST_PIN1_GPIO_Port, TEST_PIN1_Pin, 0u);
      }
    }
  }
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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
