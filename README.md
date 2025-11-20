Raw Gadget
==========

[Raw Gadget](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst) is a Linux kernel module that implements a low-level interface for the Linux USB Gadget subsystem.

Raw Gadget can be used to emulate software-controlled USB devices, both physical and virtual ones.
Emulating physical devices requires a Linux-based board with a [USB Device Controller](/README.md#usb-device-controllers) (UDC), such as a Raspberry Pi.
Emulating virtual devices requires no hardware and instead relies on the [Dummy HCD/UDC](/dummy_hcd) module (such devices get connected to the kernel Raw Gadget is running on).

This repository contains instructions, [examples](/examples), and [tests](/tests) for Raw Gadget.
In addition, this repository hosts copies of the [Raw Gadget](/raw_gadget) and the [Dummy HCD/UDC](/dummy_hcd) kernel modules for out-of-tree building.


## Learning materials

See the rest of this README for the general information and the set up instructions for Raw Gadget.

See the [Fuzzing USB with Raw Gadget](https://docs.google.com/presentation/d/1sArf2cN5tAOaovlaL3KBPNDjYOk8P6tRrzfkclsbO_c/edit?usp=sharing) talk [[video](https://www.youtube.com/watch?v=AT3PQjKxa_c)] for details about the Linux Host and Gadget USB subsystems and Raw Gadget.

See the [Attacking USB with Raw Gadget](/workshop) workshop for a few hands-on exercises of emulating and proxying USB devices with Raw Gadget.


## Comparison to other interfaces

The Linux kernel provides a number of interfaces for the USB Gadget subsystem that allow emulating USB devices.
Most notably there is the [Composite Framework](https://docs.kernel.org/usb/gadget_configfs.html)
(including the [FunctionFS-based composite function](https://elixir.bootlin.com/linux/latest/source/drivers/usb/gadget/function/f_fs.c))
and the [legacy gadget drivers modules](https://elixir.bootlin.com/linux/latest/source/drivers/usb/gadget/legacy)
(including [GadgetFS](https://elixir.bootlin.com/linux/latest/source/drivers/usb/gadget/legacy/inode.c)).

Most of the Gadget subsystem interfaces (with the exception of GadgetFS and the FunctionFS-based composite function) only allow emulating USB devices of specific classes.
Compared to them, Raw Gadget allows emulating USB devices of arbitary classes.

GadgetFS and the FunctionFS-based composite function do allow emulating USB devices of arbitrary classes.
However, these interfaces allow only a limited control over the responses to some USB requests, as they perform sanity checks on the responses provided from userspace.
This limits their ability of emulating improper USB devices, which might be useful for fuzzing or exploitation.
Compared to them, Raw Gadget has minimal checks on the provided responses.

Raw Gadget is thus the perfect choice for fuzzing and exploiting USB hosts or for software proxying of USB devices.

You can find more details about the difference between Raw Gadget and GadgetFS [in the kernel documentation](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/usb/raw-gadget.rst).

__Note__:
It's not recommended to use Raw Gadget in production for emulating USB devices with standard classes.
Instead, use the [Composite Framework](https://docs.kernel.org/usb/gadget_configfs.html) or the [legacy gadget driver modules](https://elixir.bootlin.com/linux/latest/source/drivers/usb/gadget/legacy).


## Limitations

While Raw Gadget does support emulating a wide range of USB device types, it has a set of known limitations:

- Most notably, there is no support for USB 3 SuperSpeed device emulation (see https://github.com/xairy/raw-gadget/issues/61);
- Also see [TODOs](#todo) for a list of other missing features.

These are not foundational limitations of the technology but rather just features missing from the implementation.
They might be implemented in the future.

In addition, different UDCs have their own limitations:

- Some UDCs only support certain speeds and endpoint types and provide a limited number of endpoints (the information about available endpoints is exposed via the `USB_RAW_IOCTL_EPS_INFO` Raw Gadget ioctl);
- Dummy HCD/UDC does not support isochronous transfers (can be implemented; see https://github.com/xairy/raw-gadget/issues/72).

Besides that, Raw Gadget cannot be used to emulate USB hubs.
The main reason for this is that none of the commonly-used UDCs support emulating hubs.
Aspeed UDCs [do support](https://www.spinics.net/lists/linux-usb/msg157518.html) emulating hubs, but these UDCs are only found within server BMCs.
Adding Raw Gadget support for USB hub emulation via Aspeed UDCs might be possible, but that will likely require changes across both Raw Gadget and the USB Gadget subsystem.

Depending on your use case, a viable alternative to emulating a USB hub is [combining](https://github.com/xairy/raw-gadget/issues/78#issuecomment-2318810268) the functionality of multiple USB devices within a single emulated device.
This can be done by emulating a USB device with multiple USB interfaces of different classes.
These interfaces will be functioning in parallel.


## Usage

Raw Gadget can be used on any Linux-based board that has a USB Device Controller (UDC) — a hardware component that allows the board to act as a USB peripheral device.
Consult the documentation for your board on whether it has a UDC (often marketed as `USB OTG`) and how to enable it.

To set up Raw Gadget, you need to:

1. Enable the UDC on your board;
2. [Find out](#usb-device-controllers) the UDC device and driver names;
3. [Build and load](#building) the Raw Gadget module.

Once the setup is done, you can try running the provided [examples](/examples).

See [Raw Gadget on Raspberry Pi](/docs/setup_raspberry-pi.md) for the end-to-end instructions on how to set up Raw Gadget on a Raspberry Pi board.

Refer to [API usage](#api-usage) for the guidance on how to use Raw Gadget as a part of your project.


## Building

There are two options of building Raw Gadget:

- Rebuild the whole kernel with `CONFIG_USB_RAW_GADGET` enabled.
This requires using a kernel version `5.7+`, as Raw Gadget was [merged](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f2c2e717642c66f7fe7e5dd69b2e8ff5849f4d10) into the mainline in `5.7`
(or backporting Raw Gadget patches to an older kernel);

- Build Raw Gadget as an out-of-tree kernel module without rebuilding the whole kernel; see [raw_gadget](/raw_gadget) (and [dummy_hcd](/dummy_hcd)) for the instructions.
Both modules should be compatible with kernel versions down to `4.14`; see [the table below](#usb-device-controllers).


## USB Device Controllers

Raw Gadget requires the user to provide the UDC device and driver names.
This allows using Raw Gadget with a particular UDC if a few of them are present on the system.

UDC device names can be found in `/sys/class/udc/`:

``` bash
$ ls /sys/class/udc/
dummy_udc.0
```

The UDC driver name is usually present in `/sys/class/udc/$UDC_DEVICE_NAME/uevent`:

``` bash
$ cat /sys/class/udc/dummy_udc.0/uevent
USB_UDC_NAME=dummy_udc
```

Below is a table of hardware with various UDCs that were tested with Raw Gadget.

| Hardware | Kernel | Driver | Device | Works? |
| :---: | :---: | :---: | :---: | :---: |
| Any | `5.3.0-45-generic` | `dummy_udc` | `dummy_udc.0` | [Yes](/tests#dummy-udc) |
| Raspberry Pi Zero | `4.14.97+` | `20980000.usb` | `20980000.usb` (`dwc2`) | [Yes](/tests#raspberry-pi-zero) |
| Raspberry Pi Zero 2 | `6.12.34+` | `3f980000.usb` | `3f980000.usb` (`dwc2`) | [Yes] |
| Raspberry Pi 4 | `5.10.63-v7l+` | `fe980000.usb` | `fe980000.usb` (`dwc2`) | [Yes](/tests#raspberry-pi-4) |
| Raspberry Pi 5 | `6.12.25-v8-16k+` | `1000480000.usb` | `1000480000.usb` (`dwc2`) | [Yes](/tests#raspberry-pi-5) |
| USB Armory Mk II | `5.4.87-0` | `2184000.usb` | `ci_hdrc.0` | [Yes](/tests#usb-armory-mkii) |
| Orange Pi PC | `5.10.60` | `musb-hdrc` | `musb-hdrc.4.auto` | [Yes](/tests#orange-pi-pc) |
| Orange Pi PC 2 | `5.10.60` | `musb-hdrc` | `musb-hdrc.4.auto` | [Yes](/tests#orange-pi-pc-2) |
| Khadas VIM1 | `5.10.60-meson64` | `c9100000.usb` | `c9100000.usb` | [Yes](/tests#khadas-vim1) |
| [ThinkPad X1 Carbon Gen 6](https://xairy.io/articles/thinkpad-xdci) | `5.15.0-107-generic` | `dwc3-gadget` | `dwc3.1.auto` | [Yes](/tests#thinkpad-x1-carbon-gen-6) |
| NXP i.MX8MP | `6.6.23` | `dwc3-gadget` | `38100000.usb` | [Yes](/tests#nxp-imx8mp) |
| BeagleBone Black | `4.19.94-ti-r42` | `musb-hdrc` | `musb-hdrc.0` | Probably |
| BeagleBone AI | `4.14.108-ti-r131` | `dwc3-gadget` | `48890000.usb` | Probably |
| [EC3380-AB](http://www.hwtools.net/Adapter/EC3380-AB.html) | `5.3.0-45-generic` | `net2280` | `0000:04:00.0` (e.g.) | Partially,<br />`net2280` buggy |
| Odroid C2 | `3.14.79-116` | `dwc_otg_pcd` | `dwc2_a` | No, kernel too old |

"Works" in the table above means that the setup passed the provided [tests](/tests).
However, note that those only cover a [subset of functionality](/tests#todo).


## API usage

A userspace application can interact with Raw Gadget by opening `/dev/raw-gadget` and issuing `ioctl` calls to it.
See the comments in [raw_gadget.h](raw_gadget/raw_gadget.h) for the details about each supported `ioctl`.
Multiple Raw Gadget instances (bound to different UDCs) can be used at the same time.

On the high level, the usage scenario of Raw Gadget is:

1. Create a Raw Gadget instance by opening `/dev/raw-gadget`;
2. Initialize the instance via `USB_RAW_IOCTL_INIT`;
3. Launch the instance with `USB_RAW_IOCTL_RUN`;
4. In a loop, issue `USB_RAW_IOCTL_EVENT_FETCH` to receive events from Raw Gadget and react to those depending on what kind of USB device must be implemented.

For basic examples of the API usage, see [examples](/examples).
For a more advanced example (that handles USB resets and configuration/altsetting changes) see [AristoChen/usb-proxy](https://github.com/AristoChen/usb-proxy).


### Endpoint addresses

Some UDCs impose restrictions on the endpoint parameters (transfer type, direction, etc.) that can be used with specific endpoint addresses.
Thus, the endpoint parameters in the endpoint descriptor passed to `USB_RAW_IOCTL_EP_ENABLE` must be supported by the UDC for the specific endpoint address.

Raw Gadget provides a way to find out the endpoint restrictions imposed by the used UDC.
Once `USB_RAW_EVENT_CONNECT` is received via `USB_RAW_IOCTL_EVENT_FETCH`, `USB_RAW_IOCTL_EPS_INFO` can be used to find out the information about the endpoint restrictions.
Based on that information, the device emulation code might have to dynamically assign the endpoint addresses in the endpoint descriptors to align with the endpoint parameters.

Note: `USB_RAW_IOCTL_EP_ENABLE` is implemented to go through the list of UDC endpoints and choose the first one that supports the endpoint address and parameters specified in the endpoint descriptor.


### Blocking ioctls

Every Raw Gadget ioctl that receives or sends a USB transfer is blocking.

This includes both the ioctls for handling control requests (`USB_RAW_IOCTL_EVENT_FETCH` for receiving the request and `USB_RAW_IOCTL_EP0_WRITE/READ` for responding) and the ioctls for handling non-control transfers (`USB_RAW_IOCTL_EP_WRITE/READ`).
The ioctls for responding to control requests and for sending non-control transfers typically do not block for a long time.
However, the ioctls for receiving control requests and for receiving non-control transfers might block for a while.

Thus, the userspace application that uses Raw Gadget would normally be multithreaded:

1. In the main thread, set up and launch a Raw Gadget instance;

2. Serve responses to control requests received via `USB_RAW_IOCTL_EVENT_FETCH` until a `USB_REQ_SET_CONFIGURATION` request;

3. Use `USB_RAW_IOCTL_EP_ENABLE` to enable each endpoint for the default altsetting of each interface of the configuration being set;

4. For each enabled endpoint, spawn a thread that would handle transfers on that endpoint;

5. In the main thread, continue handling `USB_RAW_IOCTL_EVENT_FETCH`.


### Resets and configuration/altsetting changes

Throughout a USB device lifetime, the host might decide to reset the device or change its configuration or interface altsettings.
These events require special handling with Raw Gadget to adjust the enabled endpoints.

If the host decides to reset the device, the UDC driver will partially handle the reset first.
The driver will disable all endpoints and any attempt to perform an endpoint operation will fail with `ESHUTDOWN`.
When this happens, Raw Gadget will issue a `USB_RAW_EVENT_RESET` event (or `USB_RAW_EVENT_DISCONNECT` [for](https://github.com/xairy/raw-gadget/issues/48) `dwc2`).
The device emulation code needs to gracefully handle `ESHUTDOWN` in the endpoint threads, disable all Raw Gadget endpoints when handling the `USB_RAW_EVENT_RESET` event, continue handling `USB_RAW_IOCTL_EVENT_FETCH` to redo the enumeration process, and reenable the endpoints and respawn the endpoint threads when handling a new `USB_REQ_SET_CONFIGURATION` request.

Similar workflow is required for handling the `USB_REQ_SET_CONFIGURATION` and `USB_REQ_SET_INTERFACE` requests (that might happen without a reset).
Unlike with a reset, in these cases, the affected endpoints will not be disabled automatically and the endpoint threads will not automatically exit with `ESHUTDOWN`.
Since the endpoint threads might still be blocked on the transfer ioctls, the proposed approach is to interrupt the threads via a signal (e.g., `SIGUSR1`), gracefully handle `EINTR` in the threads, and disable/enable the endpoints and respawn the endpoint threads corresponding to the changed configuration or altsetting.

See the [keyboard example](examples/keyboard.c) for a reference implementation of the reset handling.
For an example that handles both resets and configuration/altsetting changes, see [AristoChen/usb-proxy](https://github.com/AristoChen/usb-proxy).


## syzkaller integration

Raw Gadget powers the [syzkaller](https://github.com/google/syzkaller)'s ability to [fuzz the Linux kernel USB stack](https://github.com/google/syzkaller/blob/master/docs/linux/external_fuzzing_usb.md).

See [Running syzkaller USB reproducers](docs/syzkaller_reproducers.md) for instructions on running syzkaller USB reproducers on a Linux-based board plugged into a physical USB host.

You also set up syzkaller in the [isolated mode](https://github.com/google/syzkaller/blob/master/docs/linux/setup_linux-host_isolated.md) to fuzz physical USB hosts with the help of Raw Gadget.
Instructions for this are not provided.


## Facedancer backend

There's a [prototype](https://github.com/greatscottgadgets/facedancer/pull/164) of a Facedancer backend based on Raw Gadget.
The prototype is still being tested and reviewed.

Raw Gadget–based backend accepts a few parameters through environment variables:

| Parameter | Description | Default value |
| :---: | :---: | :---: |
| `RG_UDC_DRIVER` | UDC driver name | `dummy_udc` |
| `RG_UDC_DEVICE` | UDC device name | `dummy_udc.0` |
| `RG_USB_SPEED` | USB device speed | Defined by Facedancer, usually `1` (Full Speed) |

Example of using Facedancer with Raw Gadget to emulate a USB keyboard on a Raspberry Pi 4B:

``` bash
export BACKEND=rawgadget
export RG_UDC_DRIVER=fe980000.usb
export RG_UDC_DEVICE=fe980000.usb
./examples/rubber-ducky.py
```

Note: Some Facedancer examples might fail if an incompatible USB speed is specified.
Failures happen either with `EINVAL` in `USB_RAW_IOCTL_EP_ENABLE`, with `ESHUTDOWN` in `USB_RAW_IOCTL_EP_READ/WRITE`, or can be completely random.
For example, with Dummy UDC, `examples/ftdi-echo.py` requires `RG_USB_SPEED=2`.

Note: This backend is still a prototype.
Outstanding tasks:

1. Optionally, provide a common [Python wrapper](https://github.com/xairy/raw-gadget/issues/1) for Raw Gadget ioctls, and use it in the backend;
2. Run [Facedancer tests](https://github.com/greatscottgadgets/facedancer/tree/main/test) with a few different UDCs and make sure that they succeed;
3. Test [proxying](https://github.com/greatscottgadgets/facedancer/blob/main/docs/source/using_usb_proxy.rst) of various USB devices and check that it works.


## Troubleshooting

As a generic guidance to troubleshooting Raw Gadget errors:

1. Consider switching to the [dev branch](https://github.com/xairy/raw-gadget/tree/dev).

    This branch might contain fixes for some known issues;

2. Enable debug output for Raw Gadget (and Dummy HCD/UDC if you're using it).

    To do this, add the following line to the very beginning of `raw_gadget/raw_gadget.c`:

    ``` c
    #define DEBUG
    ```

    Then, rebuild and reinsert the Raw Gadget module;

3. Check the kernel log via `dmesg` to find out what is failing.


### No such device

`USB_RAW_IOCTL_RUN` returns `ENODEV`, error code `19`:

```
ioctl(USB_RAW_IOCTL_RUN): No such device
```

This error means that bad UDC driver/device names were provided.
Make sure that the UDC driver module is loaded.
Also see [USB Device Controllers](#usb-device-controllers) about UDC names.


### Invalid argument

`USB_RAW_IOCTL_EP_ENABLE` returns `EINVAL`, error code `22`:

```
ioctl(USB_RAW_IOCTL_EP_ENABLE): Invalid argument
```

This error means that Raw Gadget failed to find an available UDC endpoint that matched the required endpoint parameters.
This happens when either the UDC does not support emulating endpoints with such parameters at all or when all such UDC endpoints are already in use.

To verify the cause of the issue, you can trace `raw_ioctl_ep_enable` (e.g., by adding debugging print calls) and see which part of it is failing.
Or manually check the information provided by `USB_RAW_IOCTL_EPS_INFO` against the parameters in the endpoint descriptor (but note that UDC drivers might have internal requirements for endpoint parameters not exposed via `USB_RAW_IOCTL_EPS_INFO`; failing checks for these would result in `usb_ep_enable` failing).

The likely solution is to use a different UDC that supports the required number of endpoints with required parameters.


### Cannot send after transport endpoint shutdown

Endpoint operations return `ESHUTDOWN`, error code `108`:

```
ioctl(USB_RAW_IOCTL_EP0_WRITE): Cannot send after transport endpoint shutdown
```

This error means that the emulated USB device tried to perform a read or write operation on a disabled endpoint.
This error can happen due to a variety of reasons, but it is commonly observed when the host decides to reset the device during its operation, following which the UDC driver disables all enabled endpoints.

Often, a reset happens when the device emulation code does something wrong.
For example, provides a bad USB descriptor that is either malformed or inconsistent with the emulated device speed or other parameters.
As a result, either the UDC driver or the host resets or disconnects the device.

However, a reset can happen during a normal device operation.
For example, the host might decide to reconfigure the device and thus will reset it.
See [Resets and configuration/altsetting changes](#resets-and-configurationaltsetting-changes) for the details on how to gracefully handle such cases.


## Projects based on Raw Gadget

Fuzzing-related:

* [google/syzkaller](https://github.com/google/syzkaller) — kernel fuzzer, uses Raw Gadget for [fuzzing Linux kernel USB drivers](https://github.com/google/syzkaller/blob/master/docs/linux/external_fuzzing_usb.md);
* [ReUSB: Replay-Guided USB Driver Fuzzing](https://www.usenix.org/conference/usenixsecurity23/presentation/jang) — research work on extending syzkaller's USB fuzzing capability by collecting corpus seeds from real devices.

Proxies:

* [AristoChen/usb-proxy](https://github.com/AristoChen/usb-proxy) — USB proxy with injection support based on Raw Gadget and libusb;
* [blegas78/usb-sniffify](https://github.com/blegas78/usb-sniffify) — another USB proxy with injection support based on Raw Gadget and libusb;
* [patryk4815/usb-proxy](https://github.com/patryk4815/usb-proxy) — USB proxy based on Raw Gadget and written in Go;
* [michaelforney/proxy.c](https://gist.github.com/michaelforney/36f78621b79d6caaaf64be7416a8dec2) — USB proxy used for [reverse engineering](https://github.com/michaelforney/oscmix/issues/9#issuecomment-1960915504) some USB MIDI device;
* [Mjollnirs/UsbMITMAttack](https://github.com/Mjollnirs/UsbMITMAttack) — research project about using Raw Gadget on Raspberry Pi 4B to MitM USB devices.

Emulators:

* [msawahara/me56ps2-emulator](https://github.com/msawahara/me56ps2-emulator) — emulator for ME56PS2 (PlayStation 2–compatible modem) [[article](https://qiita.com/msawahara/items/f109b75919ddcf0db05a)];
* [Berghopper/360-raw-gadget](https://github.com/Berghopper/360-raw-gadget) and [Berghopper/360-w-raw-gadget](https://github.com/Berghopper/360-w-raw-gadget/) — emulators for Xbox 360 contoller.

Other:

* [blegas78/chaos](https://github.com/blegas78/chaos) — MitM tool running on Raspberry Pi that allows Twitch chat to mess around with the inputs from a Dualshock 4 Generation 2 controller;
* [kovalev0/usb-gadget-tests](https://github.com/kovalev0/usb-gadget-tests) — testing framework for the USB Gadget subsystem.


## TODO

* [GitHub issues](https://github.com/xairy/raw-gadget/issues)
* [TODOs in kernel documentation](https://elixir.bootlin.com/linux/v5.7/source/Documentation/usb/raw-gadget.rst#L74)
* [TODOs in Raw Gadget test suite](/tests#todo)


## License

The parts of code in this repository that are derived from the Linux kernel are covered by GPL-2.0. Everything else is covered by Apache-2.0. `SPDX-License-Identifier` marks the used license in each file.
