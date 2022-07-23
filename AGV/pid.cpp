// File: pid.cpp
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
  float e = pid->ref - i;
  return pid_stp_from_error(pid, e);
}

/**
 * Nhận giá trị sai số, thực hiện cập nhật
 * và gửi tín hiệu ngõ ra.
 */
float pid_stp_from_error(pid_t *pid, float e)
{
  float de, u;
  
  // Tính thay đổi của e so với chu kỳ trước
  de = (e - pid->pe);

  // Tính tích phân
  pid->se = pid->se_decay*pid->se + (e + pid->pe)/2*dt;
  //pid->se = pid->se + (e + pid->pe)/2*dt;
  
  //pid->se += (e + pid->pe)/2;

  // Tính giá trị ngõ ra
  u = pid->kp*e + pid->ki*pid->se + pid->kd*de/dt;
  //u = pid->kp*e + pid->ki*pid->se + pid->kd*de;

  // e của chu kỳ này là pe của chu kỳ kế tiếp
  pid->pe = e;

  return u;
}
