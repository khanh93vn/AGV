// File: steer.cpp
// Điều khiển góc lái.

#include "agv.h"

// Các biến toàn cục ----------------------------------------------------------
// Dữ liệu điều khiển PID của cơ cấu lái
volatile pid_t steer_pid;

// Các chương trình con --------------------------------------------------------
/**
 * Khởi tạo bộ điều khiển góc lái.
 */
void steer_init()
{
  // I) Cài đặt điều khiển ngõ ra (động cơ DC)
  pinMode(IO_STEER_P, OUTPUT);
  pinMode(IO_STEER_H1, OUTPUT);
  pinMode(IO_STEER_H2, OUTPUT);

  // II) Reset bộ PID
  pid_init(&steer_pid);

  steer_pid.ref = PI;
}

/**
 * Cập nhật bộ điều khiển và xuất tín hiệu điều khiển.
 */
void steer_step()
{
  float heading, duty_cycle, heading_error;
  uint8_t direction_bit;

  // I) Tính sai số
  heading = sys_get_heading();
  heading_error = steer_pid.ref - heading;
  while (heading_error > PI) heading_error -= TWO_PI;
  while (heading_error < -PI) heading_error += TWO_PI;

  // II) Cập nhật bộ điều khiển PID
  duty_cycle = pid_stp_from_error(&steer_pid, heading_error);

  // III) Điều khiển động cơ

  // Kiểm tra dấu của ngõ ra
  if (duty_cycle < 0) {
    direction_bit = HIGH;
    duty_cycle = -duty_cycle;
  }
  else direction_bit = LOW;

  // Chỉnh chiều quay
  digitalWrite(IO_STEER_H1, direction_bit);
  digitalWrite(IO_STEER_H2, !direction_bit);

  // Ngõ ra phải trong đoạn [0.0, 1.0]
  if (duty_cycle > 1.0) duty_cycle = 1.0;
  
  // Chỉnh tỷ lệ áp ngõ ra
  analogWrite(IO_STEER_P, (uint8_t)(duty_cycle*255));

  // TODO: cân nhắc tín hiệu điều khiển khi xe
  // không di chuyển (xe không xoay theo bánh lái)
}
