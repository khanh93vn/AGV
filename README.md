# AGV
Điều khiển AGV bậc thấp sử dụng Arduino Uno (chip ATmega328P).

# Cấu trúc code
Chương trình chính nằm trong main.cpp.<br/>
Xem tập tin agv.h để nắm thông tin sơ bộ các module sử dụng.

# Nhánh "fixed"
Một số thay đổi của nhánh fixed:
- Đổi các kiểu số thực thành fixed point. Khai báo các kiểu fixed point lưu trong file fixed_point_math.h
- Nhập module pid, drive, steer vào module system. Bây giờ tất cả code đều nằm trong chương trình con ngắt timer ISR(TIMER2_COMPA_vect). Phần khởi động (init) của các module đó thì dời vào trong sys_init()
- Module imu cũng thay đổi. Đổi thành fixed point tối ưu hóa thuật toán trong module này đáng kể
- Module protocol sửa lại cho gọn hơn, có 3 kiểu mã lệnh: đọc, ghi và lệnh đặc biệt
