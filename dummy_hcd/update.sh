#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

set -eux

REPO=https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain

# Take kernel version from command-line argument if provided.
if [ -n "${1-}" ]; then
  VERSION="?h=v$1"
else
  VERSION=""
fi

wget $REPO/drivers/usb/gadget/udc/dummy_hcd.c$VERSION -O dummy_hcd.c
