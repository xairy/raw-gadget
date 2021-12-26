// SPDX-License-Identifier: Apache-2.0
//
// Originally written for https://github.com/google/syzkaller.
//
// Copyright 2019 syzkaller project authors. All rights reserved.
// Use of this source code is governed by Apache 2 LICENSE.

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <linux/types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/hid.h>
#include <linux/usb/ch9.h>

/*----------------------------------------------------------------------*/

struct hid_class_descriptor {
	__u8  bDescriptorType;
	__le16 wDescriptorLength;
} __attribute__ ((packed));

struct hid_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__le16 bcdHID;
	__u8  bCountryCode;
	__u8  bNumDescriptors;

	struct hid_class_descriptor desc[1];
} __attribute__ ((packed));

/*----------------------------------------------------------------------*/

#define UDC_NAME_LENGTH_MAX 128

struct usb_raw_init {
	__u8 driver_name[UDC_NAME_LENGTH_MAX];
	__u8 device_name[UDC_NAME_LENGTH_MAX];
	__u8 speed;
};

enum usb_raw_event_type {
	USB_RAW_EVENT_INVALID,
	USB_RAW_EVENT_CONNECT,
	USB_RAW_EVENT_CONTROL,
};

struct usb_raw_event {
	__u32		type;
	__u32		length;
	__u8		data[0];
};

struct usb_raw_ep_io {
	__u16		ep;
	__u16		flags;
	__u32		length;
	__u8		data[0];
};

#define USB_RAW_EPS_NUM_MAX	30
#define USB_RAW_EP_NAME_MAX	16
#define USB_RAW_EP_ADDR_ANY	0xff

struct usb_raw_ep_caps {
	__u32	type_control	: 1;
	__u32	type_iso	: 1;
	__u32	type_bulk	: 1;
	__u32	type_int	: 1;
	__u32	dir_in		: 1;
	__u32	dir_out		: 1;
};

struct usb_raw_ep_limits {
	__u16	maxpacket_limit;
	__u16	max_streams;
	__u32	reserved;
};

struct usb_raw_ep_info {
	__u8				name[USB_RAW_EP_NAME_MAX];
	__u32				addr;
	struct usb_raw_ep_caps		caps;
	struct usb_raw_ep_limits	limits;
};

struct usb_raw_eps_info {
	struct usb_raw_ep_info	eps[USB_RAW_EPS_NUM_MAX];
};

#define USB_RAW_IOCTL_INIT		_IOW('U', 0, struct usb_raw_init)
#define USB_RAW_IOCTL_RUN		_IO('U', 1)
#define USB_RAW_IOCTL_EVENT_FETCH	_IOR('U', 2, struct usb_raw_event)
#define USB_RAW_IOCTL_EP0_WRITE		_IOW('U', 3, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_EP0_READ		_IOWR('U', 4, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_EP_ENABLE		_IOW('U', 5, struct usb_endpoint_descriptor)
#define USB_RAW_IOCTL_EP_DISABLE	_IOW('U', 6, __u32)
#define USB_RAW_IOCTL_EP_WRITE		_IOW('U', 7, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_EP_READ		_IOWR('U', 8, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_CONFIGURE		_IO('U', 9)
#define USB_RAW_IOCTL_VBUS_DRAW		_IOW('U', 10, __u32)
#define USB_RAW_IOCTL_EPS_INFO		_IOR('U', 11, struct usb_raw_eps_info)
#define USB_RAW_IOCTL_EP0_STALL		_IO('U', 12)
#define USB_RAW_IOCTL_EP_SET_HALT	_IOW('U', 13, __u32)
#define USB_RAW_IOCTL_EP_CLEAR_HALT	_IOW('U', 14, __u32)
#define USB_RAW_IOCTL_EP_SET_WEDGE	_IOW('U', 15, __u32)

/*----------------------------------------------------------------------*/

int usb_raw_open() {
	int fd = open("/dev/raw-gadget", O_RDWR);
	if (fd < 0) {
		perror("open()");
		exit(EXIT_FAILURE);
	}
	return fd;
}

