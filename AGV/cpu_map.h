// File: cpu_map.h
// Khai báo chân IO và các thanh ghi.

#ifndef CPU_MAP_H
#define CPU_MAP_H

#define IO_DRIVE_P  5   // Chân cấp PWM cho bánh dẫn động
#define IO_DRIVE_H1 4   // Chân cấp cầu H bánh dẫn động
#define IO_DRIVE_H2 3   // Chân cấp cầu H bánh dẫn động
#define IO_STEER_P  6   // Chân cấp PWM cho động cơ lái
#define IO_STEER_H1 9   // Chân cấp cầu H động cơ lái
#define IO_STEER_H2 10  // Chân cấp cầu H động cơ lái
#define IO_ENC0_A   7   // Encoder đọc bánh dẫn động
#define IO_ENC0_B   8   // Encoder đọc bánh dẫn động
#define IO_LED      13  // Chân LED báo hiệu

// TODO: đổi Arduino pin thành IO port ATmega328P (?)

#endif
