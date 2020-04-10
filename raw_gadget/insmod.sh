#!/bin/bash

set -eux

sudo modprobe udc_core
sudo insmod ./raw_gadget.ko
