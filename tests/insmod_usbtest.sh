#!/bin/bash
# SPDX-License-Identifier: Apache-2.0

sudo rmmod usbtest 2>/dev/null
sudo modprobe usbtest pattern=1
