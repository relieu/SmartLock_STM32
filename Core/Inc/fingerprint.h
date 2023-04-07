#ifndef SMARTLOCK_STM32_FINGERPRINT_H
#define SMARTLOCK_STM32_FINGERPRINT_H

#include "main.h"

#define FP_HEAD 0XEF01 //包头
#define FP_ADDR 0XFFFFFFFF //默认地址
#define FP_CMD_FLAG 0X01 //指令包标识
#define FP_SEND_FLAG 0X02 //数据包标识
#define FP_END_FLAG 0X08 //结束包标识

//==========数据收发==========

//发送命令
void FP_Send_Head(void);

//发送标识
void FP_Send_Flag(uint8_t flag);

//发送包长度
void FP_Send_Length(uint16_t length);

//发送指令码
void FP_Send_Cmd(uint8_t cmd);

//发送校验码
void FP_Send_Check(uint16_t check);

//发送数据
void FP_Send_Data(uint8_t data);

//发送2字节数据
void FP_Send_Word(uint16_t word);

//接收返回信息
uint8_t FP_Receive(uint8_t size);

//处理接收数据
void FP_Receive_Data(uint8_t *pData, uint8_t pos, uint8_t size);

//==========指令控制==========

/*
 * 指纹录入
 */

//提取指纹图像
void FP_getImage(void);

//生成指纹特征并存到缓存1、2
void FP_genChara(uint8_t bufferNo);

//根据两次指纹输入生成模板
void FP_genModel(void);

//存储指纹模板
void FP_storeModel(uint16_t pageNo);

/*
 * 指纹比对
 */

//指纹搜索(Buffer1)
void FP_Search(uint16_t startPage, uint16_t pageNum);

//快速搜索
void FP_fastSearch(uint16_t startPage, uint16_t pageNum);

/*
 * 其他操作
 */

//计算2个字节和
uint16_t FP_2ByteSum(uint16_t word);

//获取指纹模板库大小
void FP_getModelNum(void);

//删除模板
void FP_delModel(uint16_t startPage, uint16_t pageNum);

//删除所有模板
void FP_Clear(void);

//获取系统信息
void FP_getInfo(void);

//返回码处理
void FP_getMessage(uint8_t ensure);

#endif //SMARTLOCK_STM32_FINGERPRINT_H
