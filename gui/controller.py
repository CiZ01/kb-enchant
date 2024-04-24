"""
This module is the controller.....
"""

import os
import fcntl
import ctypes as ct
from enum import Enum

# ---- IOCTL COMMANDS ----
HID_CHANGE_MODE = 0x40044B01
HID_GET_MODE = 0x80044B02


# ------------------------

# ---- MODES ----

MODES = ("volume_mode", "brightness_mode", "scroll_mode")


def getModeByName(mode: str):
    return MODES.index(mode)


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
            new_mode = getModeByName(mode)
            fcntl.ioctl(self.device_file, HID_CHANGE_MODE, ct.c_int(new_mode))
            return True
        except Exception as e:
            print(e)
            return False

    def get_mode(self):
        try:
            mode = ct.c_int()
            fcntl.ioctl(self.device_file, HID_GET_MODE, mode)
            return mode.value
        except Exception as e:
            print(e)
            return None

    def __del__(self):
        if self.device_file:
            os.close(self.device_file)
