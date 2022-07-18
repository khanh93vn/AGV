// File: protocol.cpp
// Quản lý giao thức với các thiết bị khác.

#include "agv.h"

// Các mã lệnh điều khiển
#define PROTOCOL_STOP             0x01
#define PROTOCOL_UPDATE_REF       0x02
#define PROTOCOL_SET_DRIVE_REF    0x03
#define PROTOCOL_SET_DRIVE_KP     0x04
#define PROTOCOL_SET_DRIVE_KI     0x05
#define PROTOCOL_SET_DRIVE_KD     0x06
#define PROTOCOL_SET_STEER_REF    0x07
#define PROTOCOL_SET_STEER_KP     0x08
#define PROTOCOL_SET_STEER_KI     0x09
#define PROTOCOL_SET_STEER_KD     0x0A
#define PROTOCOL_SET_WHEEL_D      0x0B
#define PROTOCOL_SET_ENCODER_PPR  0x0C

#define BUFFERING_LONG            0xFFFFFFFF

// Các biến toàn cục ----------------------------------------------------------
volatile uint8_t protocol_flags;
volatile float protocol_drive_ref_buff;
volatile float protocol_steer_ref_buff;

// Các chương trình con --------------------------------------------------------
/**
 * Khởi động truyền thông UART.
 */
void protocol_init()
{
  // Cài đặt chân LED báo hiệu
  pinMode(IO_LED, OUTPUT);

  // Bắt đầu truyền thông
  Serial.begin(BAUDRATE);
}

/**
 * Reset truyền thông.
 */
void protocol_reset()
{
  // Xóa hết hàng đợi
  while (Serial.available()) Serial.read();

  // reset cờ hiệu
  protocol_flags = PROTOCOL_FLAG_STOPPED;
}

/**
 * Vòng lặp nhận tín hiệu điều khiển:
 * - Tốc độ tham chiếu
 * - Góc quay tham chiếu
 */
void protocol_loop()
{
  // Bắt đầu nhận tín hiệu truyền thông
  protocol_flags |= PROTOCOL_FLAG_RECEIVING;
  while (protocol_flags & PROTOCOL_FLAG_RECEIVING)
  {
    if (Serial.available() >= 17) {
      // I) Khai báo biến
      uint8_t buffer[18];   // Vùng nhớ chứa chuỗi truyền
      uint8_t ctrl_code;    // Mã lệnh
      float *ctrl_val;      // Con trỏ giá trị truyền
      uint32_t *comp_ptr;   // Con trỏ chỉ vào các vùng soát lỗi

      // Đọc chuỗi truyền.
      // Khung truyền thông theo bảng bên dưới (17 bit):
      //  ____________________________________________
      // |    |    |    |    |    |    |    |    |    |
      // | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 |
      // |____|____|____|____|____|____|____|____|____|
      // |    |    |    |    |    |    |    |    |    |
      // | FF | FF | FF | FF | C  | FF | FF | FF | FF |
      // |____|____|____|____|____|____|____|____|____|
      // ||||||||||||||||||||||||||||||||||||||||||||||
      // |    |    |    |    |    |    |    |    |    |
      // | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
      // |____|____|____|____|____|____|____|____|____|
      // |                   |    |    |    |    |    |
      // |       VALUE       | FF | FF | FF | FF |    |
      // |___________________|____|____|____|____|____|
      // Trong đó:
      // - Bit 0:3; 5:8; 13:16 là các bit đệm để kiểm
      // tra lỗi truyền (FF).
      // - Bit 4 chứa mã lệnh (C).
      // - Bit 9:12 chúa giá trị truyền
      // (VALUE, integer 32bit hoặc float 32bit)
      for (int i = 0; i < 17; i++) buffer[i] = Serial.read();

      // Kiểm tra 12 bit đệm
      comp_ptr = (uint32_t*)(buffer);
      if (*comp_ptr != BUFFERING_LONG) goto protocol_error;
      comp_ptr = (uint32_t*)(buffer+5);
      if (*comp_ptr != BUFFERING_LONG) goto protocol_error;
      comp_ptr = (uint32_t*)(buffer+13);
      if (*comp_ptr != BUFFERING_LONG) goto protocol_error;

      // Một số thông số cần đảm bảo xe dừng hẳn
      // trước khi thay đổi
      uint8_t agv_is_stopped = (sys_get_spd() == 0)?1:0;

      // Giải mã tín hiệu
      ctrl_code = buffer[4];
      ctrl_val = (float*)(buffer + 9);

      switch(ctrl_code)
      {
      case PROTOCOL_STOP:           // Dừng
        protocol_drive_ref_buff = 0;
        protocol_steer_ref_buff = sys_get_heading();
        // Thực hiện tiếp tục lệnh bên dưới
      case PROTOCOL_UPDATE_REF:     // Cập nhật tham chiếu
        protocol_flags |= PROTOCOL_FLAG_UPDATE_REF;
        break;
      case PROTOCOL_SET_DRIVE_REF:  // Đổi tham chiếu dẫn động
        protocol_drive_ref_buff = *ctrl_val;
        break;
      case PROTOCOL_SET_DRIVE_KP:   // Đổi kp
        if (agv_is_stopped) drive_pid.kp = *ctrl_val;
        break;
      case PROTOCOL_SET_DRIVE_KI:   // Đổi ki
        if (agv_is_stopped) drive_pid.ki = *ctrl_val;
        break;
      case PROTOCOL_SET_DRIVE_KD:   // Đổi kd
        if (agv_is_stopped) drive_pid.kd = *ctrl_val;
        break;
      case PROTOCOL_SET_STEER_REF:  // Đổi tham chiếu lái
        protocol_steer_ref_buff = *ctrl_val;
        break;
      case PROTOCOL_SET_STEER_KP:
        if (agv_is_stopped) steer_pid.kp = *ctrl_val;
        break;
      case PROTOCOL_SET_STEER_KI:
        if (agv_is_stopped) steer_pid.ki = *ctrl_val;
        break;
      case PROTOCOL_SET_STEER_KD:
        if (agv_is_stopped) steer_pid.kd = *ctrl_val;
        break;
      case PROTOCOL_SET_WHEEL_D:
        if (agv_is_stopped) settings.wheel_diameter = *ctrl_val;
        break;
      case PROTOCOL_SET_ENCODER_PPR:
        if (agv_is_stopped) settings.encoder_ppr = *ctrl_val;
        break;
      default:  // có lỗi xảy ra, thoát
protocol_error:
        protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;
        protocol_flags |= PROTOCOL_FLAG_ERROR;
      }
    }
  }

  // khi bị lỗi sẽ cho dừng xe
  protocol_drive_ref_buff = 0;
  protocol_steer_ref_buff = sys_get_heading();
  protocol_flags |= PROTOCOL_FLAG_UPDATE_REF;

  // Cho dừng hệ thống
  sys_halt();

  // Chạy vòng lặp báo lỗi
  uint8_t led_state = 0;
  while (protocol_flags & PROTOCOL_FLAG_ERROR)
  {
    led_state = !led_state;           // luân phiên trạng thái
    digitalWrite(IO_LED, led_state);  // chớp tắt
    delay(250);                       // Chu kỳ = 0.5s
    // TODO: thêm cách thoát khỏi vòng lặp lỗi
  }
}
