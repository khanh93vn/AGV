/*
 * Code driver điều khiển xe.
 * Các bộ điều khiển PID tự động điều khiển động cơ dẫn động và
 * động cơ lái theo tốc độ tham chiếu và góc tham chiếu.
 * 
 * Việc cần làm:
 * - Thêm module chuyển đổi đơn vị thành đơn vị
 * chuẩn:
 *   + Xung encoder thành đơn vị hệ mét:
 *   (khoảng cách) = pi*(bán kính bánh xe)*(số xung)/(số xung/vòng)
 *   + Số chu kỳ ngắt thành giây:
 *   (thời gian) = (số lần ngắt)*8ms
 * - Thêm module điều khiển PID góc lái.
 * - Thêm module lưu tham số điều chỉnh
 *   + kp, ki, kd
 *   + Bán kính bánh xe
 */

#include "agv.h"

/*
void setup()
{
  // Khởi động hệ thống
  settings_init();
  // sys_init();
  
  // Chỉnh chế độ I/O
  pinMode(IO_DRIVE_P, OUTPUT);
  pinMode(IO_DRIVE_H1, OUTPUT);
  pinMode(IO_DRIVE_H2, OUTPUT);
  pinMode(IO_STEER_P, OUTPUT);
  pinMode(IO_STEER_H1, OUTPUT);
  pinMode(IO_STEER_H2, OUTPUT);

  // Thiết lập giao tiếp UART
  Serial.begin(115200);

  // Reset các bộ điều khiển PID
  memset(&drive_pid, 0, sizeof(drive_pid));
  memset(&steer_pid, 0, sizeof(steer_pid));
  
  // lấy tham số bộ điều khiển PID từ bộ nhớ
}

void loop()
{
}*/
