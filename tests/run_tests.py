#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0

import errno
import json
import os
import subprocess
import sys

tests = [
	("test 1: bulk, non-queued, OUT", 1, {}),
	("test 2: bulk, non-queued, IN", 2, {}),
	("test 3: bulk, non-queued, OUT, varied", 3, {"vary": 421}),
	("test 4: bulk, non-queued, IN, varied", 4, {"vary": 421}),
	("test 5: bulk, queued, OUT", 5, {}),
	("test 6: bulk, queued, IN", 6, {}),
	("test 7: bulk, queued, OUT, varied", 7, {"vary": 421}),
	("test 8: bulk, queued, IN, varied", 8, {"vary": 421}),
	("test 9: control, non-queued, sanity", 9, {}),
	("test 10: control, queued", 10, {}),
	("test 11: bulk, non-queued, unlinks, IN", 11, {}),
	("test 12: bulk, non-queued, unlinks, OUT", 12, {}),
	("test 13: bulk, ep halt set/clear", 13, {}),
	("test 14: control, OUT, varied", 14, {"length": 256, "vary": 8}),
	# Tests 15 and 16 require ISO transfers support.
	("test 17: bulk, DMA, odd address, OUT", 17, {}),
	("test 18: bulk, DMA, odd address, IN", 18, {}),
	("test 19: bulk, coherent, odd address, OUT", 19, {}),
	("test 20: bulk, coherent, odd address, IN", 20, {}),
	("test 21: control, unaligned, OUT, varied", 21, {"length": 128, "vary": 8}),
	# Tests 22 and 23 require ISO transfers support.
	("test 24: bulk, queued, unlink, OUT", 24, {}),
	("test 25: interrupt, non-queued, OUT", 25, {"length": 64}),
	("test 26: interrupt, non-queued, IN", 26, {"length": 64}),
]

def run_test(device, test, count, **kwargs):
	length = kwargs.get("length", 1024)
	vary = kwargs.get("vary", 1024)
	sglen = kwargs.get("sglen", 32)
	args = ("./testusb",
		"-D", str(device),
		"-t", str(test),
		"-c", str(count),
		"-s", str(length),
		"-v", str(vary),
		"-g", str(sglen),
	)
	print(" ".join(args))
	r = subprocess.run(args)
	return r.returncode

def run_tests(device, count):
	codes = []
	for test in tests:
		print(test[0])
		r = run_test(device, test[1], count, **test[2])
		print("SUCCESS" if r == 0 else "FAILURE: %s" % (r,))
		codes.append(r)
	return codes

def save_results(codes, filename):
	results = []
	assert len(tests) == len(codes)
	for (i, c) in enumerate(codes):
		result = {}
		result["test"] = tests[i][0]
		result["code"] = c
		results.append(result)
	s = json.dumps(results, indent=4, sort_keys=True)
	with open(filename, 'w+') as f:
		f.write(s)
		f.write("\n")

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print("Usage: %s <DEVICE> <FILE>" % (sys.argv[0],))
		sys.exit(-1)
	r = run_tests(sys.argv[1], 8)
	save_results(r, sys.argv[2])
