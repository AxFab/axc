# Makefile
# ---------------------------------------------------------------------------

# Settings user
MODE=dbg
bld_dir = .

CC = gcc
LD =  ld
V = 

CFLAGS = -Wall -Wextra -Wno-unused-parameter -nostdinc
LFLAGS = -nostdlib
INC = -I $(src_dir)/include
SRCS =  

# Settings scripted
src_dir = $(shell dirname $(firstword $(MAKEFILE_LIST)))

kn_dir = $(src_dir)/../kernel
bin_dir = $(bld_dir)/bin
lib_dir = $(bld_dir)/lib
obj_dir = $(bld_dir)/obj


ifeq ($(MODE),dbg)
CFLAGS += -ggdb3
else ifeq ($(MODE),cov)
CFLAGS += -ggdb3 --coverage -fprofile-arcs -ftest-coverage
LFLAGS += --coverage
else ifeq ($(MODE),cross)
CFLAGS += -ggdb3
else
CFLAGS += -O2
LFLAGS += -s
endif


# ---------------------------------------------------------------------------
# Additional info
linuxname := $(shell uname -sr)
git_hash := $(shell git --git-dir=$(src_dir) log -n1 --pretty='%h')$(shell if [ -n "$(git status --short -uno)"]; then echo '+'; fi)
date := $(shell date '+%d %b %Y')
vtag := $(shell cat $(src_dir)/.build)

CFLAGS += -D_DATE_=\"'$(date)'\" -D_OSNAME_=\"'$(linuxname)'\" 
CFLAGS += -D_GITH_=\"'$(git_hash)'\" -D_VTAG_=\"'$(vtag)'\"

# INC += -I $(src_dir)/include
 
# ---------------------------------------------------------------------------
# Generic definitions
define objs
  $(patsubst $(src_dir)/%.c,$(obj_dir)/%.o,    \
  $(patsubst $(src_dir)/%.cpp,$(obj_dir)/%.o,  \
  $(patsubst $(src_dir)/%.asm,$(obj_dir)/%.o,  \
  $(1)                                             \
  )))
endef

ifeq ($(VERBOSE),)
V = @
else
V = 
endif

ifeq ($(QUIET),)
Q = @ echo
else
Q = @ true
endif

ifeq ($(shell [ -d $(obj_dir) ] || echo N ),N)
NODEPS=1
endif
ifeq ($(MAKECMDGOALS),help)
NODEPS = 1
endif
ifeq ($(MAKECMDGOALS),clean)
NODEPS = 1
endif
ifeq ($(MAKECMDGOALS),destroy)
NODEPS = 1
endif

# ---------------------------------------------------------------------------
# Source code

SRCS += $(wildcard $(src_dir)/src/base/*.c)
SRCS += $(wildcard $(src_dir)/src/sys/*.c)

ifeq ($(MODE),cross)
SRCS += $(wildcard $(src_dir)/src/_x86/smoke.c)
else
SRCS += $(wildcard $(src_dir)/src/_x86/linux.c)
endif


# ---------------------------------------------------------------------------
all: $(lib_dir)/libaxb.a $(lib_dir)/libaxc.a $(lib_dir)/libaxc.so $(lib_dir)/crt0.o
	@ -ls -lh $^
# @ -size $^

base: $(lib_dir)/libaxb.a

axc: $(lib_dir)/libaxc.a

$(lib_dir)/libaxc.so: $(call objs,$(SRCS)) $(SLIB)
	@ mkdir -p $(dir $@)
	$(Q) "    LD  "$@
# ifeq ($(MODE),cross)
#   $(V) $(LD) -T $(ax_dir)/bin.ld $(LFLAGS) -o $@ $(call objs,$(SRCS)) $(SLIB)
# else
	$(V) $(CC) -shared $(LFLAGS) -o $@ $^
# endif


$(obj_dir)/%.o: $(src_dir)/%.c
	@ mkdir -p $(dir $@)
	$(Q) "    CC  "$<
	$(V) $(CC) -c $(INC) $(CFLAGS) -o $@ $<


$(obj_dir)/%.d: $(src_dir)/%.c
	@ mkdir -p $(dir $@)
	$(Q) "    DP  "$<
	$(V) $(CC) -MM $(INC) $(CFLAGS) -o $@ $<
ifneq ($(DPMAX),)
	@ cp $@ $@.tmp
	@ sh -c "cat $@.tmp | fmt -1 | sed -e 's/.*://' -e 's/\\\\$$//' -e 's/^\s*//' | grep -v '^\s*$$' | sed 's/$$/:/'" >> $@
	@ rm $@.tmp
endif

# ===========================================================================
#    Static Libraries
# ===========================================================================
SRC_axb += $(wildcard $(src_dir)/src/base/*.c)

$(lib_dir)/libaxb.a: $(call objs,$(SRC_axb))

SRC_axc += $(SRCS)

$(lib_dir)/libaxc.a: $(call objs,$(SRC_axc))


$(lib_dir)/lib%.a:
	@ mkdir -p $(dir $@)
	$(Q) "    AR  "$@
	$(V) ar src $@ $^


$(lib_dir)/crt0.o: $(src_dir)/src/_x86/scall.asm
	@ mkdir -p $(dir $@)
	$(Q) "    ASM "$<
	$(V) nasm -f elf32 -o $@ $<

# ---------------------------------------------------------------------------
clean:
	@ rm -rf $(obj_dir)

destroy: clean
	@ rm -rf $(bin_dir)
	@ rm -rf $(lib_dir)

sources:
	@ echo $(SRCS) | tr ' ' '\n'

help:
	@ echo " "
	@ echo "        Makefile"
	@ echo "USAGE:"
	@ echo "  make [target] [options]"
	@ echo " "
	@ echo " The target can be the path of a special delivery or one of "
	@ echo " the commands: "
	@ echo "   all:        build the kernel image"
	@ echo "   clean:      remove temporary file (objs)"
	@ echo "   destroy:    remove all generated files."
	@ echo "   sources:    print a list of sources files."
	@ echo "   help:       print this help."
	@ echo " "
	@ echo "OPTIONS:"
	@ echo "    QUIT=1      Remove extra logs."
	@ echo "    VERBOSE=1   Print main commands."
	@ echo "    NODEPS=1    Don't compute source dependencies."
	@ echo "                Note this option may activates by-itself."
	@ echo "    MODE=?      Change preset build option. I recommand changing "
	@ echo "                also the build directory or cleaning (default:dbg)."
	@ echo "    bld_dir=?   Change the directory to put generated file (default:.)."
	@ echo " "
	@ echo "DEPENDENCIES:"
	@ echo "  This script may run only using binutils suite."
	@ echo " "


# ===========================================================================

ifeq ($(NODEPS),)
-include $(patsubst %.o,%.d,$(call objs,$(SRCS)))
endif

# ===========================================================================
# ===========================================================================
