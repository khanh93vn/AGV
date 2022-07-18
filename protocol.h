// File: protocol.h
// Quản lý giao thức với các thiết bị khác.

#ifndef PROTOCOL_H
#define PROTOCOL_H

// Các cờ của module
#define PROTOCOL_FLAG_STOPPED     0x00  // mặc định
#define PROTOCOL_FLAG_RECEIVING   0x01
#define PROTOCOL_FLAG_ERROR       0x02
#define PROTOCOL_FLAG_UPDATE_REF  0x04

extern uint8_t protocol_flags;

// Các giá trị tham chiếu lưu tạm
extern float protocol_drive_ref_buff;
extern float protocol_steer_ref_buff;

// Khởi động truyền thông
void protocol_init();

// Reset truyền thông
void protocol_reset();

// Vòng lặp chính
void protocol_loop();

#endif
