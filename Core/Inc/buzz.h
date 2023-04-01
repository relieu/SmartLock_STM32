#ifndef SMARTLOCK_STM32_BUZZ_H
#define SMARTLOCK_STM32_BUZZ_H

//BUZZ PA8
//BELLKey K1 PA0
//K2 PC13

//ding freq 1230Hz
//dong freq 680Hz

#define BEEP_CCR 100//提示音占空比
#define BELL_CCR 90//门铃声占空比
#define DING_PSC 450//叮声预分频
#define DONG_PSC 1200//咚声预分频

//初始化
void BUZZ_Init(void);

//门铃
void BUZZ_Bell(void);

//操作提示
void BUZZ_Beep(uint8_t times);

#endif //SMARTLOCK_STM32_BUZZ_H
