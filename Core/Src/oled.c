#include "oled.h"
#include "oledfonts.h"

//显存
uint8_t OLED_GRAM[128][8];

extern uint8_t FP_Receive_Buffer[];

extern rt_event_t input_event;
extern rt_sem_t finger_sem;

//发送指令
void OLED_Write_Cmd(uint8_t cmd) {
    HAL_I2C_Mem_Write(
            &hi2c1,
            OLED_ADDRESS,
            OLED_MODE_COM,
            I2C_MEMADD_SIZE_8BIT,
            &cmd,
            1,
            100
    );
}

//发送数据
void OLED_Write_Dat(uint8_t data) {
    HAL_I2C_Mem_Write(
            &hi2c1,
            OLED_ADDRESS,
            OLED_MODE_DAT,
            I2C_MEMADD_SIZE_8BIT,
            &data,
            1,
            100
    );
}

//初始化
void OLED_Init(void) {
    HAL_Delay(100);

    OLED_Write_Cmd(0xAE);    // 关闭OLED -- turn off oled panel
    OLED_Write_Cmd(0xD5);    // 设置显示时钟分频因子/振荡器频率 -- set display clock divide ratio/oscillator frequency
    OLED_Write_Cmd(0x80);    // \ set divide ratio, Set Clock as 100 Frames/Sec
    OLED_Write_Cmd(0x20);    // 设置内存寻址模式 -- Set Memory Addressing Mode (0x00 / 0x01 / 0x02)
    OLED_Write_Cmd(0x02);    // \ Page Addressing
    OLED_Write_Cmd(0xA8);    // 设置多路传输比率 -- set multiplex ratio (16 to 63)
    OLED_Write_Cmd(0x3F);    // \ 1 / 64 duty
    OLED_Write_Cmd(0xDA);    // 设置列引脚硬件配置 -- set com pins hardware configuration
    OLED_Write_Cmd(0x12);    // \ Sequential COM pin configuration，Enable COM Left/Right remap

    /* ----- 方向显示配置 ----- */
    OLED_Write_Cmd(0xA1);    // 设置段重映射 -- Set SEG / Column Mapping  0xA0正常（复位值） 0xA1左右反置（重映射值）
    OLED_Write_Cmd(0xC8);    // 设置行输出扫描方向 -- Set COM / Row Scan Direction  0xc0正常（复位值） 0xC8上下反置（重映射值）

    OLED_Write_Cmd(0x40);    // 设置设置屏幕（GDDRAM）起始行 -- Set Display Start Line (0x40~0x7F)
    OLED_Write_Cmd(0xD3);    // 设置显示偏移 -- set display offset (0x00~0x3F)
    OLED_Write_Cmd(0x00);    // \ not offset
    OLED_Write_Cmd(0x81);    // 设置对比度 -- set contrast control register (0x00~0x100)
    OLED_Write_Cmd(0xCF);    // \ Set SEG Output Current Brightness
    OLED_Write_Cmd(0xD9);    // 设置预充电期间的持续时间 -- set pre-charge period
    OLED_Write_Cmd(0xF1);    // \ Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Write_Cmd(0xDB);    // 调整VCOMH调节器的输出 -- set vcomh (0x00 / 0x20 / 0x30)
    OLED_Write_Cmd(0x20);    // \ Set VCOM Deselect Level
    OLED_Write_Cmd(0x8D);    // 电荷泵设置 -- set Charge Pump enable / disable (0x14 / 0x10)
    OLED_Write_Cmd(0x14);    // \ Enable charge pump during display on
    OLED_Write_Cmd(0xA4);    // 全局显示开启(黑屏/亮屏) -- Entire Display On (0xA4 / 0xA5)
    OLED_Write_Cmd(0xA6);    // 设置显示方式(正常/反显) -- set normal display (0xA6 / 0xA7)
    OLED_Write_Cmd(0xAF);    // 打开OLED -- turn on oled panel

    OLED_Clear();
    OLED_Display_Update();
}

//开启显示
void OLED_Display_On(void) {
    OLED_Write_Cmd(0x8D); //SET DC-DC命令
    OLED_Write_Cmd(0x14); //DC-DC ON
    OLED_Write_Cmd(0xAF); //DISPLAY ON
}

//关闭显示
void OLED_Display_Off(void) {
    OLED_Write_Cmd(0x8D); //SET DC-DC命令
    OLED_Write_Cmd(0x10); //DC-DC OFF
    OLED_Write_Cmd(0xAE); //DISPLAY OFF
}

//更新显示
void OLED_Display_Update(void) {
    uint8_t i, n;
    for (i = 0; i < 8; i++) {
        OLED_Write_Cmd(0xb0 + i); //设置页地址（0~7）
        OLED_Write_Cmd(0x00); //设置显示位置—列低地址
        OLED_Write_Cmd(0x10); //设置显示位置—列高地址
        for (n = 0; n < 128; n++)OLED_Write_Dat(OLED_GRAM[n][i]);
    }
}

//清除屏幕
void OLED_Clear(void) {
    uint8_t i, n;
    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)OLED_GRAM[n][i] = 0X00;
//    OLED_Display_Update(); //更新显示
}

//反色
void OLED_Inverse(uint8_t i) {
    if (i) OLED_Write_Cmd(0xA7);
    else OLED_Write_Cmd(0xA6);
}

//画点
void OLED_DrawPoint(uint8_t x, uint8_t y) {
    uint8_t page, bit, temp;
    if (x > 127 || y > 63) return;//超出范围
    page = y / 8;
    bit = y % 8;
    temp = 1 << bit;
    OLED_GRAM[x][page] |= temp;
}

void OLED_ClearPoint(uint8_t x, uint8_t y) {
    uint8_t page, bit, temp;
    if (x > 127 || y > 63) return;//超出范围
    page = y / 8;
    bit = y % 8;
    temp = 1 << bit;
    OLED_GRAM[x][page] &= ~temp;
}

