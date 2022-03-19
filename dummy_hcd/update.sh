#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

set -eux

REPO=https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain

wget $REPO/drivers/usb/gadget/udc/dummy_hcd.c -O dummy_hcd.c

# This patch is needed in case the kernel you're building against doesn't have
# commit 4d537f37e0d39 ("usb: introduce usb_ep_type_string() function").
git apply ./usb_ep_type_string.patch

# This patch is needed in case the kernel you're building against doesn't have
# commit 7dc0c55e9f30 ("USB: UDC core: Add udc_async_callbacks gadget op").
git apply ./usb_udc_async_callbacks.patch
