# Build ROM for Tiny68k

TOOL_PREFIX		 = /Volumes/CTNG/dst/bin/m68k-unknown-elf-
CC			 = $(TOOL_PREFIX)cc
OBJCOPY			 = $(TOOL_PREFIX)objcopy
LINKER_SCRIPT		 = tiny68k.ld
PRODUCT			 = rom
BINARY			 = $(PRODUCT).bin
ELF			 = $(PRODUCT).elf
MAP			 = $(PRODUCT).map

LIBGCC			:= $(shell $(CC) --print-file-name libgcc.a)
COMPILER_INCLUDES	 = $(dir $(LIBGCC))include

GITHASH			:= $(shell git describe --always --dirty=-modified)

OPTS			 = -I$(COMPILER_INCLUDES) \
			   -DGITHASH=$(GITHASH) \
			   -DTEST=1 \
			   -mcpu=68000 \
			   -mshort \
			   -std=gnu17 \
			   -Wall \
			   -Wextra \
			   -Werror \
			   -Os \
			   -ffreestanding \
			   -nostdinc \
			   -nostdlib \
			   -ffunction-sections \
			   -fdata-sections \
			   -Wl,-gc-sections \
			   -Wl,-Map=$(MAP)

SRCS			:= $(wildcard *.c) $(wildcard *.S)
HDRS			:= $(wildcard *.h)

$(BINARY): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(SRCS) $(HDRS) $(LINKER_SCRIPT) $(MAKEFILE_LIST)
	$(CC) $(OPTS) -T $(LINKER_SCRIPT) -o $@ $(SRCS) $(LIBGCC)

clean:
	rm -f $(BINARY) $(ELF) $(MAP)
