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
int32_t pid_step(pid_t *pid, int16_t i)
{
  return pid_stp_from_error(pid, pid->ref - i);
}

/**
 * Nhận giá trị sai số, thực hiện cập nhật
 * và gửi tín hiệu ngõ ra.
 */
int32_t pid_stp_from_error(pid_t *pid, int16_t e)
{
  int16_t de;
  int32_t u;
  
  // Tính thay đổi của e so với chu kỳ trước
  de = (e - pid->pe);

  // Tính tích phân
  if ((e >= 0 && pid->pe < 0) ||
      (e < 0 && pid->pe >= 0))
  {
    pid->se = 0;
  } else pid->se += (e + pid->pe)>>1;
  // TODO: thêm pid->se_decay*pid->se vào công thức
  
  // Tính giá trị ngõ ra
  u = ((uint32_t)pid->kp*e +
       (uint32_t)pid->ki*pid->se +
       (uint32_t)pid->kd*de);

  // e của chu kỳ này là pe của chu kỳ kế tiếp
  pid->pe = e;

  return (u >> 8);
}
