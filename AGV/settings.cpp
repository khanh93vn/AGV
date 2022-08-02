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
  settings.dr_kp_raw = DRIVE_KP;
  settings.dr_ki_raw = DRIVE_KI;
  settings.dr_kd_raw = DRIVE_KD;

  // Thông số PID hệ thống lái
  settings.st_kp_raw = STEER_KP;
  settings.st_ki_raw = STEER_KI;
  settings.st_kd_raw = STEER_KD;

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

  // Thông số PID bánh dẫn động
  settings.dr_kp = (Q7_8_t)(((int64_t)settings.dr_kp_raw*settings.k_pc2m)>>28);
  settings.dr_ki = (Q7_8_t)(((int64_t)settings.dr_ki_raw*settings.k_pc2m/UPDATE_RATE)>>28); // ki <- ki*dt
  settings.dr_kd = (Q7_8_t)(((int64_t)settings.dr_kd_raw*settings.k_pc2m*UPDATE_RATE)>>28); // kd <- kd/dt

  // Thông số PID hệ thống lái
  settings.st_kp = settings.st_kp_raw;
  settings.st_ki = settings.st_ki_raw/UPDATE_RATE;
  settings.st_kd = settings.st_kd_raw*UPDATE_RATE;
}

// TODO: thêm chương trình con set perimeter từ diameter
