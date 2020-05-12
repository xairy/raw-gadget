Raw Gadget
==========

__Note__: most likely you need GadgetFS, not Raw Gadget. See the differences [here](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst).

[USB Raw Gadget](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst) is a kernel module that allows to emulate USB devices from userspace.
This repository contains instructions and examples for using Raw Gadget.

The module has been [merged](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2c2e717642c66f7fe7e5dd69b2e8ff5849f4d10) into mainline in `5.7-rc1`. There can still be ABI breaking changes, as the module is not included in any kernel release yet.

See [dummy_hcd](/dummy_hcd) and [raw_gadget](/raw_gadget) for information on how to build and `insmod` corresponding kernel modules. Then you can try [examples](/examples).

Building kernel modules requires kernel headers.
On desktop Ubuntu you can get them by installing `` linux-headers-`uname -r` ``.
On Raspberry Pi Zero follow [these instructions](https://github.com/notro/rpi-source/wiki).

## USB Device Controllers

The name of the UDC device can be found in `/sys/class/udc/`:

``` bash
$ ls /sys/class/udc/
dummy_udc.0
```

The name of the UDC driver is usually present in `/sys/class/udc/$UDC/uevent`:

``` bash
$ cat /sys/class/udc/dummy_udc.0/uevent
USB_UDC_NAME=dummy_udc
```

"Works" in the table below means that the UDC passes the provided [tests](/tests), which only cover a subset of functionality and therefore have [limitations](/tests#todo).

| Hardware | Kernel | Driver | Device | Works? |
| :---: | :---: | :---: | :---: | :---: |
| | `5.3.0-45-generic` | `dummy_udc` | `dummy_udc.0` | [Yes](/tests#dummy-udc) |
| Raspberry Pi Zero | `4.14.97+` | `20980000.usb` | `20980000.usb` (`dwc2`) | [Yes](/tests#raspberry-pi-zero) |
| BeagleBone Black | `4.19.94-ti-r42` | `musb-hdrc` | `musb-hdrc.0` | Probably |
| BeagleBone AI | `4.14.108-ti-r131` | `48890000.usb` | `dwc3-gadget` | Not yet |
| [EC3380-AB](http://www.hwtools.net/Adapter/EC3380-AB.html) | `5.3.0-45-generic` | `net2280` | `0000:04:00.0` (e.g.) | No, `net2280` buggy |
| Odroid C2 | `3.14.79-116` | `dwc_otg_pcd` | `dwc2_a` | No, kernel too old |
| HiKey 960 | | | | ? |
