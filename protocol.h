// File: protocol.h
// Quản lý giao thức với các thiết bị khác.

#ifndef PROTOCOL_H
#define PROTOCOL_H

// Các cờ của module
#define PROTOCOL_FLAG_STOPPED     0x00  // mặc định
#define PROTOCOL_FLAG_RECEIVING   0x01  //
#define PROTOCOL_FLAG_ERROR       0x02  //
#define PROTOCOL_FLAG_UPDATE_REF  0x04  // Đánh dấu cập nhật tham chiếu

extern volatile uint8_t protocol_flags;

// Các giá trị tham chiếu lưu tạm
// Để tăng độ đồng bộ giữa hệ thống điều khiển
// chạy thẳng và lái, cần phải cập nhật chúng cùng
// một lúc. Vì vậy phải lưu tạm các biến cần cập nhật.
// Khi cờ PROTOCOL_FLAG_UPDATE_REF được set lên 1, việc
// cập nhật các giá trị tham chiếu được thực hiện
// trong chương trình ngắt của module system.
extern volatile float protocol_drive_ref_buff;
extern volatile float protocol_steer_ref_buff;

// Khởi động truyền thông
void protocol_init();

// Reset truyền thông
void protocol_reset();

// Vòng lặp chính
void protocol_loop();

#endif
