Raw Gadget Examples
===================

This directory contains two examples of using Raw Gadget.
`keyboard.c` emulates a USB keyboard and `printer.c` emulates the enumeration stage of a USB printer.

When run as is, the examples rely on the [Dummy HCD/UDC](/dummy_hcd) module to emulate virtual devices that connect to the kernel they are running on.
If you want to use them with a physical UDC instead, you need to provide the proper device and driver names as arguments (e.g. both are `20980000.usb` for Raspberry Pi Zero).

Also see [tests/gadget.c](/tests/gadget.c) for an example of a device that is used with the Raw Gadget testing suite.

## Usage

Build:

``` bash
make
```

### For keyboard

Run:

``` bash
sudo ./keyboard
```

Or:

``` bash
sudo ./keyboard 20980000.usb 20980000.usb
```

### For printer

Run:

``` bash
sudo ./printer
```

Or:

``` bash
sudo ./printer 20980000.usb 20980000.usb
```
