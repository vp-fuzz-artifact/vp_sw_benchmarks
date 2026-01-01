#!/bin/bash

#Good seeds:
#BYTES='78 01 ab ab ab ab ab ab ab ab 03 19 00 14 f2'
#BYTES='0B 78 9C 73 74 1C 00 00 04 46 01 0B'
BYTES='0D 78 01 73 74 72 1C 16 10 00 18 88 33 2D'

#Crashing seed:
#BYTES='0F 78 9C 73 74 1C 05 A3 80 FA 00 00 E0 12 98 59'

ESCAPED=$(echo "$BYTES" | sed 's/ /\\x/g')

printf "\\x$ESCAPED" > seeds/seed_0

