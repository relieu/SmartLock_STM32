#ifndef SMARTLOCK_STM32_MOTOR_H
#define SMARTLOCK_STM32_MOTOR_H

#include "main.h"

#define UNLOCK_SPEED 3
#define LOCK_SPEED 5

#define LOCK_LOOPS 100

//开锁
void MOTOR_unLock(void);

//关锁
void MOTOR_Lock(void);

#endif //SMARTLOCK_STM32_MOTOR_H
