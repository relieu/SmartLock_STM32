#ifndef SMARTLOCK_STM32_FLASH_H
#define SMARTLOCK_STM32_FLASH_H


#include "main.h"

#define W25Q64_ID 0XEF16
#define W25Q64_JEDECID 0xEF4017

//--------------------FLASH指令--------------------
#define W25X_WriteEnable           0x06
#define W25X_WriteDisable          0x04
#define W25X_ReadStatusReg1        0x05
#define W25X_ReadStatusReg2        0x35
#define W25X_ReadStatusReg3        0x15
#define W25X_WriteStatusReg1       0x01
#define W25X_WriteStatusReg2       0x31
#define W25X_WriteStatusReg3       0x11
#define W25X_ReadData              0x03
#define W25X_FastReadData          0x0B
#define W25X_FastReadDual          0x3B
#define W25X_PageProgram           0x02
#define W25X_BlockErase            0xD8
#define W25X_SectorErase           0x20
#define W25X_ChipErase             0xC7
#define W25X_PowerDown             0xB9
#define W25X_ReleasePowerDown      0xAB
#define W25X_DeviceID              0xAB
#define W25X_ManufactureDeviceID   0x90
#define W25X_JEDECDeviceID         0x9F
#define W25X_Enable4ByteAddr       0xB7
#define W25X_Exit4ByteAddr         0xE9

//SPI片选
void FLASH_SPI_Select(void);

//取消FLASH片选
void FLASH_SPI_deSelect(void);

//读取ID
uint16_t FLASH_ReadID(void);

//读取状态寄存器
uint8_t FLASH_ReadStatusReg(uint8_t RegNo);

//写状态寄存器
void FLASH_WriteStatusReg(uint8_t RegNo, uint8_t Data);

//写使能
void FLASH_Write_Enable(void);

//写禁止
void FLASH_Write_Disable(void);

//读取数据
void FLASH_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

//写入数据（256Byte MAX）
void FLASH_Write_Page(uint8_t *pBuffer, uint16_t SectorNo, uint8_t page, uint16_t NumByteToWrite);

//全盘擦除
void FLASH_Erase_Chip(void);

//块擦除64kb 128
void FLASH_Erase_Block(uint16_t BlockNo);

//扇区擦除4kb 16
void FLASH_Erase_Sector(uint16_t BlockNo, uint16_t SectorNo);

//等待操作
void FLASH_Wait_Busy(void);

//待机
void FLASH_PowerDown(void);

//唤醒
void FLASH_WAKEUP(void);

//==========综合==========

//读取数据
void FLASH_readData(uint8_t *pBuffer, uint8_t size);

//写入数据
void FLASH_writeData(uint8_t *pBuffer, uint8_t size);

#endif //SMARTLOCK_STM32_FLASH_H
