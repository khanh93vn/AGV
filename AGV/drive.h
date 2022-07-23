// File: drive.h
// Điều khiển bánh dẫn động.

#ifndef DRIVE_H
#define DRIVE_H

// Dữ liệu điều khiển PID của bánh dẫn động
extern volatile pid_t drive_pid;

// Khởi tạo
void drive_init();

// Cập nhật bộ điều khiển
void drive_step();

#endif
