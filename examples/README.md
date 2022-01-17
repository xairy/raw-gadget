Raw Gadget Examples
===================

This is an example of emulating a USB keyboard or a USB printer with Raw Gadget.

When used as is it requires the [Dummy HCD/UDC](/dummy_hcd) module.
If you're willing to use it with a physical USB controller instead of Dummy UDC, you need to provide proper device and driver names as arguments (e.g. both are `20980000.usb` for Raspberry Pi Zero).

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
