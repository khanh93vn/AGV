"""
Chứa các lệnh gửi xuống xe.
 ____________        ____________        ____________
|            |      |            |      |            |
|   CLI/GUI  |----->| xử lý lệnh |----->|    AGV     |
|____________|      |____________|      |____________|

Để cập nhật bộ mã lệnh:
- Copy từ file protocol.cpp
- Find & replace comments: đổi '//' thành '#'
- Find & replace các macro dùng regex:
Find:    #define\s([A-Z_1-9]*)(\s*)(.*)
Replace: $1$2= $3
"""

from time import sleep
from struct import pack, unpack
from math import pi, degrees

from fixed_point import *

# Các mã lệnh set giá trị biến
SET_DRIVE_REF         = 0x01  # tham chiếu bánh dẫn động
SET_STEER_REF         = 0x02  # tham chiếu góc lái
SET_DRIVE_KP          = 0x03  # kp bánh dẫn động
SET_DRIVE_KI          = 0x04  # ki bánh dẫn động
SET_DRIVE_KD          = 0x05  # kd bánh dẫn động
SET_STEER_KP          = 0x06  # kp góc lái
SET_STEER_KI          = 0x07  # ki góc lái
SET_STEER_KD          = 0x08  # kd góc lái
SET_ENCODER_PPR       = 0x09  # số xung/vòng của encoder
SET_DECAY             = 0x0A  # hệ số tắt dần (PID)
SET_WHEEL_PERIMETER   = 0x0B  # chu vi vòng lăn bánh dẫn động

# Các mã lệnh yêu cầu gửi giá trị biến
SEND_DRIVE_REF        = 0x81  # tham chiếu bánh dẫn động
SEND_STEER_REF        = 0x82  # tham chiếu góc lái
SEND_DRIVE_KP         = 0x83  # kp bánh dẫn động
SEND_DRIVE_KI         = 0x84  # ki bánh dẫn động
SEND_DRIVE_KD         = 0x85  # kd bánh dẫn động
SEND_STEER_KP         = 0x86  # kp góc lái
SEND_STEER_KI         = 0x87  # ki góc lái
SEND_STEER_KD         = 0x88  # kd góc lái
SEND_ENCODER_PPR      = 0x89  # số xung/vòng của encoder
SEND_DECAY            = 0x8A  # hệ số tắt dần (PID)
SEND_WHEEL_PERIMETER  = 0x8B  # chu vi vòng lăn bánh dẫn động

# Các mã lệnh yêu cầu đặc biệt
SEND_POSE             = 0xFF  # gửi dữ liệu tự định vị
SEND_SAMPLING_RATE    = 0xFE  # Test và gửi tần số lấy mẫu
SEND_DEBUG_VAL        = 0xFD  # Gửi giá trị debug
UPDATE_REF            = 0x7F  # cập nhật tham chiếu
UPDATE_SETTINGS       = 0x7E  # cập nhật cài đặt
SET_WHEEL_DIAMETER    = 0x7D  # đường kính bánh xe

READ_WRITE_MSK        = 0x80

# Điểm bắt đầu các mã lệnh đọc
READ_CODE_BEGIN       = SET_DRIVE_REF

# Điểm ranh giới giữa các lệnh ghi biến
# 16bit và các lệnh ghi biến 32bit
READ_32_BIT_BEGIN     = SET_WHEEL_PERIMETER

# Điểm kết thúc các mã lệnh đọc
READ_CODE_END         = SET_DRIVE_REF+1

PACKET_PAD            = b'\xFF\xFF\xFF\xFF'

def get_cmd_bytestring(cmd_code, value=0):
    cmd_bytestring = (PACKET_PAD + pack('B', cmd_code) +
                      PACKET_PAD + pack('i', int(value)) +
                      PACKET_PAD)
    return cmd_bytestring

def lin(com, pos):
    com.write(get_cmd_bytestring(SET_DRIVE_REF, pos*2**28))
    com.write(get_cmd_bytestring(UPDATE_REF))

def ang(com, heading):
    com.write(get_cmd_bytestring(SET_STEER_REF, heading))
    com.write(get_cmd_bytestring(UPDATE_REF))

def cmd_help(com):
    print("Danh sách các lệnh:")
    print('\n'.join(command_map.keys()))

command_map = dict(
    sdrf=SET_DRIVE_REF,
    ssrf=SET_STEER_REF,
    sdkp=SET_DRIVE_KP,
    sdki=SET_DRIVE_KI,
    sdkd=SET_DRIVE_KD,
    sskp=SET_STEER_KP,
    sski=SET_STEER_KI,
    sskd=SET_STEER_KD,
    swp=SET_WHEEL_PERIMETER,
    sdc=SET_DECAY,
    seppr=SET_ENCODER_PPR,

    gdrf=SEND_DRIVE_REF,
    gsrf=SEND_STEER_REF,
    gdkp=SEND_DRIVE_KP,
    gdki=SEND_DRIVE_KI,
    gdkd=SEND_DRIVE_KD,
    gskp=SEND_STEER_KP,
    gski=SEND_STEER_KI,
    gskd=SEND_STEER_KD,
    gwp=SEND_WHEEL_PERIMETER,
    gdc=SEND_DECAY,
    geppr=SEND_ENCODER_PPR,

    gp=SEND_POSE,
    gsr=SEND_SAMPLING_RATE,
    gdv=SEND_DEBUG_VAL,
    ur=UPDATE_REF,
    us=UPDATE_SETTINGS,
    lin=lin,
    ang=ang,
    help=cmd_help,
)

def execute_command(com, cmd, args):
    args = [float(arg) for arg in args]
    cmd_code = command_map.get(cmd)
    if isinstance(cmd_code, int):
        if (READ_WRITE_MSK & cmd_code):
            data = com.readall()
            if len(data) > 0:
                print("Các bytes thừa:", data)
        com.write(get_cmd_bytestring(cmd_code, *args))
        if (READ_WRITE_MSK & cmd_code):
            print("Đang nhận dữ liệu")
            if cmd_code == SEND_SAMPLING_RATE:
                sleep(1.0)
            try:
                if (~READ_WRITE_MSK & cmd_code) < READ_32_BIT_BEGIN:
                    data = com.read(2)
                    print("Đã nhận được:", unpack('h', data)[0]);
                else:
                    if (cmd_code == SEND_POSE):
                        data = com.read(20)
                        x, y, a = unpack('lli', data)
                        print(f"Định vị: x = {x/Q25_38(1.0)}, "
                              f"y = {y/Q25_38(1.0)}, "
                              f"a = {degrees(a/Q3_28(1.0))}")
                    else:
                        data = com.read(4)
                        print("Đã nhận được:", unpack('i', data)[0]);
            except Exception as e:
                print(f"Lỗi!, nhận được: {data}")
                raise e
    else:
        cmd_code(com, *args)
