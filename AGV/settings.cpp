// File: settings.cpp
// Quản lý lưu trữ cài đặt hệ thống.

#include "agv.h"

// Các biến toàn cục ----------------------------------------------------------
// Dữ liệu cài đặt
volatile settings_t settings;

// Các chương trình con --------------------------------------------------------
/**
 * Khởi động hệ quản lý cài đặt. Lây các thông
 * số cài đặt từ EEPROM hay gán giá trị mặc định
 * cho chúng.
 */
void settings_init()
{
  // I) Chỉnh các con trỏ tới các vị trí lưu
  // thông số cài đặt ngoài module
  // (Chưa có)

  // II) Gán giá trị mặc định cho các thông số cài đặt

  // Số xung/vòng của encoder
  settings.encoder_ppr = ENCODER_PPR;

  // Đường kính bánh xe
  settings.wheel_perimeter = WHEEL_PERIMETER;

  // Hệ số tắt dần giá trị sai số tích lũy
  settings.se_decay = SE_DECAY;

  // Thông số PID bánh dẫn động
  settings.dr_kp = DRIVE_KP;
  settings.dr_ki = DRIVE_KI/UPDATE_RATE;      // ki <- ki*dt
  settings.dr_kd = DRIVE_KD*UPDATE_RATE;      // kd <- kd/dt

  // Thông số PID hệ thống lái
  settings.st_kp = STEER_KP;
  settings.st_ki = STEER_KI/UPDATE_RATE;
  settings.st_kd = STEER_KD*UPDATE_RATE;

  // TODO: thêm phần load cài đặt từ EEPROM

  // Tính các thông số phụ thuộc
  settings_update();
}

/** 
 * Cập nhật các thông số phụ thuộc từ
 * các thông số độc lập đã biết.
 */
void settings_update()
{
  // Hệ số chuyển đổi từ số xung sang số m đi được
  settings.k_pc2m = settings.wheel_perimeter/settings.encoder_ppr;
}

// TODO: thêm chương trình con set perimeter từ diameter
