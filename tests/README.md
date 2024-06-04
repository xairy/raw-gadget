Raw Gadget Tests
================

A test suite for testing Raw Gadget with different UDCs.
Based on the [usbtest](http://www.linux-usb.org/usbtest/) testing module.
All tests are supposed to be first run with `g_zero` kernel module as a baseline, and then with a userspace [gadget implementation](./gadget.c) based on Raw Gadget.

## How to Run

Note: `usbtest` and `g_zero` modules can be native to the kernels running on the host and gadget sides, while `dummy_hcd` and `raw_gadget` are better be built manually (to make sure they are up-to-date).

Note: when using Dummy UDC, the host and the device are the same machine.

Note: `g_zero` binds to the first available UDC; make sure it's the right one.

Running the tests:

0. On both host and gadget sides: `make`.

1. On host side: `./insmod_usbtest.sh`.

2. On gadget side: `./insmod_g_zero.sh`

3. On host side: find out the device address via `lsusb`.

4. On host side: `./run_tests.py /dev/bus/usb/005/002 ./logs/UDC-g_zero.log`.

5. On gadget side: `rmmod g_zero`.

6. On gadget side: load `raw_gadget` module if not already loaded.

7. On gadget side: `./gadget DEVICE DRIVER`.

8. On host side: `./run_tests.py /dev/bus/usb/005/002 ./logs/UDC-raw_gadget.log`.

9. On host side: `./format_results.py ./logs/UDC-raw_gadget.log ./logs/UDC-g_zero.log`.

## TODO

* Add more checks into `gadget.c` and detect failure when those fail.

* Test more speeds (`0x201`, `0x210`, `0x320`) and protocol versions (USB 3.0+).

* Isochronous transfer tests (`usbtest` #15, #16, #22, #23).

* USB 3 Streams tests (not implemented in kernel yet).

* Run USBCV tests (see [linux-usb.org](http://www.linux-usb.org/usbtest/) for details).

* Figure out a way to properly test endpoint halts (`usbtest` #13 is fully handled by UDC drivers).

* Figure out a way to test suspend/resume.

## Results

### Dummy UDC

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied | EINVAL | EINVAL | OK |
| test 8: bulk, queued, IN, varied | EINVAL | EINVAL | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### Raspberry Pi Zero

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### Raspberry Pi 4

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### USB Armory MkII

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### Orange Pi PC

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### Orange Pi PC 2

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### Khadas VIM1

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  | EPIPE | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |

### Thinkpad X1 Carbon Gen 6

| Test | `raw_gadget` | `g_zero` | Status |
| :--- | :---: | :---: | :---: |
| test 1: bulk, non-queued, OUT |  |  | OK |
| test 2: bulk, non-queued, IN |  |  | OK |
| test 3: bulk, non-queued, OUT, varied |  |  | OK |
| test 4: bulk, non-queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 5: bulk, queued, OUT |  |  | OK |
| test 6: bulk, queued, IN |  |  | OK |
| test 7: bulk, queued, OUT, varied |  |  | OK |
| test 8: bulk, queued, IN, varied | EOVERFLOW | EOVERFLOW | OK |
| test 9: control, non-queued, sanity |  |  | OK |
| test 10: control, queued |  |  | OK |
| test 11: bulk, non-queued, unlinks, IN |  |  | OK |
| test 12: bulk, non-queued, unlinks, OUT |  |  | OK |
| test 13: bulk, ep halt set/clear |  |  | OK |
| test 14: control, OUT, varied |  |  | OK |
| test 17: bulk, DMA, odd address, OUT |  |  | OK |
| test 18: bulk, DMA, odd address, IN |  |  | OK |
| test 19: bulk, coherent, odd address, OUT |  |  | OK |
| test 20: bulk, coherent, odd address, IN |  |  | OK |
| test 21: control, unaligned, OUT, varied |  |  | OK |
| test 24: bulk, queued, unlink, OUT |  |  | OK |
| test 25: interrupt, non-queued, OUT |  | ENOTSUP | OK |
| test 26: interrupt, non-queued, IN |  | ENOTSUP | OK |
