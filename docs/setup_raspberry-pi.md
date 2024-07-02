Raw Gadget on Raspberry Pi
==========================

This document contains end-to-end instructions on how to set up Raw Gadget on a Raspberri Pi board.
The instructions were tested on Raspberry Pi Zero W and Raspberry Pi 4 B, but they should be easily adaptable to other boards.


## Required hardware

- Raspberry Pi board;
- SD card;
- USB UART adapter with jumper cables (or a [Pi-specific adapter](https://8086.net/products#80860010));
- USB Ethernet adapter (or Wi-Fi support on the board);
- USB cables.


## Instructions

1. [Download](https://www.raspberrypi.com/software/operating-systems/) the latest `Raspberry Pi OS with desktop` image;

2. Extract the downloaded image and flash it into an SD card:

    ``` bash
    unxz 2024-03-15-raspios-bookworm-armhf.img.xz
    sudo dd if=2024-03-15-raspios-bookworm-armhf.img of=/dev/sdb bs=4M conv=fsync
    ```

3. [Create](https://forums.raspberrypi.com/viewtopic.php?t=333248&p=1994926#p1994926) a user `pi` with the password `raspberry` by creating a `bootfs/userconf.txt` file with the following contents:

   ```
   pi:$6$c70VpvPsVNCG0YR5$l5vWWLsLko9Kj65gcQ8qvMkuOoRkEagI90qi3F/Y7rm8eNYZHW8CY6BOIKwMH7a3YYzZYL90zf304cAHLFaZE0
   ```

4. [Enable UART](https://www.raspberrypi.com/documentation/computers/config_txt.html#enable_uart) by appending `enable_uart=1` to the end of the `bootfs/config.txt` file;

5. Boot the board and get a shell over UART.

    [Here](https://learn.adafruit.com/raspberry-pi-zero-creation/give-it-life) are the instructions for Raspberry Pi Zero.

    For this step, you will need a USB-UART adapter with jumper cables.

    Note that getting the login prompt over UART takes a few minutes when you boot the board for the first time;

6. Get the board connected to the internet by plugging in a USB Ethernet adapter (or [use Wi-Fi](https://www.raspberrypi.com/documentation/computers/configuration.html#connect-to-a-wireless-network-2));

7. Update the packages: `sudo apt-get update && sudo apt-get dist-upgrade && sudo rpi-update && sudo reboot`;

8. Install useful packages: `sudo apt-get install vim git`;

9. [Install](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel-headers) Linux kernel headers:

   ``` bash
   sudo apt install linux-headers-rpi-{v6,v7,v7l}
   ```

10. Setup the dwc2 UDC driver:

    ``` bash
    echo "dtoverlay=dwc2" | sudo tee -a /boot/firmware/config.txt
    echo "dwc2" | sudo tee -a /etc/modules
    sudo reboot
    ```

11. Clone the Raw Gadget [repository](https://github.com/xairy/raw-gadget);

12. Build and load the USB Raw Gadget module following [these instructions](/raw_gadget);

13. Connect the Raspberry Pi USB OTG connector to a USB host.

    On Raspberry Pi Zero W, the connector is titled `USB` on the board.
    You will need to unplug the USB Ethernet adapter for this step if you have it plugged in.

    On Raspberry Pi 4 B, the OTG connector is the Type-C one titled `POWER IN`.
    You might need to power off the board to reconnect it to the USB host;

14. Build and run the Raw Gadget [keyboard emulator program](/examples):

    ``` bash
    cd raw-gadget/examples
    make
    sudo ./keyboard 20980000.usb 20980000.usb
    ```

    You might need to change the [UDC device and driver names](/README.md#usb-device-controllers);

15. Make sure that you see the letter `x` being entered on the host.

    With this step, you confirmed that Raw Gadget is working properly.


## Additional instructions

To turn a Raspberry Pi Zero W into a drive-by USB attack tool:

1. Set up a Wi-Fi hotspot;

2. Enable SSH server;

3. Solder [Zero Stem](https://zerostem.io/) onto the board.

You can now connect the board to any USB port, wait for it to boot, join its Wi-Fi network, `ssh` onto it, and emulate arbitrary USB devices via Raw Gadget or other Linux USB Gadget interfaces.
