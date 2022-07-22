"""
Giao tiếp với xe qua giao diện CLI.
 ____________        ____________        ____________
|            |      |            |      |            |
|   CLI/GUI  |----->| xử lý lệnh |----->|    AGV     |
|____________|      |____________|      |____________|
"""

import _thread
from serial import Serial
from commands import execute_command

def uart_receive(com):
    """Dùng để chạy thread nhận dữ liệu."""
    while True:
        display_char(com)

def display_char(com, received=b''):
    """Hiển thị ký tự lên màn hình."""
    received += com.read()
    try:
        print(received.decode(), end='')
    except UnicodeDecodeError:
        display_char(com, received)

# Bắt đầu truyền thông
com = Serial('/dev/ttyACM0', baudrate=115200, timeout=0.1)

_thread.start_new_thread(uart_receive, (com,))

try:
    while True:
        line = input("")
        cmd, *args = line.split()
        execute_command(com, cmd.lower(), args)
except KeyboardInterrupt:
    pass

com.close()
