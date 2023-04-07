#include "main.h"
#include "tim.h"

void BUZZ_Init(void) {
    ;
}

void BUZZ_Bell(void) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, BELL_CCR);
    //叮
    __HAL_TIM_SET_PRESCALER(&htim1, DING_PSC);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    rt_thread_delay(500);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    rt_thread_delay(100);
    //咚
    __HAL_TIM_SET_PRESCALER(&htim1, DONG_PSC);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    rt_thread_delay(500);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void BUZZ_Beep(uint8_t times) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, BEEP_CCR);
    for (int i = 0; i < times; i++) {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
        rt_thread_delay(50);
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
        rt_thread_delay(100);
    }
}