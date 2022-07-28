// File: drive.cpp
// Điều khiển bánh dẫn động.

#include "agv.h"

// Các biến toàn cục ----------------------------------------------------------
// Dữ liệu điều khiển PID của bánh dẫn động
volatile pid_t drive_pid;

// Các chương trình con --------------------------------------------------------
/**
 * Khởi tạo bộ điều khiển động cơ dẫn động.
 */
void drive_init()
{
  // I) Cài đặt điều khiển ngõ ra (động cơ DC)
  pinMode(IO_DRIVE_P, OUTPUT);
  pinMode(IO_DRIVE_H1, OUTPUT);
  pinMode(IO_DRIVE_H2, OUTPUT);

  // II) Reset bộ PID
  pid_init(&drive_pid);
}

/**
 * Cập nhật bộ điều khiển và xuất tín hiệu điều khiển.
 */
void drive_step(int16_t encoder_position)
{
  int32_t duty_cycle;
  uint8_t direction_bit;

  // I) Cập nhật bộ điều khiển PID
  duty_cycle = pid_step(&drive_pid, encoder_position);
  

  // II) Điều khiển động cơ

  // Kiểm tra dấu của ngõ ra
  if (duty_cycle < 0) {
    direction_bit = HIGH;
    duty_cycle = -duty_cycle;
  }
  else direction_bit = LOW;

  // Chỉnh chiều quay
  digitalWrite(IO_DRIVE_H1, direction_bit);
  digitalWrite(IO_DRIVE_H2, !direction_bit);
  
  // Ngõ ra phải trong đoạn [0, ]
  if (duty_cycle > 255) duty_cycle = 255;

  // Không đủ để thắng ma sát
  if (duty_cycle < 70) duty_cycle = 0;
  
  // Chỉnh tỷ lệ áp ngõ ra
  analogWrite(IO_DRIVE_P, (uint8_t)duty_cycle);
}
