#!/bin/bash

gdb-multiarch -ex 'set architecture aarch64' -ex 'file ./build/main.elf' -ex 'target remote :5555'
