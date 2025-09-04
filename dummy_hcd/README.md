Dummy HCD/UDC Kernel Module
===========================

Dummy HCD/UDC is a module that sets up virtual USB Device and Host controllers that are connected to each other inside the kernel.
This module allows connecting USB devices emulated from userspace through any of the Gadget subsystem interfaces (Raw Gadget, GadgetFS, etc.) directly to the underlying kernel.

This directory contains a copy of the Dummy HCD/UDC module source code (from the Linux kernel version 6.17).

Dummy HCD/UDC is not a part of Raw Gadget; its code is present in this repository just for convenience.


## Building and loading

1. Install the Linux kernel headers.

    On a desktop Ubuntu, you can get them by installing `` linux-headers-`uname -r` ``.

    On a Raspberry Pi, follow [these instructions](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel-headers).

2. Depending on the used kernel version, possibly update the Dummy HCD/UDC module source code:

    ``` bash
    ./update.sh 6.12
    ```

    Note that the `update.sh` script obtains the module source code from the original release of the corresponding kernel version.
    And that code might contain issues fixed in later kernel versions.
    Fixing these issues would require manually backporting the corresponding patches.

3. Build the module:

    ``` bash
    make
    ```

4. Load the module:

    ``` bash
    ./insmod.sh
    ```
