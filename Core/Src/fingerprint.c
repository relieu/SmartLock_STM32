#include "fingerprint.h"
#include "usart.h"

uint8_t FP_Receive_Buffer[32] = {0};

extern rt_sem_t uart2_sem;

//发送命令
void FP_Send_Head(void) {
    uint8_t data[6];
    //HEAD
    data[0] = ((uint8_t) (FP_HEAD >> 8));
    data[1] = ((uint8_t) FP_HEAD);
    //ADDR
    data[2] = ((uint8_t) (FP_ADDR >> 24));
    data[3] = ((uint8_t) (FP_ADDR >> 16));
    data[4] = ((uint8_t) (FP_ADDR >> 8));
    data[5] = ((uint8_t) FP_ADDR);

    HAL_UART_Transmit(&huart2, data, 6, 100);
}

//发送标识
void FP_Send_Flag(uint8_t flag) {
    HAL_UART_Transmit(&huart2, &flag, 1, 100);
}

//发送包长度
void FP_Send_Length(uint16_t length) {
    uint8_t data[2];

    data[0] = ((uint8_t) (length >> 8));
    data[1] = ((uint8_t) length);

    HAL_UART_Transmit(&huart2, data, 2, 100);
}

//发送指令码
void FP_Send_Cmd(uint8_t cmd) {
    HAL_UART_Transmit(&huart2, &cmd, 1, 100);
}

//发送校验码
void FP_Send_Check(uint16_t check) {
    uint8_t data[2];

    data[0] = ((uint8_t) (check >> 8));
    data[1] = ((uint8_t) check);

    HAL_UART_Transmit(&huart2, data, 2, 100);
}

//发送数据
void FP_Send_Data(uint8_t data) {
    HAL_UART_Transmit(&huart2, &data, 1, 100);
}

//发送2字节数据
void FP_Send_Word(uint16_t word) {
    uint8_t data[2];

    data[0] = ((uint8_t) (word >> 8));
    data[1] = ((uint8_t) word);

    HAL_UART_Transmit(&huart2, data, 2, 100);
}

//接收返回信息
void FP_Receive(uint8_t size) {
    HAL_UART_Receive_IT(&huart2, FP_Receive_Buffer, size);
}

//提取指纹图像
void FP_getImage(void) {
    //开启接收中断
    FP_Receive(12);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x03);
    FP_Send_Cmd(0x01);

    sumCheck = FP_CMD_FLAG + 0x03 + 0x01;
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//生成指纹特征并存到缓存1、2
void FP_genChara(uint8_t bufferNo) {
    //开启接收中断
    FP_Receive(12);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x04);
    FP_Send_Cmd(0x02);

    FP_Send_Data(bufferNo);

    sumCheck = FP_CMD_FLAG + 0x04 + 0x02
               + FP_2ByteSum(bufferNo);
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//根据两次指纹输入生成模板
void FP_genModel(void) {
    //开启接收中断
    FP_Receive(12);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x03);
    FP_Send_Cmd(0x05);

    sumCheck = FP_CMD_FLAG + 0x03 + 0x05;
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//存储指纹模板
void FP_storeModel(uint16_t pageNo) {
    //开启接收中断
    FP_Receive(12);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x06);
    FP_Send_Cmd(0x06);

    FP_Send_Data(0x01); //选择缓冲区的模板
    FP_Send_Word(pageNo);

    sumCheck = FP_CMD_FLAG + 0x06 + 0x06 + 0x01
               + FP_2ByteSum(pageNo);
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//自动提取指纹
void FP_autoStore(void) {
    //开启接收中断
    FP_Receive(14);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x03);
    FP_Send_Cmd(0x10);

    sumCheck = FP_CMD_FLAG + 0x03 + 0x10;
    FP_Send_Check(sumCheck);

    //等待应答
    rt_kprintf("a\n");
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//指纹搜索
void FP_Search(uint16_t startPage, uint16_t pageNum) {
    //开启接收中断
    FP_Receive(16);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x08);
    FP_Send_Cmd(0x04);

    FP_Send_Data(0x01); //选择缓冲区1的特征值
    FP_Send_Word(startPage);
    FP_Send_Word(pageNum);

    sumCheck = FP_CMD_FLAG + 0x08 + 0x04 + 0x01
               + FP_2ByteSum(startPage + pageNum);
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//快速搜索
void FP_fastSearch(uint16_t startPage, uint16_t pageNum) {
    //开启接收中断
    FP_Receive(16);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x08);
    FP_Send_Cmd(0x1B);

    FP_Send_Data(0x01); //选择缓冲区1的特征值
    FP_Send_Word(startPage);
    FP_Send_Word(pageNum);

    sumCheck = FP_CMD_FLAG + 0x08 + 0x1B + 0x01
               + FP_2ByteSum(startPage + pageNum);
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//计算2个字节和
uint16_t FP_2ByteSum(uint16_t word) {
    uint16_t retVal = 0;
    retVal += (uint8_t) (word >> 8);
    retVal += (uint8_t) word;
    return retVal;
}

//获取指纹模板库大小
void FP_getModelNum(void) {
    //开启接收中断
    FP_Receive(14);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x03);
    FP_Send_Cmd(0x1D);

    sumCheck = FP_CMD_FLAG + 0x03 + 0x1D;
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//删除模板
void FP_delModel(uint16_t startPage, uint16_t pageNum) {
    //开启接收中断
    FP_Receive(12);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x07);
    FP_Send_Cmd(0x0C);

    FP_Send_Word(startPage);
    FP_Send_Word(pageNum);

    sumCheck = FP_CMD_FLAG + 0x07 + 0x0C
               + FP_2ByteSum(startPage + pageNum);
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//删除所有模板
void FP_Clear(void) {
    //开启接收中断
    FP_Receive(12);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x03);
    FP_Send_Cmd(0x0D);

    sumCheck = FP_CMD_FLAG + 0x03 + 0x0D;
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}

//获取系统信息
void FP_getInfo(void) {
    //开启接收中断
    FP_Receive(28);

    uint16_t sumCheck = 0;

    FP_Send_Head();

    FP_Send_Flag(FP_CMD_FLAG);
    FP_Send_Length(0x03);
    FP_Send_Cmd(0x0F);

    sumCheck = FP_CMD_FLAG + 0x03 + 0x0F;
    FP_Send_Check(sumCheck);

    //等待应答
    rt_sem_take(uart2_sem, RT_WAITING_FOREVER);
}