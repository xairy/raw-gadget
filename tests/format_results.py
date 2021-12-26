#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0

import errno
import json
import os
import sys

def read_log(filename):
	with open(filename, 'r') as f:
		data = f.read()
		return json.loads(data)

def format_results(raw_filename, zero_filename):
	raw = read_log(raw_filename)
	zero = read_log(zero_filename)
	assert(len(raw) == len(zero))

	print("| Test | `raw_gadget` | `g_zero` | Status |")
	print("| :--- | :---: | :---: | :---: |")

	for i in range(len(raw)):
		assert(raw[i]["test"] == zero[i]["test"])
		test = raw[i]["test"]
		raw_status = "" if raw[i]["code"] == 0 \
			else errno.errorcode[raw[i]["code"]]
		zero_status = "" if zero[i]["code"] == 0 \
			else errno.errorcode[zero[i]["code"]]
		status = "**FAIL**"
		if raw[i]["code"] == 0:
			status = "OK"
		elif raw[i]["code"] == zero[i]["code"]:
			status = "OK"
		print("| %s | %s | %s | %s |" % \
			(test, raw_status, zero_status, status))

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print("Usage: %s <RAW_GADGET.LOG> <G_ZERO.LOG>" % (sys.argv[0],))
		sys.exit(-1)
	format_results(sys.argv[1], sys.argv[2])
