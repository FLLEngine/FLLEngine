FROZEN_MANIFEST ?= manifest.py
FROZEN_DIR =
FROZEN_MPY_DIR =

ifeq ("$(wildcard .micropython/lib/axtls/README)","")
$(info GIT cloning axtls submodule)
$(info $(shell cd .micropython && git submodule update --init lib/axtls))
ifeq ("$(wildcard .micropython/lib/axtls/README)","")
$(error failed)
endif
endif
ifeq ("$(wildcard .micropython/lib/berkeley-db-1.xx/README)","")
$(info GIT cloning berkeley-db-1.xx submodule)
$(info $(shell cd .micropython && git submodule update --init lib/berkeley-db-1.xx))
ifeq ("$(wildcard .micropython/lib/berkeley-db-1.xx/README)","")
$(error failed)
endif
endif

ifneq ($(FROZEN_MANIFEST)$(FROZEN_MPY_DIR),)
# To use frozen code create a manifest.py file with a description of files to
# freeze, then invoke make with FROZEN_MANIFEST=manifest.py (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMPZ_DIG_SIZE=16 # force 16 bits to work on both 32 and 64 bit archs
MPY_CROSS_FLAGS += -mcache-lookup-bc
endif

ifneq ($(FROZEN_MANIFEST)$(FROZEN_DIR),)
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

main:
	make -C .micropython/ports/unix USER_C_MODULES=../../.. V=1 PYBRICKS_VERSION_LEVEL_STR='idgaf'
