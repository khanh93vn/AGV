// File: pid.c
// Bộ điều khiển vi tích phân tỷ lệ.

#include "agv.h"

// Các biến toàn cục ----------------------------------------------------------

// Các chương trình con --------------------------------------------------------
/**
 * Khởi động bộ PID.
 */
void pid_init(pid_t *pid)
{
  pid->se = 0;
  pid->pe = 0;
  pid->ref = 0;
}

/**
 * Nhận tín hiệu hồi tiếp, thực hiện cập nhật
 * và gửi tín hiệu ngõ ra.
 */
float pid_step(pid_t *pid, float i)
{
  float de, e, o;

  // Tính sai số e
  e = pid->ref - i;

  // Tính thay đổi của e so với chu kỳ trước
  de = e - pid->pe;

  // Tính tích phân
  pid->se += (e + pid->pe)/2*dt;

  // Tính giá trị ngõ ra
  o = pid->kp*e + pid->ki*pid->se + pid->kd*de/dt;

  // e của chu kỳ này là pe của chu kỳ kế tiếp
  pid->pe = e;

  return o;
}
