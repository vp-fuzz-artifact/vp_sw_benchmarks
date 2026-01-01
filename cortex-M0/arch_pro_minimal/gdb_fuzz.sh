#!/bin/bash

# Absolute path of $0 (aka gdb_fuzz.sh)
CURRENT_FILE=$(realpath "$0")
SCRIPT_DIR=$(dirname "$CURRENT_FILE")

echo "Using settings.bash from: $SCRIPT_DIR"

if [ ! -f "$SCRIPT_DIR/settings.bash" ]; then
    echo "Error: settings.bash not found in $SCRIPT_DIR"
    exit 1
fi


. "$SCRIPT_DIR/settings.bash"

mkdir -p $SCRIPT_DIR/out

gdb --args "$(pwd)"/afl-fuzz -i "$SCRIPT_DIR/seeds" -o "$SCRIPT_DIR/out" -m none -v -- "$SCRIPT_DIR/arch_pro_minimal.cfg"
