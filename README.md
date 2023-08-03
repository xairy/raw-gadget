Raw Gadget
==========

__Note__: Raw Gadget is a debugging feature, and it should not be used in production. Use GadgetFS instead. See the differences [here](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst).

[Raw Gadget](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst) is a Linux kernel module that implements a low-level interface for the Linux USB Gadget subsystem.

Raw Gadget can be used to emulate USB devices, both physical and virtual ones.
Emulating physical devices requires a Linux board with a [USB Device Controller](/README.md#usb-device-controllers) (UDC), such as a Raspberry Pi.
Emulating virtual devices requires no hardware and instead relies on the [Dummy HCD/UDC](/dummy_hcd) module (such devices get connected to the kernel Raw Gadget is running on).

This repository contains instructions and [examples](/examples) for using Raw Gadget.

Raw Gadget has been [merged](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2c2e717642c66f7fe7e5dd69b2e8ff5849f4d10) into the mainline Linux kernel in `5.7`.
There's no need to use `5.7+` kernels; see [raw_gadget](/raw_gadget) and [dummy_hcd](/dummy_hcd) for information on how to build and `insmod` corresponding modules on older kernels.
The modules should be compatible with kernel versions down to `4.14`; see the table below.

Building the Raw Gadget and Dummy HCD/UDC kernel modules requires kernel headers.
On desktop Ubuntu, you can get them by installing `` linux-headers-`uname -r` ``.
On a Raspberry Pi, follow [these instructions](https://github.com/notro/rpi-source/wiki).

See the [Fuzzing USB with Raw Gadget](https://docs.google.com/presentation/d/1sArf2cN5tAOaovlaL3KBPNDjYOk8P6tRrzfkclsbO_c/edit?usp=sharing) talk [[video](https://www.youtube.com/watch?v=AT3PQjKxa_c)] for details about Linux Host and Gadget USB subsystems and Raw Gadget.


## USB Device Controllers

Raw Gadget requires the user to provide the UDC device and driver names.
This allows using Raw Gadget with a particular UDC if a few of them are present on the system.

UDC device names can be found in `/sys/class/udc/`:

``` bash
$ ls /sys/class/udc/
dummy_udc.0
```

The UDC driver name is usually present in `/sys/class/udc/$UDC/uevent`:

``` bash
$ cat /sys/class/udc/dummy_udc.0/uevent
USB_UDC_NAME=dummy_udc
```

Below is a table of UDCs that were tested with Raw Gadget.

| Hardware | Kernel | Driver | Device | Works? |
| :---: | :---: | :---: | :---: | :---: |
| | `5.3.0-45-generic` | `dummy_udc` | `dummy_udc.0` | [Yes](/tests#dummy-udc) |
| Raspberry Pi Zero | `4.14.97+` | `20980000.usb` | `20980000.usb` (`dwc2`) | [Yes](/tests#raspberry-pi-zero) |
| Raspberry Pi 4 | `5.10.63-v7l+` | `fe980000.usb` | `fe980000.usb` (`dwc2`) | [Yes](/tests#raspberry-pi-4) |
| USB Armory MkII | `5.4.87-0` | `2184000.usb` | `ci_hdrc.0` | [Yes](/tests#usb-armory-mkii) |
| Orange Pi PC | `5.10.60` | `musb-hdrc` | `musb-hdrc.4.auto` | [Yes](/tests#orange-pi-pc) |
| Orange Pi PC 2 | `5.10.60` | `musb-hdrc` | `musb-hdrc.4.auto` | [Yes](/tests#orange-pi-pc-2) |
| Khadas VIM1 | `5.10.60-meson64` | `c9100000.usb` | `c9100000.usb` | [Yes](/tests#khadas-vim1) |
| BeagleBone Black | `4.19.94-ti-r42` | `musb-hdrc` | `musb-hdrc.0` | Probably |
| BeagleBone AI | `4.14.108-ti-r131` | `48890000.usb` | `dwc3-gadget` | Not yet |
| [EC3380-AB](http://www.hwtools.net/Adapter/EC3380-AB.html) | `5.3.0-45-generic` | `net2280` | `0000:04:00.0` (e.g.) | Partially,<br />`net2280` buggy |
| Odroid C2 | `3.14.79-116` | `dwc_otg_pcd` | `dwc2_a` | No, kernel too old |

"Works" in the table above means that the UDC passes the provided [tests](/tests), which only cover a [subset of functionality](/tests#todo).


## Facedancer backend

There's a [prototype](https://github.com/xairy/Facedancer/tree/rawgadget) of a Facedancer backend based on Raw Gadget.

This backend relies on a few out-of-tree Raw Gadget patches present in the [dev branch](https://github.com/xairy/raw-gadget/tree/dev).
Once the backend is thoroughly tested, these patches will be submitted to the mainline.

Raw Gadget-based backend accepts a few parameters through environment variables:

| Parameter | Description | Default value |
| :---: | :---: | :---: |
| `RG_UDC_DRIVER` | UDC driver name | `dummy_udc` |
| `RG_UDC_DEVICE` | UDC device name | `dummy_udc.0` |
| `RG_USB_SPEED` | USB device speed | `3` (High Speed) |

Example of using Facedancer with Raw Gadget to emulate a USB keyboard on a Raspberry Pi 4:

``` bash
export BACKEND=rawgadget
export RG_UDC_DRIVER=fe980000.usb
export RG_UDC_DEVICE=fe980000.usb
./legacy-applets/facedancer-keyboard.py
```

Note: Some Facedancer examples might fail if a wrong USB speed is specified.
Failures happen either with `EINVAL` in `USB_RAW_IOCTL_EP_ENABLE`, with `ESHUTDOWN` in `USB_RAW_IOCTL_EP_READ/WRITE`, or can be completely random.
For example, with Dummy UDC, `examples/ftdi-echo.py` requires `RG_USB_SPEED=2` and `legacy-applets/facedancer-ftdi.py` requires `RG_USB_SPEED=3`.
In turn, `legacy-applets/facedancer-umass.py` requires `RG_USB_SPEED=2`.

Note: This backend is still a prototype.
Outstanding tasks:

1. Make sure that all required backend callbacks are implemented. For example, `read_from_endpoint` should probably be implemented.
2. Provide a common [Python wrapper](#1) for Raw Gadget ioctls, and use it in the backend.
3. Finalize and submit out-of-tree Raw Gadget patches to the mainline.

Note: Facedancer assumes that every backend supports non-blocking I/O, which is not the case for Raw Gadget.
To work around this limitation, the backend prototype relies on timeouts.
The proper solution to this issue would be to add non-blocking I/O support to Raw Gadget.


## Troubleshooting

As generic guidance to troubleshooting Raw Gadget errors:

1. Switch to the [dev branch](https://github.com/xairy/raw-gadget/tree/dev).

    This branch contains fixes for some known issues and prints more debug output.

2. Enable debug output for Raw Gadget (and Dummy HCD/UDC if you're using it).

    Add the following line to the very beginning of `raw_gadget/raw_gadget.c`:

    ``` c
    #define DEBUG
    ```

    Rebuild and reinsert the module.

3. Check the kernel log via `dmesg` to figure out what is failing.


### No such device

`USB_RAW_IOCTL_RUN` returns `ENODEV`, error code `19`:

```
ioctl(USB_RAW_IOCTL_RUN): No such device
```

This error means that bad UDC driver/device names were provided.
Make sure that the UDC driver module is loaded.
Also see [USB Device Controllers](#usb-device-controllers) about UDC names.


### Cannot send after transport endpoint shutdown

Endpoint operations return `ESHUTDOWN`, error code `108`:

```
ioctl(USB_RAW_IOCTL_EP0_WRITE): Cannot send after transport endpoint shutdown
```

This error likely means that the emulated USB device did something wrong.
For example, tried to perform an endpoint operation before the device is configured.
Or provided an endpoint descriptor that does not match the USB device speed.
As a result, either the UDC driver or the host decided to disconnect the device.

Note: During device operation, the host might decide to reconfigure the device.
The UDC driver will then issue a reset or a disconnect event (depends on which UDC driver is in use).
After this, endpoint operations will fail with `ESHUTDOWN` until the device emulation code calls `USB_RAW_IOCTL_CONFIGURE` again when handling a new `SET_CONFIGURATION` request.
Getting notifications about the reset and disconnect events requires using the Raw Gadget patches from the [dev branch](https://github.com/xairy/raw-gadget/tree/dev).


## Projects based on Raw Gadget

* [google/syzkaller](https://github.com/google/syzkaller) — a kernel fuzzer, uses Raw Gadget for fuzzing Linux kernel [USB drivers](https://github.com/google/syzkaller/blob/master/docs/linux/external_fuzzing_usb.md).
* [AristoChen/usb-proxy](https://github.com/AristoChen/usb-proxy) — A USB proxy based on Raw Gadget and libusb.
* [blegas78/usb-sniffify](https://github.com/blegas78/usb-sniffify) — Another USB proxy based on Raw Gadget and libusb.
* [patryk4815/usb-proxy](https://github.com/patryk4815/usb-proxy) — A USB proxy based on Raw Gadget and written in Go.


## TODO

* [TODOs in kernel documentation](https://elixir.bootlin.com/linux/v5.7/source/Documentation/usb/raw-gadget.rst#L74)
* [TODOs in Raw Gadget test suite](/tests#todo)
* [GitHub issues](https://github.com/xairy/raw-gadget/issues)

Other potential fixes/improvements to investigate:

* Set `ep->maxburst`, `ep->mult` and `ep->maxpacket` in gadget drivers.
* OTG support.
* Set `ep->dev` on `ep` allocation.
* Don't pass `ep0_status` and `ep_status` through `dev`, get from `req` instead.


## License

The parts of code in this repository that are derived from the Linux kernel are covered by GPL-2.0. Everything else is covered by Apache-2.0. `SPDX-License-Identifier` marks the used license in each file.
