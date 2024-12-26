#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

set -eux

modprobe udc_core
insmod ./dummy_hcd.ko
