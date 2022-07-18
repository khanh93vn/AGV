// File: defaults.h
// Khai báo thông số mặc định và hằng số.

#ifndef DEFAULTS_H
#define DEFAULTS_H

// Các giá trị thay đổi được --------------------------------------------------

// Số xung encoder / vòng mặc định
#define ENCODER_PPR     600

// Đường kính mặc định bánh dẫn động (m)
#define WHEEL_DIAMETER  0.05

// Thông số PID mặc định
#define DRIVE_KP        0.5
#define DRIVE_KI        0.0
#define DRIVE_KD        0.0
#define STEER_KP        0.5
#define STEER_KI        0.0
#define STEER_KD        0.0

// Các giá trị yêu cầu thận trọng khi thay đổi -------------------------------

// Chu kỳ lấy mẫu (s) và tần số cập nhật (Hz)
// CHÚ Ý KHI ĐIỀU CHỈNH PHẢI ĐẢM BẢO 2 GIÁ
// TRỊ NÀY PHÙ HỢP VỚI NHAU
#define dt              8e-3
#define UPDATE_RATE     125

// Chọn xung clock đầu vào (chế độ chia)
// LƯU Ý KHI ĐIỀU CHỈNH PHẢI ĐẢM BẢO
// THANH GHI ORC CỦA BỘ ĐỊNH THỜI ĐƯỢC CHỌN
// KHÔNG BỊ TRÀN
#define CLK_SEL         0b111

// CÁC GIÁ TRỊ KHÔNG ĐƯỢC PHÉP THAY ĐỔI ---------------------------------------

// Các hằng số
#define PI              3.141592653589793238

// Tần số clock hệ thống
// KHÔNG ĐƯỢC THAY ĐỔI GIÁ TRỊ NÀY
#define F_CK            16000000L

// Tần số đếm sau khi qua bộ chia
#if CLK_SEL == 0b001
  #define F_CT (F_CK >> 3)
#elif CLK_SEL == 0b010
  #define F_CT (F_CK >> 5)
#elif CLK_SEL == 0b011
  #define F_CT (F_CK >> 6)
#elif CLK_SEL == 0b101
  #define F_CT (F_CK >> 7)
#elif CLK_SEL == 0b110
  #define F_CT (F_CK >> 8)
#elif CLK_SEL == 0b111
  #define F_CT (F_CK >> 10)
#else
  #error "No clock source!"
#endif

#endif
