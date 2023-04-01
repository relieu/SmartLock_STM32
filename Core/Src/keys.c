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