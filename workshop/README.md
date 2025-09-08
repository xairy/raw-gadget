Attacking USB with Raw Gadget
=============================

This is a workshop that serves as an introduction to the USB hacking topic in general and to [Raw Gadget](https://github.com/xairy/raw-gadget) specifically.

The workshop gives an overview of the USB protocol, the Linux USB Gadget subsystem, and the Raw Gadget interface.
The offered hands-on exercises include capturing and analyzing the communication of USB devices, emulating USB devices via Raw Gadget, and proxying USB devices via Raw Gadget and libusb to aid reverse engineering.

Note that you must bring your own [hardware](#required-hardware) to attend this workshop at [BalCCon2k25](https://2k25.balccon.org/).


## Required hardware

- Raspberry Pi 5 (or 4 B, but note [1]);
- SD card and SD card reader;
- USB-UART adapter with jumper cables (for getting a shell on the Raspberry Pi);
- Laptop with Linux and a USB PD 3.0–compatible port (Type-C ports on modern laptops typically are) [2, 3];
- Type-C-to-your-laptop-port USB cable;
- USB mouse (prefer simplest mice that require no special drivers);
- Optionally, other USB devices (if you want to try proxying them).

[1]: Technically, any Linux-based board with a UDC (USB Device Controller) and a separate USB host port running the Linux kernel version 4.14+ might work.
However, to avoid discrepancies in the setup, please use Raspberry Pi 5 — the instructions are only provided for this board.
Raspberry Pi 4 B might work, but only if the USB port on your laptop can provide enough power for the board [2] (note that Raspberry Pi 4 B does not support USB PD).

[2]: The USB port on your laptop must provide enough power to power the Raspberry Pi board and the mouse (or other devices) connected to the board.
For example, a laptop with a USB PD 3.0–compatible port should work (such port can provide provide 3A/5V according to the USB PD specification, which should be enough to power the board).
If your laptop does not have such a port, please check the specification of the USB ports on your laptop and test powering the Raspberry Pi.
Powering the Raspberry Pi via an external power supply will not work, as the Type-C port on the Raspberry Pi will need to be connected to the laptop to allow emulating USB devices for the laptop.
Powering the Rapspberry Pi via a powered USB hub might work, but [test the setup](#setup-instructions) in advance and make sure that Raw Gadget works.

[3]: Windows or Mac might work, but the instructions for these OSes are not provided.
If you bring a laptop without Linux, you must figure out how to capture USB communication on it.


## Setup instructions

It's recommended to follow through these instructions before the workshop starts to confirm that your setup is working properly.

1. Follow through the [instructions](/docs/setup_raspberry-pi.md) for setting up Raw Gadget on Raspberry Pi;

2. Install Wireshark on your laptop and check that you can [capture USB communication](https://wiki.wireshark.org/CaptureSetup/USB#linux) on the laptop (choose the `usbmon0` interface after starting Wireshark).


## Workshop instructions

Slides to be provided later.


## Lab 1: Checking descriptors of USB mouse

1. Plug in the USB mouse into your laptop;

2. Run `lsusb` and find the mouse in the list of USB devices.

    Take note of the Vendor and Product IDs (shown as `ID VVVV:PPPP`);

3. Run `lsusb -v | less` and check the USB descriptors for the mouse.

    Locate the Device, Configuration, Interface, and Endpoint descriptors.


## Lab 2: Inspecting communication of USB mouse

1. Follow through the [instructions](https://wiki.wireshark.org/CaptureSetup/USB#linux) for setting up Wireshark on your laptop;

2. Start sniffing the `usbmon0` interface with Wireshark;

3. Apply the `usb.transfer_type == 2` display filter in Wireshark to only see the control requests;

4. Reconnect the mouse to the laptop;

5. Inspect the control requests sent to the mouse and their responses.

    The enumeration process starts with the `GET_DESCRIPTOR` request for the Device descriptor.

    The USB core enumeration process ends with the `SET_CONFIGURATION` request.

    The HID driver should also send the `GET_DESCRIPTOR` request for the HID Report descriptor.
    Locate the contents of the HID Report descriptor within the response data.

    Take note that all requests are initiated by the host, and the device only sends the responses;

6. Apply the `usb.bus_id == X && usb.device_address == Y` display filter to see all USB requests sent to the mouse.

    Take the `X` and the `Y` values from `lsusb`.
    For example, for `Bus 003 Device 038`, `X` == `3` and `Y` == `38`;

7. Move the mouse around and inspect the Interrupt requests sent to the mouse and their responses.

    Locate the contents of the HID reports (aka `HID Data`) within the response data.

    Figure out how the HID reports change depending on the mouse movement or the button clicks.


## Lab 3: Emulating USB keyboard via Raw Gadget

1. Follow through the [instructions](/docs/setup_raspberry-pi.md) for setting up Raw Gadget on Raspberry Pi;

2. Run the [keyboard emulator program](/examples) as is.

    Make sure that you see the letter `X` being entered on the laptop;

3. Modify the `ep_int_in_loop` function in the keyboard emulator program to enter the string `Hi` (letter by letter) instead of the letter `X`.

    `ep_int_in_loop` sends responses to the requests for the Interrupt endpoint received from the host.
    The responses contain HID reports that specify which keys are currently pressed.


## Lab 4: Emulating USB mouse via Raw Gadget

1. Create a copy of `keyboard.c` called `mouse.c` and modify the `Makefile` to also build `mouse.c`.

    The code for emulating a USB mouse via Raw Gadget will be very similar to the code for emulating a USB keyboard.
    The only differences would be the HID Report descriptor and the sent HID reports;

2. Get the HID Report descriptor for your USB mouse from Wireshark.

    Right click on `HID Report`, `Show Packet Bytes...`, `Show as` -> `C Array`;

3. Replace the contents of the `char usb_hid_report[]` variable in `mouse.c` with the extracted array;

4. Modify `ep_int_in_loop` in `mouse.c` to make the mouse automatically move up and down.

    Use the knowledge from Lab 2 to figure out what kind of HID reports to send in `ep_int_in_loop`.

<details>
  <summary>Click to see the HID Report descriptor and a HID report example for my USB mouse.</summary>

  HID Report descriptor:

  ``` c
  char packet_bytes[] = {
  	0x05, 0x01, 0x09, 0x02, 0xa1, 0x01, 0x09, 0x01,
  	0xa1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03,
  	0x15, 0x00, 0x25, 0x01, 0x95, 0x08, 0x75, 0x01,
  	0x81, 0x02, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31,
  	0x09, 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75, 0x08,
  	0x95, 0x03, 0x81, 0x06, 0xc0, 0xc0
  };
  ```

  Example of a HID report when moving the mouse:

  ```
  HID Data: 00fefb00
  	.... ...0 = Button: 1 (primary/trigger): UP
  	.... ..0. = Button: 2 (secondary): UP
  	.... .0.. = Button: 3 (tertiary): UP
  	Padding: 00
  	1111 1110 = X Axis: -2
  	1111 1011 = Y Axis: -5
  	0000 0000 = Usage: Wheel: 0
  ```

</details>


## Lab 5: Proxying USB mouse via Raw Gadget and usb-proxy

1. Clone and build the [AristoChen/usb-proxy](https://github.com/AristoChen/usb-proxy) tool:

    ``` bash
    sudo apt install libusb-1.0-0-dev libjsoncpp-dev
    git clone https://github.com/AristoChen/usb-proxy.git
    cd usb-proxy/ && make
    ```

2. Connect the USB mouse to the Raspberry Pi (disconnect the mouse from the laptop);

3. On the Raspberry Pi, obtain the Vendor and Product IDs for the mouse from `lsusb`;

4. On the Raspberry Pi, run `./usb-proxy` to proxy the mouse:

    ``` bash
    sudo ./usb-proxy --device=1000480000.usb --driver=1000480000.usb \
    			--vendor_id=VVVV --product_id=PPPP
    ```

    Replace `VVVV` and `PPPP` with the Vendor and Product IDs in the command above.

    You might need to also [modify the UDC device and driver names](/#usb-device-controllers) if you're not using Raspberry Pi 5;

5. Check that you see the mouse cursor moving on the laptop.

    Note: The mouse cursor might be moving very slowly.
This is a bug in usb-proxy that needs to be fixed (caused by improper Interrupt request timeouts);

6. Try proxying other USB devices if you have them.

    Proxying some devices might fail due to missing features or bugs in the Raw Gadget and usb-proxy implementations.


## Lab 6: MitM'ing USB mouse buttons via Raw Gadget and usb-proxy

1. Sniff and inspect the HID reports that are sent when the left and right mouse buttons are clicked;

2. [Put together](https://github.com/AristoChen/usb-proxy#how-to-do-mitm-attack-with-this-project) a `rules.json` file that swaps these buttons;

3. Run usb-proxy with the defined rules:

    ``` bash
    sudo ./usb-proxy --device=1000480000.usb --driver=1000480000.usb \
    			--vendor_id=VVVV --product_id=PPPP \
    			--enable_injection --injection_file=rules.json
    ```

4. Make sure that the laptop sees a right click when the left button is pressed and vice versa.

<details>
  <summary>Click to see the rules that work for my USB mouse.</summary>

  ``` json
  {
      "control": {
          "modify": [],
          "ignore": [],
          "stall": []
      },
      "int": [
          {
              "ep_address": 81,
              "enable": true,
              "content_pattern": ["\\x01\\x00\\x00\\x00"],
              "replacement": "\\x02\\x00\\x00\\x00"
          },
          {
              "ep_address": 81,
              "enable": true,
              "content_pattern": ["\\x02\\x00\\x00\\x00"],
              "replacement": "\\x01\\x00\\x00\\x00"
          }
      ],
      "bulk": [],
      "isoc": []
  }
  ```

</details>


## Lab 7: MitM'ing USB mouse movement via Raw Gadget and usb-proxy

1. Modify usb-proxy to inverse the mouse movement direction.

    The built-in JSON-based pattern rules are not flexible enought to achieve this.

    Instead, modify `ep_loop_read` in `proxy.cpp` to change the `io.data` bytes manually;

2. Rebuild and run `./usb-proxy`;

3. Check that the mouse cursor moves up on the laptop when you move the mouse down.

<details>
  <summary>Click to see a hacky usb-proxy patch that works for my USB mouse.</summary>

  ``` c
  diff --git a/proxy.cpp b/proxy.cpp
  index d88cdc2..508c7d4 100644
  --- a/proxy.cpp
  +++ b/proxy.cpp
  @@ -222,6 +222,10 @@ void *ep_loop_read(void *arg) {
                                  io.inner.flags = 0;
                                  io.inner.length = nbytes;
   
  +                               // Inverse mouse movement.
  +                               io.data[1] = -io.data[1];
  +                               io.data[2] = -io.data[2];
  +
                                  if (injection_enabled)
                                          injection(io, ep, transfer_type);
   
  ```

</details>


## Lab 8: Running Facedancer with Raw Gadget backend

1. Clone and install the Facedancer fork with a [prototype of the Raw Gadget–based backend](/#facedancer-backend):

    ``` bash
    git clone -b rawgadget-v2 https://github.com/xairy/Facedancer.git
    cd Facedancer/
    pip install --user --break-system-packages .
    ```

2. Change the permissions on `/dev/raw-gadget` to allow running Facedancer as a non-root user:

    ``` bash
    sudo chmod 666 /dev/raw-gadget
    ```

3. Run the `rubber-ducky.py` Facedancer example that emulates a USB keyboard:

    ```
    export BACKEND=rawgadget
    export RG_UDC_DRIVER=1000480000.usb
    export RG_UDC_DEVICE=1000480000.usb
    ./examples/rubber-ducky.py
    ```

4. Check that you see `echo Hello, Facedancer!` being entered on the laptop;

5. Prepare a disk image `disk.img` for running the `mass-storage.py` example:

    ``` bash
    dd if=/dev/zero of=disk.img bs=512 count=2880
    mkdosfs ./disk.img
    mkdir mnt
    sudo mount ./disk.img ./mnt -o loop
    echo hi | sudo tee ./mnt/file
    sudo umount ./mnt
    ```

6. Run the `mass-storage.py` example that emulates a USB drive:

    ``` bash
    ./examples/mass-storage.py disk.img
    ```

7. Check that you see the USB drive on the laptop and can access the file contents.

You can also try running other Facedancer examples.
But note that some of them fail to function without setting a specific USB speed via `export RG_USB_SPEED=1` (or `2` or `3`).
And note that running `usbproxy.py` requires applying the changes from [this PR](https://github.com/greatscottgadgets/facedancer/pull/162).
