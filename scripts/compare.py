#!/usr/bin/python3

import sys

if len(sys.argv) != 3:
  print("Usage: compare.py output_dir1 output_dir2")
  exit(0)

in1 = sys.argv[1]
in2 = sys.argv[2]

total = 0
for i in range(0, 20):
  f1 = open(in1 + "/output{}.txt".format(i), "r")
  f2 = open(in2 + "/output{}.txt".format(i), "r")

  res1 = float(f1.readline().split()[0])
  res2 = float(f2.readline().split()[0])

  delta = res1 - res2
  total += delta

  print("{} \t\t delta: {}".format(i, delta))

print("total change: {}".format(total))
