#include "main.h"

void BUZZ_Init(void) {
    ;
}

void BUZZ_Bell(void) {
    //TODO 门铃
}

void BUZZ_Beep(uint8_t times) {
    for (int i = 0; i < times; ++i) {
        HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_SET);
        rt_thread_delay(50);
        HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_RESET);
        rt_thread_delay(100);
    }
}