void usb_raw_init(int fd, enum usb_device_speed speed,
			const char *driver, const char *device) {
	struct usb_raw_init arg;
	strcpy((char *)&arg.driver_name[0], driver);
	strcpy((char *)&arg.device_name[0], device);
	arg.speed = speed;
	int rv = ioctl(fd, USB_RAW_IOCTL_INIT, &arg);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_INIT)");
		exit(EXIT_FAILURE);
	}
}

void usb_raw_run(int fd) {
	int rv = ioctl(fd, USB_RAW_IOCTL_RUN, 0);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_RUN)");
		exit(EXIT_FAILURE);
	}
}

void usb_raw_event_fetch(int fd, struct usb_raw_event *event) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EVENT_FETCH, event);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EVENT_FETCH)");
		exit(EXIT_FAILURE);
	}
}

int usb_raw_ep0_read(int fd, struct usb_raw_ep_io *io) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP0_READ, io);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP0_READ)");
		exit(EXIT_FAILURE);
	}
	return rv;
}

int usb_raw_ep0_write(int fd, struct usb_raw_ep_io *io) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP0_WRITE, io);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP0_WRITE)");
		exit(EXIT_FAILURE);
	}
	return rv;
}

int usb_raw_ep_enable(int fd, struct usb_endpoint_descriptor *desc) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP_ENABLE, desc);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP_ENABLE)");
		exit(EXIT_FAILURE);
	}
	return rv;
}

int usb_raw_ep_read(int fd, struct usb_raw_ep_io *io) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP_READ, io);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP_READ)");
		exit(EXIT_FAILURE);
	}
	return rv;
}

int usb_raw_ep_write(int fd, struct usb_raw_ep_io *io) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP_WRITE, io);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP_WRITE)");
		exit(EXIT_FAILURE);
	}
	return rv;
}

void usb_raw_configure(int fd) {
	int rv = ioctl(fd, USB_RAW_IOCTL_CONFIGURE, 0);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_CONFIGURED)");
		exit(EXIT_FAILURE);
	}
}

void usb_raw_vbus_draw(int fd, uint32_t power) {
	int rv = ioctl(fd, USB_RAW_IOCTL_VBUS_DRAW, power);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_VBUS_DRAW)");
		exit(EXIT_FAILURE);
	}
}

int usb_raw_eps_info(int fd, struct usb_raw_eps_info *info) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EPS_INFO, info);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EPS_INFO)");
		exit(EXIT_FAILURE);
	}
	return rv;
}

void usb_raw_ep0_stall(int fd) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP0_STALL, 0);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP0_STALL)");
		exit(EXIT_FAILURE);
	}
}

void usb_raw_ep_set_halt(int fd, int ep) {
	int rv = ioctl(fd, USB_RAW_IOCTL_EP_SET_HALT, ep);
	if (rv < 0) {
		perror("ioctl(USB_RAW_IOCTL_EP_SET_HALT)");
		exit(EXIT_FAILURE);
	}
}

/*----------------------------------------------------------------------*/

void log_control_request(struct usb_ctrlrequest *ctrl) {
	printf("  bRequestType: 0x%x (%s), bRequest: 0x%x, wValue: 0x%x,"
		" wIndex: 0x%x, wLength: %d\n", ctrl->bRequestType,
		(ctrl->bRequestType & USB_DIR_IN) ? "IN" : "OUT",
		ctrl->bRequest, ctrl->wValue, ctrl->wIndex, ctrl->wLength);

	switch (ctrl->bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		printf("  type = USB_TYPE_STANDARD\n");
		break;
	case USB_TYPE_CLASS:
		printf("  type = USB_TYPE_CLASS\n");
		break;
	case USB_TYPE_VENDOR:
		printf("  type = USB_TYPE_VENDOR\n");
		break;
	default:
		printf("  type = unknown = %d\n", (int)ctrl->bRequestType);
		break;
	}

	switch (ctrl->bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		switch (ctrl->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			printf("  req = USB_REQ_GET_DESCRIPTOR\n");
			switch (ctrl->wValue >> 8) {
			case USB_DT_DEVICE:
				printf("  desc = USB_DT_DEVICE\n");
				break;
			case USB_DT_CONFIG:
				printf("  desc = USB_DT_CONFIG\n");
				break;
			case USB_DT_STRING:
				printf("  desc = USB_DT_STRING\n");
				break;
			case USB_DT_INTERFACE:
				printf("  desc = USB_DT_INTERFACE\n");
				break;
			case USB_DT_ENDPOINT:
				printf("  desc = USB_DT_ENDPOINT\n");
				break;
			case USB_DT_DEVICE_QUALIFIER:
				printf("  desc = USB_DT_DEVICE_QUALIFIER\n");
				break;
			case USB_DT_OTHER_SPEED_CONFIG:
				printf("  desc = USB_DT_OTHER_SPEED_CONFIG\n");
				break;
			case USB_DT_INTERFACE_POWER:
				printf("  desc = USB_DT_INTERFACE_POWER\n");
				break;
			case USB_DT_OTG:
				printf("  desc = USB_DT_OTG\n");
				break;
			case USB_DT_DEBUG:
				printf("  desc = USB_DT_DEBUG\n");
				break;
			case USB_DT_INTERFACE_ASSOCIATION:
				printf("  desc = USB_DT_INTERFACE_ASSOCIATION\n");
				break;
			case USB_DT_SECURITY:
				printf("  desc = USB_DT_SECURITY\n");
				break;
			case USB_DT_KEY:
				printf("  desc = USB_DT_KEY\n");
				break;
			case USB_DT_ENCRYPTION_TYPE:
				printf("  desc = USB_DT_ENCRYPTION_TYPE\n");
				break;
			case USB_DT_BOS:
				printf("  desc = USB_DT_BOS\n");
				break;
			case USB_DT_DEVICE_CAPABILITY:
				printf("  desc = USB_DT_DEVICE_CAPABILITY\n");
				break;
			case USB_DT_WIRELESS_ENDPOINT_COMP:
				printf("  desc = USB_DT_WIRELESS_ENDPOINT_COMP\n");
				break;
			case USB_DT_PIPE_USAGE:
				printf("  desc = USB_DT_PIPE_USAGE\n");
				break;
			case USB_DT_SS_ENDPOINT_COMP:
				printf("  desc = USB_DT_SS_ENDPOINT_COMP\n");
				break;
			case HID_DT_HID:
				printf("  descriptor = HID_DT_HID\n");
				return;
			case HID_DT_REPORT:
				printf("  descriptor = HID_DT_REPORT\n");
				return;
			case HID_DT_PHYSICAL:
				printf("  descriptor = HID_DT_PHYSICAL\n");
				return;
			default:
				printf("  desc = unknown = 0x%x\n",
							ctrl->wValue >> 8);
				break;
			}
			break;
		case USB_REQ_SET_CONFIGURATION:
			printf("  req = USB_REQ_SET_CONFIGURATION\n");
			break;
		case USB_REQ_GET_CONFIGURATION:
			printf("  req = USB_REQ_GET_CONFIGURATION\n");
			break;
		case USB_REQ_SET_INTERFACE:
			printf("  req = USB_REQ_SET_INTERFACE\n");
			break;
		case USB_REQ_GET_INTERFACE:
			printf("  req = USB_REQ_GET_INTERFACE\n");
			break;
		case USB_REQ_GET_STATUS:
			printf("  req = USB_REQ_GET_STATUS\n");
			break;
		case USB_REQ_CLEAR_FEATURE:
			printf("  req = USB_REQ_CLEAR_FEATURE\n");
			break;
		case USB_REQ_SET_FEATURE:
			printf("  req = USB_REQ_SET_FEATURE\n");
			break;
		default:
			printf("  req = unknown = 0x%x\n", ctrl->bRequest);
			break;
		}
		break;
	case USB_TYPE_CLASS:
		switch (ctrl->bRequest) {
		case HID_REQ_GET_REPORT:
			printf("  req = HID_REQ_GET_REPORT\n");
			break;
		case HID_REQ_GET_IDLE:
			printf("  req = HID_REQ_GET_IDLE\n");
			break;
		case HID_REQ_GET_PROTOCOL:
			printf("  req = HID_REQ_GET_PROTOCOL\n");
			break;
		case HID_REQ_SET_REPORT:
			printf("  req = HID_REQ_SET_REPORT\n");
			break;
		case HID_REQ_SET_IDLE:
			printf("  req = HID_REQ_SET_IDLE\n");
			break;
		case HID_REQ_SET_PROTOCOL:
			printf("  req = HID_REQ_SET_PROTOCOL\n");
			break;
		default:
			printf("  req = unknown = 0x%x\n", ctrl->bRequest);
			break;
		}
		break;
	default:
		printf("  req = unknown = 0x%x\n", ctrl->bRequest);
		break;
	}
}

