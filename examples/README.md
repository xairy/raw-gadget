Raw Gadget Examples
===================

This is an example of emulating a USB device with Raw Gadget.
When used as is it requires the [Dummy HCD/UDC](/dummy_hcd) module.
If you're willing to use it with a physical USB controller, you need to patch `driver_name` and `device_name` to the values the controller uses (e.g. both are `20980000.usb` for Raspberry Pi Zero, see [the patch](./rpi_zero.patch)).

## Usage

Build:

``` bash
make
```

Run:

``` bash
sudo ./keyboard
```
