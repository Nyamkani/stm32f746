/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f7xx_hal.h"

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
#define DI18_Pin GPIO_PIN_2
#define DI18_GPIO_Port GPIOE
#define DI19_Pin GPIO_PIN_3
#define DI19_GPIO_Port GPIOE
#define DI20_Pin GPIO_PIN_4
#define DI20_GPIO_Port GPIOE
#define DI_EMG1_Pin GPIO_PIN_5
#define DI_EMG1_GPIO_Port GPIOE
#define DI_EMG2_Pin GPIO_PIN_6
#define DI_EMG2_GPIO_Port GPIOE
#define MCX512_INT0_Pin GPIO_PIN_14
#define MCX512_INT0_GPIO_Port GPIOC
#define MCX512_INT1_Pin GPIO_PIN_15
#define MCX512_INT1_GPIO_Port GPIOC
#define CO1_Pin GPIO_PIN_4
#define CO1_GPIO_Port GPIOF
#define CO2_Pin GPIO_PIN_5
#define CO2_GPIO_Port GPIOF
#define LED1_R_Pin GPIO_PIN_6
#define LED1_R_GPIO_Port GPIOF
#define LED1_G_Pin GPIO_PIN_7
#define LED1_G_GPIO_Port GPIOF
#define LED1_B_Pin GPIO_PIN_8
#define LED1_B_GPIO_Port GPIOF
#define LED2_R_Pin GPIO_PIN_9
#define LED2_R_GPIO_Port GPIOF
#define LED2_G_Pin GPIO_PIN_10
#define LED2_G_GPIO_Port GPIOF
#define UART5_DIR_Pin GPIO_PIN_0
#define UART5_DIR_GPIO_Port GPIOB
#define UART6_DIR_Pin GPIO_PIN_1
#define UART6_DIR_GPIO_Port GPIOB
#define UART4_DIR_Pin GPIO_PIN_2
#define UART4_DIR_GPIO_Port GPIOB
#define LED2_B_Pin GPIO_PIN_11
#define LED2_B_GPIO_Port GPIOF
#define DO1_Pin GPIO_PIN_12
#define DO1_GPIO_Port GPIOF
#define DO2_Pin GPIO_PIN_13
#define DO2_GPIO_Port GPIOF
#define DO3_Pin GPIO_PIN_14
#define DO3_GPIO_Port GPIOF
#define DO4_Pin GPIO_PIN_15
#define DO4_GPIO_Port GPIOF
#define DI1_Pin GPIO_PIN_0
#define DI1_GPIO_Port GPIOG
#define DI2_Pin GPIO_PIN_1
#define DI2_GPIO_Port GPIOG
#define DI3_Pin GPIO_PIN_2
#define DI3_GPIO_Port GPIOG
#define DI4_Pin GPIO_PIN_3
#define DI4_GPIO_Port GPIOG
#define DI5_Pin GPIO_PIN_4
#define DI5_GPIO_Port GPIOG
#define DI6_Pin GPIO_PIN_5
#define DI6_GPIO_Port GPIOG
#define DI7_Pin GPIO_PIN_6
#define DI7_GPIO_Port GPIOG
#define DI8_Pin GPIO_PIN_7
#define DI8_GPIO_Port GPIOG
#define DI9_Pin GPIO_PIN_8
#define DI9_GPIO_Port GPIOG
#define MCX512_RST_Pin GPIO_PIN_8
#define MCX512_RST_GPIO_Port GPIOC
#define MCX512_EMG_Pin GPIO_PIN_9
#define MCX512_EMG_GPIO_Port GPIOC
#define DI10_Pin GPIO_PIN_9
#define DI10_GPIO_Port GPIOG
#define DI11_Pin GPIO_PIN_10
#define DI11_GPIO_Port GPIOG
#define DI12_Pin GPIO_PIN_12
#define DI12_GPIO_Port GPIOG
#define DI13_Pin GPIO_PIN_13
#define DI13_GPIO_Port GPIOG
#define DI14_Pin GPIO_PIN_14
#define DI14_GPIO_Port GPIOG
#define DI15_Pin GPIO_PIN_15
#define DI15_GPIO_Port GPIOG
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOB
#define SLED1_Pin GPIO_PIN_6
#define SLED1_GPIO_Port GPIOB
#define SLED2_Pin GPIO_PIN_7
#define SLED2_GPIO_Port GPIOB
#define SLED3_Pin GPIO_PIN_8
#define SLED3_GPIO_Port GPIOB
#define SLED4_Pin GPIO_PIN_9
#define SLED4_GPIO_Port GPIOB
#define DI16_Pin GPIO_PIN_0
#define DI16_GPIO_Port GPIOE
#define DI17_Pin GPIO_PIN_1
#define DI17_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
