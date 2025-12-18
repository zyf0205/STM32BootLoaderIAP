#include "usart.h"

uint8_t g_rx_buf[RX_BUF_SIZE];  /*串口接收缓冲区*/
volatile uint16_t g_rx_len = 0; /*当前已接受的数据长度,使用volatile防止编译器优化,每次都从内容中读取变量*/

/**
 * @brief 初始化 USART1
 * PA9  -> TX
 * PA10 -> RX
 */
void USART1_Init(void)
{
  GPIO_InitTypeDef gpio;
  USART_InitTypeDef usart;
  NVIC_InitTypeDef nvic;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_UP; /*上拉,空闲时为高电平*/
  GPIO_Init(GPIOA, &gpio);

  /*引脚复用到usart1*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  usart.USART_BaudRate = 115200;
  usart.USART_WordLength = USART_WordLength_8b;     /*8数据位*/
  usart.USART_StopBits = USART_StopBits_1;          /*1停止位*/
  usart.USART_Parity = USART_Parity_No;             /*无校验*/
  usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /*收发模式*/
  USART_Init(USART1, &usart);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /*接收中断使能*/

  nvic.NVIC_IRQChannel = USART1_IRQn;         /*中断通道*/
  nvic.NVIC_IRQChannelPreemptionPriority = 2; /*抢占优先级*/
  nvic.NVIC_IRQChannelSubPriority = 0;        /*子优先级*/
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);

  USART_Cmd(USART1, ENABLE); /*开启usart1*/
}

/**
 * @brief 发送 1 个字节（阻塞方式）
 */
void USART_SendByte(uint8_t ch)
{
  /* 等待发送数据寄存器空 */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    ;

  /* 写入数据寄存器 */
  USART_SendData(USART1, ch);

  /* 等待发送完成 */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    ;
}

void USART_SendString(const char *str)
{
  while (*str)
  {
    USART_SendByte(*str++);
  }
}

/**
 * @brief USART1 中断,触发条件:接收寄存器非空
 */
void USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_RXNE); /*清除中断标志位*/
    if (g_rx_len < RX_BUF_SIZE)                     /*防止接收缓冲区溢出*/
    {
      g_rx_buf[g_rx_len++] = USART_ReceiveData(USART1); /*接收数据*/
    }
  }
}
