#usage example
# make TOOLCHAIN_PATH=/usr/bin/ DEVICE_PATH=~/.cache/arm/packs/NordicSemiconductor/nRF_DeviceFamilyPack/8.44.1 CMSIS_CORE=~/.cache/arm/packs/ARM/CMSIS/6.2.0/CMSIS/Core/Include

.PHONY: all clean

# Path to the toolchain's 'bin' directory. Must end with a '/'.
TOOLCHAIN_PATH ?=

# Path to the specific Nordic device pack being used.
DEVICE_PACK ?= 

# Path to the CMSIS Core include directory.
CMSIS_CORE ?= 

SRC ?= src # Set the target dir of not already set
INC = include
SRC_DIR := $(abspath $(SRC))
INC_DIR := $(abspath $(INC))
BUILD_DIR = build
OUT = main

CPU = cortex-m0

TOOLCHAIN_PREFIX = arm-none-eabi-
CC      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
LD      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
OBJCOPY = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy

NRFX_ROOT = ./nrfx
CMSIS_CORE = $(CMSIS_PACK_ROOT)/ARM/CMSIS/6.2.0/CMSIS/Core/Include
DEVICE_INCLUDE = $(DEVICE_PACK)/Device/Include
STARTUP = $(NRFX_ROOT)/mdk/gcc_startup_nrf51.S 
LINKER_SCRIPT = $(NRFX_ROOT)/mdk/nrf51422_xxac.ld 

CFLAGS = -mcpu=$(CPU) -mthumb -std=c11 -Wall -g -O0
# Define chip macros to enable device-specific code sections.
CFLAGS += -DNRF51 -DNRF51422_XXAC 
# Add it to your existing flags
# CFLAGS += -D_USE_MATH_DEFINES

CFLAGS += -I$(INC_DIR) \
		-I$(CMSIS_CORE) \
		-I$(DEVICE_INCLUDE) \
		-I$(NRFX_ROOT) \
		-I$(NRFX_ROOT)/drivers \
		-I$(NRFX_ROOT)/drivers/include \
		-I$(NRFX_ROOT)/hal \
		-I$(NRFX_ROOT)/mdk \
		-I$(NRFX_ROOT)/templates \
		-Isrc/MPU6050

# Use the -L (search directory) linker flag to specify to look for additional .ld scripts
# -Wl, flag is used to pass options directly to the linker
LDFLAGS = -T$(LINKER_SCRIPT) -L $(NRFX_ROOT)/mdk/ -Wl,-lm,--gc-sections

SRC = $(SRC_DIR)/main.c \
		$(SRC_DIR)/peripherals.c \
		$(SRC_DIR)/circular_buffer.c \
		$(SRC_DIR)/i2c.c \
		$(SRC_DIR)/serial.c \
		$(SRC_DIR)/timing.c \
		$(SRC_DIR)/syscalls.c \
		$(NRFX_ROOT)/drivers/src/nrfx_uart.c \
		$(NRFX_ROOT)/drivers/src/nrfx_twi.c \
		$(NRFX_ROOT)/drivers/src/nrfx_timer.c \
		$(NRFX_ROOT)/mdk/system_nrf51.c \
		$(SRC_DIR)/MPU6050/MPU6050.c

# for every .c/.s file in SRC/STARTUP, make a corresponding .o file, OBJ = main.o nrfx_uart.o etc
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRC)))
ASM_OBJ := $(BUILD_DIR)/$(notdir $(STARTUP:.S=.o)) 

OUT_ELF := $(BUILD_DIR)/$(OUT).elf
OUT_HEX := $(BUILD_DIR)/$(OUT).hex
OUT_BIN := $(BUILD_DIR)/$(OUT).bin

# 1) When we type make, it builds main.elf
all: $(OUT_ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 3) For each object file (e.g., main.o), make searches for a rule to build it:
# it runs this command to build any .o file from a .c file of the same name
# $< is the first prerequisite (the .c file), and $@ is the target (the .o file).
# Rule to compile C files into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(NRFX_ROOT)/drivers/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(NRFX_ROOT)/mdk/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile C files from the MPU6050 subdirectory
$(BUILD_DIR)/%.o: $(SRC_DIR)/MPU6050/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_OBJ): $(STARTUP) | $(BUILD_DIR)
	$(CC) -mcpu=$(CPU) -mthumb -c $< -o $@

# 2)
$(OUT_ELF): $(OBJ) $(ASM_OBJ) # To build main.elf, we first need to build all .o files (from .c and .S)
	$(LD) $(CFLAGS) $(OBJ) $(ASM_OBJ) $(LDFLAGS) -o $@
	$(OBJCOPY) -O ihex $@ $(OUT_HEX)
	$(OBJCOPY) -O binary $@ $(OUT_BIN)



# Preprocesses a single C file to see the output after macro expansion.
# Usage: make preprocess FILE=path/to/your/file.c
preprocess:
	@if [ -z "$(FILE)" ]; then \
		echo "Error: Please specify a file. Usage: make preprocess FILE=<path_to_file.c>"; \
		exit 1; \
	fi
	@echo "Preprocessing $(FILE) and saving output to $(FILE:.c=.i)..."
	$(CC) $(CFLAGS) -E $(FILE) -o $(FILE:.c=.i)






clean:
	rm -rf $(BUILD_DIR)

