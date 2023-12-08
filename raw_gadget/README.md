Raw Gadget Kernel Module
========================

This directory contains the Raw Gadget module source code patched to allow out-of-tree building on older kernels.
Check the `patches/` directory to see the applied patches.


## Usage

Build the module:

``` bash
make
```

Load the module:

``` bash
./insmod.sh
```


## Updating

You can optionally update the Raw Gadget module source code to fetch the changes from the `usb-next` Raw Gadget version:

``` bash
./update.sh
```

__Note:__
Do not use the updating script if you're on the `dev` branch: it will revert all changes applied to that branch.

Note:
The updating script applies the patches from `patches/`.
You might need to revert them to build Raw Gadget against a modern kernel.
