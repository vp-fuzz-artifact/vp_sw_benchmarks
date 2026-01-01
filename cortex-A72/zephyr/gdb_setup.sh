#!/bin/bash

gdb-multiarch -ex 'set architecture aarch64' -ex 'file ./babbling/build/zephyr.elf' -ex 'target remote :5555' -ex 'b can_tx_callback' -ex 'b main'
