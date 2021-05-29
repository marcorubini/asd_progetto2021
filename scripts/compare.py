#!/usr/bin/python3

import sys

if len(sys.argv) != 3:
  print("Usage: compare.py output_dir1 output_dir2")
  exit(0)

in1 = sys.argv[1]
in2 = sys.argv[2]

total_glove = 0
total_energy = 0
total_time = 0

for i in range(0, 20):
  f1 = open(in1 + "/output{}.txt".format(i), "r")
  f2 = open(in2 + "/output{}.txt".format(i), "r")

  l1 = f1.readline().split(' ')
  l2 = f2.readline().split(' ')

  l1 = [float(x) for x in l1]
  l2 = [float(x) for x in l2]

  e1, g1, t1 = (l1[0], l1[1], l1[2])
  e2, g2, t2 = (l2[0], l2[1], l2[2])

  total_energy += e1 - e2
  total_glove += g1 - g2
  total_time += t1 - t2


  print("{:3} \t glove: {:8.3f} \t energy: {:8.3f} \t time: {:8.3f}".format(i, e1-e2, g1-g2, t1-t2))

print("total energy delta (final score): {}".format(total_energy))
print("total glove delta (accumulated energy): {}".format(total_glove))
print("total time delta: {}".format(total_time))
