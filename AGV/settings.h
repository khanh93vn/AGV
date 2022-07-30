// File: settings.h
// Quản lý lưu trữ cài đặt hệ thống.

#ifndef SETTINGS_H
#define SETTINGS_H

// Dữ liệu cài đặt
typedef struct {
  uint16_t encoder_ppr;   // Số xung/vòng của encoder
  Q3_28 wheel_perimeter;  // Chu vi vòng lăn bánh dẫn động (m)
  Q1_14 se_decay;         // Hệ số tắt dần của giá trị sai số tích lũy

  // Dữ liệu thông số bộ PID bánh dẫn động
  Q7_8 dr_kp, dr_ki, dr_kd;

  // Dữ liệu thông số bộ PID lái
  Q7_8 st_kp, st_ki, st_kd;

  // Các thông số phụ thuộc
  Q3_28 k_pc2m;           // Hệ số chuyển đổi từ số xung sang số m đi được
} settings_t;
extern volatile settings_t settings;

// khởi động hệ quản lý cài đặt
void settings_init();

// Cập nhật các thông số phụ thuộc
void settings_update();

#endif
