// File: imu.cpp
// Giao tiếp với cảm biến quán tính MPU6050.
// Tận dụng chức năng bộ xử lý chuyển động -
// Digital Motion Processor (DMP) được tích hợp
// sẵn trong MPU6050.
// Tham khảo:
// https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050/examples/MPU6050_DMP6

#include "agv.h"

MPU6050 mpu;

// biến control/status của cảm biến MPU
bool dmp_ready = false;   // = true nếu khởi động DMP thành công
uint8_t mpu_int_status;   // giữ giá trị ngắt từ MPU
uint8_t dev_status;       // giá trị trả về của thiết bị sau mỗi lần thực thi (0 = success, !0 = error)
uint16_t packet_size;     // kỳ vọng cỡ gói dữ liệu DMP (mặc định là 42 bytes)
uint16_t fifo_count;      // đếm số bytes trong FIFO
uint8_t fifo_buffer[64];  // hàng đợi (FIFO)

volatile bool mpu_interrupt = false;
void dmp_data_ready()
{
  mpu_interrupt = true;
}

/**
 * Khởi động hệ thống đọc cảm biến quán tính.
 * Thiết lập giao tiếp với MPU6050 và khởi động DMP.
 */
void imu_init()
{
  // khởi động truyền thông I2C
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  mpu.initialize();                   // Khởi động module MPU6050
  pinMode(INT_IMU, INPUT);            // Chỉnh chế độ input cho chân ngắt

  // TODO: thêm cách báo hiệu lỗi đường truyền
  //mpu.testConnection();             // Kiểm tra đường truyền

  dev_status = mpu.dmpInitialize();   // Khởi động DMP

  // Chỉnh offset cho thiết bị
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);

  // Kiểm tra thiết bị
  if (dev_status == 0) {
    // calib
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);

    // bật DMP lên
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INT_IMU),
                    dmp_data_ready, RISING);
    mpu_int_status = mpu.getIntStatus();
    dmp_ready = true;
    packet_size = mpu.dmpGetFIFOPacketSize();
  } else {  // Lỗi
    // TODO: thêm xử lý lỗi
  }
}

/**
 * Lấy góc xoay của xe (góc xoay quanh trục Z).
 * Dữ liệu thể hiện dưới dạng vector.
 */
uint8_t imu_update()
{
  Q1_14_t q_w, q_x, q_y, q_z;     // Biến giữ giá trị quarternion
  if (!dmp_ready || !mpu.dmpGetCurrentFIFOPacket(fifo_buffer))
    return 1;

  // Dữ liệu quarternion (integer 16 bit) được chứa trong các
  // byte (0, 1), (4, 5), (8, 9), (12, 13)
  q_w = (Q1_14_t)fifo_buffer[0] << 8 | fifo_buffer[1];
  q_x = (Q1_14_t)fifo_buffer[4] << 8 | fifo_buffer[5];
  q_y = (Q1_14_t)fifo_buffer[8] << 8 | fifo_buffer[9];
  q_z = (Q1_14_t)fifo_buffer[12] << 8 | fifo_buffer[13];

  // Tính hướng (góc xoay theo trục Z).
  // Công thức đối với quarternion kiểu float:
  //   angle = atan2(2.0*(z*w + x*y),
  //                 2.0*(w*w + x*x) - 1.0);
  //
  // sys_pose_curr->u = -((Q3_28_t)q_z*q_w + (Q3_28_t)q_x*q_y)<<1;
  // sys_pose_curr->v = (((Q3_28_t)q_w*q_w + (Q3_28_t)q_x*q_x)<<1) - Q3_28(1);
  // Quay các vector 90 cùng chiều kim đồng hồ (-90 độ)
  sys_pose_curr->u = (((Q3_28_t)q_w*q_w + (Q3_28_t)q_x*q_x)<<1) - Q3_28(1);
  sys_pose_curr->v = -((Q3_28_t)q_z*q_w + (Q3_28_t)q_x*q_y)<<1;


  // Tìm góc từ vector chỉ hướng (Tính arctan)
  sys_pose_curr->a = Q3_28atan2(sys_pose_curr->v,
                                sys_pose_curr->u);


  // Hiển thị để kiểm tra xem đọc đúng chưa
//  dprint("Quarternion: ");
//  dprint(q[0]); dprint(' ');
//  dprint(q[1]); dprint(' ');
//  dprint(q[2]); dprint(' ');
//  dprintln(q[3]);
//  dprint("Heading: ");
//  dprint((double)sys_pose.v[0]/268435456.0d); dprint(' ');
//  dprintln((double)sys_pose.v[1]/268435456.0d);
//  dprint("Angle: "); dprintln((double)(sys_pose.a)/268435456.0d);

  return 0;
}
