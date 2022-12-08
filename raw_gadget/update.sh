#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

set -eux

REPO=https://git.kernel.org/pub/scm/linux/kernel/git/gregkh/usb.git/plain
BRANCH=usb-next

wget $REPO/drivers/usb/gadget/legacy/raw_gadget.c?h=$BRANCH -O raw_gadget.c
wget $REPO/include/uapi/linux/usb/raw_gadget.h?h=$BRANCH -O raw_gadget.h

# Fix up include names for local usage.
git apply ./patches/include.patch

# Fix up usb_gadget_probe_driver rename introduced by af1969a2d734 (23 Apr 2022).
git apply ./patches/usb_gadget_probe_driver.patch