//填充区域
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t isLight) {
    uint8_t x, y;
    if (x1 >= x2 || y1 >= y2) return;
    for (x = x1; x <= x2; x++) {
        for (y = y1; y <= y2; y++) {
            if (isLight) OLED_DrawPoint(x, y);
            else OLED_ClearPoint(x, y);
        }
    }
//    OLED_Display_Update();//更新显示
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t size) {
    uint8_t byte;
    const uint8_t y0 = y;

    //得到字体一个字符对应点阵集所占的字节数
    uint8_t chSize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    //偏移值
    ch -= ' ';

    for (uint8_t i = 0; i < chSize; i++) {
        if (size == 12) byte = ASCII_12x06[ch][i];        //调用1206字体
        else if (size == 16) byte = ASCII_16x08[ch][i];    //调用1608字体
        else if (size == 24) byte = ASCII_24x12[ch][i];    //调用2412字体
        else return;                                //没有的字库
        for (uint8_t j = 0; j < 8; j++) {
            if (byte & 0x80) OLED_DrawPoint(x, y);
            else OLED_ClearPoint(x, y);
            byte <<= 1;
            y++;
            if ((y - y0) == size) {
                y = y0;
                x++;
                break;
            }
        }
    }
//    OLED_Display_Update();//更新显示
}

//显示数字
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size) {
    uint8_t nums[len];

    //分解数字
    for (int i = 0; i < len; i++) {
        if (num) nums[len - i - 1] = num % 10 + '0';
        else nums[len - i - 1] = '0';
        num /= 10;
    }

    for (int i = 0; i < len; i++) {
        OLED_ShowChar(x + (size / 2) * i, y, nums[i], size);
    }
//    OLED_Display_Update();//更新显示
}

//显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, const char *pStr, uint8_t size) {
    while ((*pStr <= '~') && (*pStr >= ' '))//判断非法字符 含'\0'
    {
        OLED_ShowChar(x, y, *pStr, size);
        x += size / 2;
        pStr++;
    }
//    OLED_Display_Update();//更新显示
}

//显示中文16x16
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t fontNo) {
    uint8_t byte;

    for (uint8_t i = 0; i < 2; ++i) {

        for (uint8_t j = 0; j < 16; ++j) {

            byte = Chinese_16x16[fontNo * 2 + i][j];

            for (uint8_t k = 0; k < 8; ++k) {
                if (byte & 0x80) OLED_DrawPoint(x, y);
                else OLED_ClearPoint(x, y);
                byte <<= 1;
                x++;
            }
            x -= 8;
            y++;
        }
        x += 8;
        y -= 16;
    }
}

//显示位图
void OLED_showBMP(uint8_t x, uint8_t y, uint8_t bmpNo) {
    uint8_t byte;

    for (uint8_t i = 0; i < 4; ++i) {

        for (uint8_t j = 0; j < 32; ++j) {

            byte = BMP_32x32[bmpNo * 4 + i][j];

            for (uint8_t k = 0; k < 8; ++k) {
                if (byte & 0x80) OLED_DrawPoint(x, y);
                else OLED_ClearPoint(x, y);
                byte <<= 1;
                x++;
            }
            x -= 8;
            y++;
        }
        x += 8;
        y -= 32;
    }
}

//显示主界面
void OLED_showMenu_Main(void) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    char weekday[4];

    switch (date.WeekDay) {
        case RTC_WEEKDAY_MONDAY:
            weekday[0] = 'M';
            weekday[1] = 'o';
            weekday[2] = 'n';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_TUESDAY:
            weekday[0] = 'T';
            weekday[1] = 'u';
            weekday[2] = 'e';
            weekday[3] = 's';
            break;
        case RTC_WEEKDAY_WEDNESDAY:
            weekday[0] = 'W';
            weekday[1] = 'e';
            weekday[2] = 'd';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_THURSDAY:
            weekday[0] = 'T';
            weekday[1] = 'h';
            weekday[2] = 'u';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_FRIDAY:
            weekday[0] = 'F';
            weekday[1] = 'r';
            weekday[2] = 'i';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_SATURDAY:
            weekday[0] = 'S';
            weekday[1] = 'a';
            weekday[2] = 't';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_SUNDAY:
            weekday[0] = 'S';
            weekday[1] = 'u';
            weekday[2] = 'n';
            weekday[3] = ' ';
            break;
        default:
            break;
    }
    OLED_Clear();

    OLED_ShowString(100, 8, weekday, 16);
    OLED_ShowNum(68, 30, date.Year + 2000, 4, 12);
    OLED_ShowChar(92, 30, '-', 12);
    OLED_ShowNum(98, 30, date.Month, 2, 12);
    OLED_ShowChar(110, 30, '-', 12);
    OLED_ShowNum(116, 30, date.Date, 2, 12);

    OLED_showBMP(16, 32, 0);

    while (1) {
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

        OLED_ShowNum(0, 0, time.Hours, 2, 24);
        OLED_ShowChar(24, 0, ':', 24);
        OLED_ShowNum(36, 0, time.Minutes, 2, 24);
        OLED_ShowChar(60, 0, ':', 24);
        OLED_ShowNum(72, 0, time.Seconds, 2, 24);

        OLED_Display_Update();

        //接收到输入event
        if (rt_event_recv(input_event, 0x0001FFFF,
                          RT_EVENT_FLAG_OR,
                          500, RT_NULL)
            == RT_EOK) {
            return;
        }
    }
}

//显示信息-开锁
void OLED_showInfo_unLocking(void) {
    OLED_Clear();

    OLED_ShowChinese(32, 16, 0);
    OLED_ShowChinese(48, 16, 1);
    OLED_ShowChinese(64, 16, 2);

    OLED_ShowChar(80, 16, '.', 12);
    OLED_ShowChar(86, 16, '.', 12);
    OLED_ShowChar(92, 16, '.', 12);

    OLED_Display_Update();
}

//显示信息-锁定
void OLED_showInfo_Locking(void) {
    OLED_Clear();

    OLED_ShowChinese(32, 16, 1);
    OLED_ShowChinese(48, 16, 5);
    OLED_ShowChinese(64, 16, 2);

    OLED_ShowChar(80, 16, '.', 12);
    OLED_ShowChar(86, 16, '.', 12);
    OLED_ShowChar(92, 16, '.', 12);

    OLED_Display_Update();
}

//显示信息-开锁成功
void OLED_showInfo_unLock(void) {
    OLED_Clear();

    OLED_ShowChinese(40, 10, 4);
    OLED_ShowChinese(56, 10, 3);
    OLED_ShowChinese(72, 10, 1);

    OLED_showBMP(48, 30, 1);

    OLED_Display_Update();
}

