#!/bin/bash

gdb-multiarch -ex 'set architecture armv8-m.base' -ex 'file ./build/main.elf' -ex 'target remote :5555' 

