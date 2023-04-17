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
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x5051) {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_SUNDAY;
  DateToUpdate.Month = RTC_MONTH_APRIL;
  DateToUpdate.Date = 0x16;
  DateToUpdate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
        __HAL_RTC_SECOND_ENABLE_IT(&hrtc, RTC_IT_SEC);
        date = DateToUpdate;

        //使能BKP写入操作
        __HAL_RCC_BKP_CLK_ENABLE();
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);
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
        __HAL_RTC_SECOND_ENABLE_IT(&hrtc, RTC_IT_SEC);
    }
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
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

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void RTC_printTime(RTC_TimeTypeDef time, RTC_DateTypeDef date) {

    rt_thread_delay(1000);
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    /* Display date Format : yy/mm/dd */
    rt_kprintf("%02d/%02d/%02d\r\n", 2000 + date.Year, date.Month, date.Date);
    /* Display time Format : hh:mm:ss */
    rt_kprintf("%02d:%02d:%02d\r\n", time.Hours, time.Minutes, time.Seconds);

    rt_kprintf("\r\n");
}

void RTC_setDate(RTC_DateTypeDef date) {
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t) date.Year);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t) date.Month);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t) date.Date);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t) date.WeekDay);
    if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
}
/* USER CODE END 1 */
