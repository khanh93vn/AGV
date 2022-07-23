// File: steer.h
// Điều khiển góc lái.

#ifndef STEER_H
#define STEER_H

// Dữ liệu điều khiển PID của cơ cấu lái
extern volatile pid_t steer_pid;

// Khởi tạo
void steer_init();

// Cập nhật bộ điều khiển
void steer_step();

#endif
