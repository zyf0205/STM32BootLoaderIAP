#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"

#define RX_BUF_SIZE 2048

extern uint8_t g_rx_buf[RX_BUF_SIZE];
extern volatile uint16_t g_rx_len;

void USART1_Init(void);
void USART_SendByte(uint8_t ch);
void USART_SendString(const char *str);

#endif