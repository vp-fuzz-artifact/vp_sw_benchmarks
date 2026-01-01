#!/bin/bash

# Use the correct 32-bit GDB from your toolchain
GDB="arm-none-eabi-gdb"

# ELF File Path
ELF="./build/main.elf"

# Run GDB
# 1. Load the symbols from the ELF file
# 2. Connect to the simulator
$GDB $ELF -ex 'target remote :5555'