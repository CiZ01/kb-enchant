#! /bin/bash

KB_DRIVER_FOLDER_NAME=kb_driver
DRIVER_PATH=/lib/modules/$(uname -r)/extra
SRC_DRIVER_PATH=./kernel-module

echo "Entering in $SRC_DRIVER_PATH folder"
cd $SRC_DRIVER_PATH
echo $PWD
make clean || { echo "Error: failed to clean kernel module"; exit 1; }
make build || { echo "Error: failed to build kernel module"; exit 1; }

echo "Copy builded kernel module in $DRIVER_PATH"

if ! [[ -d $DRIVER_PATH ]] then 
    echo "Error: $DRIVER_PATH not found"
    input=$(read -p "Do you want to create it? [y/N]: ")
    if [ $input == "y" ]; then
        sudo mkdir -p $DRIVER_PATH || { echo "Error: failed to create $DRIVER_PATH"; exit 1; }
    else
        echo "Please create $DRIVER_PATH first"
        exit 1
    fi
fi

# check if driver folder exists yet
if ! [[ -f $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME/kb-driver.ko ]] then
    sudo mkdir -p $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME || { echo "Error: failed to create $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME"; exit 1; }
    sudo cp -R ./* $DRIVER_PATH/$KB_DRIVER_FOLDER_NAME || { echo "Error: failed to copy kernel module"; exit 1; }
fi

echo "Done!"
echo "Run ./install.sh to complete the installation"
exit 0