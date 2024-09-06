#! /bin/bash

SYSD_NAME=kb-ench
GUI_NAME=kb-ench-gui
GUI_PATH=$1
KB_DRIVER_FOLDER_NAME=kb_driver
DRIVER_PATH=/lib/modules/$(uname -r)/extra

SRC_FOLDER_PATH=$(pwd)

echo "Load kernel module"

# if ! [[ -d $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME ]] then 
#    echo "Error: kernel module not found in $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME"
#    echo "Please build the kernel module first using ./build.sh"
#    exit 1
# fi

# cd $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME
# echo "Entering in $PWD folder" 
# make load || { echo "Error: failed to load kernel module"; exit 1; }

# echo "Kernel module loaded!"
# echo "Build and install user space program"

cd $SRC_FOLDER_PATH
# # build and install user space program
# cargo install --path ./gui-rs || { echo "Error: failed to build and install user space program"; exit 1; }

# sudo cp ./gui-rs/target/release/gui-rs /usr/bin/$GUI_NAME || { echo "Error: failed to copy user space program to /usr/bin"; exit 1; }


echo "Done!"

echo "Install systemd service"

# install systemd service
sudo cp ./service/$SYSD_NAME.service /etc/systemd/system/
systemctl daemon-reload
systemctl enable $SYSD_NAME.service
systemctl start $SYSD_NAME.service

# add module to /etc/modules for auto load
echo "Add module to /etc/modules.d for auto load"
cp ./service/kb-ench.conf /etc/modules-load.d/

# check systemd-modules-load.service
echo "Check systemd-modules-load.service"
output=$(systemctl status systemd-modules-load.service | grep "active (.*) ")
if [ -z "$output" ]; then
    echo "Error: systemd-modules-load.service is not running"
    systemctl start systemd-modules-load.service
    systemctl status systemd-modules-load.service | grep "active (.*)" || { echo "Error: failed to start systemd-modules-load.service"; exit 1; }
fi


echo "Done!"
echo "For a better experience, add a keybinding to start $GUI_PATH"
exit 0
