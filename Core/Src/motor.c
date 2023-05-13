#include "motor.h"

uint8_t isUnlock = 0;

//开锁
void MOTOR_unLock(void) {
    isUnlock = 1;
    for (int i = 0; i < LOCK_LOOPS; ++i) {
        HAL_GPIO_WritePin(MOTOR_A_GPIO_Port, MOTOR_A_Pin, GPIO_PIN_SET);
        rt_thread_delay(LOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_A_GPIO_Port, MOTOR_A_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MOTOR_B_GPIO_Port, MOTOR_B_Pin, GPIO_PIN_SET);
        rt_thread_delay(LOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_B_GPIO_Port, MOTOR_B_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_SET);
        rt_thread_delay(LOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MOTOR_D_GPIO_Port, MOTOR_D_Pin, GPIO_PIN_SET);
        rt_thread_delay(LOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_D_GPIO_Port, MOTOR_D_Pin, GPIO_PIN_RESET);
    }
}

//关锁
void MOTOR_Lock(void) {
    isUnlock = 0;
    for (int i = 0; i < LOCK_LOOPS; ++i) {
        HAL_GPIO_WritePin(MOTOR_D_GPIO_Port, MOTOR_D_Pin, GPIO_PIN_SET);
        rt_thread_delay(UNLOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_D_GPIO_Port, MOTOR_D_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_SET);
        rt_thread_delay(UNLOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MOTOR_B_GPIO_Port, MOTOR_B_Pin, GPIO_PIN_SET);
        rt_thread_delay(UNLOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_B_GPIO_Port, MOTOR_B_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MOTOR_A_GPIO_Port, MOTOR_A_Pin, GPIO_PIN_SET);
        rt_thread_delay(UNLOCK_SPEED);
        HAL_GPIO_WritePin(MOTOR_A_GPIO_Port, MOTOR_A_Pin, GPIO_PIN_RESET);
    }
}