//显示信息-锁定完成
void OLED_showInfo_Lock(void) {
    OLED_Clear();

    OLED_ShowChinese(40, 10, 4);
    OLED_ShowChinese(56, 10, 1);
    OLED_ShowChinese(72, 10, 5);

    OLED_showBMP(48, 30, 0);

    OLED_Display_Update();
}

//显示密码输入
void OLED_showMenu_Input() {
    OLED_Clear();

    OLED_ShowNum(28, 16, 1, 1, 12);
    OLED_ShowNum(46, 16, 2, 1, 12);
    OLED_ShowNum(64, 16, 3, 1, 12);
    OLED_ShowString(82, 16, "Lock", 12);

    OLED_ShowNum(28, 28, 4, 1, 12);
    OLED_ShowNum(46, 28, 5, 1, 12);
    OLED_ShowNum(64, 28, 6, 1, 12);
    OLED_ShowString(82, 28, "Del ", 12);

    OLED_ShowNum(28, 40, 1, 1, 12);
    OLED_ShowNum(46, 40, 2, 1, 12);
    OLED_ShowNum(64, 40, 3, 1, 12);
    OLED_ShowString(82, 40, "Dis ", 12);

    OLED_ShowChar(28, 52, '*', 12);
    OLED_ShowNum(46, 52, 0, 1, 12);
    OLED_ShowChar(64, 52, '#', 12);
    OLED_ShowString(82, 52, "OK  ", 12);

    //"密码："
    OLED_ShowChinese(16, 0, 6);
    OLED_ShowChinese(32, 0, 7);
    OLED_ShowChinese(48, 0, 8);


    uint8_t Flag_showPW = 0;
    uint8_t Flag_errorPW = 0;

    uint8_t inputCnt = 0;
    char inputStr[7] = {' ', ' ', ' ', ' ', ' ', ' ', 0};
    char inputStars[7] = {' ', ' ', ' ', ' ', ' ', ' ', 0};
    uint8_t password[7];

    while (1) {

        //超出范围 or 密码错误
        if (inputCnt > 6 || Flag_errorPW) {
            OLED_showInfo_PWError();
            return;
        }

        //按下锁定按键A
        if (rt_event_recv(input_event, 0x00001000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            rt_event_send(input_event, 0x80000000);
            return;
        }

        //1
        if (rt_event_recv(input_event, 0x00000001,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '1';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //2
        if (rt_event_recv(input_event, 0x00000010,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '2';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //3
        if (rt_event_recv(input_event, 0x00000100,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '3';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //4
        if (rt_event_recv(input_event, 0x00000002,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '4';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //5
        if (rt_event_recv(input_event, 0x00000020,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '5';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //6
        if (rt_event_recv(input_event, 0x00000200,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '6';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //7
        if (rt_event_recv(input_event, 0x00000004,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '7';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //8
        if (rt_event_recv(input_event, 0x00000040,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '8';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //9
        if (rt_event_recv(input_event, 0x00000400,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '9';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //0
        if (rt_event_recv(input_event, 0x00000080,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '0';
            inputStars[inputCnt] = '*';
            inputCnt++;
        }

        //backspace
        if (rt_event_recv(input_event, 0x00002000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputCnt--;
            inputStr[inputCnt] = ' ';
            inputStars[inputCnt] = ' ';
        }

        //*
        if (rt_event_recv(input_event, 0x00000008,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //do nothing
        }

        //#
        if (rt_event_recv(input_event, 0x00000800,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //do nothing
        }

        //显示密码
        if (rt_event_recv(input_event, 0x00004000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            if (Flag_showPW) Flag_showPW = 0;
            else Flag_showPW = 1;
        }

        //提交密码
        if (rt_event_recv(input_event, 0x00008000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            FLASH_readData(password, 6);

            for (int i = 0; i < 6; ++i) {
                if (password[i] == inputStr[i]) continue;
                else {
                    Flag_errorPW = 1;
                    break;
                }
            }

            //验证成功
            if (Flag_errorPW != 1) {
                rt_event_send(input_event, 0x48000000);
                OLED_Clear();
                OLED_showInfo_PWOK();
                //暂停指纹检测
                rt_sem_take(finger_sem, RT_WAITING_FOREVER);
                return;
            }
        }

        if (Flag_showPW) OLED_ShowString(64, 0, inputStr, 16);
        else OLED_ShowString(64, 0, inputStars, 16);

        OLED_Display_Update();

    }
}

//显示设置界面1
void OLED_showPage_Settings1(void) {
    OLED_Clear();

    //"1. 录入指纹"
    OLED_ShowString(24, 0, "1. ", 16);
    OLED_ShowChinese(48, 0, 12);
    OLED_ShowChinese(64, 0, 13);
    OLED_ShowChinese(80, 0, 14);
    OLED_ShowChinese(96, 0, 15);

    //"2. 删除指纹"
    OLED_ShowString(24, 16, "2. ", 16);
    OLED_ShowChinese(48, 16, 16);
    OLED_ShowChinese(64, 16, 17);
    OLED_ShowChinese(80, 16, 14);
    OLED_ShowChinese(96, 16, 15);

    //"3. 修改密码"
    OLED_ShowString(24, 32, "3. ", 16);
    OLED_ShowChinese(48, 32, 18);
    OLED_ShowChinese(64, 32, 19);
    OLED_ShowChinese(80, 32, 6);
    OLED_ShowChinese(96, 32, 7);

    OLED_showInfo_SettingsFooter(1);
}

//显示设置界面2
void OLED_showPage_Settings2(void) {
    OLED_Clear();

    //"4. 修改时间"
    OLED_ShowString(24, 0, "4. ", 16);
    OLED_ShowChinese(48, 0, 18);
    OLED_ShowChinese(64, 0, 19);
    OLED_ShowChinese(80, 0, 20);
    OLED_ShowChinese(96, 0, 21);

    //"5. 显示信息"
    OLED_ShowString(24, 16, "5. ", 16);
    OLED_ShowChinese(48, 16, 55);
    OLED_ShowChinese(64, 16, 23);
    OLED_ShowChinese(80, 16, 24);
    OLED_ShowChinese(96, 16, 25);

    OLED_showInfo_SettingsFooter(2);
}

//显示设置页面底部
void OLED_showInfo_SettingsFooter(uint8_t page) {

    //↑ 1/2 ↓ 确认
    OLED_ShowChinese(0, 48, 10);

    OLED_ShowChar(28, 48, '0' + page, 16);
    OLED_ShowString(36, 48, "/2", 16);

    OLED_ShowChinese(64, 48, 11);

    OLED_ShowChinese(92, 48, 26);
    OLED_ShowChinese(108, 48, 27);
}

//密码错误
void OLED_showInfo_PWError(void) {
    OLED_Clear();
    //"密码错误"
    OLED_ShowChinese(32, 16, 6);
    OLED_ShowChinese(48, 16, 7);
    OLED_ShowChinese(64, 16, 28);
    OLED_ShowChinese(80, 16, 29);
    //"请重试"
    OLED_ShowChinese(40, 32, 32);
    OLED_ShowChinese(56, 32, 33);
    OLED_ShowChinese(72, 32, 34);

    OLED_Display_Update();
    BUZZ_Beep(3);

    rt_event_send(input_event, 0x80000000);
    rt_thread_delay(1000);
}

//显示密码正确
void OLED_showInfo_PWOK(void) {
    OLED_Clear();

    //"密码正确"
    OLED_ShowChinese(32, 10, 6);
    OLED_ShowChinese(48, 10, 7);
    OLED_ShowChinese(64, 10, 35);
    OLED_ShowChinese(80, 10, 36);

    OLED_Display_Update();
}

//进入设置页面
void OLED_showMenu_Settings(void) {
    uint8_t state = 0;

    OLED_showPage_Settings1();
    OLED_ShowChinese(0, (state % 3) * 16, 9);
    OLED_Display_Update();

    while (1) {

        //按下锁定按键A
        if (rt_event_recv(input_event, 0x00001000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            OLED_showInfo_Locking();
            MOTOR_Lock();
            rt_event_send(input_event, 0x80000000);
            OLED_showInfo_Lock();
            return;
        }

        //按下'*'键 向上
        if (rt_event_recv(input_event, 0x00000008,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {

            if (state == 0) {
                state = 5;
                OLED_showPage_Settings2();
            }
            state--;

            if (state == 2) OLED_showPage_Settings1();

            OLED_Fill(0, 0, 16, 48, FALSE);
            OLED_ShowChinese(0, (state % 3) * 16, 9);
            OLED_Display_Update();
        }

        //按下'#'键 向下
        if (rt_event_recv(input_event, 0x00000800,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {

            state++;
            if (state == 5) {
                state = 0;
                OLED_showPage_Settings1();
            }

            if (state == 3) OLED_showPage_Settings2();

            OLED_Fill(0, 0, 16, 48, FALSE);
            OLED_ShowChinese(0, (state % 3) * 16, 9);
            OLED_Display_Update();
        }

        //确认
        if (rt_event_recv(input_event, 0x000008000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {

            switch (state) {
                case 0:
                    //录入指纹
                    OLED_showInfo_addFP();
                    OLED_showPage_Settings1();
                    OLED_ShowChinese(0, (state % 3) * 16, 9);
                    OLED_Display_Update();
                    break;
                case 1:
                    //删除指纹
                    OLED_showMenu_delFP();
                    OLED_showPage_Settings1();
                    OLED_ShowChinese(0, (state % 3) * 16, 9);
                    OLED_Display_Update();
                    break;
                case 2:
                    //修改密码
                    OLED_showMenu_resetPW();
                    OLED_showPage_Settings1();
                    OLED_ShowChinese(0, (state % 3) * 16, 9);
                    OLED_Display_Update();
                    break;
                case 3:
                    //修改时间
                    OLED_showMenu_resetTime();
                    OLED_showPage_Settings2();
                    OLED_ShowChinese(0, (state % 3) * 16, 9);
                    OLED_Display_Update();
                    break;
                case 4:
                    //查看信息
                    OLED_showInfo_Overall();
                    OLED_showPage_Settings2();
                    OLED_ShowChinese(0, (state % 3) * 16, 9);
                    OLED_Display_Update();
                    break;
                default:
                    break;
            }
        }
    }
}

//修改密码界面
void OLED_showMenu_resetPW(void) {
    OLED_Clear();

    OLED_ShowNum(28, 16, 1, 1, 12);
    OLED_ShowNum(46, 16, 2, 1, 12);
    OLED_ShowNum(64, 16, 3, 1, 12);
    OLED_ShowString(82, 16, "Lock", 12);

    OLED_ShowNum(28, 28, 4, 1, 12);
    OLED_ShowNum(46, 28, 5, 1, 12);
    OLED_ShowNum(64, 28, 6, 1, 12);
    OLED_ShowString(82, 28, "Del ", 12);

    OLED_ShowNum(28, 40, 1, 1, 12);
    OLED_ShowNum(46, 40, 2, 1, 12);
    OLED_ShowNum(64, 40, 3, 1, 12);
    OLED_ShowString(82, 40, "Dis ", 12);

    OLED_ShowChar(28, 52, '*', 12);
    OLED_ShowNum(46, 52, 0, 1, 12);
    OLED_ShowChar(64, 52, '#', 12);
    OLED_ShowString(82, 52, "OK  ", 12);

    //"新密码："
    OLED_ShowChinese(0, 0, 37);
    OLED_ShowChinese(16, 0, 6);
    OLED_ShowChinese(32, 0, 7);
    OLED_ShowChinese(48, 0, 8);

    OLED_Display_Update();


    uint8_t inputCnt = 0;
    char inputStr[7] = {' ', ' ', ' ', ' ', ' ', ' ', 0};

    while (1) {

        //超出范围
        if (inputCnt > 6) {
            OLED_showInfo_PWError();
            return;
        }

        //按下锁定按键A
        if (rt_event_recv(input_event, 0x00001000,
                          RT_EVENT_FLAG_OR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            rt_event_send(input_event, 0x80000000);
            return;
        }

        //1
        if (rt_event_recv(input_event, 0x00000001,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '1';
            inputCnt++;
        }

        //2
        if (rt_event_recv(input_event, 0x00000010,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '2';
            inputCnt++;
        }

        //3
        if (rt_event_recv(input_event, 0x00000100,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '3';
            inputCnt++;
        }

        //4
        if (rt_event_recv(input_event, 0x00000002,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '4';
            inputCnt++;
        }

        //5
        if (rt_event_recv(input_event, 0x00000020,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '5';
            inputCnt++;
        }

        //6
        if (rt_event_recv(input_event, 0x00000200,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '6';
            inputCnt++;
        }

        //7
        if (rt_event_recv(input_event, 0x00000004,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '7';
            inputCnt++;
        }

        //8
        if (rt_event_recv(input_event, 0x00000040,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '8';
            inputCnt++;
        }

        //9
        if (rt_event_recv(input_event, 0x00000400,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '9';
            inputCnt++;
        }

        //0
        if (rt_event_recv(input_event, 0x00000080,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputStr[inputCnt] = '0';
            inputCnt++;
        }

        //backspace
        if (rt_event_recv(input_event, 0x00002000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            inputCnt--;
            inputStr[inputCnt] = ' ';
        }

        //*
        if (rt_event_recv(input_event, 0x00000008,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //do nothing
        }

        //#
        if (rt_event_recv(input_event, 0x00000800,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //do nothing
        }

        OLED_ShowString(64, 0, inputStr, 16);
        OLED_Display_Update();

        //确认修改
        if (rt_event_recv(input_event, 0x00008000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            FLASH_writeData((uint8_t *) inputStr, 6);
            OLED_showInfo_resetOk();
            rt_thread_delay(500);
            rt_event_send(input_event, 0x00001000);
        }
    }

}

//显示修改密码成功
void OLED_showInfo_resetOk(void) {
    OLED_Clear();

    //"修改成功"
    OLED_ShowChinese(32, 10, 18);
    OLED_ShowChinese(48, 10, 19);
    OLED_ShowChinese(64, 10, 38);
    OLED_ShowChinese(80, 10, 39);

    OLED_Display_Update();
}

//进入修改时间界面
void OLED_showMenu_resetTime(void) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    uint8_t select = 0;
    uint8_t Flag_reset = 0;

    char weekday[4];

    switch (date.WeekDay) {
        case RTC_WEEKDAY_MONDAY:
            weekday[0] = 'M';
            weekday[1] = 'o';
            weekday[2] = 'n';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_TUESDAY:
            weekday[0] = 'T';
            weekday[1] = 'u';
            weekday[2] = 'e';
            weekday[3] = 's';
            break;
        case RTC_WEEKDAY_WEDNESDAY:
            weekday[0] = 'W';
            weekday[1] = 'e';
            weekday[2] = 'd';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_THURSDAY:
            weekday[0] = 'T';
            weekday[1] = 'h';
            weekday[2] = 'u';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_FRIDAY:
            weekday[0] = 'F';
            weekday[1] = 'r';
            weekday[2] = 'i';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_SATURDAY:
            weekday[0] = 'S';
            weekday[1] = 'a';
            weekday[2] = 't';
            weekday[3] = ' ';
            break;
        case RTC_WEEKDAY_SUNDAY:
            weekday[0] = 'S';
            weekday[1] = 'u';
            weekday[2] = 'n';
            weekday[3] = ' ';
            break;
        default:
            break;
    }

    OLED_Clear();

    OLED_ShowString(100, 8, weekday, 16);
    OLED_ShowNum(68, 30, date.Year + 2000, 4, 12);
    OLED_ShowChar(92, 30, '-', 12);
    OLED_ShowNum(98, 30, date.Month, 2, 12);
    OLED_ShowChar(110, 30, '-', 12);
    OLED_ShowNum(116, 30, date.Date, 2, 12);

    OLED_ShowNum(0, 0, time.Hours, 2, 24);
    OLED_ShowChar(24, 0, ':', 24);
    OLED_ShowNum(36, 0, time.Minutes, 2, 24);
    OLED_ShowChar(60, 0, ':', 24);
    OLED_ShowNum(72, 0, time.Seconds, 2, 24);

    //← OK → 确认
    OLED_ShowChinese(0, 48, 42);
    OLED_ShowString(32, 48, "OK", 16);
    OLED_ShowChinese(64, 48, 9);
    OLED_ShowChinese(92, 48, 26);
    OLED_ShowChinese(108, 48, 27);

    OLED_Display_Update();

    while (1) {

        switch (select) {
            case 0:
                OLED_Fill(0, 0, 24, 24, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowNum(0, 0, time.Hours, 2, 24);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            case 1:
                OLED_Fill(36, 0, 60, 24, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowNum(36, 0, time.Minutes, 2, 24);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            case 2:
                OLED_Fill(72, 0, 96, 24, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowNum(72, 0, time.Seconds, 2, 24);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            case 3:
                OLED_Fill(100, 8, 128, 24, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowString(100, 8, weekday, 16);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            case 4:
                OLED_Fill(68, 30, 92, 42, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowNum(68, 30, date.Year + 2000, 4, 12);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            case 5:
                OLED_Fill(98, 30, 110, 42, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowNum(98, 30, date.Month, 2, 12);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            case 6:
                OLED_Fill(116, 30, 128, 42, FALSE);
                OLED_Display_Update();
                rt_thread_delay(300);
                OLED_ShowNum(116, 30, date.Date, 2, 12);
                OLED_Display_Update();
                rt_thread_delay(300);
                break;
            default:
                break;
        }

        //按下锁定按键A
        if (rt_event_recv(input_event, 0x00001000,
                          RT_EVENT_FLAG_OR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            rt_event_send(input_event, 0x80000000);
            return;
        }

        //按下'*'键 向上/左
        if (rt_event_recv(input_event, 0x00000008,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //选择模式
            if (Flag_reset == 0) {
                if (select == 0) select = 7;
                select--;
            } else {
                switch (select) {
                    case 0:
                        time.Hours++;
                        if (time.Hours == 24) time.Hours = 0;
                        break;
                    case 1:
                        time.Minutes++;
                        if (time.Minutes == 60) time.Minutes = 0;
                        break;
                    case 2:
                        time.Seconds++;
                        if (time.Seconds == 60) time.Seconds = 0;
                    case 3:
                        if (date.WeekDay == RTC_WEEKDAY_SATURDAY) date.WeekDay = RTC_WEEKDAY_SUNDAY;
                        else date.WeekDay++;
                        switch (date.WeekDay) {
                            case RTC_WEEKDAY_MONDAY:
                                weekday[0] = 'M';
                                weekday[1] = 'o';
                                weekday[2] = 'n';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_TUESDAY:
                                weekday[0] = 'T';
                                weekday[1] = 'u';
                                weekday[2] = 'e';
                                weekday[3] = 's';
                                break;
                            case RTC_WEEKDAY_WEDNESDAY:
                                weekday[0] = 'W';
                                weekday[1] = 'e';
                                weekday[2] = 'd';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_THURSDAY:
                                weekday[0] = 'T';
                                weekday[1] = 'h';
                                weekday[2] = 'u';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_FRIDAY:
                                weekday[0] = 'F';
                                weekday[1] = 'r';
                                weekday[2] = 'i';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_SATURDAY:
                                weekday[0] = 'S';
                                weekday[1] = 'a';
                                weekday[2] = 't';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_SUNDAY:
                                weekday[0] = 'S';
                                weekday[1] = 'u';
                                weekday[2] = 'n';
                                weekday[3] = ' ';
                                break;
                            default:
                                break;
                        }
                        break;

                    case 4:
                        date.Year++;
                        break;
                    case 5:
                        if (date.Month == RTC_MONTH_DECEMBER) date.Month = RTC_MONTH_JANUARY;
                        else date.Month++;
                        break;
                    case 6:
                        if (date.Date == 31) date.Date = 0;
                        else date.Date++;
                        break;
                    default:
                        break;
                }
            }
        }

        //按下'#'键 向下/右
        if (rt_event_recv(input_event, 0x00000800,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //选择模式
            if (Flag_reset == 0) {
                select++;
                if (select == 7) select = 0;
            } else {
                switch (select) {
                    case 0:
                        if (time.Hours == 0) time.Hours = 24;
                        time.Hours--;
                        break;
                    case 1:
                        if (time.Minutes == 0) time.Minutes = 60;
                        time.Minutes--;
                        break;
                    case 2:
                        if (time.Seconds == 0) time.Seconds = 60;
                        time.Seconds--;
                    case 3:
                        if (date.WeekDay == RTC_WEEKDAY_SUNDAY) date.WeekDay = RTC_WEEKDAY_SATURDAY;
                        else date.WeekDay--;
                        switch (date.WeekDay) {
                            case RTC_WEEKDAY_MONDAY:
                                weekday[0] = 'M';
                                weekday[1] = 'o';
                                weekday[2] = 'n';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_TUESDAY:
                                weekday[0] = 'T';
                                weekday[1] = 'u';
                                weekday[2] = 'e';
                                weekday[3] = 's';
                                break;
                            case RTC_WEEKDAY_WEDNESDAY:
                                weekday[0] = 'W';
                                weekday[1] = 'e';
                                weekday[2] = 'd';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_THURSDAY:
                                weekday[0] = 'T';
                                weekday[1] = 'h';
                                weekday[2] = 'u';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_FRIDAY:
                                weekday[0] = 'F';
                                weekday[1] = 'r';
                                weekday[2] = 'i';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_SATURDAY:
                                weekday[0] = 'S';
                                weekday[1] = 'a';
                                weekday[2] = 't';
                                weekday[3] = ' ';
                                break;
                            case RTC_WEEKDAY_SUNDAY:
                                weekday[0] = 'S';
                                weekday[1] = 'u';
                                weekday[2] = 'n';
                                weekday[3] = ' ';
                                break;
                            default:
                                break;
                        }
                        break;

                    case 4:
                        date.Year--;
                        break;
                    case 5:
                        if (date.Month == RTC_MONTH_JANUARY) date.Month = RTC_MONTH_DECEMBER;
                        else date.Month--;
                        break;
                    case 6:
                        if (date.Date == 0) date.Date = 31;
                        else date.Date--;
                        break;
                    default:
                        break;
                }
            }
        }

        //按下'0'键 修改/锁定
        if (rt_event_recv(input_event, 0x00000080,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            if (Flag_reset == 0) {
                Flag_reset = 1;
                //上下
                OLED_ShowChinese(0, 48, 10);
                OLED_ShowChinese(64, 48, 11);
            } else {
                Flag_reset = 0;
                //左右
                OLED_ShowChinese(0, 48, 42);
                OLED_ShowChinese(64, 48, 9);
            }
        }

        //确认修改
        if (rt_event_recv(input_event, 0x00008000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            HAL_RTC_SetTime(&hrtc, &time, FORMAT_BIN);
            HAL_RTC_SetDate(&hrtc, &date, FORMAT_BIN);
            RTC_setDate(date);

            OLED_showInfo_resetOk();
            rt_thread_delay(500);
            return;
        }
    }
}

//检测到指纹
void OLED_showInfo_FPDetected(void) {
    OLED_Clear();

    //"指纹检测中"
    OLED_ShowChinese(24, 20, 14);
    OLED_ShowChinese(40, 20, 15);
    OLED_ShowChinese(56, 20, 43);
    OLED_ShowChinese(72, 20, 44);
    OLED_ShowChinese(88, 20, 2);

    OLED_Display_Update();

    rt_thread_delay(100);

    FP_genChara(1);
    if (FP_Receive_Buffer[9] != 0x00) {
        OLED_showInfo_FPError();
        return;
    }

    FP_Search(0, 100);
    if (FP_Receive_Buffer[9] != 0x00) {
        OLED_showInfo_FPError();
        return;
    } else {
        OLED_showInfo_FPOk();
        rt_thread_delay(500);
        rt_event_send(input_event, 0x44000000);
    }
}

//指纹检测失败
void OLED_showInfo_FPError(void) {
    OLED_Clear();

    //"指纹检测失败"
    OLED_ShowChinese(16, 16, 14);
    OLED_ShowChinese(32, 16, 15);
    OLED_ShowChinese(48, 16, 43);
    OLED_ShowChinese(64, 16, 44);
    OLED_ShowChinese(80, 16, 30);
    OLED_ShowChinese(96, 16, 31);

    //"请重试"
    OLED_ShowChinese(40, 32, 32);
    OLED_ShowChinese(56, 32, 33);
    OLED_ShowChinese(72, 32, 34);

    OLED_Display_Update();

    BUZZ_Beep(3);

    rt_thread_delay(500);
    rt_event_send(input_event, 0x80000000);
    rt_sem_release(finger_sem);
}

//指纹检测成功
void OLED_showInfo_FPOk(void) {
    OLED_Clear();

    //"指纹检测成功"
    OLED_ShowChinese(16, 12, 14);
    OLED_ShowChinese(32, 12, 15);
    OLED_ShowChinese(48, 12, 43);
    OLED_ShowChinese(64, 12, 44);
    OLED_ShowChinese(80, 12, 38);
    OLED_ShowChinese(96, 12, 39);

    uint16_t id = (FP_Receive_Buffer[10] << 8) + FP_Receive_Buffer[11];
    uint16_t score = (FP_Receive_Buffer[12] << 8) + FP_Receive_Buffer[13];

    OLED_ShowString(40, 28, "ID:", 16);
    OLED_ShowNum(64, 28, id, 3, 16);

    OLED_ShowString(40, 44, "Sc:", 16);
    OLED_ShowNum(64, 44, score, 3, 16);

    OLED_Display_Update();
}

//添加指纹菜单
void OLED_showInfo_addFP(void) {

    OLED_Clear();

    //"请输入指纹"
    OLED_ShowChinese(24, 24, 32);
    OLED_ShowChinese(40, 24, 40);
    OLED_ShowChinese(56, 24, 41);
    OLED_ShowChinese(72, 24, 14);
    OLED_ShowChinese(88, 24, 15);

    OLED_Display_Update();

    //开启指纹检测
    rt_sem_release(finger_sem);

    //检测到指纹输入
    if (rt_event_recv(input_event, 0x00010000,
                      RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      10000, RT_NULL)
        == RT_EOK) { ;
    } else {
        OLED_showInfo_addError();
        return;
    }

    FP_getImage();
    if (!FP_Receive_Buffer[9]) { ;
    } else {
        OLED_showInfo_addError();
        return;
    }

    OLED_Clear();

    //请再次输入指纹
    OLED_ShowChinese(8, 24, 32);
    OLED_ShowChinese(24, 24, 45);
    OLED_ShowChinese(40, 24, 46);
    OLED_ShowChinese(56, 24, 40);
    OLED_ShowChinese(72, 24, 41);
    OLED_ShowChinese(88, 24, 14);
    OLED_ShowChinese(104, 24, 15);

    OLED_Display_Update();

    rt_thread_delay(2000);

    //开启指纹检测
    rt_sem_release(finger_sem);

    //检测到指纹输入
    if (rt_event_recv(input_event, 0x00010000,
                      RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      10000, RT_NULL)
        == RT_EOK) { ;
    } else {
        OLED_showInfo_addError();
        return;
    }

    FP_autoStore();
    if (!FP_Receive_Buffer[9]) {
        OLED_showInfo_addOK();
        return;
    } else {
        OLED_showInfo_addError();
        return;
    }
}

//添加指纹失败
void OLED_showInfo_addError(void) {
    OLED_Clear();

    //"指纹录入失败"
    OLED_ShowChinese(16, 16, 14);
    OLED_ShowChinese(32, 16, 15);
    OLED_ShowChinese(48, 16, 12);
    OLED_ShowChinese(64, 16, 13);
    OLED_ShowChinese(80, 16, 30);
    OLED_ShowChinese(96, 16, 31);

    //"请重试"
    OLED_ShowChinese(40, 32, 32);
    OLED_ShowChinese(56, 32, 33);
    OLED_ShowChinese(72, 32, 34);

    OLED_Display_Update();

    rt_thread_delay(1000);
}

//添加指纹成功
void OLED_showInfo_addOK(void) {
    OLED_Clear();

    //"指纹录入成功"
    OLED_ShowChinese(16, 16, 14);
    OLED_ShowChinese(32, 16, 15);
    OLED_ShowChinese(48, 16, 12);
    OLED_ShowChinese(64, 16, 13);
    OLED_ShowChinese(80, 16, 38);
    OLED_ShowChinese(96, 16, 39);

    uint16_t id = (FP_Receive_Buffer[10] << 8) + FP_Receive_Buffer[11];

    //"ID:"
    OLED_ShowString(40, 32, "ID:", 16);

    OLED_ShowNum(64, 32, id, 3, 16);

    OLED_Display_Update();

    rt_thread_delay(1000);
}

//显示指纹删除菜单
void OLED_showMenu_delFP(void) {
    uint8_t inputCnt = 0;
    uint16_t intID = 0;
    uint16_t intTem = 0;
    uint8_t Flag_errorID = 0;
    uint8_t Flag_allInput = 0;

    OLED_Clear();

    //"请输入删除指纹"
    OLED_ShowChinese(8, 0, 32);
    OLED_ShowChinese(24, 0, 40);
    OLED_ShowChinese(40, 0, 41);
    OLED_ShowChinese(56, 0, 16);
    OLED_ShowChinese(72, 0, 17);
    OLED_ShowChinese(88, 0, 14);
    OLED_ShowChinese(104, 0, 15);

    //"ID:"
    OLED_ShowString(40, 16, "ID:", 16);
    OLED_ShowNum(64, 16, intID, 3, 16);

    //"*: ALL 确定"
    OLED_ShowString(8, 48, "*: ALL", 16);
    OLED_ShowChinese(92, 48, 26);
    OLED_ShowChinese(108, 48, 27);

    OLED_Display_Update();

    while (1) {
        //超出范围 or 密码错误
        if (inputCnt > 3 || Flag_errorID) {
            OLED_showMenu_delError();
            return;
        }

        //按下锁定按键A
        if (rt_event_recv(input_event, 0x00001000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            rt_event_send(input_event, 0x80000000);
            return;
        }

        //1
        if (rt_event_recv(input_event, 0x00000001,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 1;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //2
        if (rt_event_recv(input_event, 0x00000010,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 2;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //3
        if (rt_event_recv(input_event, 0x00000100,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 3;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //4
        if (rt_event_recv(input_event, 0x00000002,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 4;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //5
        if (rt_event_recv(input_event, 0x00000020,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 5;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //6
        if (rt_event_recv(input_event, 0x00000200,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 6;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //7
        if (rt_event_recv(input_event, 0x00000004,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 7;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //8
        if (rt_event_recv(input_event, 0x00000040,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 8;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //9
        if (rt_event_recv(input_event, 0x00000400,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 9;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //0
        if (rt_event_recv(input_event, 0x00000080,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            intTem = 0;
            if (!Flag_allInput) {
                inputCnt++;
                for (uint8_t i = 1; i < inputCnt; i++) intTem *= 10;
                intID += intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //backspace
        if (rt_event_recv(input_event, 0x00002000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            if (!Flag_allInput) {
                inputCnt--;
                intID -= intTem;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            } else {
                Flag_allInput = 0;

                OLED_ShowNum(64, 16, intID, 3, 16);
                OLED_Display_Update();
            }
        }

        //*
        if (rt_event_recv(input_event, 0x00000008,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            Flag_allInput = 1;
            OLED_ShowString(64, 16, "*  ", 16);
            OLED_Display_Update();
        }

        //#
        if (rt_event_recv(input_event, 0x00000800,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            //do nothing
        }

        //提交
        if (rt_event_recv(input_event, 0x00008000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            if (Flag_allInput) {
                FP_Clear();
                if (!FP_Receive_Buffer[9]) {
                    OLED_showInfo_delOK();
                    rt_thread_delay(500);
                    return;
                } else {
                    Flag_errorID = 1;
                }
            } else {
                FP_delModel(intID, 1);
                if (!FP_Receive_Buffer[9]) {
                    OLED_showInfo_delOK();
                    rt_thread_delay(500);
                    return;
                } else {
                    Flag_errorID = 1;
                }
            }
        }
    }
}

//删除失败
void OLED_showMenu_delError(void) {
    OLED_Clear();

    //"删除失败"
    OLED_ShowChinese(16, 16, 14);
    OLED_ShowChinese(32, 16, 15);
    OLED_ShowChinese(48, 16, 16);
    OLED_ShowChinese(64, 16, 17);
    OLED_ShowChinese(80, 16, 30);
    OLED_ShowChinese(96, 16, 31);
    //"请重试"
    OLED_ShowChinese(40, 32, 32);
    OLED_ShowChinese(56, 32, 33);
    OLED_ShowChinese(72, 32, 34);

    OLED_Display_Update();
    BUZZ_Beep(3);

    rt_thread_delay(500);
}

//删除成功
void OLED_showInfo_delOK(void) {

    OLED_Clear();

    //"指纹删除成功"
    OLED_ShowChinese(16, 16, 14);
    OLED_ShowChinese(32, 16, 15);
    OLED_ShowChinese(48, 16, 16);
    OLED_ShowChinese(64, 16, 17);
    OLED_ShowChinese(80, 16, 38);
    OLED_ShowChinese(96, 16, 39);

    FP_getModelNum();
    uint16_t cntFP = (FP_Receive_Buffer[10] << 8) + FP_Receive_Buffer[11];
    //"剩余指纹数:"
    OLED_ShowChinese(16, 32, 47);
    OLED_ShowChinese(32, 32, 48);
    OLED_ShowChinese(48, 32, 14);
    OLED_ShowChinese(64, 32, 15);
    OLED_ShowChinese(80, 32, 49);
    OLED_ShowChinese(96, 32, 8);

    OLED_ShowNum(48, 48, cntFP, 4, 16);

    OLED_Display_Update();

    rt_thread_delay(500);
}

//显示系统信息
void OLED_showInfo_Overall(void) {

    OLED_Clear();

    //"智能门锁系统"
    OLED_ShowChinese(16, 0, 50);
    OLED_ShowChinese(32, 0, 51);
    OLED_ShowChinese(48, 0, 52);
    OLED_ShowChinese(64, 0, 1);
    OLED_ShowChinese(80, 0, 53);
    OLED_ShowChinese(96, 0, 54);

    FP_getModelNum();
    uint16_t cntFP = (FP_Receive_Buffer[10] << 8) + FP_Receive_Buffer[11];
    //"剩余指纹数:"
    OLED_ShowChinese(0, 16, 47);
    OLED_ShowChinese(16, 16, 48);
    OLED_ShowChinese(32, 16, 14);
    OLED_ShowChinese(48, 16, 15);
    OLED_ShowChinese(64, 16, 49);
    OLED_ShowChinese(80, 16, 8);

    OLED_ShowNum(96, 16, cntFP, 4, 16);

    //"开锁次数:"
    OLED_ShowChinese(0, 32, 0);
    OLED_ShowChinese(16, 32, 1);
    OLED_ShowChinese(32, 32, 22);
    OLED_ShowChinese(48, 32, 49);

    OLED_ShowNum(64, 32, 1314, 4, 16);

    //"确定"
    OLED_ShowChinese(92, 48, 26);
    OLED_ShowChinese(108, 48, 27);

    OLED_Display_Update();

    while (1) {

        //确定
        if (rt_event_recv(input_event, 0x00008000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            return;
        }

        //按下锁定按键A
        if (rt_event_recv(input_event, 0x00001000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_NO, RT_NULL)
            == RT_EOK) {
            rt_event_send(input_event, 0x80000000);
            return;
        }
    }
}

//显示上电欢迎界面
void OLED_showInfo_Boot(void) {

    OLED_Clear();

    //"智能门锁系统"
    OLED_ShowChinese(16, 16, 50);
    OLED_ShowChinese(32, 16, 51);
    OLED_ShowChinese(48, 16, 52);
    OLED_ShowChinese(64, 16, 1);
    OLED_ShowChinese(80, 16, 53);
    OLED_ShowChinese(96, 16, 54);

    OLED_Display_Update();

    rt_thread_delay(500);
}

//OLED模块 汇总
void OLED_Overall(void) {
    rt_event_send(input_event, 0x80000000);

    while (1) {

        //解锁后关锁，重新打开指纹检测
        if (rt_event_recv(input_event, 0x0C000000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          500, RT_NULL)
            == RT_EOK) {
            rt_sem_release(finger_sem);
        }

        //默认进入主界面
        if (rt_event_recv(input_event, 0x80000000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          500, RT_NULL)
            == RT_EOK) {
            OLED_showMenu_Main();
        }

        //检测到按键输入
        if (rt_event_recv(input_event, 0x00000FFF,
                          RT_EVENT_FLAG_OR,
                          500, RT_NULL)
            == RT_EOK) {
            OLED_showMenu_Input();
        }

        //检测到指纹输入
        if (rt_event_recv(input_event, 0x00010000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          500, RT_NULL)
            == RT_EOK) {

            OLED_showInfo_FPDetected();
        }

        //开锁 进入设置页面
        if (rt_event_recv(input_event, 0x40000000,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          500, RT_NULL)
            == RT_EOK) {

            OLED_showInfo_unLocking();
            MOTOR_unLock();
            OLED_showInfo_unLock();
            rt_thread_delay(500);
            OLED_showMenu_Settings();
        }
    }
}