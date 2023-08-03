Dummy HCD/UDC Kernel Module
===========================

Dummy HCD/UDC is a module that sets up virtual USB Device and Host controllers that are connected to each other inside the kernel.
This module allows connecting USB devices from userspace to the underlying kernel through any of the interfaces for the Gadget subsystem (Raw Gadget, GadgetFS, etc).

## Usage

Optionally update:

``` bash
./update.sh
```

Build:

``` bash
make
```

Insmod:

``` bash
./insmod.sh
```
