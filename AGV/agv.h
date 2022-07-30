// File: agv.h
// Tổng hợp các việnthư viện và module.

#ifndef AGV_H
#define AGV_H

#define DEBUG 1

#if DEBUG
  extern volatile long debugval01, debugval02;
  extern volatile long *debugval_ptr;
  #define dprint(m) Serial.print(m)
  #define dprintln(m) Serial.println(m)
#else
  #define dprint(m)
  #define dprintln(m)
#endif

// nhập thư viện chuẩn
#include <stdint.h>     // Cần để khai báo các biến số nguyên
#include <stdlib.h>     // Hàm memset và strcpy
#include <avr/wdt.h>    // Watchdog (để tắt watchdog)
#include <Arduino.h>    // Thư viện Arduino
#include <HardwareSerial.h> // Thư viện truyền thông UART

// nhập thư viện cộng đồng
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include <Wire.h>
#endif

// nhập các module
#include "fixed_point_math.h"
#include "cpu_map.h"    // Khai báo chân GPIO và các thanh ghi
#include "defaults.h"   // Các thông số mặc định và hằng số
#include "settings.h"   // Quản lý thông số cài đặt
#include "imu.h"        // Module đọc cảm biến quán tính (MPU6050)
#include "protocol.h"   // Module truyền thông, giao tiếp
#include "system.h"     // Module quản lý thực thi hệ thống thời gian thực

#endif