void log_event(struct usb_raw_event *event) {
	switch (event->type) {
	case USB_RAW_EVENT_CONNECT:
		printf("event: connect, length: %u\n", event->length);
		break;
	case USB_RAW_EVENT_CONTROL:
		printf("event: control, length: %u\n", event->length);
		log_control_request((struct usb_ctrlrequest *)&event->data[0]);
		break;
	default:
		printf("event: unknown, length: %u\n", event->length);
	}
}

/*----------------------------------------------------------------------*/

#define BCD_USB		0x0200

#define USB_VENDOR	0x046d
#define USB_PRODUCT	0xc312 

#define STRING_ID_MANUFACTURER	0
#define STRING_ID_PRODUCT	1
#define STRING_ID_SERIAL	2
#define STRING_ID_CONFIG	3
#define STRING_ID_INTERFACE	4

#define EP_MAX_PACKET_CONTROL	64
#define EP_MAX_PACKET_INT	8

// Assigned dynamically.
#define EP_NUM_INT_IN	0x0

struct usb_device_descriptor usb_device = {
	.bLength =		USB_DT_DEVICE_SIZE,
	.bDescriptorType =	USB_DT_DEVICE,
	.bcdUSB =		__constant_cpu_to_le16(BCD_USB),
	.bDeviceClass =		0,
	.bDeviceSubClass =	0,
	.bDeviceProtocol =	0,
	.bMaxPacketSize0 =	EP_MAX_PACKET_CONTROL,
	.idVendor =		__constant_cpu_to_le16(USB_VENDOR),
	.idProduct =		__constant_cpu_to_le16(USB_PRODUCT),
	.bcdDevice =		0,
	.iManufacturer =	STRING_ID_MANUFACTURER,
	.iProduct =		STRING_ID_PRODUCT,
	.iSerialNumber =	STRING_ID_SERIAL,
	.bNumConfigurations =	1,
};

struct usb_qualifier_descriptor usb_qualifier = {
	.bLength =		sizeof(struct usb_qualifier_descriptor),
	.bDescriptorType =	USB_DT_DEVICE_QUALIFIER,
	.bcdUSB =		__constant_cpu_to_le16(BCD_USB),
	.bDeviceClass =		0,
	.bDeviceSubClass =	0,
	.bDeviceProtocol =	0,
	.bMaxPacketSize0 =	EP_MAX_PACKET_CONTROL,
	.bNumConfigurations =	1,
	.bRESERVED =		0,
};

struct usb_config_descriptor usb_config = {
	.bLength =		USB_DT_CONFIG_SIZE,
	.bDescriptorType =	USB_DT_CONFIG,
	.wTotalLength =		0,  // computed later
	.bNumInterfaces =	1,
	.bConfigurationValue =	1,
	.iConfiguration = 	STRING_ID_CONFIG,
	.bmAttributes =		USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower =		0x32,
};

struct usb_interface_descriptor usb_interface = {
	.bLength =		USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bInterfaceNumber =	0,
	.bAlternateSetting =	0,
	.bNumEndpoints =	1,
	.bInterfaceClass =	USB_CLASS_HID,
	.bInterfaceSubClass =	1,
	.bInterfaceProtocol =	1,
	.iInterface =		STRING_ID_INTERFACE,
};

struct usb_endpoint_descriptor usb_endpoint = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN | EP_NUM_INT_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	EP_MAX_PACKET_INT,
	.bInterval =		5,
};

