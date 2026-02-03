## Webcam-device-driver for linux systeam

Webcam device driver build and tested on 6.14.0-37-generic kernel version.User need to specify vendor ID and product ID of device into webcam_table.
Unbind old driver from system and add this driver for better result.

### To unbind old driver
Find your device's bus ID (e.g., 1-4:1.0) by looking at dmesg or /sys/bus/usb/devices/.

echo "1-4:1.0" | sudo tee /sys/bus/usb/drivers/uvcvideo/unbind

### bind original driver
echo "1-4:1.0" | sudo tee /sys/bus/usb/drivers/uvcvideo/bind

### How to compile ?

### Requirements
```
sudo apt get make
sudo apt-get install linux-headers-$(uname -r)
sudo apt install build-essential
sudo apt install gcc-13
sudo apt install g++-13
```
### 1. Build using make
make

### 2. insert kernel object file into systeam
sudo insmod webcam_driver.ko

### 3. check dmesg for kernel logs
sudo dmesg

### 4. for removing module use following command
sudo rmmod webcam_driver

### 5. check which usb driver is being used
lsusb -t

