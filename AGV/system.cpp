// File: system.cpp
// Quản lý hệ thống:
// - Định thời cho việc lấy mẫu và điều khiển PID.
// - Ngắt đọc encoder.

#include "agv.h"

// Các biến toàn cục của module ----------------------------------------------
// Dữ liệu điều khiển PID của bánh dẫn động
volatile uint16_t sys_dr_ref, dr_pe, dr_se;

// Dữ liệu điều khiển PID của cơ cấu lái
volatile Q3_12 sys_st_ref, st_pe, st_se;

// Dữ liệu tự định vị của xe
volatile sys_pose_t sys_pose;
volatile frame_t frame;

// Biến đếm số lần lấy mẫu của hệ thống (số lần ngắt timer)
volatile uint16_t sys_sample_cnt;

// Biến đọc cập nhật encoder
volatile int16_t encoder_pulse_cnt;

// Biến đếm số xung encoder di chuyển được mỗi chu kỳ ngắt.
// Tương đương với vận tốc trung bình.
// Đơn vị: (sỗ xung encoder)/(chu kỳ ngắt)
volatile int16_t encoder_pos_dot;

// Biến lưu vị trí encoder. Đơn vị: số xung encoder
volatile int16_t encoder_position;

// Độ dài ước tính di chuyển được mỗi chu kỳ ngắt
volatile Q3_28 meters_per_dt;
// Các chương trình con --------------------------------------------------------
/**
 * Khởi động hệ thống, thiết lập chương trình
 * ngắt đọc encoder và ngắt bộ định thời.
 */
void sys_init()
{
  // I) Cài đặt các bộ PID
  // PID bánh dẫn động
  pinMode(IO_DRIVE_P, OUTPUT);
  pinMode(IO_DRIVE_H1, OUTPUT);
  pinMode(IO_DRIVE_H2, OUTPUT);
  sys_dr_ref = 0;
  dr_pe = 0;
  dr_se = 0;


  // PID cơ cấu lái
  pinMode(IO_STEER_P, OUTPUT);
  pinMode(IO_STEER_H1, OUTPUT);
  pinMode(IO_STEER_H2, OUTPUT);
  sys_st_ref = 0;
  st_pe = 0;
  st_se = 0;

  // II) Thiết lập hệ đọc encoder

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

  // III) Thiết lập ngắt bộ định thời (Timer2A)
  // Xóa cờ chọn clock đầu vào và bộ chia (CS22:0 = 000)
  TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));

  // Chế độ CTC (WGM21:0 = 10)
  TCCR2A &= ~_BV(WGM20);
  TCCR2A |= _BV(WGM21);
  TCCR2B &= ~_BV(WGM22);

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
  dprint("Tần số CPU:"); dprintln(F_CPU);
  dprint("Tần số đếm:"); dprintln(F_CT);
  dprint("Giá trị OCR2A:"); dprintln(OCR2A);

  // Chọn xung đếm từ clock hệ thống và chế độ bộ chia
  // Có thể tham khảo giá trị bộ chia từ datasheet của vđk
  // Ví dụ bộ chia 1024 thì CS22:0 = 111
  TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20)); // reset CS22:0 về 000
  TCCR2B |= CLK_SEL;
}

/**
 * Cho dừng hệ thống do bị lỗi hoặc mục đích khác.
 * Chủ yếu là dừng các động cơ và dừng ngắt bộ
 * định thời để không chạy vòng lặp cập nhật.
 */
void sys_halt()
{
  // Dừng bộ định thời bằng cách xóa thanh ghi chọn
  // xung clock đầu vào của nó
  TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));

  // Dừng các động cơ
  digitalWrite(IO_DRIVE_P, LOW);
  digitalWrite(IO_DRIVE_H1, LOW);
  digitalWrite(IO_DRIVE_H2, LOW);
  digitalWrite(IO_STEER_P, LOW);
  digitalWrite(IO_STEER_H1, LOW);
  digitalWrite(IO_STEER_H2, LOW);
}

/**
 * Lấy dữ liệu về tốc độ hiện tại.
 * Đơn vị ngõ ra: m/s.
 */
