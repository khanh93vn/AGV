// File: settings.cpp
// Quản lý lưu trữ cài đặt hệ thống.

#include "agv.h"

// Cấu trúc lưu các con trỏ tới các thông số
// cài đặt nằm ngoài module
struct {
  float *drive_pid_params;
  float *steer_pid_params;
} ext_settings;

// Các biến toàn cục ----------------------------------------------------------
// Dữ liệu cài đặt
settings_t settings;

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

  // Thông số kp, ki, kd của module drive
  ext_settings.drive_pid_params = &drive_pid.kp;

  // Thông số kp, ki, kd của module steer
  ext_settings.steer_pid_params = &steer_pid.kp;

  // II) Gán giá trị mặc định cho các thông số cài đặt

  // Đường kính bánh xe
  settings.wheel_diameter = WHEEL_DIAMETER;

  // Số xung/vòng của encoder
  settings.encoder_ppr = ENCODER_PPR;

  // Thông số PID bánh dẫn động
  ext_settings.drive_pid_params[0] = DRIVE_KP;
  ext_settings.drive_pid_params[1] = DRIVE_KI;
  ext_settings.drive_pid_params[2] = DRIVE_KD;
  ext_settings.drive_pid_params[3] = SE_DECAY;

  // Thông số PID hệ thống lái
  ext_settings.steer_pid_params[0] = STEER_KP;
  ext_settings.steer_pid_params[1] = STEER_KI;
  ext_settings.steer_pid_params[2] = STEER_KD;
  ext_settings.steer_pid_params[3] = SE_DECAY;

  // TODO: thêm phần load cài đặt từ EEPROM
}