char usb_hid_report[] = {
	0x05, 0x01,                    // Usage Page (Generic Desktop)        0
	0x09, 0x06,                    // Usage (Keyboard)                    2
	0xa1, 0x01,                    // Collection (Application)            4
	0x05, 0x07,                    //  Usage Page (Keyboard)              6
	0x19, 0xe0,                    //  Usage Minimum (224)                8
	0x29, 0xe7,                    //  Usage Maximum (231)                10
	0x15, 0x00,                    //  Logical Minimum (0)                12
	0x25, 0x01,                    //  Logical Maximum (1)                14
	0x75, 0x01,                    //  Report Size (1)                    16
	0x95, 0x08,                    //  Report Count (8)                   18
	0x81, 0x02,                    //  Input (Data,Var,Abs)               20
	0x95, 0x01,                    //  Report Count (1)                   22
	0x75, 0x08,                    //  Report Size (8)                    24
	0x81, 0x01,                    //  Input (Cnst,Arr,Abs)               26
	0x95, 0x03,                    //  Report Count (3)                   28
	0x75, 0x01,                    //  Report Size (1)                    30
	0x05, 0x08,                    //  Usage Page (LEDs)                  32
	0x19, 0x01,                    //  Usage Minimum (1)                  34
	0x29, 0x03,                    //  Usage Maximum (3)                  36
	0x91, 0x02,                    //  Output (Data,Var,Abs)              38
	0x95, 0x05,                    //  Report Count (5)                   40
	0x75, 0x01,                    //  Report Size (1)                    42
	0x91, 0x01,                    //  Output (Cnst,Arr,Abs)              44
	0x95, 0x06,                    //  Report Count (6)                   46
	0x75, 0x08,                    //  Report Size (8)                    48
	0x15, 0x00,                    //  Logical Minimum (0)                50
	0x26, 0xff, 0x00,              //  Logical Maximum (255)              52
	0x05, 0x07,                    //  Usage Page (Keyboard)              55
	0x19, 0x00,                    //  Usage Minimum (0)                  57
	0x2a, 0xff, 0x00,              //  Usage Maximum (255)                59
	0x81, 0x00,                    //  Input (Data,Arr,Abs)               62
	0xc0,                          // End Collection                      64
};

struct hid_descriptor usb_hid = {
	.bLength =		9,
	.bDescriptorType =	HID_DT_HID,
	.bcdHID =		__constant_cpu_to_le16(0x0110),
	.bCountryCode =		0,
	.bNumDescriptors =	1,
	.desc =			{
		{
			.bDescriptorType =	HID_DT_REPORT,
			.wDescriptorLength =	sizeof(usb_hid_report),
		}
	},
};

int build_config(char *data, int length) {
	struct usb_config_descriptor *config =
		(struct usb_config_descriptor *)data;
	int total_length = 0;

	assert(length >= sizeof(usb_config));
	memcpy(data, &usb_config, sizeof(usb_config));
	data += sizeof(usb_config);
	length -= sizeof(usb_config);
	total_length += sizeof(usb_config);

	assert(length >= sizeof(usb_interface));
	memcpy(data, &usb_interface, sizeof(usb_interface));
	data += sizeof(usb_interface);
	length -= sizeof(usb_interface);
	total_length += sizeof(usb_interface);

	assert(length >= sizeof(usb_hid));
	memcpy(data, &usb_hid, sizeof(usb_hid));
	data += sizeof(usb_hid);
	length -= sizeof(usb_hid);
	total_length += sizeof(usb_hid);

	assert(length >= USB_DT_ENDPOINT_SIZE);
	memcpy(data, &usb_endpoint, USB_DT_ENDPOINT_SIZE);
	data += USB_DT_ENDPOINT_SIZE;
	length -= USB_DT_ENDPOINT_SIZE;
	total_length += USB_DT_ENDPOINT_SIZE;

	config->wTotalLength = __cpu_to_le16(total_length);
	printf("config->wTotalLength: %d\n", total_length);

	return total_length;
}

/*----------------------------------------------------------------------*/

