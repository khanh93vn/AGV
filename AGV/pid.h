// File: pid.h
// Bộ điều khiển vi tích phân tỷ lệ.

#ifndef PID_H
#define PID_H

// Dữ liệu thông số bộ PID
typedef struct {
  // Tham số mô hình
  int16_t kp, ki, kd;
  uint16_t se_decay;

  // Sai số lần lấy mẫu trước và sai số tích lũy
  int16_t pe, se;

  // Điểm tham chiếu
  int16_t ref;
} pid_t;

// khởi động bộ PID
void pid_init(pid_t *pid);

// chương trình con cập nhật pid
int32_t pid_step(pid_t *pid, int16_t i);

// chương trình con cập nhật pid,
// nhận sai số thay vì giá trị hồi tiếp
int32_t pid_stp_from_error(pid_t *pid, int16_t e);

#endif
