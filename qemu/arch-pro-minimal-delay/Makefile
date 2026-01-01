SRCDIR           ?= $(dir $(firstword $(MAKEFILE_LIST)))

O                ?= $(PWD)/build
P                ?= $(O)/install

# Use ARM Linux toolchain instead of bare-metal
CC               := arm-none-linux-gnueabihf-gcc
CXX              := arm-none-linux-gnueabihf-g++
LD               := arm-none-linux-gnueabihf-g++
OBJCPY           := arm-none-linux-gnueabihf-objcopy

# Compilation flags for ARM 32-bit Linux  
CFLAGS           := -march=armv7 -O0 -static -mthumb
LFLAGS           := -Wl,--build-id=none -static
CXXFLAGS         := $(CFLAGS)

PROGRAM          := $(O)/main.elf
OBJECTS          := $(O)/main.o    \

.PHONY: all clean install install_dirs install_program

all: $(PROGRAM)

install: $(PROGRAM)
	$(S) echo -e "  INSTALL\t$(notdir $(PROGRAM))"
	$(S) install -m 755 $(PROGRAM) $(P)

clean:
	$(S) echo -e "  CLEAN"
	$(S) $(RM) $(OBJECTS) $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(S) echo -e "  LINK\t$(notdir $@)"
	$(S) $(LD) $(LFLAGS) -o $@ $^

$(O)/%.o: %.cpp
	$(S) echo -e "  CXX\t$(notdir $@)"
	$(S) mkdir -p $(dir $@)
	$(S) $(CXX) $(CXXFLAGS) -c -o $@ $<

$(O)/%.o: %.S
	$(S) echo -e "  CC\t$(notdir $@)"
	$(S) mkdir -p $(dir $@)
	$(S) $(CC) $(CFLAGS) -c -o $@ $<
