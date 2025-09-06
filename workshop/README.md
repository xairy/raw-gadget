Attacking USB with Raw Gadget
=============================

This is a workshop that serves as an introduction to the USB hacking topic in general and to [Raw Gadget](https://github.com/xairy/raw-gadget) specifically.

The workshop gives an overview of the USB protocol, the Linux USB Gadget subsystem, and the Raw Gadget interface.
The offered hands-on exercises include capturing and analyzing the communication of USB devices, emulating USB devices via Raw Gadget, and proxying USB devices via Raw Gadget and libusb to aid reverse engineering.

Note that you must bring your own [hardware](#required-hardware) to attend this workshop at [BalCCon2k25](https://2k25.balccon.org/).


## Required hardware

- Raspberry Pi 5 (or 4 B, but note [1]);
- SD card and SD card reader;
- USB-UART adapter with jumper cables (for getting a shell on the Raspberry Pi);
- Laptop with Linux and a USB PD 3.0–compatible port (Type-C ports on modern laptops typically are) [2, 3];
- Type-C-to-your-laptop-port USB cable;
- USB mouse (prefer simplest mice that require no special drivers);
- Optionally, other USB devices (if you want to try proxying them).

[1]: Technically, any Linux-based board with a UDC (USB Device Controller) and a separate USB host port running the Linux kernel version 4.14+ might work.
However, to avoid discrepancies in the setup, please use Raspberry Pi 5 — the instructions are only provided for this board.
Raspberry Pi 4 B might work, but only if the USB port on your laptop can provide enough power for the board [2] (note that Raspberry Pi 4 B does not support USB PD).

[2]: The USB port on your laptop must provide enough power to power the Raspberry Pi board and the mouse (or other devices) connected to the board.
For example, a laptop with a USB PD 3.0–compatible port should work (such port can provide provide 3A/5V according to the USB PD specification, which should be enough to power the board).
If your laptop does not have such a port, please check the specification of the USB ports on your laptop and test powering the Raspberry Pi.
Powering the Raspberry Pi via an external power supply will not work, as the Type-C port on the Raspberry Pi will need to be connected to the laptop to allow emulating USB devices for the laptop.
Powering the Rapspberry Pi via a powered USB hub might work, but [test the setup](#pre-setup-instructions) in advance and make sure that Raw Gadget works.

[3]: Windows or Mac might work, but the instructions for these OSes are not provided.
If you bring a laptop without Linux, you must figure out how to capture USB communication on it.


## Pre-setup instructions

It's recommended to follow through these instructions before the workshop starts to confirm that your setup is working properly.

1. Follow through the [instructions](/docs/setup_raspberry-pi.md) for setting up Raw Gadget on Raspberry Pi;

2. Install Wireshark on your laptop and check that you can [capture USB communication](https://wiki.wireshark.org/CaptureSetup/USB#linux) on the laptop (choose the `usbmon0` interface after starting Wireshark).


## Workshop instructions

To be provided later.
