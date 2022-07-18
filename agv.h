// File: agv.h
// Tổng hợp các thư viện và module.

#ifndef AGV_H
#define AGV_H

// nhập thư viện chuẩn
#include <stdint.h>     // Cần để khai báo các kiểu số nguyên
#include <stdlib.h>     // Hàm memset và strcpy
#include <avr/wdt.h>
#include <Arduino.h>

// nhập các module
#include "cpu_map.h"
#include "defaults.h"
#include "settings.h"
#include "pid.h"
#include "drive.h"
#include "steer.h"
#include "protocol.h"
#include "system.h"

#endif