bool assign_ep_address(struct usb_raw_ep_info *info,
				struct usb_endpoint_descriptor *ep) {
	if (usb_endpoint_num(ep) != 0)
		return false;  // Already assigned.
	if (usb_endpoint_dir_in(ep) && !info->caps.dir_in)
		return false;
	if (usb_endpoint_dir_out(ep) && !info->caps.dir_out)
		return false;
	switch (usb_endpoint_type(ep)) {
	case USB_ENDPOINT_XFER_BULK:
		if (!info->caps.type_bulk)
			return false;
		break;
	case USB_ENDPOINT_XFER_INT:
		if (!info->caps.type_int)
			return false;
		break;
	default:
		assert(false);
	}
	if (info->addr == USB_RAW_EP_ADDR_ANY) {
		static int addr = 1;
		ep->bEndpointAddress |= addr++;
	} else
		ep->bEndpointAddress |= info->addr;
	return true;
}

void process_eps_info(int fd) {
	struct usb_raw_eps_info info;
	memset(&info, 0, sizeof(info));

	int num = usb_raw_eps_info(fd, &info);
	for (int i = 0; i < num; i++) {
		printf("ep #%d:\n", i);
		printf("  name: %s\n", &info.eps[i].name[0]);
		printf("  addr: %u\n", info.eps[i].addr);
		printf("  type: %s %s %s\n",
			info.eps[i].caps.type_iso ? "iso" : "___",
			info.eps[i].caps.type_bulk ? "blk" : "___",
			info.eps[i].caps.type_int ? "int" : "___");
		printf("  dir : %s %s\n",
			info.eps[i].caps.dir_in ? "in " : "___",
			info.eps[i].caps.dir_out ? "out" : "___");
		printf("  maxpacket_limit: %u\n",
			info.eps[i].limits.maxpacket_limit);
		printf("  max_streams: %u\n", info.eps[i].limits.max_streams);
	}

	for (int i = 0; i < num; i++) {
		if (assign_ep_address(&info.eps[i], &usb_endpoint))
			continue;
	}

	int int_in_addr = usb_endpoint_num(&usb_endpoint);
	assert(int_in_addr != 0);
	printf("int_in: addr = %u\n", int_in_addr);
}

/*----------------------------------------------------------------------*/

#define EP0_MAX_DATA 256

struct usb_raw_control_event {
	struct usb_raw_event		inner;
	struct usb_ctrlrequest		ctrl;
};

struct usb_raw_control_io {
	struct usb_raw_ep_io		inner;
	char				data[EP0_MAX_DATA];
};

struct usb_raw_int_io {
	struct usb_raw_ep_io		inner;
	char				data[EP_MAX_PACKET_INT];
};

int ep_int_in = -1;

bool ep0_request(int fd, struct usb_raw_control_event *event,
				struct usb_raw_control_io *io, bool *done) {
	switch (event->ctrl.bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		switch (event->ctrl.bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			switch (event->ctrl.wValue >> 8) {
			case USB_DT_DEVICE:
				memcpy(&io->data[0], &usb_device,
							sizeof(usb_device));
				io->inner.length = sizeof(usb_device);
				return true;
			case USB_DT_DEVICE_QUALIFIER:
				memcpy(&io->data[0], &usb_qualifier,
							sizeof(usb_qualifier));
				io->inner.length = sizeof(usb_qualifier);
				return true;
			case USB_DT_CONFIG:
				io->inner.length =
					build_config(&io->data[0],
						sizeof(io->data));
				return true;
			case USB_DT_STRING:
				io->data[0] = 4;
				io->data[1] = USB_DT_STRING;
				if ((event->ctrl.wValue & 0xff) == 0) {
					io->data[2] = 0x09;
					io->data[3] = 0x04;
				} else {
					io->data[2] = 'x';
					io->data[3] = 0x00;
				}
				io->inner.length = 4;
				return true;
			case HID_DT_REPORT:
				memcpy(&io->data[0], &usb_hid_report[0],
							sizeof(usb_hid_report));
				io->inner.length = sizeof(usb_hid_report);
				return true;
			default:
				printf("fail: no response\n");
				exit(EXIT_FAILURE);
			}
			break;
		case USB_REQ_SET_CONFIGURATION:
			ep_int_in = usb_raw_ep_enable(fd, &usb_endpoint);
			usb_raw_vbus_draw(fd, usb_config.bMaxPower);
			usb_raw_configure(fd);
			io->inner.length = 0;
			return true;
		case USB_REQ_GET_INTERFACE:
			io->data[0] = usb_interface.bAlternateSetting;
			io->inner.length = 1;
			return true;
		default:
			printf("fail: no response\n");
			exit(EXIT_FAILURE);
		}
		break;
	case USB_TYPE_CLASS:
		switch (event->ctrl.bRequest) {
		case HID_REQ_SET_REPORT:
			io->inner.length = 1;
			*done = true;
			return true;
		case HID_REQ_SET_IDLE:
			io->inner.length = 0;
			return true;
		case HID_REQ_SET_PROTOCOL:
			io->inner.length = 0;
			*done = true;
			return true;
		default:
			printf("fail: no response\n");
			exit(EXIT_FAILURE);
		}
		break;
	case USB_TYPE_VENDOR:
		switch (event->ctrl.bRequest) {
		default:
			printf("fail: no response\n");
			exit(EXIT_FAILURE);
		}
		break;
	default:
		printf("fail: no response\n");
		exit(EXIT_FAILURE);
	}
}

