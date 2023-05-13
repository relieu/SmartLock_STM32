/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"

#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include "tim.h"
#include "spi.h"
#include "rtc.h"

#include "oled.h"
#include "keys.h"
#include "buzz.h"
#include "fingerprint.h"
#include "flash.h"
#include "motor.h"
#include "threads.h"
#include "wifi.h"

#include "rtthread.h"
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
#define FLASH_SS_Pin GPIO_PIN_0
#define FLASH_SS_GPIO_Port GPIOC
#define BUZZKey_Pin GPIO_PIN_0
#define BUZZKey_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_1
#define LED_BLUE_GPIO_Port GPIOB
#define MOTOR_A_Pin GPIO_PIN_12
#define MOTOR_A_GPIO_Port GPIOB
#define MOTOR_B_Pin GPIO_PIN_13
#define MOTOR_B_GPIO_Port GPIOB
#define MOTOR_C_Pin GPIO_PIN_14
#define MOTOR_C_GPIO_Port GPIOB
#define MOTOR_D_Pin GPIO_PIN_15
#define MOTOR_D_GPIO_Port GPIOB
#define KEYS_COL3_Pin GPIO_PIN_6
#define KEYS_COL3_GPIO_Port GPIOC
#define KEYS_COL4_Pin GPIO_PIN_7
#define KEYS_COL4_GPIO_Port GPIOC
#define KEYS_ROW1_Pin GPIO_PIN_8
#define KEYS_ROW1_GPIO_Port GPIOC
#define KEYS_ROW2_Pin GPIO_PIN_9
#define KEYS_ROW2_GPIO_Port GPIOC
#define KEYS_ROW3_Pin GPIO_PIN_10
#define KEYS_ROW3_GPIO_Port GPIOC
#define KEYS_ROW4_Pin GPIO_PIN_11
#define KEYS_ROW4_GPIO_Port GPIOC
#define KEYS_COL2_Pin GPIO_PIN_12
#define KEYS_COL2_GPIO_Port GPIOC
#define KEYS_COL1_Pin GPIO_PIN_2
#define KEYS_COL1_GPIO_Port GPIOD
#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOB
#define WIFI_EN_Pin GPIO_PIN_8
#define WIFI_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define TRUE 1
#define FALSE 0
#define FP_OK 0x00;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
