Dummy HCD/UDC Kernel Module
===========================

Dummy HCD/UDC is a module that sets up virtual USB Device and Host controllers that are connected to each other inside the kernel.
This module allows connecting USB devices emulated from userspace through any of the Gadget subsystem interfaces (Raw Gadget, GadgetFS, etc.) directly to the underlying kernel.

This directory contains the Dummy HCD/UDC module source code patched to allow out-of-tree building on older kernels.
Check the `patches/` directory to see the applied patches.

Dummy HCD/UDC is not a part of Raw Gadget; its code is present in this repository just for convenience.


## Usage

Build the module:

``` bash
make
```

Note:
You might need to revert some of the patches from `patches/` to build the module on newer kernels.

Load the module:

``` bash
./insmod.sh
```


## Updating

You can optionally update the Dummy HCD/UDC module source code to fetch the changes from the mainline Dummy HCD/UDC version:

``` bash
./update.sh
```

Note:
The updating script applies the patches from `patches/`.
You might need to revert them to build Dummy HCD/UDC against a modern kernel.
