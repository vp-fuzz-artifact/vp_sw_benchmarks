#!/bin/bash

#BYTES='78 01 03 00 00 00 00 01 00 00 00 01'
BYTES='0x72, 0x61, 0x74, 0x73 0x00'
# Crashing seed
#BYTES='0x70, 0x61, 0x73, 0x73 0x00'

CLEANED=$(echo "$BYTES" | tr -d ' \t\n\r,')

# 2. Then change '0x' to '\x' for printf
ESCAPED=$(echo "$CLEANED" | sed 's/0x/\\x/g')

# Write binary file
printf "$ESCAPED" > seeds/seed_0