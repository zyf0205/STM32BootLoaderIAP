#include "key.h"

/**
 * @brief  初始化 USER KEY（PC13）
 *         配置为输入 + 上拉
 */
void Key_Init(void)
{
    GPIO_InitTypeDef gpio;

    /* 1. 使能 GPIOC 时钟 */
    RCC_AHB1PeriphClockCmd(KEY_GPIO_CLK, ENABLE);

    /* 2. 配置 PC13 为输入模式 */
    gpio.GPIO_Pin  = KEY_GPIO_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;   // 内部上拉，保证未按下为高电平
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(KEY_GPIO_PORT, &gpio);
}

/**
 * @brief  判断 USER KEY 是否被按下
 * @retval 1：按下
 *         0：未按下
 */
uint8_t Key_IsPressed(void)
{
    /* 低电平表示按下 */
    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == Bit_RESET)
    {
        return 1;
    }
    return 0;
}
