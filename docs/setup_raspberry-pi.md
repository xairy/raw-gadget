Raw Gadget on Raspberry Pi
==========================

This document contains the end-to-end instructions on how to set up Raw Gadget on a Raspberry Pi board.
The instructions were tested on Raspberry Pi Zero, Raspberry Pi Zero W, Raspberry Pi 4 B, and Raspberry Pi 5, but they should be adaptable to other boards.


## Required hardware

- Raspberry Pi [1];
- SD card and SD card reader;
- Generic USB-UART adapter with jumper cables (or a [Pi-specific adapter](https://8086.net/products#80860010));
- USB Ethernet adapter (or Wi-Fi support on the Raspberry Pi);
- Host computer with a USB port that can provide enough power [1, 2];
- USB cable to connect the Raspberry Pi to the host.

[1] For dual-role hosts such as Android phones (that can act either as a host or as a device/peripheral), Raspberry Pi 5 (and possibly 4 B) might fail to act in the USB peripheral mode (likely due to the way the CC pins are wired in the USB connector).
Instead, use Raspberry Pi Zero.
But note that with some hosts (e.g. Pixel 8), the Raspberry Pi Zero will need to be powered externally, and the USB connection to the host will need to have its `VBUS` line cut off (e.g. via [PortaPow USB Power Blocker](https://portablepowersupplies.co.uk/product/usb-power-blocker)).

[2] Powering Raspberry Pi Zero from the host is usually not a problem.
But for power-hungry boards such as Raspberry Pi 4 B or 5, the USB port on your host must be able to provide enough power to the board.
For example, if your host has a USB PD 3.0–compatible port (Type-C ports on modern computers typically are), such port should work (it can provide provide 3A/5V according to the USB PD specification, which should be enough).
Powering the Raspberry Pi via an external power supply connected to the Type-C port will not work, as this port (at least on 4 B and 5) needs to be connected to the host to allow emulating USB devices for the host.
Powering the Raspberry Pi via a powered USB hub might work.


## Instructions

1. [Download](https://www.raspberrypi.com/software/operating-systems/) the latest `Raspberry Pi OS with desktop` image for your board.

    Note that some Raspberry Pi Zero boards only support the 32-bit version.

    The `Lite` version will likely work as well;

2. Extract the downloaded image and flash it into an SD card:

    ``` bash
    unxz 2025-05-13-raspios-bookworm-arm64.img.xz
    sudo dd if=2025-05-13-raspios-bookworm-arm64.img of=/dev/sdb bs=4M conv=fsync
    ```

3. [Create](https://forums.raspberrypi.com/viewtopic.php?t=333248&p=1994926#p1994926) a user `pi` with the password `raspberry` by creating a `bootfs/userconf.txt` file with the following contents:

   ```
   pi:$6$c70VpvPsVNCG0YR5$l5vWWLsLko9Kj65gcQ8qvMkuOoRkEagI90qi3F/Y7rm8eNYZHW8CY6BOIKwMH7a3YYzZYL90zf304cAHLFaZE0
   ```

4. [Enable UART](https://www.raspberrypi.com/documentation/computers/config_txt.html#enable_uart) by appending `enable_uart=1` to the end of the `bootfs/config.txt` file;

5. Boot the board and get a shell over UART.

    For this step, you will need a USB-UART adapter with jumper cables (or a [Pi-specific adapter](https://8086.net/products#80860010)).

    [Here](https://learn.adafruit.com/raspberry-pi-zero-creation/give-it-life) are the instructions for Raspberry Pi Zero.

    Note that getting the login prompt over UART might take a few minutes when you boot the board for the first time.

    Getting a shell over SSH is also fine as long as the USB device port on the board remains available.
    Thus, on Raspberry Pi Zero (without Wi-Fi), you must get a shell over UART, as the USB Ethernet cable would take up the USB device port;

6. Get the board connected to the Internet by plugging in a USB Ethernet adapter or [use Wi-Fi](https://www.raspberrypi.com/documentation/computers/configuration.html#connect-to-a-wireless-network-2) on a Wi-Fi–enabled board;

7. Update the packages: `sudo apt update && sudo apt full-upgrade && sudo reboot`;

8. Install useful packages: `sudo apt install vim git`;

9. Install the Linux kernel headers by following [these instructions](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel-headers);

10. Setup the dwc2 UDC driver:

    ``` bash
    echo "dtoverlay=dwc2" | sudo tee -a /boot/firmware/config.txt
    echo "dwc2" | sudo tee -a /etc/modules
    sudo reboot
    ```

11. Clone the Raw Gadget [repository](https://github.com/xairy/raw-gadget);

12. Build and load the USB Raw Gadget module following [these instructions](/raw_gadget);

13. Connect the Raspberry Pi USB device port to a USB host.

    On Raspberry Pi Zero, this port is titled `USB` on the board.
    You will need to unplug the USB Ethernet adapter for this step if you have it plugged in.

    On Raspberry Pi 4 B and 5, the device port is the only Type-C port on the board (titled `POWER IN` on 4 B).
    It can be used for both powering the board and emulating USB devices at the same time.
    You might need to power off the board to reconnect it to the USB host;

14. Build and run the Raw Gadget [keyboard emulator program](/examples):

    ``` bash
    cd raw-gadget/examples
    make
    sudo ./keyboard 1000480000.usb 1000480000.usb
    ```

    You might need to change the [UDC device and driver names](/README.md#usb-device-controllers);

15. Make sure that you see the letter `X` being entered on the host.

    With this step, you confirmed that Raw Gadget is working.


## Additional instructions

To turn Raspberry Pi Zero W into a drive-by USB attack tool:

1. Set up a Wi-Fi hotspot;

2. Enable SSH server;

3. Solder [Zero Stem](https://zerostem.io/) onto the board.

You can now connect the board to any USB port, wait for it to boot, join its Wi-Fi network, `ssh` onto it, and emulate arbitrary USB devices via Raw Gadget or other Linux USB Gadget interfaces.
