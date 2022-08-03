"""
Giao tiếp với xe qua giao diện CLI.
 ____________        ____________        ____________
|            |      |            |      |            |
|   CLI/GUI  |----->| xử lý lệnh |----->|    AGV     |
|____________|      |____________|      |____________|
"""

import sys
import _thread
from serial import Serial
from commands import execute_command

def uart_receive(com):
    """Dùng để chạy thread nhận dữ liệu."""
    while True:
        try:
            display_char(com)
        except RecursionError:
            pass

def display_char(com, received=b''):
    """Hiển thị ký tự lên màn hình."""
    received += com.read()
    try:
        print(received.decode(), end='')
    except UnicodeDecodeError:
        display_char(com, received)

# Bắt đầu truyền thông
if len(sys.argv) > 1:
    device = sys.argv[1]
else:
    device = '/dev/ttyACM0'

# com = Serial(device, baudrate=115200, timeout=10.0)
com = Serial(device, baudrate=9600, timeout=10.0)

# com.timeout = 0.1
# _thread.start_new_thread(uart_receive, (com,))
# ready = True

import readline
print("Đang kết nối...")
ready_str = com.read_until('\r')
ready = b'AGV ready!' in ready_str
if ready:
    print("Kết nối đã sẵn sàng")
    com.timeout = 1.0
    try:
        while True:
            line = input(">>> ")
            cmd, *args = line.split()
            if cmd.lower() == "exit":
                break
            else:
                execute_command(com, cmd.lower(), args)
    except KeyboardInterrupt:
        pass
    except Exception as e:
        raise e
    com.close()
else:
    if len(ready_str) > 0:
        print("Không nhận dạng được hồi đáp:", ready_str)
    else:
        print("Không hồi đáp")
