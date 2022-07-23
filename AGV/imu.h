// File: imu.h
// Giao tiếp với cảm biến quán tính MPU6050.
// Tận dụng chức năng bộ xử lý chuyển động -
// Digital Motion Processor (DMP) được tích hợp
// sẵn trong MPU6050.
// Tham khảo:
// https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050/examples/MPU6050_DMP6

#ifndef IMU_H
#define IMU_H

void imu_init();

float imu_get_angle();

#endif
