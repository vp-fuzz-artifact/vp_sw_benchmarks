
SRCDIR           ?= $(dir $(firstword $(MAKEFILE_LIST)))

O                ?= $(PWD)/build
P                ?= $(O)/install

CC               := arm-none-eabi-gcc
CXX              := arm-none-eabi-g++ 
LD               := arm-none-eabi-g++
OBJCPY           := arm-none-eabi-objcopy

# The -g is meant for debugging purposes, take out if not interested
CFLAGS           := -ffreestanding -mcpu=cortex-m0 -O0 -mthumb -g
LFLAGS           := -nostartfiles -T link.ld -Wl,--build-id=none -O0
CXXFLAGS         := $(CFLAGS)

PROGRAM          := $(O)/main.elf
BIN_PROGRAM      := $(O)/main.bin
OBJECTS          := $(O)/boot.o      \
                    $(O)/main.o    \

.PHONY: all clean install install_dirs install_program

all: $(PROGRAM)

install: $(PROGRAM)
	$(S) echo -e "  INSTALL\t$(notdir $(PROGRAM))"
	$(S) install -m 755 $(PROGRAM) $(P)
	$(S) echo -e "  INSTALL\t$(notdir $(BIN_PROGRAM))"
	$(S) install -m 755 $(BIN_PROGRAM) $(P)

clean:
	$(S) echo -e "  CLEAN"
	$(S) $(RM) $(OBJECTS) $(PROGRAM) $(BIN_PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(S) echo -e "  LINK\t$(notdir $@)"
	$(S) $(LD) $(LFLAGS) -o $@ $^
	$(S) $(OBJCPY) -O binary $(PROGRAM) $(BIN_PROGRAM)

$(O)/%.o: %.cpp
	$(S) echo -e "  CXX\t$(notdir $@)"
	$(S) mkdir -p $(dir $@)
	$(S) $(CXX) $(CXXFLAGS) -c -o $@ $<

$(O)/%.o: %.S
	$(S) echo -e "  CC\t$(notdir $@)"
	$(S) mkdir -p $(dir $@)
	$(S) $(CC) $(CFLAGS) -c -o $@ $<
