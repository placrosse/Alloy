PLATFORM ?= baremetal
CFLAGS += -fomit-frame-pointer
CFLAGS += -fno-stack-protector
CFLAGS += -mno-red-zone
CFLAGS += -DALLOY_WITH_BAREMETAL=1
ifndef TOP
$(error "TOP must be defined.")
endif
include $(TOP)/make/default-config.mk
