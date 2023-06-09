#include "main.h"

static uint8_t isBellBusy = FALSE;

//初始化
void KEYS_Init(void) {
    ;
}

//门铃键位
void KEYS_BellKey_Press() {
    rt_thread_delay(100);
    if (isBellBusy) return;
    else {
        isBellBusy = TRUE;
        BUZZ_Bell();
        isBellBusy = FALSE;
    }
}


extern rt_sem_t uart2_sem;
extern rt_event_t system_event;

//按键扫描
void KEYS_Scan(void) {
    while (1) {

        //门铃按键
        rt_thread_delay(100);
        if (HAL_GPIO_ReadPin(BUZZKey_GPIO_Port, BUZZKey_Pin)) {
            KEYS_BellKey_Press();
        }

        rt_thread_delay(100);
        HAL_GPIO_WritePin(KEYS_COL1_GPIO_Port, KEYS_COL1_Pin, GPIO_PIN_RESET);

        if (!HAL_GPIO_ReadPin(KEYS_ROW1_GPIO_Port, KEYS_ROW1_Pin)) {
            rt_kprintf("1\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 0));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW2_GPIO_Port, KEYS_ROW2_Pin)) {
            rt_kprintf("4\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 1));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW3_GPIO_Port, KEYS_ROW3_Pin)) {
            rt_kprintf("7\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 2));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW4_GPIO_Port, KEYS_ROW4_Pin)) {
            rt_kprintf("*\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 3));
        }

        HAL_GPIO_WritePin(KEYS_COL1_GPIO_Port, KEYS_COL1_Pin, GPIO_PIN_SET);


        rt_thread_delay(100);
        HAL_GPIO_WritePin(KEYS_COL2_GPIO_Port, KEYS_COL2_Pin, GPIO_PIN_RESET);

        if (!HAL_GPIO_ReadPin(KEYS_ROW1_GPIO_Port, KEYS_ROW1_Pin)) {
            rt_kprintf("2\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 4));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW2_GPIO_Port, KEYS_ROW2_Pin)) {
            rt_kprintf("5\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 5));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW3_GPIO_Port, KEYS_ROW3_Pin)) {
            rt_kprintf("8\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 6));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW4_GPIO_Port, KEYS_ROW4_Pin)) {
            rt_kprintf("0\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 7));
        }

        HAL_GPIO_WritePin(KEYS_COL2_GPIO_Port, KEYS_COL2_Pin, GPIO_PIN_SET);


        rt_thread_delay(100);
        HAL_GPIO_WritePin(KEYS_COL3_GPIO_Port, KEYS_COL3_Pin, GPIO_PIN_RESET);

        if (!HAL_GPIO_ReadPin(KEYS_ROW1_GPIO_Port, KEYS_ROW1_Pin)) {
            rt_kprintf("3\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 8));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW2_GPIO_Port, KEYS_ROW2_Pin)) {
            rt_kprintf("6\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 9));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW3_GPIO_Port, KEYS_ROW3_Pin)) {
            rt_kprintf("9\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 10));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW4_GPIO_Port, KEYS_ROW4_Pin)) {
            rt_kprintf("#\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 11));
        }

        HAL_GPIO_WritePin(KEYS_COL3_GPIO_Port, KEYS_COL3_Pin, GPIO_PIN_SET);


        rt_thread_delay(100);
        HAL_GPIO_WritePin(KEYS_COL4_GPIO_Port, KEYS_COL4_Pin, GPIO_PIN_RESET);

        if (!HAL_GPIO_ReadPin(KEYS_ROW1_GPIO_Port, KEYS_ROW1_Pin)) {
            rt_kprintf("A\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 12));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW2_GPIO_Port, KEYS_ROW2_Pin)) {
            rt_kprintf("B\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 13));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW3_GPIO_Port, KEYS_ROW3_Pin)) {
            rt_kprintf("C\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 14));
        }
        if (!HAL_GPIO_ReadPin(KEYS_ROW4_GPIO_Port, KEYS_ROW4_Pin)) {
            rt_kprintf("D\r\n");
            BUZZ_Beep(1);
            rt_event_send(system_event, (1 << 15));
        }

        HAL_GPIO_WritePin(KEYS_COL4_GPIO_Port, KEYS_COL4_Pin, GPIO_PIN_SET);

    }
}
