// File: system.h
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển.
// - Ngắt đọc encoder.

#ifndef SYSTEM_H
#define SYSTEM_H

// Khởi động hệ thống
void sys_init();

// Dừng hệ thống (do lỗi)
void sys_halt();

// Lấy dữ liệu về tốc độ hiện tại (m/s)
float sys_get_spd();

// Lấy dữ liệu về góc quay hiện tại (radians)
float sys_get_heading();

#endif
