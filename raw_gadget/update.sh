#!/bin/bash

set -eux

REPO=https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain

wget $REPO/drivers/usb/gadget/legacy/raw_gadget.c -O raw_gadget.c
wget $REPO/include/uapi/linux/usb/raw_gadget.h -O raw_gadget.h

git apply ./include.patch
