CC ?= $(CROSS_COMPILE)gcc
CFLAGS := $(CFLAGS) -Wall -Wextra -Werror -Wfatal-errors -std=c99

LD ?= $(CROSS_COMPILE)gcc
LDFLAGS ?=
LDLIBS ?=

.PHONY: all
all: alloy-font-generator

alloy-font-generator: LDLIBS=$(shell pkg-config --libs freetype2)

alloy-font-generator.o: CFLAGS=$(shell pkg-config --cflags freetype2)

alloy-font-generator.o: alloy-font-generator.c

.PHONY: clean
clean:
	$(RM) alloy-font-generator alloy-font-generator.o

.PHONY: test
test:

.PHONY: install
install:

