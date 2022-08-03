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
typedef int16_t Q7_8_t;     // Kiểu số đa dụng 16-bit
typedef int16_t Q1_14_t;    // Kiểu trả về của chip MPU6050
typedef int32_t Q26_6_t;    // Kiểu tọa độ trả về khi có yêu cầu gửi
typedef int32_t Q17_14_t;   // Kiểu số đa dụng 32-bit
typedef int32_t Q7_24_t;    // Kiểu số 32-bit dùng lưu giá trị góc (radians)
typedef int32_t Q3_28_t;    // Bình phương kiểu trả về của chip MPU6050

// Kiểu của biến duty_cycle
// Giá trị 0 -> 1.0 tương ứng
// với 8 bit 0 -> 255
typedef int32_t Q23_8_t;

// Kiểu lưu các giá trị trong
// bộ điều khiển PID lái
typedef int16_t Q3_12_t;

// Kiểu số dùng lưu
// tọa độ của xe (64-bit
// và 32-bit).
// - Chu vi trái đất là:
//    R = 40075017 m
// - Giá trị lớn nhất số
// có thể đạt tới của phần
// nguyên là:
//    2^26 - 1 = 67108863 > R
typedef int64_t Q25_38_t;

// Các macro nhân và chia
#define Q7_8(A) (int16_t)((A)*256)
#define Q7_8MUL(A,B) (((int32_t)(A)*(B))>>8)
#define Q7_8DIV(A,B) ((((int32_t)(A)<<16)/(B))>>8)
#define Q1_14(A) (int16_t)((A)*16384)
#define Q1_14MUL(A,B) (((int32_t)(A)*(B))>>14)
#define Q1_14DIV(A,B) ((((int32_t)(A)<<16)/(B))>>2)
#define Q17_14(A) (int32_t)((A)*16384)
#define Q17_14MUL(A,B) (((int64_t)(A)*(B))>>14)
#define Q17_14DIV(A,B) ((((int64_t)(A)<<32)/(B))>>18)
#define Q7_24(A) (int32_t)((A)*16777216)
#define Q7_24MUL(A,B) (((int64_t)(A)*(B))>>24)
#define Q7_24DIV(A,B) ((((int64_t)(A)<<32)/(B))>>8)
#define Q3_28(A) (int32_t)((A)*268435456)
#define Q3_28MUL(A,B) (((int64_t)(A)*(B))>>28)
#define Q3_28DIV(A,B) ((((int64_t)(A)<<32)/(B))>>4)
#define Q27_8(A) (int32_t) ((A)*256)
#define Q27_8MUL(A,B) (((int64_t)(A)*(B))>>8)
#define Q27_8DIV(A,B) ((((int64_t)(A)<<32)/(B))>>24)
#define Q3_12(A) (int32_t) ((A)*4096)
#define Q3_12MUL(A,B) (((int32_t)(A)*(B))>>12)
#define Q3_12DIV(A,B) ((((int32_t)(A)<<16)/(B))>>4)

// Chuyển đổi giữa các kiểu fixed-point
#define Q25_38_TO_Q25_6(A) (Q25_6_t)((A)>>32)
#define Q7_24_TO_Q17_14(A) (Q17_14_t)((A)>>10)
#define Q7_24_TO_Q3_12(A) (Q3_12_t)((A)>>12)
#define Q3_28_TO_Q17_14(A) (Q7_24_t)((A)>>14)
#define Q3_28_TO_Q7_24(A) (Q7_24_t)((A)>>4)
#define Q3_28_TO_Q3_12(A) (Q3_12_t)((A)>>16)

// Phép nhân giữa kiểu hai số
// kiểu Q3_28, kết quả gán cho
// biến kiểu Q25_38. vì vậy cần
// dịch phải 18 bit:
// c*2^-38 = (a*2^-28)*(b*2^-28)
//         = (a*b*2^-18)*2^-38
// =>  c   = (a*b)*2^-18
#define Q3_28_MUL_AS_Q25_38(A,B) (((int64_t)(A)*(B))>>18)

// Tìm arccos của một giá trị kiểu Q3_28
Q3_28_t Q3_28acos_lookup(Q3_28_t c);

// Hàm tìm arctan xấp xỉ
Q3_28_t Q3_28atan2(Q3_28_t y, Q3_28_t x);

//Tính atan trong góc phần tư thứ nhất.
Q3_28_t Q3_28atan(Q3_28_t a);

#endif
