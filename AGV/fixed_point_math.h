// File: fixed_point_math.h
// Hỗ trợ tính toán kiểu fixed-point.

#ifndef FIXED_POINT_MATH_H
#define FIXED_POINT_MATH_H

// Kiểu fixed point 16-bit,
// có giá trị trong đoạn
// [-127.99609375, 128.0]
// độ phân giải 0.00390625
typedef int16_t Q1_14;

#define Q8_8CONST(A) (int16_t)((A + 1.52587890625e-05)*256)
#define Q8_8MUL(A,B) ((int32_t)A*B + 128)>>8  // Thành 24-bit
#define Q8_8DIV(A,B) (((int32_t)A<<16)/B)>>8

// Giá trị góc xoay được lưu
// trong một số nguyên 32-bit,
// giá trị trong nửa khoảng
// [-2147483648, 2147483647)
// tương ứng với giá trị giải
// mã [-pi, pi). Giá trị 0
// tương ứng với 0 radians
typedef int32_t angle_t;

// Giá trị khoảng cách được lưu
// trong một số nguyên 32-bit,
// giá trị trong nửa khoảng
// [-2147483648, 2147483647)
// tương ứng với giá trị giải
// mã [-32768.0, 32767.9999847412109375).
typedef int32_t distance_t;

#endif
