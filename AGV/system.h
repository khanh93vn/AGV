// File: system.h
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển PID.
// - Ngắt đọc encoder.

#ifndef SYSTEM_H
#define SYSTEM_H

// Dữ liệu điều khiển PID của bánh dẫn động
extern volatile uint16_t sys_dr_ref;

// Dữ liệu điều khiển PID của cơ cấu lái
extern volatile Q3_12_t sys_st_ref;

// Dữ liệu tự định vị của xe
typedef struct {
  // x, y
  Q7_24_t x, y;

  // vector chỉ hướng đầu xe và
  // góc tương ứng
  Q3_28_t a, v[2], pv[2];   
} sys_pose_t;
extern volatile sys_pose_t sys_pose;

// Biến đếm số lần lấy mẫu của hệ thống (số lần ngắt timer)
extern volatile uint16_t sys_sample_cnt;

// Khởi động hệ thống
void sys_init();

// Dừng hệ thống (do lỗi)
void sys_halt();

// Lấy dữ liệu về tốc độ hiện tại (m/s)
Q17_14_t sys_get_spd();

#endif
