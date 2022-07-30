// File: system.h
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển PID.
// - Ngắt đọc encoder.

#ifndef SYSTEM_H
#define SYSTEM_H

// Dữ liệu điều khiển PID của bánh dẫn động
extern volatile uint16_t sys_dr_ref;

// Dữ liệu điều khiển PID của cơ cấu lái
extern volatile Q3_12 sys_st_ref;

// Dữ liệu tự định vị của xe
typedef struct {
  Q7_24 x, y, a;      // x, y và góc hướng đầu xe
  Q3_28 v[2], pv[2];  // vector chỉ hướng đầu xe
} sys_pose_t;
extern volatile sys_pose_t sys_pose;

// Hệ quy chiếu làm mốc
typedef struct {
  Q25_6 x, y;
} frame_t;
extern volatile frame_t frame;

// Biến đếm số lần lấy mẫu của hệ thống (số lần ngắt timer)
extern volatile uint16_t sys_sample_cnt;

// Khởi động hệ thống
void sys_init();

// Dừng hệ thống (do lỗi)
void sys_halt();

// Lấy dữ liệu về tốc độ hiện tại (m/s)
Q17_14 sys_get_spd();

#endif
