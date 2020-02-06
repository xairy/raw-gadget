#!/bin/bash

set -eux

sudo modprobe udc_core
sudo insmod ./dummy_hcd.ko
