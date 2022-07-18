// File: settings.h
// Quản lý lưu trữ cài đặt hệ thống.

#ifndef SETTINGS_H
#define SETTINGS_H

// Dữ liệu cài đặt
typedef struct {
  float wheel_diameter;
  uint16_t encoder_ppr;
} settings_t;
extern settings_t settings;

// khởi động hệ quản lý cài đặt
void settings_init();

#endif
