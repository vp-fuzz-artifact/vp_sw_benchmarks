#!/bin/bash

gdb-multiarch -ex 'set architecture armv8-m.base' -ex 'file ~/usima-testing/benchmark/cortex-M0/zephyr/uart_polling/build/zephyr.elf' -ex 'target remote :5555' 

