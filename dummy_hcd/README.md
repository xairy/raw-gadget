Dummy HCD/UDC Kernel Module
===========================

Dummy HCD/UDC is a module that sets up virtual USB Device and Host controllers that are connected to each other inside the kernel.
This module allows connecting USB devices emulated from userspace through any of the Gadget subsystem interfaces (Raw Gadget, GadgetFS, etc.) directly to the underlying kernel.

This directory contains a copy of the Dummy HCD/UDC module source code patched to allow out-of-tree building.
Dummy HCD/UDC is not a part of Raw Gadget; its code is present in this repository just for convenience.


## Building and loading

1. Install the Linux kernel headers.

    On a desktop Ubuntu, you can get them by installing `` linux-headers-`uname -r` ``.

    On a Raspberry Pi, follow [these instructions](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel-headers).

2. Depending on the used kernel version, possibly patch the Raw Gadget module source code.

    Check the [updating script](./update.sh) to see the potentially required patches.

3. Build the module:

    ``` bash
    make
    ```

4. Load the module:

    ``` bash
    ./insmod.sh
    ```


## Updating

You can optionally update the Dummy HCD/UDC module source code to fetch the changes from the mainline Dummy HCD/UDC version:

``` bash
./update.sh
```

Note:
The updating script will revert the locally applied patches.
