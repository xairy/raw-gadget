Raw Gadget
==========

__Note__: most likely you need GadgetFS, not Raw Gadget. See the differences [here](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst).

[USB Raw Gadget](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst) is a low-level interface for the Linux USB Gadget subsystem.
It can be used to emulate physical USB devices with [special hardware](/README.md#usb-device-controllers), or virtual ones (for the kernel it's running on) with [Dummy HCD/UDC](/dummy_hcd).
This repository contains instructions and [examples](/examples) for using Raw Gadget.

Raw Gadget has been [merged](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2c2e717642c66f7fe7e5dd69b2e8ff5849f4d10) into mainline Linux kernel in `5.7`.
There's no need to use `5.7+` kernels, see [dummy_hcd](/dummy_hcd) and [raw_gadget](/raw_gadget) for information on how to build and `insmod` corresponding modules on older kernels.
The modules should be compatible with kernel versions down to `4.14`, see the table below.

Building kernel modules requires kernel headers.
On desktop Ubuntu you can get them by installing `` linux-headers-`uname -r` ``.
On Raspberry Pi Zero follow [these instructions](https://github.com/notro/rpi-source/wiki).

## USB Device Controllers

USB Raw Gadget requires the user to provide UDC device and driver names, see [examples](/examples).

UDC device name can be found in `/sys/class/udc/`:

``` bash
$ ls /sys/class/udc/
dummy_udc.0
```

UDC driver name is usually present in `/sys/class/udc/$UDC/uevent`:

``` bash
$ cat /sys/class/udc/dummy_udc.0/uevent
USB_UDC_NAME=dummy_udc
```

"Works" in the table below means that the UDC passes the provided [tests](/tests), which only cover a subset of functionality and therefore have [limitations](/tests#todo).

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

## Projects based on Raw Gadget

* [syzkaller](https://github.com/google/syzkaller) — a kernel fuzzer, uses Raw Gadget for fuzzing Linux kernel [USB drivers](https://github.com/google/syzkaller/blob/master/docs/linux/external_fuzzing_usb.md).
* [usb-proxy](https://github.com/AristoChen/usb-proxy) — A USB proxy based on Raw Gadget and libusb.

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

The parts of code in this repository that are derived from the Linux kernel are covered by GPL-2.0. Everything else is currently covered by Apache-2.0. `SPDX-License-Identifier` marks the used license in each file.
