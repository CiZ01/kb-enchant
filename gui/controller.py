"""
This module is the controller.....
"""

import os
import fcntl
import ctypes as ct

# ---- IOCTL COMMANDS ----
KB_IOCTL_MAGIC = "K"
HID_CHANGE_MODE = ord("K") << 8 | 1
HID_GET_MODE = ord("K") << 8 | 2


# ------------------------

# ---- MODES ----
MODES = {
    "volume_mode": 0,
    "brightness_mode": 1,
    "scroll_mode": 2,
}

# ------------------------


class KBController:
    def __init__(self):
        self.device_file = None
        try:
            self.device_file = os.open("/dev/kb-ench", os.O_RDWR)
        except FileNotFoundError:
            print("Device file not found")
            exit(1)

    def set_mode(self, mode: str):
        try:
            new_mode = MODES[mode]
            fcntl.ioctl(self.device_file, HID_CHANGE_MODE, ct.c_int(new_mode))
            return True
        except Exception as e:
            print(e)
            return False

    def get_mode(self):
        pass

    def __del__(self):
        if self.device_file:
            os.close(self.device_file)
