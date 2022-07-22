"""
Chứa các lệnh gửi xuống xe.
 ____________        ____________        ____________
|            |      |            |      |            |
|   CLI/GUI  |----->| xử lý lệnh |----->|    AGV     |
|____________|      |____________|      |____________|
"""

from struct import pack

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

PROTOCOL_PAD = b'\xFF\xFF\xFF\xFF'
IEEE_ZERO = b'\x00\x00\x00\x00'

def get_cmd_bytestring(cmd_code, value=IEEE_ZERO):
    cmd_bytestring = (PROTOCOL_PAD + cmd_code +
                      PROTOCOL_PAD + value + PROTOCOL_PAD)
    return cmd_bytestring

def lin(com, spd):
    com.write(get_cmd_bytestring(PROTOCOL_SET_DRIVE_REF, spd))
    com.write(get_cmd_bytestring(PROTOCOL_UPDATE_REF))

command_map = dict(
    stop=PROTOCOL_STOP,
    update_ref=PROTOCOL_UPDATE_REF,
    set_drive_ref=PROTOCOL_SET_DRIVE_REF,
    set_drive_kp=PROTOCOL_SET_DRIVE_KP,
    set_drive_ki=PROTOCOL_SET_DRIVE_KI,
    set_drive_kd=PROTOCOL_SET_DRIVE_KD,
    set_steer_ref=PROTOCOL_SET_STEER_REF,
    set_steer_kp=PROTOCOL_SET_STEER_KP,
    set_steer_ki=PROTOCOL_SET_STEER_KI,
    set_steer_kd=PROTOCOL_SET_STEER_KD,
    set_wheel_d=PROTOCOL_SET_WHEEL_D,
    set_encoder_ppr=PROTOCOL_SET_ENCODER_PPR,
    get_sample_rate=PROTOCOL_GET_SAMPLE_RATE,
    lin=lin,
)

def execute_command(com, cmd, args):
    args = [pack('f', float(arg)) for arg in args]
    cmd_code = command_map.get(cmd)
    if isinstance(cmd_code, (bytes, bytearray)):
        com.write(get_cmd_bytestring(cmd_code, *args))
    else:
        cmd_code(com, *args)
