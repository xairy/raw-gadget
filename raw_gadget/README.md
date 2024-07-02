Raw Gadget Kernel Module
========================

This directory contains a copy of the Raw Gadget module source code patched to allow out-of-tree building.
This code is provided only for convenience;
Raw Gadget is maintained as a [part](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/usb/gadget/legacy/raw_gadget.c) of the mainline Linux kernel source code ([merged](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2c2e717642c66f7fe7e5dd69b2e8ff5849f4d10) in `5.7`).


## Building and loading

1. Install the Linux kernel headers.

    On a desktop Ubuntu, you can get them by installing `` linux-headers-`uname -r` ``.

    On a Raspberry Pi, follow [these instructions](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel-headers).

2. Depending on the used kernel version, possibly patch the Raw Gadget module source code.

    For `5.19+` kernels, no patching should be required.

    For building Raw Gadget on `4.19`–`5.18` kernels, apply the [revert](patches/usb_gadget_probe_driver.patch) of the `USB: gadget: Rename usb_gadget_probe_driver()` [patch](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=af1969a2d734d6272c0640b50c3ed31e59e203a9) via `git apply ./patches/usb_gadget_probe_driver.patch`.

    For building Raw Gadget on `4.14`–`4.18` kernels, manually revert the `usb: gadget: Fix non-unique driver names in raw-gadget driver` [patch](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2d8c2606825317b77db1f9ba0fc26ef26160b30).

    Building Raw Gadget on kernels older than `4.14` might be possible but not supported.

3. Build the module:

    ``` bash
    make
    ```

4. Load the module:

   ``` bash
   ./insmod.sh
   ```


## Updating

You can optionally update the Raw Gadget module source code to fetch the changes from the `usb-next` Raw Gadget version:

``` bash
./update.sh
```

__Note:__
Do not run the updating script if you're on the `dev` branch: it will revert all changes applied to that branch.
