# Build ROM for Tiny68k

TOOL_PREFIX		 = /Volumes/CTNG/dst/bin/m68k-unknown-elf-
CC			 = $(TOOL_PREFIX)cc
OBJCOPY			 = $(TOOL_PREFIX)objcopy
SIZE			 = $(TOOL_PREFIX)size
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

CSRCS			:= $(wildcard *.c)
ASRCS			:= $(wildcard *.S)
LIBSRCS			:= $(wildcard newlib/*.c) $(wildcard newlib/*.S) $(wildcard pffs/*.c)
HDRS			:= $(wildcard *.h)

ALL_SRCS		:= $(CSRCS) $(ASRCS) $(LIBSRCS)
FORMAT_SRCS		:= $(CSRCS) $(HDRS)

$(BINARY): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(CSRCS) $(ASRCS) $(HDRS) $(LINKER_SCRIPT) $(MAKEFILE_LIST)
	$(CC) $(OPTS) -T $(LINKER_SCRIPT) -o $@ $(ALL_SRCS) $(LIBGCC)
	$(SIZE) $@

clean:
	rm -f $(BINARY) $(ELF) $(MAP)

.PHONY: reformat
reformat:
	astyle \
	--style=kr \
	--indent=spaces=4 \
	--indent-cases \
	--indent-preprocessor \
	--break-blocks \
	--pad-oper \
	--pad-header \
	--unpad-paren \
	--add-brackets \
	--convert-tabs \
	--align-pointer=name \
	--keep-one-line-blocks \
	--formatted \
	--suffix=none \
	$(FORMAT_SRCS)
