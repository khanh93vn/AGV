// File: pid.h
// Bộ điều khiển vi tích phân tỷ lệ.

#ifndef PID_H
#define PID_H

// Dữ liệu thông số bộ PID
typedef struct {
  // Tham số mô hình
  float kp, ki, kd;

  // Sai số lần lấy mẫu trước và sai số tích lũy
  float pe, se;

  // Điểm tham chiếu
  volatile float ref;
} pid_t;

// khởi động bộ PID
void pid_init(pid_t *pid);

// chương trình con cập nhật pid
float pid_step(pid_t *pid, float i);

#endif
