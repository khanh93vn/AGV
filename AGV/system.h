// File: system.h
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển.
// - Ngắt đọc encoder.

#ifndef SYSTEM_H
#define SYSTEM_H

// Trạng thái tự định vị của xe
extern volatile float sys_pose_x;   // Tọa độ x
extern volatile float sys_pose_y;   // Tọa độ y
extern volatile float sys_pose_a;   // Hướng đầu xe

// Biến đếm số lần lấy mẫu của hệ thống (số lần ngắt timer)
extern volatile uint16_t sys_sample_cnt;

// Khởi động hệ thống
void sys_init();

// Dừng hệ thống (do lỗi)
void sys_halt();

// Lấy dữ liệu về tốc độ hiện tại (m/s)
float sys_get_spd();

// Lấy dữ liệu vị trí bánh xe (radians)
float sys_get_wheel_angle();

#endif
