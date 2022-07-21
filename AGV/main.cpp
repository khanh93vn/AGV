// File: main.cpp
// Chứa chương trình chính.

#include "agv.h"

#if DEBUG
  volatile long debugval;
  volatile long *debugval_ptr = &debugval;
#endif

int main()
{
  // Tắt watchdog
//  MCUSR = 0;
//  wdt_disable();
  init();

  // I) Cài đặt ban đầu

  // Khởi tạo thông số cài đặt
  settings_init();

  // Khởi động truyền thông
  protocol_init();
  dprintln("Hoàn tất khởi động module protocol");
  
  // II) Vòng lặp chính
  while (1)
  {
    // Khởi động các bộ điều khiển
    drive_init();
    dprintln("Hoàn tất khởi động module drive");
    steer_init();
    dprintln("Hoàn tất khởi động module steer");

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
