# SPDX-License-Identifier: Apache-2.0

obj-m := dummy_hcd.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) SUBDIRS=$(PWD) modules
