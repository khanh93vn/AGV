// File: main.cpp
// Chứa chương trình chính.

#include "agv.h"

#if DEBUG
  volatile unsigned long debugval01, debugval02;
#endif
volatile unsigned char *debug_ptr;

int main()
{
  // Tắt watchdog
//  MCUSR = 0;
//  wdt_disable();
  init();

  // I) Cài đặt ban đầu

  // Khởi tạo thông số cài đặt
  settings_init();

  // Khởi động module imu (đọc cảm biến quán tính)
  imu_init();

  // Khởi động truyền thông
  protocol_init();
  dprintln("Hoàn tất khởi động module protocol");
  
  // II) Vòng lặp chính
  while (1)
  {
    // Khởi động các hệ thống ngắt
    sys_init();
    dprintln("Hoàn tất khởi động hệ thống");

    // reset truyền thông
    protocol_reset();
    
    // Chạy vòng lặp truyền thông
    protocol_loop();
  }

  return 0;
}
