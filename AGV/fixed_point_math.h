// File: fixed_point_math.h
// Hỗ trợ tính toán kiểu fixed-point.
// Tham khảo:
// https://www.embedded.com/fixed-point-math-in-c/
// http://www.olliw.eu/2014/fast-functions/

#ifndef FIXED_POINT_MATH_H
#define FIXED_POINT_MATH_H

// Các kiểu fixed point Qx_y
// được định nghĩa dùng x bits
// cho phần nguyên và y bits
// cho phần thập phân
typedef int16_t Q7_8;     // Kiểu số đa dụng 16-bit
typedef int16_t Q1_14;    // Kiểu trả về của chip MPU6050
typedef int32_t Q17_14;   // Kiểu số đa dụng 32-bit
typedef int32_t Q7_24;    // Kiểu số 32-bit dùng lưu giá trị góc (radians)
typedef int32_t Q3_28;    // Bình phương kiểu trả về của chip MPU6050

// Kiểu số 32-bit dùng lưu
// vị trí gốc tọa độ của hệ
// quy chiếu.
// - Chu vi trái đất là:
//    R = 40075017 m
// - Giá trị số phần nguyên
// lớn nhất là:
//    2^26 - 1 = 67108863 > R
typedef int32_t Q25_6;

// Kiểu của biến duty_cycle
// Giá trị 0 -> 1.0 tương ứng
// với 8 bit 0 -> 255
typedef int32_t Q23_8;

// Kiểu lưu các giá trị trong
// bộ điều khiển PID lái
typedef int16_t Q3_12;


// Các macro nhân và chia
#define Q7_8MUL(A,B) ((int32_t)A*B)>>8
#define Q7_8DIV(A,B) (((int32_t)A<<16)/B)>>8
#define Q1_14MUL(A,B) ((int32_t)A*B) >> 14
#define Q1_14DIV(A,B) (((int32_t)A<<16)/B)) >> 2
#define Q17_14MUL(A,B) ((int64_t)A*B)>>14
#define Q17_14DIV(A,B) (((int64_t)A<<32)/B)>>18
#define Q7_24MUL(A,B) ((int64_t)A*B)>>24
#define Q7_24DIV(A,B) (((int64_t)A<<32)/B)>>8
#define Q3_28MUL(A,B) ((int64_t)A*B)>>28
#define Q3_28DIV(A,B) (((int64_t)A<<32)/B)>>4
#define Q25_6MUL(A,B) ((int64_t)A*B)>>6
#define Q25_6DIV(A,B) (((int64_t)A<<32)/B)>>26
#define Q27_8MUL(A,B) ((int64_t)A*B)>>8
#define Q27_8DIV(A,B) (((int64_t)A<<32)/B)>>24
#define Q3_12MUL(A,B) ((int32_t)A*B)>>12
#define Q3_12DIV(A,B) (((int32_t)A<<16)/B)>>4

// Chuyển đổi giữa các kiểu fixed-point
#define Q7_24_TO_Q17_14(A) (Q17_14)(A>>10)
#define Q7_24_TO_Q3_12(A) (Q3_12)(A>>12)
#define Q3_28_TO_Q17_14(A) (Q7_24)(A>>14)
#define Q3_28_TO_Q7_24(A) (Q7_24)(A>>4)

// Phép nhân giữa kiểu Q7_24 với
// kiểu Q3_28, kết quả gán cho
// biến kiểu Q7_24. vì vậy cần
// dịch phải 28 bit:
// c*2^-24 = (a*2^-24)*(b*2^-28)
//         = (a*b*2^-28)*2^-24
// =>  c   = (a*b)*2^-28
#define Q7_24_MUL_Q3_28_AS_Q7_24(A,B) (Q7_24)(((int64_t)A*B)>>28)

// Tìm arccos của một giá trị kiểu Q3_28
Q3_28 Q3_28acos_lookup(Q3_28 c);

#endif
