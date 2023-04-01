#ifndef SMARTLOCK_STM32_OLED_H
#define SMARTLOCK_STM32_OLED_H

//--------------------Includes--------------------
#include "i2c.h"
#include "main.h"

//--------------------Defines--------------------

//I2C OLED模块地址
#define OLED_ADDRESS 0x78
//OLED 模块控制数据
#define OLED_MODE_COM 0x00 //命令
#define OLED_MODE_DAT 0x40 //数据

//--------------------函数定义--------------------
//发送指令
void OLED_Write_Cmd(uint8_t cmd);

//发送数据
void OLED_Write_Dat(uint8_t data);

//初始化
void OLED_Init(void);

//开启显示
void OLED_Display_On(void);

//关闭显示
void OLED_Display_Off(void);

//更新显示
void OLED_Display_Update(void);

//清除屏幕
void OLED_Clear(void);

//反色
void OLED_Inverse(uint8_t i);

//画点
void OLED_DrawPoint(uint8_t x, uint8_t y);

//清楚点
void OLED_ClearPoint(uint8_t x, uint8_t y);

//填充屏幕
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t isLight);

//显示字符
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t size);

//显示数字
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);

//显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, const char *pStr, uint8_t size);

//显示中文
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t number);

#endif //SMARTLOCK_STM32_OLED_H
