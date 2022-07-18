// File: main.c
// Chứa chương trình chính.

#include "agv.h"

void main()
{
  // Tắt watchdog
  MCUSR = 0;
  wdt_disable();

  // Cài đặt ban đầu
  settings_init();
  sys_init();

  // Vòng lặp chính
  while (1)
  {
  }
}
