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
#define SET_DECAY             0x0A  // hệ số tắt dần (PID)
#define SET_WHEEL_PERIMETER   0x0B  // chu vi vòng lăn bánh dẫn động

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
#define SEND_DECAY            0x8A  // hệ số tắt dần (PID)
#define SEND_WHEEL_PERIMETER  0x8B  // chu vi vòng lăn bánh dẫn động

// Các mã lệnh yêu cầu đặc biệt
#define SEND_POSE             0xFF  // gửi dữ liệu tự định vị
#define SEND_SAMPLING_RATE    0xFE  // Test và gửi tần số lấy mẫu
#define SEND_DEBUG_VAL        0xFD  // Gửi giá trị debug
#define UPDATE_REF            0x7F  // cập nhật tham chiếu
#define SET_WHEEL_DIAMETER    0x7E  // đường kính bánh xe

#define PACKET_PAD            0xFFFFFFFF

// Điểm bắt đầu các mã lệnh đọc
#define READ_CODE_BEGIN       SET_DRIVE_REF

// Điểm ranh giới giữa các lệnh ghi biến
// 16bit và các lệnh ghi biến 32bit
#define READ_32_BIT_BEGIN     SET_WHEEL_PERIMETER

// Điểm kết thúc các mã lệnh đọc
#define READ_CODE_END         SEND_WHEEL_PERIMETER+1

// Bảng địa chỉ của các biến cần đọc ghi
// như đã liệt kê ở trên (trừ các lệnh yêu
// cầu đặc biệt)
const void* RW_ADDRESSES[] = {
  (void*)&protocol_drive_ref_buff,
  (void*)&protocol_steer_ref_buff,
  (void*)&settings.dr_kp_raw,
  (void*)&settings.dr_ki_raw,
  (void*)&settings.dr_kd_raw,
  (void*)&settings.st_kp_raw,
  (void*)&settings.st_ki_raw,
  (void*)&settings.st_kd_raw,
  (void*)&settings.encoder_ppr,
  (void*)&settings.se_decay,
  (void*)&settings.wheel_perimeter,
};

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

#if BLUETOOTH
  // Tần số mặc định của HC-06
  Serial.begin(9600);
#else
  Serial.begin(BAUDRATE);
#endif
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
  Serial.write("AGV ready!\r");
  while (protocol_flags & PROTOCOL_FLAG_RECEIVING)
  {
    if (Serial.available() >= 17) {
      // I) Khai báo biến
      uint8_t buffer[18];   // Vùng nhớ chứa chuỗi truyền
      uint8_t ctrl_code;    // Mã lệnh
      uint32_t *data_ptr;   // Con trỏ 32bit
      uint32_t data;        // Vùng nhớ đệm để gửi hoặc nhận dữ liệu
      uint8_t addr_code;    // Giá trị chỉ biến cần đọc/ghi

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
      data_ptr = (uint32_t*)(buffer);
      if (*data_ptr != PACKET_PAD) goto protocol_error;
      data_ptr = (uint32_t*)(buffer+5);
      if (*data_ptr != PACKET_PAD) goto protocol_error;
      data_ptr = (uint32_t*)(buffer+13);
      if (*data_ptr != PACKET_PAD) goto protocol_error;
      dprintln("Packet is ok!");
      
      // Một số thông số cần đảm bảo xe dừng hẳn
      // trước khi thay đổi
      uint8_t agv_is_stopped = (sys_get_spd() == 0)?1:0;

      // Giải mã tín hiệu
      ctrl_code = buffer[4];
      switch(ctrl_code)         // Kiểm tra các mã lệnh đặc biệt
      {
      case SEND_POSE:           // gửi dữ liệu tự định vị
        // TODO: thêm code gửi dữ liệu tự định vị
        // Vấn đề: trong lúc đang copy dữ liệu, có thể bị ngắt,
        // làm dữ liệu sai, không ăn khớp
        break;
      case SEND_SAMPLING_RATE:  // Test và gửi tần số lấy mẫu
        if (agv_is_stopped) {
          protocol_flags &= ~PROTOCOL_FLAG_RECEIVING;
          // bắt đầu test
          sys_sample_cnt = 0;
          protocol_flags |= PROTOCOL_FLAG_SAMPLE_RATE;
          delay(1000);
          data = (uint32_t)sys_sample_cnt;
          dprint("Tần số lấy mẫu: "); dprintln(data);
          SEND_4BYTES_INT(&data);
          protocol_flags &= ~PROTOCOL_FLAG_SAMPLE_RATE;

          protocol_flags |= PROTOCOL_FLAG_RECEIVING;
        }
        break;
      case SEND_DEBUG_VAL:
        SEND_4BYTES_INT(debug_ptr);
        break;
      case UPDATE_REF:  // cập nhật tham chiếu
        protocol_flags |= PROTOCOL_FLAG_UPDATE_REF;
        dprintln("Đã cập nhật tham chiếu");
        break;
      default:  // Kiểm tra các mã lệnh gửi nhận thường
        addr_code = ctrl_code & ~READ_WRITE_MSK;    // lấy 7 bit đầu
        if (addr_code < READ_CODE_END) {
          data_ptr = RW_ADDRESSES[addr_code-READ_CODE_BEGIN];
          if (ctrl_code & READ_WRITE_MSK) {       // lấy bit cuối
            if (addr_code < READ_32_BIT_BEGIN)    // nếu là 1 thì gửi
              SEND_2BYTES_INT(data_ptr);
            else SEND_4BYTES_INT(data_ptr);
            dprint("Giá trị: "); dprintln(*data_ptr);
          }
          else {                                  // nếu là 0 thì nhận
            data = *(uint32_t*)(buffer + 9);
            // Tiền xử lý một số trường hợp
            switch(addr_code) {
            case SET_DRIVE_REF:       // tham chiếu bánh dẫn động
              *(int16_t*)&data = (*(int32_t*)&data)/settings.k_pc2m;
              break;
            }
            if (addr_code < READ_32_BIT_BEGIN)
              *(uint16_t*)data_ptr = (uint16_t)data;
            else *data_ptr = data;
            dprint("Đã nhận được: "); dprintln(data);
          }
        }
        else {
protocol_error:
            dprintln("Có lỗi xảy ra");
            protocol_flags |= PROTOCOL_FLAG_ERROR;
        }
      }
      
      // Hậu xử lý một số biến
      switch(ctrl_code) {
      case SET_DRIVE_KI:        // ki bánh dẫn động
      case SET_STEER_KI:        // ki góc lái
      case SET_DRIVE_KD:        // kd bánh dẫn động
      case SET_STEER_KD:        // kd góc lái
      case SET_WHEEL_DIAMETER:  // đổi đường kính bánh xe
      case SET_ENCODER_PPR:     // số xung/vòng của encoder
      case SET_WHEEL_PERIMETER: // chu vi vòng lăn bánh dẫn động
        settings_update();      // Cập nhật các thông số phụ thuộc
        break;
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
