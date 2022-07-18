// File: agv.h
// Tổng hợp các thư viện và module.

#ifndef AGV_H
#define AGV_H

// nhập thư viện chuẩn
#include <stdint.h>     // Cần để khai báo các biến số nguyên
#include <stdlib.h>     // Hàm memset và strcpy
#include <avr/wdt.h>    // Watchdog (để tắt watchdog)
#include <Arduino.h>    // Thư viện Arduino
#include <HardwareSerial.h> // Thư viện truyền thông UART

// nhập các module
#include "cpu_map.h"    // Khai báo chân GPIO và các thanh ghi
#include "defaults.h"   // Các thông số mặc định và hằng số
#include "settings.h"   // Quản lý thông số cài đặt
#include "pid.h"        // Code thư viện PID đơn giản
#include "drive.h"      // Module điều khiển bánh dẫn động
#include "steer.h"      // Module điều khiển động cơ lái
#include "protocol.h"   // Module truyền thông, giao tiếp
#include "system.h"     // Module quản lý thực thi hệ thống thời gian thực

#endif
