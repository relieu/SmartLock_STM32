#include "wifi.h"

extern rt_sem_t wifi_sem;
extern rt_sem_t finger_sem;
extern rt_event_t system_event;

extern uint8_t isUnlock;
uint8_t isRecv = 0;

uint8_t WIFI_recvBuffer[32];

void WIFI_Enable(void) {
    HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_SET);
    rt_thread_delay(2000);

    //消除其他串口数据
    HAL_UART_Receive(&huart3, WIFI_recvBuffer, 32, 100);
    rt_memset(WIFI_recvBuffer, 0, 32);
}

void WIFI_openServer(void) {

    uint8_t str1[13] = "AT+CIPMUX=1\r\n";
    uint8_t str2[22] = "AT+CIPSERVER=1,25565\r\n";

    HAL_UART_Transmit(&huart3, str1, 13, 100);
    HAL_UART_Receive(&huart3, WIFI_recvBuffer, 20, 100);
//    HAL_UART_Transmit(&huart1, WIFI_recvBuffer, 32, 100);
//    rt_kprintf("第一次接收完成；\r\n");
    rt_thread_delay(500);

    rt_memset(WIFI_recvBuffer, 'x', 32);

    HAL_UART_Transmit(&huart3, str2, 22, 100);
    HAL_UART_Receive(&huart3, WIFI_recvBuffer, 29, 100);
//    HAL_UART_Transmit(&huart1, WIFI_recvBuffer, 32, 100);
//    rt_kprintf("第二次接收完成；\r\n");
    rt_thread_delay(500);

    //清除缓冲（可选）
    rt_memset(WIFI_recvBuffer, 'x', 32);
}

void WIFI_Receive(void) {
//    阻塞式接收串口信息，因会影响其他线程的工作，暂时放弃
//    若需要使用需要调整阻塞时间
    if (HAL_UART_Receive(&huart3, WIFI_recvBuffer, 10, 200)
        == HAL_OK) {
        //接收到数据:"\r\n+IPD..."
        if (WIFI_recvBuffer[0] == '\r') {
            //继续接收后续内容
            HAL_UART_Receive(
                    &huart3,
                    &WIFI_recvBuffer[10],
                    WIFI_recvBuffer[9] + 1,
                    100);
        }
//        HAL_UART_Transmit(&huart1, WIFI_recvBuffer, 32, 100);
//        rt_kprintf("完成接收；\r\n");
    } else {
        rt_thread_delay(100);
        return;
    }
}

void WIFI_startReceiveIt(void) {

    //TODO: 考虑改进，当接收失败清楚缓冲区，重新接收
    HAL_UART_Receive_IT(&huart3, WIFI_recvBuffer, 10);
    //使用信号量等待输入完成
    rt_sem_take(wifi_sem, RT_WAITING_FOREVER);

    //接收到数据:"\r\n+IPD..."
    if (WIFI_recvBuffer[0] == '\r') {
        WIFI_receiveData();
    }

    //接收到连接状态
    if (WIFI_recvBuffer[0] == '0') {
        WIFI_receiveStatus();
    }

//    //DEBUG
//    HAL_UART_Transmit(&huart1, WIFI_recvBuffer, 32, 100);
//    rt_kprintf("完成上传；\r\n");
}

void WIFI_receiveStatus() {
    //连接成功
    if (WIFI_recvBuffer[3] == 'O') {
        HAL_UART_Receive(&huart3, &WIFI_recvBuffer[10], 1, 100);
//        rt_kprintf("连接成功\r\n");
        //开启蓝色灯
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
        //发送状态信息
        WIFI_sendStatus();
    }

    //连接失败
    if (WIFI_recvBuffer[3] == 'L') {
//        rt_kprintf("连接断开\r\n");
        HAL_UART_Receive(&huart3, &WIFI_recvBuffer[10], 3, 100);
        //关闭蓝色灯
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
    }
}

void WIFI_receiveData() {

    //继续接收后续内容
    HAL_UART_Receive(
            &huart3,
            &WIFI_recvBuffer[10],
            WIFI_recvBuffer[9] - '0' + 1,
            100);

//    rt_kprintf("收到数据\r\n");

    uint8_t password[7];
    uint8_t recvData[7];
    uint8_t Flag_errorPW = 0;

    //密码检测
    rt_memset(recvData, ' ', 6);
    rt_memcpy(recvData, &WIFI_recvBuffer[11], WIFI_recvBuffer[9] - '0');

    FLASH_readData(password, 6);

    for (int i = 0; i < 6; ++i) {
        if (password[i] == recvData[i]) continue;
        else {
            Flag_errorPW = 1;
            break;
        }
    }

    //验证成功 开锁
    if (Flag_errorPW != 1) {
        rt_event_send(system_event, 0x40020000);
        OLED_Clear();
        OLED_showInfo_PWOK();
        //暂停指纹检测
        rt_sem_take(finger_sem, RT_WAITING_FOREVER);
//        rt_kprintf("密码正确");
        isUnlock = 1;
    } else {
//        rt_kprintf("密码错误");
    }

    rt_thread_delay(300);
    isRecv = 1;
    WIFI_sendStatus();
    isRecv = 0;
}

void WIFI_sendStatus() {
    uint8_t status = 0;
    uint8_t sendStr[] = "AT+CIPSEND=0,1\r\n";

    status = isUnlock + (isRecv << 1);

    uint8_t sendData = '0' + status;

    rt_kprintf("发送%c", sendData);

    HAL_UART_Transmit(&huart3, sendStr, 16, 100);
    HAL_UART_Receive(&huart3, WIFI_recvBuffer, 25, 200);
    HAL_UART_Transmit(&huart3, &sendData, 1, 100);
    HAL_UART_Receive(&huart3, WIFI_recvBuffer, 12, 200);
}