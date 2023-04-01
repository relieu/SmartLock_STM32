#include "oled.h"
#include "oledfonts.h"

//显存
uint8_t OLED_GRAM[128][8];

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
    OLED_Display_Update(); //更新显示
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
    if (x1 < x2 || y1 < y2) return;
    for (x = x1; x <= x2; x++) {
        for (y = y1; y <= y2; y++) {
            if (isLight) OLED_DrawPoint(x, y);
            else OLED_ClearPoint(x, y);
        }
    }
    OLED_Display_Update();//更新显示
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
        else nums[len - i - 1] = ' ';
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

//显示中文
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t number)
{
    //TODO 中文界面
}
