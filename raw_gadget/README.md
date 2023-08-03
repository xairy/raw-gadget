Raw Gadget Kernel Module
========================

## Usage

Optionally update:

``` bash
./update.sh
```

Note that for newer kernels that contain [af1969a2d734](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=af1969a2d734d6272c0640b50c3ed31e59e203a9) `USB: gadget: Rename usb_gadget_probe_driver()` committed on 23 Apr 2022, you will need to comment out the corresponding line in [update.sh](update.sh).

Build:

``` bash
make
```

Insmod:

``` bash
./insmod.sh
```
