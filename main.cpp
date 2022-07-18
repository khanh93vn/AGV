// File: main.c
// Chứa chương trình chính.

#include "agv.h"

int main()
{
  // Tắt watchdog
  MCUSR = 0;
  wdt_disable();

  // I) Cài đặt ban đầu

  // Khởi tạo thông số cài đặt
  settings_init();

  // Khởi động các bộ điều khiển
  drive_init();
  steer_init();

  // Khởi động các hệ thống ngắt
  sys_init();

  // Khởi động truyền thông
  protocol_init();

  // II) Vòng lặp chính
  while (1)
  {
  }
  
  return 0;
}
