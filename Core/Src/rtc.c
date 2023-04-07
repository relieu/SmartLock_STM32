/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void) {

    /* USER CODE BEGIN RTC_Init 0 */
    RTC_TimeTypeDef time;   //时间结构体参数
    RTC_DateTypeDef date;   //日期结构体参数
    /* USER CODE END RTC_Init 0 */

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef DateToUpdate = {0};

    /* USER CODE BEGIN RTC_Init 1 */
    __HAL_RCC_BKP_CLK_ENABLE();       //开启后备区域时钟
    __HAL_RCC_PWR_CLK_ENABLE();          //开启电源时钟
    /* USER CODE END RTC_Init 1 */

    /** Initialize RTC Only
    */
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    /* USER CODE BEGIN Check_RTC_BKUP */
    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x5051) {
        /* USER CODE END Check_RTC_BKUP */

        /** Initialize RTC and set the Time and Date
        */
        sTime.Hours = 0x22;
        sTime.Minutes = 0x49;
        sTime.Seconds = 0x0;

        if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
            Error_Handler();
        }
        DateToUpdate.WeekDay = RTC_WEEKDAY_FRIDAY;
        DateToUpdate.Month = RTC_MONTH_APRIL;
        DateToUpdate.Date = 0x7;
        DateToUpdate.Year = 0x23;

        if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK) {
            Error_Handler();
        }
        /* USER CODE BEGIN RTC_Init 2 */
        __HAL_RTC_SECOND_ENABLE_IT(&hrtc, RTC_IT_SEC);     //开启RTC时钟秒中断
        date = DateToUpdate;  //把日期数据拷贝到自己定义的data中
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//向指定的后备区域寄存器写入数据
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t) date.Year);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t) date.Month);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t) date.Date);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t) date.WeekDay);

    } else {
        date.Year = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
        date.Month = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
        date.Date = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
        date.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
        DateToUpdate = date;
        if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK) {
            Error_Handler();
        }
        __HAL_RTC_SECOND_ENABLE_IT(&hrtc, RTC_IT_SEC);     //开启RTC时钟秒中断
    }
    /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef *rtcHandle) {

    if (rtcHandle->Instance == RTC) {
        /* USER CODE BEGIN RTC_MspInit 0 */

        /* USER CODE END RTC_MspInit 0 */
        HAL_PWR_EnableBkUpAccess();
        /* Enable BKP CLK enable for backup registers */
        __HAL_RCC_BKP_CLK_ENABLE();
        /* RTC clock enable */
        __HAL_RCC_RTC_ENABLE();
        /* USER CODE BEGIN RTC_MspInit 1 */

        /* USER CODE END RTC_MspInit 1 */
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *rtcHandle) {

    if (rtcHandle->Instance == RTC) {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
        /* USER CODE BEGIN RTC_MspDeInit 1 */

        /* USER CODE END RTC_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
