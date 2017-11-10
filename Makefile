.PHONY: all
all: alloy-font.o

alloy-font.o: alloy-font.c alloy-font.h

alloy-font.c: alloy-font-generator
	./$<

alloy-font-generator: CFLAGS=$(shell pkg-config --cflags freetype2)
alloy-font-generator: LDLIBS=$(shell pkg-config --libs freetype2)

alloy-font-generator: alloy-font-generator.c

.PHONY: clean
clean:
	$(RM) alloy-font-generator

.PHONY: test
test:

.PHONY: install
install:

