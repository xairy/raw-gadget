Raw Gadget
==========

__Note__: most likely you need GadgetFS, not Raw Gadget. See the differences [here](https://github.com/google/kasan/blob/usb-fuzzer/Documentation/usb/raw-gadget.rst#comparison-to-gadgetfs).

[USB Raw Gadget](https://github.com/google/kasan/blob/usb-fuzzer/Documentation/usb/raw-gadget.rst) is a kernel module that allows to emulate USB devices from userspace.
This repository contains instructions and examples for using Raw Gadget.

The module is currently [under review](https://patchwork.kernel.org/cover/11332295/) for upstream inclusion, so the interface it provides for the userspace might change.

See [dummy_hcd](/dummy_hcd) and [raw_gadget](/raw_gadget) for information on how to build and `insmod` corresponding kernel modules. Then you can try [examples](/examples).

Building kernel modules requires kernel headers.
On desktop Ubuntu you can get them by installing `` linux-headers-`uname -r` ``.
On Raspberry Pi Zero follow [these instructions](https://github.com/notro/rpi-source/wiki).
