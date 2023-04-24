#include "threads.h"


rt_thread_t oled_thread = RT_NULL;
rt_thread_t key_thread = RT_NULL;
rt_thread_t finger_thread = RT_NULL;

rt_sem_t uart2_sem = RT_NULL;
rt_sem_t finger_sem = RT_NULL;

rt_event_t input_event = RT_NULL;

extern uint8_t FP_Receive_Buffer[];

void key_thread_entry(void *parameters) {
    KEYS_Scan();
}

void oled_thread_entry(void *parameters) {
    OLED_Overall();
}

void finger_thread_entry(void *parameters) {
    while (1) {

        rt_sem_take(finger_sem, RT_WAITING_FOREVER);

        FP_getImage();
        if (!FP_Receive_Buffer[9]) {
            rt_event_send(input_event, 0x00010000);
        }
        else {
            rt_thread_delay(500);
            rt_sem_release(finger_sem);
        }
    }

}