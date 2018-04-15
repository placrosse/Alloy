ifndef TOP
$(error "The TOP variable should be defined.")
endif
DESTDIR ?=
PREFIX ?= /opt/return-infinity
PLATFORM ?= linux
PLATFORM_$(PLATFORM) = 1
CROSS_COMPILE ?=
CFLAGS += -Wall -Wextra -Werror -Wfatal-errors
CFLAGS += -I $(TOP)/include
CFLAGS += -g
CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
AS := $(CROSS_COMPILE)as
LD := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
NASM := nasm