Q17_14 sys_get_spd()
{
  return Q3_28_TO_Q17_14(meters_per_dt)*UPDATE_RATE;
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

  int32_t duty_cycle;
  uint8_t direction_bit;
  int16_t dr_e, dr_de;
  Q3_12 st_e, st_de;
  // II) Chạy bộ điều khiển PID bánh dẫn động:
  // u = kp.e + ki.∫edt + kd.de/dt
  // Trường hợp lấy mẫu rời rạc:
  // u = kp.e + ki.Σe.Δt + kd.de/Δt
  // Khi Δt là hằng số, đặt Δt ra:
  // u = kp.e + (ki.Δt).Σe + (kd/Δt).de
  // Cho ki = ki.Δt và kd = kd/Δt, sẽ rút
  // được một phép nhân và một phép chia
  // trong thuật toán (được tính sẵn
  // trong module settings)
  dr_e = encoder_position - sys_dr_ref; // Tính sai số
  dr_de = encoder_pos_dot;              // Tính de
  if ((dr_e>=0 && dr_pe<0) || (dr_e<0 && dr_pe>=0))
    dr_se = 0;
  else dr_se += dr_e;                   // Tính Σe
  // TODO: thêm hệ số tắt dần vào công thức

  // Tính giá trị ngõ ra:
  duty_cycle = ((int32_t)(settings.dr_kp)*dr_e +
                (int32_t)(settings.dr_ki)*dr_se +
                (int32_t)(settings.dr_kd)*dr_de);

  // e của chu kỳ này là pe của chu kỳ kế tiếp
  dr_pe = dr_e;

  // III) Điều khiển động cơ
  if (duty_cycle < 0) {                      // xác định chiều
    direction_bit = HIGH;
    duty_cycle = -duty_cycle;
  } else direction_bit = LOW;
  digitalWrite(IO_DRIVE_H1, direction_bit);  // Chỉnh chiều quay
  digitalWrite(IO_DRIVE_H2, !direction_bit);
  if (duty_cycle > 255) duty_cycle = 255;    // Ngõ ra phải trong đoạn [0, 255]
  if (duty_cycle < 70) duty_cycle = 0;       // Không đủ để thắng ma sát
  analogWrite(IO_DRIVE_P, (uint8_t)duty_cycle);

//  dprint(dr_e); dprint(" ");
//  dprint(sys_dr_ref); dprint(" ");
//  dprintln(duty_cycle);

  // IV) Cập nhật góc đầu xe từ cảm biến quán tính
  imu_update();
  // TODO: tính sys_pose.a bằng hàm arctan

  // V) Chạy bộ điều khiển PID góc lái
  st_e = Q7_24_TO_Q3_12(sys_pose.a) - sys_st_ref;   // Tính sai số
  while (st_e > Q3_12PI)
    st_e -= Q3_12TWO_PI;     // Đảm bảo e < pi
  while (st_e < -Q3_12PI)
    st_e += Q3_12TWO_PI;     // Đảm bảo e > -pi
  st_de = st_e - st_pe;             // Tính de
  if ((st_e>=0 && st_pe<0) || (st_e<0 && st_pe>=0))
    st_se = 0;
  else st_se += st_e;               // Tính Σe
  // TODO: thêm hệ số tắt dần vào công thức

  // Tính giá trị ngõ ra:
  duty_cycle = ((int32_t)(settings.st_kp)*st_e +
                (int32_t)(settings.st_ki)*st_se +
                (int32_t)(settings.st_kd)*st_de) >> 12;

  // e của chu kỳ này là pe của chu kỳ kế tiếp
  st_pe = st_e;

  // VI) Điều khiển động cơ lái
  if (duty_cycle < 0) {                      // Kiểm tra dấu của ngõ ra
    direction_bit = HIGH;
    duty_cycle = -duty_cycle;
  } else direction_bit = LOW;
  digitalWrite(IO_STEER_H1, direction_bit);  // Chỉnh chiều quay
  digitalWrite(IO_STEER_H2, !direction_bit);
  if (duty_cycle > 255) duty_cycle = 255;    // Ngõ ra phải trong đoạn [0, 255]
  analogWrite(IO_STEER_P, (uint8_t)duty_cycle);

  // VII) Tính tọa độ của xe trong hệ quy chiếu hiện tại

  // Tính quãng đường đi được (m)
  // trong khoảng thời gian giữa
  // 2 lần lấy mẫu
  meters_per_dt = encoder_pos_dot*settings.k_pc2m;

  // Cập nhật tọa độ xe (kiểu Q3_28)
  // quãng chênh lệch tính được bằng
  // phép nhân giữa quãng đường đi
  // với sin hoặc cos góc hướng đầu
  // xe (kiểu Q3_28).
  sys_pose.x += Q3_28_TO_Q7_24(Q3_28MUL(meters_per_dt, sys_pose.v[0]));
  sys_pose.y += Q3_28_TO_Q7_24(Q3_28MUL(meters_per_dt, sys_pose.v[1]));

  // Cập nhật pv
  sys_pose.pv[0] = sys_pose.v[0];
  sys_pose.pv[1] = sys_pose.v[1];

  // VIII) Cập nhật các điểm tham chiếu từ module protocol
  if (protocol_flags & PROTOCOL_FLAG_UPDATE_REF) {
    sys_dr_ref += protocol_drive_ref_buff;
    sys_st_ref = protocol_steer_ref_buff;
    protocol_flags &= ~PROTOCOL_FLAG_UPDATE_REF;
    dprint(sys_dr_ref); dprint(" ");
    dprintln(protocol_drive_ref_buff);
  }

  // IX) Cập nhật để đảm bảo các biến
  // ở trong khoảng nhất định
  if (encoder_position < 0) {
    encoder_position += settings.encoder_ppr;
    sys_dr_ref += settings.encoder_ppr;
  } else if (encoder_position > settings.encoder_ppr) {
    encoder_position -= settings.encoder_ppr;
    sys_dr_ref -= settings.encoder_ppr;
  }
  // TODO: thêm cập nhật sys_pose và hệ quy chiếu

  if (protocol_flags & PROTOCOL_FLAG_SAMPLE_RATE) {
    sys_sample_cnt++;
  }
}
