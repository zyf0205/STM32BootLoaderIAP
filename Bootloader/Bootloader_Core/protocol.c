#include "protocol.h"
#include "usart.h"
#include "flash.h"
#include "boot.h"
#include "systick.h"

extern uint8_t g_rx_buf[RX_BUF_SIZE];
extern volatile uint16_t g_rx_len;

uint32_t g_write_addr = APP_START_ADDR;

/* 计算校验和 */
static uint8_t CalcChecksum(uint8_t *data, uint16_t len)
{
  uint8_t sum = 0;
  for (uint16_t i = 0; i < len; i++)
  {
    sum += data[i];
  }
  return sum & 0xFF;
}

/* 发送应答 */
static void SendAck(void)
{
  USART_SendByte(ACK);
}

/*发送非应答*/
static void SendNack(void)
{
  USART_SendByte(NACK);
}

void Protocol_Process(void)
{
  /* 至少需要: HEAD(2) + CMD(1) + LEN(2) + CHECKSUM(1) = 6 bytes */
  if (g_rx_len < 6)
    return;

  /* 1. 检查帧头 */
  if (g_rx_buf[0] != FRAME_HEAD_1 || g_rx_buf[1] != FRAME_HEAD_2)
  {
    g_rx_len = 0;
    return;
  }

  /* 2. 解析协议 */
  uint8_t cmd = g_rx_buf[2];
  uint16_t payload_len = g_rx_buf[3] | (g_rx_buf[4] << 8); // 小端

  /* 3. 等待完整数据包 */
  uint16_t total_len = 2 + 1 + 2 + payload_len + 1; // HEAD + CMD + LEN + PAYLOAD + CHECKSUM
  if (g_rx_len < total_len)
    return;

  /* 4. 校验和验证 */
  uint8_t recv_checksum = g_rx_buf[total_len - 1];                     /*得到和校验位的数据*/
  uint8_t calc_checksum = CalcChecksum(&g_rx_buf[2], 3 + payload_len); // 计算和校验 CMD + LEN + PAYLOAD

  /*和校验不相等*/
  if (recv_checksum != calc_checksum)
  {
    SendNack(); /*发送非应答*/
    g_rx_len = 0;
    return;
  }

  /* 5. 处理命令 */
  uint8_t *payload = &g_rx_buf[5];

  switch (cmd)
  {
  case CMD_CONNECT:
    /* 握手命令 - 擦除 Flash */
    FLASH_EraseApplication();
    g_write_addr = APP_START_ADDR;
    SendAck();
    break;

  case CMD_DATA:
    /* 写入数据 */
    FLASH_WriteData(g_write_addr, payload, payload_len);
    g_write_addr += payload_len;
    SendAck();
    break;

  case CMD_FINISH:
    /* 升级完成 - 跳转到 APP */
    SendAck();
    Delay_ms(100);
    Boot_JumpToApp();
    break;

  default:
    SendNack();
    break;
  }

  /* 6. 清空接收缓冲区 */
  g_rx_len = 0;
}