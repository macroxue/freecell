#!/bin/bash

cut -d: -f2 block.* | awk '{print length/2}' | sort -n | uniq -c
cut -d: -f2 block.* | awk '{print length/2}' |\
  awk '{sum += $1; count++} END {print "sum", sum, "count", count, "average", sum/count}'

