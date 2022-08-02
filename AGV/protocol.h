// File: protocol.h
// Quản lý giao thức với các thiết bị khác.

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SEND_2BYTES_INT(ptr) Serial.write((uint8_t*)ptr, 2)
#define SEND_4BYTES_INT(ptr) Serial.write((uint8_t*)ptr, 4)

// Các cờ của module
#define PROTOCOL_FLAG_CLEARED     0x00  // Xóa tất cả các cờ
#define PROTOCOL_FLAG_RECEIVING   0x01  // Cờ báo sẵn sàng nhận
#define PROTOCOL_FLAG_ERROR       0x02  // Cờ báo hiệu hệ thống lỗi
#define PROTOCOL_FLAG_UPDATE_REF  0x04  // Cờ cập nhật tham chiếu
#define PROTOCOL_FLAG_SAMPLE_RATE 0x08  // Cờ đo tần số lấy mẫu

extern volatile uint8_t protocol_flags;

// Các giá trị tham chiếu lưu tạm
// Để tăng độ đồng bộ giữa hệ thống điều khiển
// chạy thẳng và lái, cần phải cập nhật chúng cùng
// một lúc. Vì vậy phải lưu tạm các biến cần cập nhật.
// Khi cờ PROTOCOL_FLAG_UPDATE_REF được set lên 1, việc
// cập nhật các giá trị tham chiếu được thực hiện
// trong chương trình ngắt của module system.
extern volatile int16_t protocol_drive_ref_buff;
extern volatile Q3_12_t protocol_steer_ref_buff;

// Khởi động truyền thông
void protocol_init();

// Reset truyền thông
void protocol_reset();

// Vòng lặp chính
void protocol_loop();

#endif
