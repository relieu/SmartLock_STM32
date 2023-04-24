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
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t fontNo);

//显示位图
void OLED_showBMP(uint8_t x, uint8_t y, uint8_t bmpNo);

//--------------------功能--------------------

//显示主界面
void OLED_showMenu_Main(void);

//显示信息-开锁中
void OLED_showInfo_unLocking(void);

//显示信息-锁定中
void OLED_showInfo_Locking(void);

//显示信息-开锁成功
void OLED_showInfo_unLock(void);

//显示信息-锁定完成
void OLED_showInfo_Lock(void);

//显示密码输入
void OLED_showMenu_Input();

//显示设置界面页1
void OLED_showPage_Settings1(void);

//显示设置界面页2
void OLED_showPage_Settings2(void);

//显示设置页面底部
void OLED_showInfo_SettingsFooter(uint8_t page);

//密码错误
void OLED_showInfo_PWError(void);

//显示密码正确
void OLED_showInfo_PWOK(void);

//进入设置页面
void OLED_showMenu_Settings(void);

//修改密码界面
void OLED_showMenu_resetPW(void);

//显示修改密码成功
void OLED_showInfo_resetOk(void);

//进入修改时间界面
void OLED_showMenu_resetTime(void);

//检测到指纹
void OLED_showInfo_FPDetected(void);

//指纹检测失败
void OLED_showInfo_FPError(void);

//指纹检测成功
void OLED_showInfo_FPOk(void);

//添加指纹菜单
void OLED_showInfo_addFP(void);

//添加指纹失败
void OLED_showInfo_addError(void);

//添加指纹成功
void OLED_showInfo_addOK(void);

//显示指纹删除菜单
void OLED_showMenu_delFP(void);

//删除失败
void OLED_showMenu_delError(void);

//删除成功
void OLED_showInfo_delOK(void);

//显示系统信息
void OLED_showInfo_Overall(void);

//显示上电欢迎界面
void OLED_showInfo_Boot(void);

//--------------------OLED综合--------------------

//OLED模块 汇总
void OLED_Overall(void);

#endif //SMARTLOCK_STM32_OLED_H
