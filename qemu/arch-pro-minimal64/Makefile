SRCDIR           ?= $(dir $(firstword $(MAKEFILE_LIST)))

O                ?= $(PWD)/build
P                ?= $(O)/install

# Use ARM Linux toolchain instead of bare-metal
CC               := aarch64-none-linux-gnu-gcc
CXX              := aarch64-none-linux-gnu-g++
LD               := aarch64-none-linux-gnu-g++
OBJCPY           := aarch64-none-linux-gnu-objcopy

# Compilation flags for ARM 32-bit Linux  
CFLAGS           := -O0 -static
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
