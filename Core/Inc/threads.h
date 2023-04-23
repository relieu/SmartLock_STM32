#ifndef SMARTLOCK_STM32_THREADS_H
#define SMARTLOCK_STM32_THREADS_H

#include "main.h"

void key_thread_entry(void *parameters);

void oled_thread_entry(void *parameters);

void finger_thread_entry(void *parameters);

#endif //SMARTLOCK_STM32_THREADS_H
