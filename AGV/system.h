// File: system.h
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển.
// - Ngắt đọc encoder.

#ifndef SYSTEM_H
#define SYSTEM_H

// Biến đếm số lần lấy mẫu của hệ thống (số lần ngắt timer)
extern volatile uint32_t sys_sample_cnt;

// Biến xác định xe đã dừng hẳn

// Khởi động hệ thống
void sys_init();

// Dừng hệ thống (do lỗi)
void sys_halt();

// Lấy dữ liệu về tốc độ hiện tại (m/s)
float sys_get_spd();

// Lấy dữ liệu về góc quay hiện tại (radians)
float sys_get_heading();

#endif