// Not technically a loop as we stop processing ep0 requests to make the
// example simpler.
void ep0_loop(int fd) {
	bool done = false;
	while (!done) {
		struct usb_raw_control_event event;
		event.inner.type = 0;
		event.inner.length = sizeof(event.ctrl);

		usb_raw_event_fetch(fd, (struct usb_raw_event *)&event);
		log_event((struct usb_raw_event *)&event);

		if (event.inner.type == USB_RAW_EVENT_CONNECT)
			process_eps_info(fd);

		if (event.inner.type != USB_RAW_EVENT_CONTROL)
			continue;

		struct usb_raw_control_io io;
		io.inner.ep = 0;
		io.inner.flags = 0;
		io.inner.length = 0;

		bool reply = ep0_request(fd, &event, &io, &done);
		if (!reply) {
			printf("ep0: stalling\n");
			usb_raw_ep0_stall(fd);
			continue;
		}

		if (event.ctrl.wLength < io.inner.length)
			io.inner.length = event.ctrl.wLength;
		int rv = -1;
		if (event.ctrl.bRequestType & USB_DIR_IN) {
			rv = usb_raw_ep0_write(fd, (struct usb_raw_ep_io *)&io);
			printf("ep0: transferred %d bytes (in)\n", rv);
		} else {
			rv = usb_raw_ep0_read(fd, (struct usb_raw_ep_io *)&io);
			printf("ep0: transferred %d bytes (out)\n", rv);
		}
	}
}

void ep_int_in_loop(int fd) {
	struct usb_raw_int_io io;
	io.inner.ep = ep_int_in;
	io.inner.flags = 0;
	io.inner.length = 8;

	while (true) {
		memcpy(&io.inner.data[0],
				"\x00\x00\x1b\x00\x00\x00\x00\x00", 8);
		int rv = usb_raw_ep_write(fd, (struct usb_raw_ep_io *)&io);
		printf("int_in: key down: %d\n", rv);

		memcpy(&io.inner.data[0],
				"\x00\x00\x00\x00\x00\x00\x00\x00", 8);
		rv = usb_raw_ep_write(fd, (struct usb_raw_ep_io *)&io);
		printf("int_in: key up: %d\n", rv);

		sleep(1);
	}
}

int main(int argc, char **argv) {
	const char *device = "dummy_udc.0";
	const char *driver = "dummy_udc";
	if (argc >= 2)
		device = argv[1];
	if (argc >= 3)
		driver = argv[2];

	int fd = usb_raw_open();
	usb_raw_init(fd, USB_SPEED_HIGH, driver, device);
	usb_raw_run(fd);

	ep0_loop(fd);
	ep_int_in_loop(fd);

	close(fd);

	return 0;
}
