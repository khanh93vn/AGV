"""
Chứa các lệnh gửi xuống xe.
 ____________        ____________        ____________
|            |      |            |      |            |
|   CLI/GUI  |----->| xử lý lệnh |----->|    AGV     |
|____________|      |____________|      |____________|
"""

from struct import pack, unpack
from math import pi

# Các mã lệnh set giá trị biến
SET_DRIVE_REF         = b'\x01'  # tham chiếu bánh dẫn động
SET_STEER_REF         = b'\x02'  # tham chiếu góc lái
SET_DRIVE_KP          = b'\x03'  # kp bánh dẫn động
SET_DRIVE_KI          = b'\x04'  # ki bánh dẫn động
SET_DRIVE_KD          = b'\x05'  # kd bánh dẫn động
SET_STEER_KP          = b'\x06'  # kp góc lái
SET_STEER_KI          = b'\x07'  # ki góc lái
SET_STEER_KD          = b'\x08'  # kd góc lái
SET_ENCODER_PPR       = b'\x09'  # số xung/vòng của encoder
SET_WHEEL_PERIMETER   = b'\x0A'  # chu vi vòng lăn bánh dẫn động
SET_DECAY             = b'\x0B'  # hệ số tắt dần (PID)

# Các mã lệnh yêu cầu gửi giá trị biến
SEND_DRIVE_REF        = b'\x81'  # tham chiếu bánh dẫn động
SEND_STEER_REF        = b'\x82'  # tham chiếu góc lái
SEND_DRIVE_KP         = b'\x83'  # kp bánh dẫn động
SEND_DRIVE_KI         = b'\x84'  # ki bánh dẫn động
SEND_DRIVE_KD         = b'\x85'  # kd bánh dẫn động
SEND_STEER_KP         = b'\x86'  # kp góc lái
SEND_STEER_KI         = b'\x87'  # ki góc lái
SEND_STEER_KD         = b'\x88'  # kd góc lái
SEND_ENCODER_PPR      = b'\x89'  # số xung/vòng của encoder
SEND_WHEEL_PERIMETER  = b'\x8A'  # chu vi vòng lăn bánh dẫn động
SEND_DECAY            = b'\x8B'  # hệ số tắt dần (PID)

# Các mã lệnh yêu cầu đặc biệt
SEND_POSE             = b'\xFF'  # gửi dữ liệu tự định vị
SEND_SAMPLING_RATE    = b'\xFE'  # Test và gửi tần số lấy mẫu
UPDATE_REF            = b'\x7F'  # cập nhật tham chiếu
SET_WHEEL_DIAMETER    = b'\x7E'  # đường kính bánh xe

READ_WRITE_MSK          = b'\x80'
PACKET_PAD              = b'\xFF\xFF\xFF\xFF'

def get_cmd_bytestring(cmd_code, value=0):
    cmd_bytestring = (PACKET_PAD + cmd_code +
                      PACKET_PAD + pack('f', value) +
                      PACKET_PAD)
    return cmd_bytestring

def lin(com, pos):
    pos_rad = pos*pi/180
    com.write(get_cmd_bytestring(PROTOCOL_SET_DRIVE_REF, pos_rad))
    com.write(get_cmd_bytestring(PROTOCOL_UPDATE_REF))

def ang(com, heading):
    heading_rad = heading*pi/180
    com.write(get_cmd_bytestring(PROTOCOL_SET_STEER_REF, heading_rad))
    com.write(get_cmd_bytestring(PROTOCOL_UPDATE_REF))

def cmd_help(com):
    print("Danh sách các lệnh:")
    print('\n'.join(command_map.keys()))

command_map = dict(
    # ur=UPDATE_REF,
    # sdrf=SET_DRIVE_REF,
    # sdkp=SET_DRIVE_KP,
    # sdki=SET_DRIVE_KI,
    # sdkd=SET_DRIVE_KD,
    # ssrf=SET_STEER_REF,
    # sskp=SET_STEER_KP,
    # sski=SET_STEER_KI,
    # sskd=SET_STEER_KD,
    # swp=SET_WHEEL_PERIMETER,
    # seppr=SET_ENCODER_PPR,
    gsr=SEND_SAMPLING_RATE,
    # sdc=SET_STEER_DECAY,
    # gp=GET_POSE,
    lin=lin,
    ang=ang,
    help=cmd_help,
)

def execute_command(com, cmd, args):
    args = [float(arg) for arg in args]
    cmd_code = command_map.get(cmd)
    if isinstance(cmd_code, (bytes, bytearray)):
        com.write(get_cmd_bytestring(cmd_code, *args))
        if (READ_WRITE_MSK[0] & cmd_code[0]):
            try:
                data = b''
                while len(data) < 4:
                    data += com.read()
                print("Received:", unpack('>i', data));
            except:
                print("Error, data received:", data)
    else:
        cmd_code(com, *args)
