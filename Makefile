.SUFFIXES:

CACHE_DIR := .tear_cache
$(shell mkdir -p $(CACHE_DIR) > /dev/null)

# ==========
# = CONFIG =
# ==========

TOOLS_DIR ?= tools

EA_HOME ?= $(TOOLS_DIR)/ea
FE8_GBA ?= $(TOOLS_DIR)/fe8_us.gba

CLIB_HOME ?= $(TOOLS_DIR)/clib

HACK_GBA := fe8-hack.gba
MAIN_EVENT := main.event

INCLUDE_DIRS := $(CLIB_HOME)/include

# ====================
# = TOOL DEFINITIONS =
# ====================

TOOLCHAIN ?= $(DEVKITARM)

ifneq (,$(TOOLCHAIN))
  export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif

ifeq ($(shell python3 -c 'import sys; print(int(sys.version_info[0] > 2))'),1)
	PYTHON3 := python3
else
	PYTHON3 := python
endif

PREFIX := arm-none-eabi-

CC := $(PREFIX)gcc
AS := $(PREFIX)as
OBJCOPY := $(PREFIX)objcopy

EA                := $(EA_HOME)/ColorzCore
EADEP             := $(EA_HOME)/ea-dep
PARSEFILE         := $(EA_HOME)/Tools/ParseFile
PORTRAITFORMATTER := $(EA_HOME)/Tools/PortraitFormatter
PNG2DMP           := $(EA_HOME)/Tools/Png2Dmp
COMPRESS          := $(EA_HOME)/Tools/compress
LYN               := $(EA_HOME)/Tools/lyn

TEXT_PROCESS      := $(PYTHON3) $(TOOLS_DIR)/scripts/text-process-c.py
#TEXT_PROCESS      := $(PYTHON3) $(TOOLS_DIR)/fe-pytools/text-process-classic.py
TMX2TSA           := $(PYTHON3) $(TOOLS_DIR)/scripts/tmx2tsa.py

FE8_SYM := $(CLIB_HOME)/fe8_us.sym
FE8_REF := $(CLIB_HOME)/fe8_us.s

# ===============
# = MAIN TARGET =
# ===============

hack: $(HACK_GBA)

.PHONY: hack

MAIN_DEPENDS := $(shell $(EADEP) $(MAIN_EVENT) -I $(EA_HOME) --add-missings)

# TODO: sym stuff
$(HACK_GBA): $(FE8_GBA) $(MAIN_EVENT) $(MAIN_DEPENDS)
	cp -f $(FE8_GBA) $(HACK_GBA)
	$(EA) A FE8 -input:$(MAIN_EVENT) -output:$(HACK_GBA) --nocash-sym || rm -f $(HACK_GBA)
	cat $(FE8_SYM) >> $(HACK_GBA:.gba=.sym)

# ==================
# = OBJECTS & DMPS =
# ==================

LYN_REFERENCE := $(FE8_REF:.s=.o)

# OBJ to event
%.lyn.event: %.o $(LYN_REFERENCE)
	$(LYN) $< $(LYN_REFERENCE) > $@

# OBJ to DMP rule
%.dmp: %.o
	$(OBJCOPY) -S $< -O binary $@

# ========================
# = ASSEMBLY/COMPILATION =
# ========================

# Setting C/ASM include directories up
INCFLAGS := $(foreach dir, $(INCLUDE_DIRS), -I "$(dir)")

# setting up compilation flags
ARCH    := -mcpu=arm7tdmi -mthumb -mthumb-interwork
CFLAGS  := $(ARCH) $(INCFLAGS) -Wall -Wextra -Wno-unused-parameter -O2 -mtune=arm7tdmi -mlong-calls
ASFLAGS := $(ARCH) $(INCFLAGS)

# defining dependency flags
CDEPFLAGS = -MMD -MT "$*.o" -MT "$*.asm" -MF "$(CACHE_DIR)/$(notdir $*).d" -MP
SDEPFLAGS = --MD "$(CACHE_DIR)/$(notdir $*).d"

# ASM to OBJ rule
%.o: %.s
	$(AS) $(ASFLAGS) $(SDEPFLAGS) -I $(dir $<) $< -o $@

# C to ASM rule
%.o: %.c
	$(CC) $(CFLAGS) $(CDEPFLAGS) -g -c $< -o $@

# C to ASM rule
%.asm: %.c
	$(CC) $(CFLAGS) $(CDEPFLAGS) -S $< -o $@ -fverbose-asm

# =================
# = GRAPHICS DATA =
# =================

%.tsa: %.tmx
	$(TMX2TSA) $< $@

%.lz: %
	$(COMPRESS) $< $@

# =============
# = TEXT DATA =
# =============

TEXT_ALL         := $(shell find 'src/PrepScreenGuide' -type f -name '*.txt')
TEXT_PARSEDEFS   := src/Text/ParseDefinitions.txt
TEXT_INSTALLER   := src/Text/TextInstaller.event
TEXT_DEFINITIONS := src/Text/TextDefinitions.h

# Make text installer and definitions from text
$(TEXT_INSTALLER) $(TEXT_DEFINITIONS): $(TEXT_ALL)
	@$(TEXT_PROCESS) $< --definitions $(TEXT_PARSEDEFS) --installer $(TEXT_INSTALLER) --definitions $(TEXT_DEFINITIONS) --parser-exe $(PARSEFILE)
#	@$(TEXT_PROCESS) $< --parse-definitions $(TEXT_PARSEDEFS) --installer $(TEXT_INSTALLER) --definitions $(TEXT_DEFINITIONS) --parser-exe $(PARSEFILE)
#	@$(TEXT_PROCESS) $< --definitions $(TEXT_PARSEDEFS) --installer $(dir $<)/$(basename $(notdir $<))_TextInstaller.event --definitions $(dir $<)/$(basename $(notdir $<))_TextDefinitions.h --parser-exe $(PARSEFILE)

# Convert formatted text to insertable binary
%.fetxt.dmp: %.fetxt
	@$(PARSEFILE) $< -o $@

# Avoid make deleting objects it thinks it doesn't need anymore
# Without this make may fail to detect some files as being up to date
# TODO: investigate why
.PRECIOUS: %.o;

-include $(wildcard $(CACHE_DIR)/*.d)
