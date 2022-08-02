// File: agv.h
// Tổng hợp các việnthư viện và module.

#ifndef AGV_H
#define AGV_H

// Chế độ chạy thử với app điện thoại kết
// nối qua bluetooth. Tốc độ truyền mặc
// định của module HC-06 là 9600.
#define BLUETOOTH 1

// Nếu bật chế độ DEBUG, các chức năng
// truyền dữ liệu sẽ không thực hiện được
#define DEBUG 1

#if DEBUG
  extern volatile unsigned long debugval01, debugval02;
  #define dprint(m) Serial.print(m)
  #define dprintln(m) Serial.println(m)
#else
  #define dprint(m)
  #define dprintln(m)
#endif
extern volatile unsigned char *debug_ptr;

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

// TODO: viết lại module imu và uart thành code C, chuyển toàn bộ project thành C
// TODO: tách cpu_map và các phần có cài đặt thanh ghi ra để

#endif
