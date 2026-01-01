#!/bin/bash

CURRENT_FILE=$(realpath "$0")
SCRIPT_DIR=$(dirname "$CURRENT_FILE")
JSON_CFG="$SCRIPT_DIR/mmio_config.json"

# Source settings.bash if it exists
if [[ -f "$SCRIPT_DIR/settings.bash" ]]; then
    echo "Using settings.bash from: $SCRIPT_DIR"
    . "$SCRIPT_DIR/settings.bash"
else
    echo "Error: settings.bash not found in $SCRIPT_DIR"
    exit 1
fi

mkdir -p "$SCRIPT_DIR/out"

if [[ ! -f "$JSON_CFG" ]]; then
    echo "Error: mmio_config.json not found in $SCRIPT_DIR"
    exit 1
fi

export JSON_CFG_PATH="$JSON_CFG"
export PUT_DIR="$SCRIPT_DIR"

./afl-fuzz -i $SCRIPT_DIR/seeds -o $SCRIPT_DIR/out -m none -v -- $SCRIPT_DIR/Robot.cfg
#gdb --args ./afl-fuzz -i $SCRIPT_DIR/seeds -o $SCRIPT_DIR/out -m none -v -- $SCRIPT_DIR/Robot.cfg