# Build ROM for Tiny68k

TOOL_PREFIX		 = m68k-unknown-elf-
CC			 = $(TOOL_PREFIX)cc
OBJCOPY			 = $(TOOL_PREFIX)objcopy
SIZE			 = $(TOOL_PREFIX)size

LIBDIRS			 = ihr newlib pffs

LIBGCC			:= $(shell $(CC) --print-file-name libgcc.a)
COMPILER_INCLUDES	 = $(dir $(LIBGCC))include

GITHASH			:= $(shell git describe --always --dirty=-modified)

OPTS			 = -I$(COMPILER_INCLUDES) \
			   -DGITHASH=$(GITHASH) \
			   -DAUTO_SCRIPT="$(AUTO_SCRIPT)\0" \
			   -std=gnu17 \
			   -Wall \
			   -Wextra \
			   -Werror \
			   -Os \
			   -g \
			   -fomit-frame-pointer \
			   -ffreestanding \
			   -nostdinc \
			   -nostdlib \
			   -ffunction-sections \
			   -fdata-sections \
			   -Wl,-gc-sections \
			   -Wl,-Map=$(board).map \
			   -T $(board).ld

OPTS_tiny68k		 = -mcpu=68000 \
			   -DWITH_STARTUP_RELOCATION \
			   -DWITH_SRECORD \
			   -DWITH_MINIX \
			   -DWITH_EMUTOS \
			   -DWITH_MEM_CMDS \
			   -DWITH_TESTS

OPTS_t68krc		 = -mcpu=68000 \
			   -DWITH_STARTUP_RELOCATION

OPTS_p90mb		 = -mcpu=68010 \
			   -DWITH_STARTUP_DATA_COPY \
			   -DWITH_SRECORD \
			   -DWITH_MEM_CMDS \
			   -DWITH_TESTS

OVERRIDES		?=

CSRCS			:= $(wildcard *.c)
ASRCS			:= $(wildcard *.S)
HDRS			:= $(wildcard *.h)
LIBSRCS			:= $(foreach dir,$(LIBDIRS),$(foreach suffix,c S,$(wildcard $(dir)/*.$(suffix))))

ALL_SRCS		:= $(CSRCS) $(ASRCS) $(LIBSRCS)
FORMAT_SRCS		:= $(CSRCS) $(HDRS)

BOARDS			:= $(patsubst %.ld, %, $(wildcard *.ld))
ROMS			 = $(addsuffix .rom, $(BOARDS))
ELFS			 = $(addsuffix .elf, $(BOARDS))
SRECS			 = $(addsuffix .s19, $(BOARDS))
MAPS			 = $(addsuffix .map, $(BOARDS))

.PHONY: all $(BOARDS)
all:	$(ROMS) $(SRECS)
$(BOARDS):
	@$(MAKE) $@.rom

%.rom: %.elf
	$(OBJCOPY) -O binary $< $@

%.s19: %.elf
	$(OBJCOPY) -O srec $< $@

$(ELFS): board=$(basename $@)
$(ELFS): $(ALL_SRCS) $(HDRS) $(@:.elf.ld) $(MAKEFILE_LIST)
	@echo ==== BUILD: $(board)
	$(CC) -DCONFIG_BOARD_$(board) $(OPTS_$(board)) $(OPTS) $(OVERRIDES) -o $@ $(ALL_SRCS) $(LIBGCC)
	$(SIZE) $@

clean:
	rm -f $(ROMS) $(ELFS) $(SRECS) $(MAPS)

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
