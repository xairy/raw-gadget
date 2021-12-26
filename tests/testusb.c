// SPDX-License-Identifier: GPL-2.0-or-later
//
// A helper program to issue ioctls to the usbtest driver.
// Part of the USB Raw Gadget test suite.
// Based on https://github.com/torvalds/linux/blob/master/tools/usb/testusb.c.
// See https://github.com/xairy/raw-gadget for details.
//
// Andrey Konovalov <andreyknvl@gmail.com>

#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/usb/ch9.h>
#include <linux/usbdevice_fs.h>

struct usbtest_param {
	unsigned		test_num;
	unsigned		iterations;
	unsigned		length;
	unsigned		vary;
	unsigned		sglen;

	struct timeval		duration;
};

#define USBTEST_REQUEST	_IOWR('U', 100, struct usbtest_param)

static int usbdev_ioctl(int fd, int ifno, unsigned request, void *param) {
	struct usbdevfs_ioctl wrapper;
	wrapper.ifno = ifno;
	wrapper.ioctl_code = request;
	wrapper.data = param;
	return ioctl(fd, USBDEVFS_IOCTL, &wrapper);
}

static int parse_num(const char *str, unsigned int *num) {
	unsigned long val;
	char *end;

	errno = 0;
	val = strtoul(str, &end, 0);
	if (errno || *end || val > UINT_MAX)
		return -1;
	*num = val;
	return 0;
}

int main (int argc, char **argv) {
	struct usbtest_param param;
	param.iterations = 1000;
	param.length = 1024;
	param.vary = 1024;
	param.sglen = 32;

	char *device = NULL;
	int test = -1;

	int opt;
	while ((opt = getopt(argc, argv, "D:t:c:s:v:g:h")) != EOF) {
		switch (opt) {
		case 'D':  // Device path, e.g. /dev/bus/usb/005/003.
			device = optarg;
			continue;
		case 't':  // Test number.
			test = atoi(optarg);
			if (test < 0)
				goto usage;
			continue;
		case 'c':  // Iterations number.
			if (parse_num(optarg, &param.iterations))
				goto usage;
			continue;
		case 's':  // Transfer length.
			if (parse_num(optarg, &param.length))
				goto usage;
			continue;
		case 'v':  // Vary packet length by ...
			if (parse_num(optarg, &param.vary))
				goto usage;
			continue;
		case 'g':  // Scatter/gather entries length.
			if (parse_num(optarg, &param.sglen))
				goto usage;
			continue;
		case 'h':
		default:
usage:
			fprintf (stderr,
				"usage: %s [options]\n"
				"Options:\n"
				"\t-D device path\n"
				"\t-t test number\n"
				"Case arguments:\n"
				"\t-c iterations\t\tdefault 1000\n"
				"\t-s transfer length\tdefault 1024\n"
				"\t-v vary\t\t\tdefault 1024\n"
				"\t-g s/g length\t\tdefault 32\n",
				argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (optind != argc)
		goto usage;
	if (!device)
		goto usage;
	if (test == -1)
		goto usage;

	param.test_num = test;

	int fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("open(device)");
		return EXIT_FAILURE;
	}

	int ifnum = 0;
	int status = usbdev_ioctl(fd, ifnum, USBTEST_REQUEST, &param);

	if (status < 0) {
		char buf[80];
		int err = errno;
		if (strerror_r(errno, buf, sizeof(buf))) {
			snprintf(buf, sizeof(buf), "error %d", err);
			errno = err;
		}
		printf("%s test %02d: FAILURE: %d (%s)\n",
			device, test, errno, buf);
		return errno;
	}

	printf("%s test %02d: SUCCESS: %d.%.06d secs\n", device, test,
		(int)param.duration.tv_sec, (int)param.duration.tv_usec);

	return EXIT_SUCCESS;
}
