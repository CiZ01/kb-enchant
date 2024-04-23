"""
This module is the controller.....
"""

import os
import fcntl


# Define IOCTL command
HID_CHANGE_MODE = ord("H") << 8 | 0

# Open device file
device_file = os.open("/dev/mykb_cdev0", os.O_RDWR)

# Send custom IOCTL command
ioctl_data = bytearray([1, 2, 3, 4])  # Example data to send
fcntl.ioctl(device_file, HID_CHANGE_MODE, ioctl_data)

# Close device file
os.close(device_file)
