// File: protocol.cpp
// Quản lý giao thức với các thiết bị khác.

#include "agv.h"


#define READ_WRITE_MSK      0x80  // mask đọc/ghi 

// Các mã lệnh set giá trị biến
#define SET_DRIVE_REF         0x01  // tham chiếu bánh dẫn động
#define SET_STEER_REF         0x02  // tham chiếu góc lái
#define SET_DRIVE_KP          0x03  // kp bánh dẫn động
#define SET_DRIVE_KI          0x04  // ki bánh dẫn động
#define SET_DRIVE_KD          0x05  // kd bánh dẫn động
#define SET_STEER_KP          0x06  // kp góc lái
#define SET_STEER_KI          0x07  // ki góc lái
#define SET_STEER_KD          0x08  // kd góc lái
#define SET_ENCODER_PPR       0x09  // số xung/vòng của encoder
#define SET_WHEEL_PERIMETER   0x0A  // chu vi vòng lăn bánh dẫn động
#define SET_DECAY             0x0B  // hệ số tắt dần (PID)

// Các mã lệnh yêu cầu gửi giá trị biến
#define SEND_DRIVE_REF        0x81  // tham chiếu bánh dẫn động
#define SEND_STEER_REF        0x82  // tham chiếu góc lái
#define SEND_DRIVE_KP         0x83  // kp bánh dẫn động
#define SEND_DRIVE_KI         0x84  // ki bánh dẫn động
#define SEND_DRIVE_KD         0x85  // kd bánh dẫn động
#define SEND_STEER_KP         0x86  // kp góc lái
#define SEND_STEER_KI         0x87  // ki góc lái
#define SEND_STEER_KD         0x88  // kd góc lái
#define SEND_ENCODER_PPR      0x89  // số xung/vòng của encoder
#define SEND_WHEEL_PERIMETER  0x8A  // chu vi vòng lăn bánh dẫn động
#define SEND_DECAY            0x8B  // hệ số tắt dần (PID)

// Các mã lệnh yêu cầu đặc biệt
#define SEND_POSE             0xFF  // gửi dữ liệu tự định vị
#define SEND_SAMPLING_RATE    0xFE  // Test và gửi tần số lấy mẫu
#define UPDATE_REF            0x7F  // cập nhật tham chiếu
#define SET_WHEEL_DIAMETER    0x7E  // cập nhật tham chiếu

#define PACKET_PAD            0xFFFFFFFF

// Các biến toàn cục ----------------------------------------------------------
volatile uint8_t protocol_flags;
volatile int16_t protocol_drive_ref_buff;
volatile Q3_12 protocol_steer_ref_buff;

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
    if (Serial.available() >= 17) {
      // I) Khai báo biến
      uint8_t buffer[18];   // Vùng nhớ chứa chuỗi truyền
      uint8_t ctrl_code;    // Mã lệnh
      uint32_t *long_ptr;   // Con trỏ 32bit

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
      long_ptr = (uint32_t*)(buffer);
      if (*long_ptr != PACKET_PAD) goto protocol_error;
      long_ptr = (uint32_t*)(buffer+5);
      if (*long_ptr != PACKET_PAD) goto protocol_error;
      long_ptr = (uint32_t*)(buffer+13);
      if (*long_ptr != PACKET_PAD) goto protocol_error;

      // Một số thông số cần đảm bảo xe dừng hẳn
      // trước khi thay đổi
      uint8_t agv_is_stopped = (sys_get_spd() == 0)?1:0;

      // Giải mã tín hiệu
      ctrl_code = buffer[4];
      long_ptr = (uint32_t*)(buffer + 9);
      switch(ctrl_code)
      {
      case SEND_POSE:           // gửi dữ liệu tự định vị
      case SEND_SAMPLING_RATE:  // Test và gửi tần số lấy mẫu
        if (agv_is_stopped) {
          protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;

          // bắt đầu test
          sys_sample_cnt = 0;
          protocol_flags |= PROTOCOL_FLAG_SAMPLE_RATE;
          delay(1000);
          Serial.write("\0\0");
          Serial.write((uint8_t)sys_sample_cnt);
          protocol_flags &= ~PROTOCOL_FLAG_SAMPLE_RATE;
          
          protocol_flags |= PROTOCOL_FLAG_RECEIVING;
        }
        break;
      case UPDATE_REF:  // cập nhật tham chiếu
        protocol_flags |= PROTOCOL_FLAG_UPDATE_REF;
        dprintln("Đã cập nhật tham chiếu");
        break;
      default:  // có lỗi xảy ra, thoát
protocol_error:
        dprintln("Có lỗi xảy ra");
        protocol_flags |= PROTOCOL_FLAG_ERROR;
      /*
      case PROTOCOL_STOP:           // Dừng
        //protocol_drive_ref_buff = 
        //protocol_ref_buffsteer = 
        dprintln("Đã dừng");
        // Thực hiện tiếp tục lệnh bên dưới
      case PROTOCOL_UPDATE_REF:     // Cập nhật tham chiếu
      case PROTOCOL_ADD_DRIVE_REF:  // Đổi tham chiếu dẫn động (tăng/giảm)
        protocol_drive_ref_buff = *ctrl_val_ptr;
        dprint("Tăng/giảm tham chiếu dẫn động: ");
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
      case PROTOCOL_SET_DRIVE_DECAY:  // set hệ số tắt dần
        drive_pid.se_decay = *ctrl_val_ptr;
        break;
      case PROTOCOL_SET_STEER_DECAY:  // set hệ số tắt dần
        steer_pid.se_decay = *ctrl_val_ptr;
        break;
      case PROTOCOL_GET_POSE:         // Lấy dữ liệu tự định vị của xe
        dprint("(x = ");
        dprint(sys_pose_x);
        dprint(", y = ");
        dprint(sys_pose_y);
        dprint(", a = ");
        dprint(sys_pose_a);
        dprintln(")");
        break;*/
      }
    }
    
    if (protocol_flags & PROTOCOL_FLAG_ERROR)
      protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;
  }

  // khi bị lỗi sẽ cho dừng hệ thống
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
