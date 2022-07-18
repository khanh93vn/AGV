// File: steer.c
// Điều khiển góc lái.

#include "agv.h"

// Các biến toàn cục ----------------------------------------------------------
// Dữ liệu điều khiển PID của cơ cấu lái
pid_t steer_pid;

// Các chương trình con --------------------------------------------------------
/**
 * Khởi tạo bộ điều khiển góc lái
 */
void steer_init()
{
  // I) Cài đặt điều khiển ngõ ra (động cơ DC)
  pinMode(IO_STEER_P, OUTPUT);
  pinMode(IO_STEER_H1, OUTPUT);
  pinMode(IO_STEER_H2, OUTPUT);

  // II) Cài đặt ngõ vào (cảm biến MPU6050)

  // III) Reset bộ PID
  pid_init(&steer_pid);
}

/**
 * Cập nhật bộ điều khiển step và xuất tính
 * hiệu điều khiển.
 */
void steer_step()
{
  float heading, duty_cycle;

  // I) Cập nhật hướng từ cảm biến góc xoay


  // II) Cập nhật bộ điều khiển PID
  duty_cycle = pid_step(&steer_pid, heading);

  // III) Điều khiển động cơ

  // Chỉnh chiều quay

  // Chỉnh tỷ lệ áp ngõ ra
  analogWrite(IO_STEER_P, )

  // TODO: cân nhắc tín hiệu điều khiển khi xe
  // không di chuyển (xe không xoay theo bánh lái)
}
