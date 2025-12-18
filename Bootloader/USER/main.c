#include "stm32f4xx.h"
#include "usart.h"
#include "protocol.h"
#include "boot.h"
#include "key.h"
#include "systick.h"

int main(void)
{
    SysTick_Init(1000);
    Key_Init();
    Delay_ms(20);

    if (!Key_IsPressed()) {
        SysTick->CTRL = 0;
        Boot_JumpToApp();
    }

    USART1_Init();
    Delay_ms(300);

    while (1) {
        Protocol_Process();
    }
}
