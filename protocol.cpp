// File: protocol.c
// Quản lý giao thức với các thiết bị khác.

#include "agv.h"

// Các biến toàn cục ----------------------------------------------------------

// Các chương trình con --------------------------------------------------------
/**
 * Khởi động truyền thông UART.
 */
void protocol_init()
{
  Serial.begin(BAUDRATE);
}
