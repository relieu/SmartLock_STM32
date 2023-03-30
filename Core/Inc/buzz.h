#ifndef SMARTLOCK_STM32_BUZZ_H
#define SMARTLOCK_STM32_BUZZ_H

//初始化
void BUZZ_Init(void);

//门铃
void BUZZ_Bell(void);

//操作提示
void BUZZ_Beep(uint8_t times);

#endif //SMARTLOCK_STM32_BUZZ_H
