// File: system.c
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển.
// - Ngắt đọc encoder.

#include "agv.h"

// Các biến toàn cục của module ----------------------------------------------
// Biến đọc cập nhật encoder
volatile int16_t encoder_pulse_cnt;

// Biến đếm số xung encoder di chuyển được mỗi chu kỳ ngắt.
// Tương đương với vận tốc trung bình.
// Đơn vị: (sỗ xung encoder)/(chu kỳ ngắt)
int16_t encoder_pos_dot;

// Biến lưu vị trí encoder. Đơn vị: số xung encoder
int64_t encoder_position;

// Góc xoay đầu xe
float vehicle_heading;

// Các chương trình con --------------------------------------------------------
/**
 * Khởi động hệ thống, thiết lập chương trình
 * ngắt đọc encoder và ngắt bộ định thời.
 */
void sys_init()
{
  // I) Thiết lập hệ đọc encoder

  // Thiết lập chế độ input cho chân đọc encoder
  pinMode(IO_ENC0_A, INPUT_PULLUP);
  pinMode(IO_ENC0_B, INPUT_PULLUP);

  // Thiết lập ngắt khi chân IO_ENC0_A thay đổi trạng thái
  *digitalPinToPCMSK(IO_ENC0_A) |= bit(digitalPinToPCMSKbit(IO_ENC0_A));
  PCIFR  |= bit(digitalPinToPCICRbit(IO_ENC0_A)); // xóa các cờ ngắt hiện tại
  PCICR  |= bit(digitalPinToPCICRbit(IO_ENC0_A)); // cho phép ngắt

  // Reset các biến đọc encoder
  encoder_pulse_cnt = 0;
  encoder_pos_dot = 0;
  encoder_position = 0;

  // II) Thiết lập ngắt bộ định thời (Timer2A)
  // Xóa cờ chọn clock đầu vào và bộ chia (CS22:0 = 000)
  TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));

  // Chế độ CTC (WGM21:0 = 10)
  TCCR2A |= _BV(WGM21); TCCR2B &= ~_BV(WGM22);

  // Chế độ ngắt (COM2A1:0 = 00 và COM2B1:0 = 00)
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0) | _BV(COM2B1) | _BV(COM2B0));

  // Xóa cờ ngắt
  TIFR2 &= ~_BV(OCF2A);

  // Cho phép ngắt
  TIMSK2 |= _BV(OCIE2A);

  // Reset biến đếm
  TCNT2 = 0;

  // Thiết lập tần số ngắt thông qua giá trị so sánh bộ ngắt
  // Với tần số ngắt. Ví dụ khi clock hệ thống 16MHz, qua bộ
  // chia 1024, giá trị OCR là 124 thì tần số ngắt là:
  // 16000000/(1024*(124+1)) = 125
  OCR2A = (uint8_t)(F_CT/UPDATE_RATE - 1);

  // Chọn xung đếm từ clock hệ thống và chế độ bộ chia
  // Có thể tham khảo giá trị bộ chia từ datasheet của vđk
  // Ví dụ bộ chia 1024 thì CS22:0 = 111
  TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20)); // reset CS22:0 về 000
  TCCR2B |= CLK_SEL;
}

/**
 * Lấy dữ liệu về tốc độ hiện tại.
 * Đơn vị ngõ ra: m/s.
 */
float sys_get_spd()
{
  return PI*settings.wheel_diameter*encoder_pos_dot/settings.encoder_ppr/dt;
}

/**
 * Lấy dữ liệu về góc xoay hiện tại.
 * Đơn vị ngõ ra: radians.
 */
float sys_get_heading()
{
  return vehicle_heading;
}
// Các chương trình ngắt ------------------------------------------------------
/**
 * Chương trình ngắt cập nhật vị trí encoder.
 * Kích hoạt khi chân đọc encoder kênh A thay
 * đổi trạng thái (cạnh lên hoặc xuống).
 */
ISR (PCINT2_vect)
{
  // Chỉ thực hiện đếm ở cạnh lên, nếu là cạnh xuống thì không đếm
  if (digitalRead(IO_ENC0_A)) return;

  // Đọc kênh B để xác định chiều thay đổi
  if (digitalRead(IO_ENC0_B)) encoder_pulse_cnt--;
  else encoder_pulse_cnt++;
}

/**
 * Chương trình ngắt timer. Chu kỳ 8ms
 * (Tần số 125Hz).
 * Thực hiện cập nhật biến lưu vị trí
 * và vận tốc, điều khiển vị trí hoặc
 * tốc độ bánh xe theo mức tham chiếu.
 */
ISR(TIMER2_COMPA_vect)
{
  // I) Cập nhật dữ liệu encoder

  // Chuyển giá trị qua encoder_pos_prime
  encoder_pos_dot = encoder_pulse_cnt;

  // Reset biến đếm
  encoder_pulse_cnt = 0;

  // cho phép ngắt chồng (để cập nhật encoder)
  sei();

  // Cập nhật vị trí tuyệt đối encoder
  encoder_position += encoder_pos_dot;

  // II) Chạy bộ điều khiển PID bánh dẫn động
  drive_step();

  // III) Cập nhật góc đầu xe từ cảm biến quán tính


  // IV) Chạy bộ điều khiển PID góc lái
  steer_step();

  // V) Gửi dữ liệu trạng thái qua UART, dữ liệu bao gồm
  // các thông số thay đổi vị trí và góc lái so với chu kỳ trước

}
