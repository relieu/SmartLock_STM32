#ifndef SMARTLOCK_STM32_KEYS_H
#define SMARTLOCK_STM32_KEYS_H

//初始化
void KEYS_Init(void);

//门铃键位
void KEYS_BellKey_Press(void);

//按键扫描
void KEYS_Scan(void);

#endif //SMARTLOCK_STM32_KEYS_H
