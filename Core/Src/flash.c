#include "flash.h"

//SPI片选
void FLASH_SPI_Select(void) {
    HAL_GPIO_WritePin(
            FLASH_SS_GPIO_Port,
            FLASH_SS_Pin,
            GPIO_PIN_RESET
    );
}

//取消FLASH片选
void FLASH_SPI_deSelect(void) {
    HAL_GPIO_WritePin(
            FLASH_SS_GPIO_Port,
            FLASH_SS_Pin,
            GPIO_PIN_SET
    );
}

//读取ID
uint16_t FLASH_ReadID(void) {
    uint16_t retID;
    uint8_t id[2] = {0};
    uint8_t cmd[4] = {
            W25X_ManufactureDeviceID,
            0x00,
            0x00,
            0x00
    };

    FLASH_SPI_Select();
    HAL_SPI_Transmit(&hspi1, cmd, 4, 1000);
    HAL_SPI_Receive(&hspi1, id, 2, 1000);
    FLASH_SPI_deSelect();

    retID = (((uint16_t) id[0]) << 8) | id[1];
    return retID;
}

//读取状态寄存器
uint8_t FLASH_ReadStatusReg(uint8_t RegNo) {
    uint8_t byte = 0, cmd = 0;
    switch (RegNo) {
        case 1:
            cmd = W25X_ReadStatusReg1;
            break;
        case 2:
            cmd = W25X_ReadStatusReg2;
            break;
        case 3:
            cmd = W25X_ReadStatusReg3;
            break;
        default:
            cmd = W25X_ReadStatusReg1;
            break;
    }

    FLASH_SPI_Select();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    HAL_SPI_Receive(&hspi1, &byte, 1, 1000);
    FLASH_SPI_deSelect();

    return byte;
}

//写状态寄存器
void FLASH_WriteStatusReg(uint8_t RegNo, uint8_t Data) {
    ; //NOT USING
}

void FLASH_Write_Enable(void) {
    uint8_t cmd = W25X_WriteEnable;
//    FLASH_SPI_Select();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
//    FLASH_SPI_deSelect();
}

//写禁止
void FLASH_Write_Disable(void) {
    uint8_t cmd = W25X_WriteDisable;
//    FLASH_SPI_Select();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
//    FLASH_SPI_deSelect();
}

//读取数据
void FLASH_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
    uint8_t cmd[4] = {0};
    cmd[0] = W25X_ReadData;
    cmd[1] = ((uint8_t) (ReadAddr >> 16));
    cmd[2] = ((uint8_t) (ReadAddr >> 8));
    cmd[3] = ((uint8_t) ReadAddr);

    FLASH_SPI_Select();
    HAL_SPI_Transmit(&hspi1, cmd, 4, 1000);
//    if (HAL_SPI_Receive(&hspi1, pBuffer, NumByteToRead, 1000) != HAL_OK) {
//        Error_Handler();
//    }
    FLASH_SPI_deSelect();
}

//写入数据（256Byte MAX）
void FLASH_Write_Page(uint8_t *pBuffer, uint16_t SectorNo, uint8_t page, uint16_t NumByteToWrite) {
    //注意 写入前需要擦除该区域数据
    //一个扇区最多15页
    uint32_t WriteAddr = SectorNo * 4000 + page * 256;

    uint8_t cmd[4] = {0};
    cmd[0] = W25X_PageProgram;
    cmd[1] = ((uint8_t) (WriteAddr >> 16));
    cmd[2] = ((uint8_t) (WriteAddr >> 8));
    cmd[3] = ((uint8_t) WriteAddr);

    //超出大小截断
    if (NumByteToWrite > 256) {
        NumByteToWrite = 256;
    }

    FLASH_SPI_Select();
    FLASH_Write_Enable();

    HAL_SPI_Transmit(&hspi1, cmd, 4, 1000);
    HAL_SPI_Transmit(&hspi1, pBuffer, NumByteToWrite, 1000);

    FLASH_Write_Disable();
    FLASH_SPI_deSelect();

    FLASH_Wait_Busy(); //等待写入完成
}

//全盘擦除
void FLASH_Erase_Chip(void) {
    //NOT SAFE
}

//块擦除64kb
void FLASH_Erase_Block(uint16_t BlockNo) {
    //NOT SAFE
}

//扇区擦除4kb
void FLASH_Erase_Sector(uint16_t BlockNo, uint16_t SectorNo) {
    //TODO
}

//等待操作
void FLASH_Wait_Busy(void) {
    while ((FLASH_ReadStatusReg(1) & 0x01) == 0x01)
        rt_thread_delay(100);
}