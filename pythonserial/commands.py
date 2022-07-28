"""
Chứa các lệnh gửi xuống xe.
 ____________        ____________        ____________
|            |      |            |      |            |
|   CLI/GUI  |----->| xử lý lệnh |----->|    AGV     |
|____________|      |____________|      |____________|
"""

from struct import pack
from math import pi

PROTOCOL_STOP             = b'\x01'
PROTOCOL_UPDATE_REF       = b'\x02'
PROTOCOL_SET_DRIVE_REF    = b'\x03'
PROTOCOL_SET_DRIVE_KP     = b'\x04'
PROTOCOL_SET_DRIVE_KI     = b'\x05'
PROTOCOL_SET_DRIVE_KD     = b'\x06'
PROTOCOL_SET_STEER_REF    = b'\x07'
PROTOCOL_SET_STEER_KP     = b'\x08'
PROTOCOL_SET_STEER_KI     = b'\x09'
PROTOCOL_SET_STEER_KD     = b'\x0A'
PROTOCOL_SET_WHEEL_D      = b'\x0B'
PROTOCOL_SET_ENCODER_PPR  = b'\x0C'
PROTOCOL_GET_SAMPLE_RATE  = b'\x0D'
PROTOCOL_SET_DRIVE_DECAY  = b'\x0E'
PROTOCOL_SET_STEER_DECAY  = b'\x0F'

PROTOCOL_GET_POSE         = b'\x81'

PROTOCOL_PAD = b'\xFF\xFF\xFF\xFF'
IEEE_ZERO = b'\x00\x00\x00\x00'

def get_cmd_bytestring(cmd_code, value=0):
    cmd_bytestring = (PROTOCOL_PAD + cmd_code +
                      PROTOCOL_PAD + pack('f', value) +
                      PROTOCOL_PAD)
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
    stop=PROTOCOL_STOP,
    ur=PROTOCOL_UPDATE_REF,
    sdrf=PROTOCOL_SET_DRIVE_REF,
    sdkp=PROTOCOL_SET_DRIVE_KP,
    sdki=PROTOCOL_SET_DRIVE_KI,
    sdkd=PROTOCOL_SET_DRIVE_KD,
    ssrf=PROTOCOL_SET_STEER_REF,
    sskp=PROTOCOL_SET_STEER_KP,
    sski=PROTOCOL_SET_STEER_KI,
    sskd=PROTOCOL_SET_STEER_KD,
    swd=PROTOCOL_SET_WHEEL_D,
    seppr=PROTOCOL_SET_ENCODER_PPR,
    gsr=PROTOCOL_GET_SAMPLE_RATE,
    sddc=PROTOCOL_SET_DRIVE_DECAY,
    ssdc=PROTOCOL_SET_STEER_DECAY,
    gp=PROTOCOL_GET_POSE,
    lin=lin,
    ang=ang,
    help=cmd_help,
)

def execute_command(com, cmd, args):
    args = [float(arg) for arg in args]
    cmd_code = command_map.get(cmd)
    if isinstance(cmd_code, (bytes, bytearray)):
        com.write(get_cmd_bytestring(cmd_code, *args))
    else:
        cmd_code(com, *args)
