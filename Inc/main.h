/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PROBE_Pin LL_GPIO_PIN_3
#define PROBE_GPIO_Port GPIOA
#define STEP_Z_Pin LL_GPIO_PIN_4
#define STEP_Z_GPIO_Port GPIOA
#define DIR_Z_Pin LL_GPIO_PIN_5
#define DIR_Z_GPIO_Port GPIOA
#define STEP_Y_Pin LL_GPIO_PIN_6
#define STEP_Y_GPIO_Port GPIOA
#define DIR_Y_Pin LL_GPIO_PIN_7
#define DIR_Y_GPIO_Port GPIOA
#define ENABLE_Pin LL_GPIO_PIN_4
#define ENABLE_GPIO_Port GPIOC
#define STEP_X_Pin LL_GPIO_PIN_5
#define STEP_X_GPIO_Port GPIOC
#define DIR_X_Pin LL_GPIO_PIN_0
#define DIR_X_GPIO_Port GPIOB
#define XLIMIT_Pin LL_GPIO_PIN_1
#define XLIMIT_GPIO_Port GPIOB
#define XLIMIT_EXTI_IRQn EXTI1_IRQn
#define YLIMIT_Pin LL_GPIO_PIN_2
#define YLIMIT_GPIO_Port GPIOB
#define YLIMIT_EXTI_IRQn EXTI2_IRQn
#define ZLIMIT_Pin LL_GPIO_PIN_10
#define ZLIMIT_GPIO_Port GPIOB
#define ZLIMIT_EXTI_IRQn EXTI15_10_IRQn
#define EE_WP_Pin LL_GPIO_PIN_5
#define EE_WP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
