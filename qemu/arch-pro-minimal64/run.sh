#!/bin/bash

CURRENT_FILE=$(realpath "$0")
SCRIPT_DIR=$(dirname "$CURRENT_FILE")

echo "Script folder: $SCRIPT_DIR"

echo "#"
echo "#"
echo "# Comment: Qemu mode must be build with: export CPU_TARGET=arm and ./build_qemu_support.sh before running this!"
echo "#"
echo "#"

export AFL_SKIP_CPUFREQ=1
export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1

#export AFL_QEMU_PERSISTENT_EXITS=exit

#export AFL_NO_FORKSRV=1 what is this doing ?

#export QEMU_LD_PREFIX=/usr/arm-linux-gnueabihf
export AFL_QEMU_PERSISTENT_ADDR=0x0000401e8c
#export AFL_DEBUG=1
#export AFL_QEMU_PERSISTENT_HOOK=1
#export AFL_QEMU_PERSISTENT_CNT=1000
#export AFL_QEMU_PERSISTENT_GPR=1
#export AFL_QEMU_PERSISTENT_EXITS=0
export AFL_QEMU_PERSISTENT_RET=0x0008223e

#unset AFL_QEMU_PERSISTENT_ADDR

mkdir -p $SCRIPT_DIR/out

./afl-fuzz -i $SCRIPT_DIR/seeds -o $SCRIPT_DIR/out -m none -Q -- $SCRIPT_DIR/build/main.elf