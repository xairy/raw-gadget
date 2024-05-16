Raw Gadget Kernel Module
========================

Raw Gadget is maintained as a [part](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/usb/gadget/legacy/raw_gadget.c) of the mainline Linux kernel source code.

Nevertheless, for convenience, this directory contains a copy of the Raw Gadget module source code patched to allow out-of-tree building on 4.19–5.18 kernels.

For building Raw Gadget on 5.19+ kernels, you need to undo the [revert](update.sh#L16) of the `USB: gadget: Rename usb_gadget_probe_driver()` [patch](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=af1969a2d734d6272c0640b50c3ed31e59e203a9).

For building Raw Gadget on kernels older than 4.19, you need to manually revert the `usb: gadget: Fix non-unique driver names in raw-gadget driver` [patch](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2d8c2606825317b77db1f9ba0fc26ef26160b30).

Note that building Raw Gadget on very old kernels will just fail.


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

You can optionally update the Raw Gadget module source code to fetch the changes from the `usb-next` Raw Gadget version:

``` bash
./update.sh
```

__Note:__
Do not use the updating script if you're on the `dev` branch: it will revert all changes applied to that branch.

Note:
The updating script applies the patches from `patches/`.
You might need to revert them to build Raw Gadget against a modern kernel.
