# SPDX-License-Identifier: Apache-2.0

CC=gcc
CFLAGS=-O2 -Wall -g

.PHONY: all

all: keyboard printer

keyboard: keyboard.c
	$(CC) -o $@ $< $(CFLAGS) -lpthread

printer: printer.c
	$(CC) -o $@ $< $(CFLAGS) -lpthread
