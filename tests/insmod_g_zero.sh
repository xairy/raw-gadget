#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

sudo rmmod g_zero 2>/dev/null
sudo modprobe g_zero pattern=1
