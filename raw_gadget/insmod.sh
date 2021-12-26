#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

set -eux

sudo modprobe udc_core
sudo insmod ./raw_gadget.ko
