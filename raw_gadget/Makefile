# SPDX-License-Identifier: Apache-2.0

obj-m := raw_gadget.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) SUBDIRS=$(PWD) clean
