#! /bin/bash

SYSD_NAME=kb-ench
GUI_NAME=kb-ench-gui
GUI_PATH=$1

echo $GUI_PATH
exit 0

# build kernel module
make -C ./kernel_module clean 
make -C ./kernel_module build

# build and install user space program
cargo install --path ./gui-rs

# gui path
if [ -z "$GUI_PATH" ]; then
    GUI_PATH=$(which $GUI_NAME)
    if [ -z "$GUI_PATH" ]; then
        echo "Error: $GUI_NAME not found in PATH"
        echo "Please specify the path to $GUI_NAME as first argument"
        exit 1
    fi
fi 

# add udev rules
sudo cp ./udev/99-usb-serial.rules /etc/udev/rules.d/


# install kernel module
make -C ./kernel_module load

# install udev rules
# cp ./udev/99-usb-serial.rules /etc/udev/rules.d/

# install systemd service
cp ./systemd/$SYSD_NAME.service /etc/systemd/system/
sudo systemctl daemon-reload
systemctl enable $SYSD_NAME.service
systemctl start $SYSD_NAME.service


