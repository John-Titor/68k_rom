# Build ROM for various m68k boards

TOOL_PREFIX		 = m68k-unknown-elf-
CC			 = $(TOOL_PREFIX)cc
OBJCOPY			 = $(TOOL_PREFIX)objcopy
SIZE			 = $(TOOL_PREFIX)size

LIBDIRS			 = newlib pffs

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

# Startup options
# ===============
#
# WITH_STARTUP_RELOCATION
# 	Relocates from _reloc_source to _reloc_destination/_reloc_end at early
# 	startup, then jumps to the runtime address of the image. Useful for moving
# 	from a load area to a safe run area, or copying from slow ROM to faster RAM.
# WITH_STARTUP_DATA_COPY
# 	Copies from _reloc_source to _reloc_destination/_reloc_end at early 
# 	startup. Useful for copying non-constant .data from ROM to RAM.
# WITH_STARTUP_VECTOR_COPY
# 	Copies vectors from _vector_base to 0 at startup. Useful when vectors are
# 	in ROM but need to be in RAM, either so they can be written to or because
# 	the hardware does not support VBR. Mutually exclusive of WITH_VBR.
# WITH_VBR
# 	Sets %vbr to _vector_base at startup. Mutually exclusive of
# 	WITH_STARTUP_VECTOR_COPY
# STARTUP_TOUCH_ADDRESS=<address>
# 	Causes the early startup code to read from <address>, which is often
# 	used to disable ROM-over-RAM mapping.
# 	
# Feature options
# ===============
# 	
# WITH_NATIVE_FEATURES
# 	Enables support for the NatFeat emulator interface. Requires that
# 	vectors be writable.
# WITH_EMUTOS
# 	Enables EMUTOS boot support.
# WITH_MEM_CMDS
# 	Enables memory read/write commands.
# WITH_MINIX
# 	Enables Minix boot support.
# WITH_OSK
# 	Enables OS-9 / OSK boot support.
# WITH_SRECORD
# 	Enables S-record upload.
# WITH_TESTS
# 	Enables self-tests.
# 	

OPTS_tiny68k		 = -mcpu=68000 \
			   -DWITH_STARTUP_RELOCATION \
			   -DWITH_NATIVE_FEATURES \
			   -DWITH_EMUTOS \
			   -DWITH_MINIX \
			   -DWITH_MEM_CMDS \
			   -DWITH_SRECORD \
			   -DWITH_TESTS

OPTS_t68krc		 = -mcpu=68000 \
			   -DWITH_STARTUP_RELOCATION

OPTS_p90mb		 = -mcpu=68010 \
			   -DWITH_STARTUP_DATA_COPY \
			   -DWITH_STARTUP_VECTOR_COPY \
			   -DWITH_NATIVE_FEATURES \
			   -DWITH_MEM_CMDS \
			   -DWITH_SRECORD \
			   -DWITH_TESTS

# CB030 options
# =============
#
# WITH_8BIT_COMPACT_FLASH
# 	Configure for a rev1 board with the original 8-bit CF
# 	interface.
#
OPTS_cb030		 = -mcpu=68030 \
			   -DWITH_STARTUP_DATA_COPY \
			   -DWITH_STARTUP_VECTOR_COPY \
			   -DSTARTUP_TOUCH_ADDRESS=0xffff8000 \
			   -DWITH_NATIVE_FEATURES \
			   -DWITH_MEM_CMDS \
			   -DWITH_OSK \
			   -DWITH_SRECORD \
			   -DWITH_TESTS \
			   -DWITH_8BIT_COMPACT_FLASH

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
