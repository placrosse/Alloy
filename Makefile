TOP = $(CURDIR)
CONFIG ?= default
include $(TOP)/make/$(CONFIG)-config.mk
include $(TOP)/make/patterns.mk

.PHONY: all clean
all clean:
	$(MAKE) -C utils $@
	$(MAKE) -C fonts/source-code-pro $@
	$(MAKE) -C src $@

.PHONY: test
test:

.PHONY: install
install:
	$(MAKE) -C utils
	$(MAKE) -C fonts/source-code-pro
	$(MAKE) -C src $@

$(V).SILENT:
