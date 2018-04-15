TOP ?= ..
CONFIG ?= default
include $(TOP)/make/$(CONFIG)-config.mk
include $(TOP)/make/patterns.mk

LDLIBS=$(shell pkg-config --libs freetype2)

CFLAGS=$(shell pkg-config --cflags freetype2)

.PHONY: all
all: alloy-font-generator

alloy-font-generator: alloy-font-generator.o

alloy-font-generator.o: alloy-font-generator.c

.PHONY: clean
clean:
	$(RM) alloy-font-generator alloy-font-generator.o

.PHONY: test
test:

.PHONY: install
install:

$(V).SILENT:
