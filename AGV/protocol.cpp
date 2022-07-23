// File: protocol.cpp
// Quản lý giao thức với các thiết bị khác.

#include "agv.h"

// Các mã lệnh điều khiển
#define PROTOCOL_STOP             0x01  // Dừng
#define PROTOCOL_UPDATE_REF       0x02  // Cập nhật tham chiếu
#define PROTOCOL_SET_DRIVE_REF    0x03  // Set tham chiếu dẫn động (tốc độ tiến-lùi)
#define PROTOCOL_SET_DRIVE_KP     0x04  // Set Kp dẫn động
#define PROTOCOL_SET_DRIVE_KI     0x05  // Set Ki dẫn động
#define PROTOCOL_SET_DRIVE_KD     0x06  // Set Kd dẫn động
#define PROTOCOL_SET_STEER_REF    0x07  // Set tham chiếu góc lái
#define PROTOCOL_SET_STEER_KP     0x08  // Set Kp lái
#define PROTOCOL_SET_STEER_KI     0x09  // Set Ki lái
#define PROTOCOL_SET_STEER_KD     0x0A  // Set Kd lái
#define PROTOCOL_SET_WHEEL_D      0x0B  // Cập nhật đường kính bánh xe
#define PROTOCOL_SET_ENCODER_PPR  0x0C  // Cập nhật số xung/vòng của encoder
#define PROTOCOL_GET_SAMPLE_RATE  0x0D  // Test tần số lấy mẫu
#define PROTOCOL_SET_DRIVE_DECAY  0x0E  // Set hệ số tắt dần pid dẫn động
#define PROTOCOL_SET_STEER_DECAY  0x0F  // Set hệ số tắt dần pid lái

#define PROTOCOL_PAD              0xFFFFFFFF

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
  protocol_flags = PROTOCOL_FLAG_CLEARED;
}

/**
 * Vòng lặp nhận tín hiệu điều khiển:
 * - Tốc độ tham chiếu
 * - Góc quay tham chiếu
 */
void protocol_loop()
{
  // Bắt đầu nhận tín hiệu truyền thông
  dprintln("Bắt đầu nhận truyền thông");
  protocol_flags |= PROTOCOL_FLAG_RECEIVING;
  while (protocol_flags & PROTOCOL_FLAG_RECEIVING)
  {
    /*
    float spd = sys_get_spd();
    if (spd != 0) {
      dprint(sys_get_heading());
      dprint(' ');
      dprintln(sys_get_wheel_angle());
    }*/
    if (Serial.available() >= 17) {
      // I) Khai báo biến
      uint8_t buffer[18];   // Vùng nhớ chứa chuỗi truyền
      uint8_t ctrl_code;    // Mã lệnh
      float *ctrl_val_ptr;  // Con trỏ giá trị truyền
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
      if (*comp_ptr != PROTOCOL_PAD) goto protocol_error;
      comp_ptr = (uint32_t*)(buffer+5);
      if (*comp_ptr != PROTOCOL_PAD) goto protocol_error;
      comp_ptr = (uint32_t*)(buffer+13);
      if (*comp_ptr != PROTOCOL_PAD) goto protocol_error;

      // Một số thông số cần đảm bảo xe dừng hẳn
      // trước khi thay đổi
      uint8_t agv_is_stopped = (sys_get_spd() == 0)?1:0;

      // Giải mã tín hiệu
      ctrl_code = buffer[4];
      ctrl_val_ptr = (float*)(buffer + 9);
      switch(ctrl_code)
      {
      case PROTOCOL_STOP:           // Dừng
        protocol_stop();
        dprintln("Đã dừng");
        // Thực hiện tiếp tục lệnh bên dưới
      case PROTOCOL_UPDATE_REF:     // Cập nhật tham chiếu
        protocol_flags |= PROTOCOL_FLAG_UPDATE_REF;
        dprintln("Đã cập nhật tham chiếu");
        break;
      case PROTOCOL_SET_DRIVE_REF:  // Đổi tham chiếu dẫn động
        protocol_drive_ref_buff = *ctrl_val_ptr;
        dprint("Tham chiếu dẫn động mới: ");
        dprint(protocol_drive_ref_buff);
        dprintln(" radians");
        break;
      case PROTOCOL_SET_DRIVE_KP:   // Đổi kp
        if (agv_is_stopped) drive_pid.kp = *ctrl_val_ptr;
        dprint("kp dẫn động mới: ");
        dprintln(drive_pid.kp);
        break;
      case PROTOCOL_SET_DRIVE_KI:   // Đổi ki
        if (agv_is_stopped) drive_pid.ki = *ctrl_val_ptr;
        dprint("ki dẫn động mới: ");
        dprintln(drive_pid.ki);
        break;
      case PROTOCOL_SET_DRIVE_KD:   // Đổi kd
        if (agv_is_stopped) drive_pid.kd = *ctrl_val_ptr;
        dprint("kd dẫn động mới: ");
        dprintln(drive_pid.kd);
        break;
      case PROTOCOL_SET_STEER_REF:  // Đổi tham chiếu lái
        protocol_steer_ref_buff = *ctrl_val_ptr;
        dprint("Tham chiếu lái mới: ");
        dprint(protocol_steer_ref_buff);
        dprintln(" radians");
        break;
      case PROTOCOL_SET_STEER_KP:   // Đổi kp lái
        if (agv_is_stopped) steer_pid.kp = *ctrl_val_ptr;
        dprint("kp lái mới: ");
        dprintln(steer_pid.kp);
        break;
      case PROTOCOL_SET_STEER_KI:   // Đổi ki lái
        if (agv_is_stopped) steer_pid.ki = *ctrl_val_ptr;
        dprint("ki lái mới: ");
        dprintln(steer_pid.ki);
        break;
      case PROTOCOL_SET_STEER_KD:   // Đổi kd lái
        if (agv_is_stopped) steer_pid.kd = *ctrl_val_ptr;
        dprint("kd lái mới: ");
        dprintln(steer_pid.kd);
        break;
      case PROTOCOL_SET_WHEEL_D:    // cập nhật đường kính bánh dẫn động
        if (agv_is_stopped) settings.wheel_diameter = *ctrl_val_ptr;
        dprint("Đường kính bánh xe đã cập nhật: ");
        dprint(settings.wheel_diameter);
        dprintln(" m");
        break;
      case PROTOCOL_SET_ENCODER_PPR:  // cập nhật số cài đặt xung/vòng của encoder
        if (agv_is_stopped) settings.encoder_ppr = *ctrl_val_ptr;
        dprint("Số xung/vòng encoder đã cập nhật: ");
        dprint(settings.encoder_ppr);
        dprintln(" m");
        break;
      case PROTOCOL_GET_SAMPLE_RATE:  // test tần số lấy mẫu
        if (agv_is_stopped) {
          protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;

          // bắt đầu test
          sys_sample_cnt = 0;
          protocol_flags |= PROTOCOL_FLAG_SAMPLE_RATE;
          delay(1000);
          dprint("Số lần lấy mẫu trong 1s: "); dprintln(sys_sample_cnt);
          protocol_flags &= ~PROTOCOL_FLAG_SAMPLE_RATE;
          protocol_flags |= PROTOCOL_FLAG_RECEIVING;
        }
        break;
      case PROTOCOL_SET_DRIVE_DECAY:  // set hệ số tắt dần
        drive_pid.se_decay = *ctrl_val_ptr;
        break;
      case PROTOCOL_SET_STEER_DECAY:  // set hệ số tắt dần
        steer_pid.se_decay = *ctrl_val_ptr;
        break;
      default:  // có lỗi xảy ra, thoát
protocol_error:
        dprintln("Có lỗi xảy ra");
        protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;
        protocol_flags |= PROTOCOL_FLAG_ERROR;
      }
    }
  }

  // khi bị lỗi sẽ cho dừng xe
  protocol_stop();
  protocol_flags |= PROTOCOL_FLAG_UPDATE_REF;

  // Cho dừng hệ thống
  sys_halt();
  dprintln("Đã cho dừng hệ thống");

  protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;
  protocol_flags |= PROTOCOL_FLAG_ERROR;
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

/**
 * Cho dừng xe. Reset trị số tích phân trong các bộ PID.
 */
void protocol_stop()
{
  protocol_drive_ref_buff = sys_get_wheel_angle();
  protocol_steer_ref_buff = sys_get_heading();
  drive_pid.se = 0;
  steer_pid.se = 0;
}
