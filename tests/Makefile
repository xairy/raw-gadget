# SPDX-License-Identifier: Apache-2.0

CC=gcc
CFLAGS=-O2 -Wall -g

.PHONY: all

all: gadget testusb

gadget: gadget.c
	$(CC) -o $@ $< $(CFLAGS) -lpthread

testusb: testusb.c
	$(CC) -o $@ $< $(CFLAGS) -lpthread
