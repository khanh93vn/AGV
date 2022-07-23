// File: pid.h
// Bộ điều khiển vi tích phân tỷ lệ.

#ifndef PID_H
#define PID_H

// Dữ liệu thông số bộ PID
typedef struct {
  // Tham số mô hình
  float kp, ki, kd, se_decay;

  // Sai số lần lấy mẫu trước và sai số tích lũy
  float pe, se;

  // Điểm tham chiếu
  float ref;
} pid_t;

// khởi động bộ PID
void pid_init(pid_t *pid);

// chương trình con cập nhật pid
float pid_step(pid_t *pid, float i);

// chương trình con cập nhật pid,
// nhận sai số thay vì giá trị hồi tiếp
float pid_stp_from_error(pid_t *pid, float i);

#endif
