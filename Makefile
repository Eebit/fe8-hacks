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

# Avoid make deleting objects it thinks it doesn't need anymore
# Without this make may fail to detect some files as being up to date
# TODO: investigate why
.PRECIOUS: %.o;

-include $(wildcard $(CACHE_DIR)/*.d)
